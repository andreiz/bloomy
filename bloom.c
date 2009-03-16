/*
  +----------------------------------------------------------------------+
  | Copyright (c) 2009 The PHP Group                                     |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Andrei Zmievski <andrei@php.net>                            |
  +----------------------------------------------------------------------+
*/


#include "bloom.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "php.h"

#define BITS_PER_CHAR   8
#define BITS_CHAR_SHIFT 3
#define MAX_HASHES      50

void hashword2 (
		const uint32_t *k,                   /* the key, an array of uint32_t values */
		size_t          length,               /* the length of the key, in uint32_ts */
		uint32_t       *pc,                      /* IN: seed OUT: primary hash value */
		uint32_t       *pb);              /* IN: more seed OUT: secondary hash value */
void hashlittle2( 
  const void *key,       /* the key to hash */
  size_t      length,    /* length of the key */
  uint32_t   *pc,        /* IN: primary initval, OUT: primary hash */
  uint32_t   *pb);       /* IN: secondary initval, OUT: secondary hash */

static void bloom_gen_salts(bloom_t *bloom)
{
	int i, num_salts = 2;
	uint32_t salt;
	uint32_t* salts[2] = { &bloom->salt1, &bloom->salt2 };

	for (i = 0; i < num_salts; i++) {
		salt = ((uint32_t)rand() ^ (uint32_t)rand());
		if (salt == 0) continue;
		*salts[i] = salt;
	}
}

static void bloom_hash(bloom_t *bloom, const char *data, size_t data_len, uint32_t *hash1, uint32_t *hash2)
{
	*hash1 = bloom->salt1;
	*hash2 = bloom->salt2;

	if ((data_len & 0x3) == 0) {
		hashword2((uint32_t *)data, data_len / 4, hash1, hash2);
	} else {
		hashlittle2((void *)data, data_len, hash1, hash2);
	}
}

bloom_return bloom_init(bloom_t *bloom, size_t num_elements, double max_error_rate)
{
	filter_spec_t spec;
	bloom_return status;

	if (!bloom) {
		return BLOOM_BADARG;
	}

	if (num_elements == 0       ||
		num_elements > SIZE_MAX ||
		max_error_rate <= 0.0   ||
		max_error_rate >= 1.0) {

		return BLOOM_BADARG;
	}

	memset(bloom, 0, sizeof(*bloom));

	status = bloom_calc_optimal(&spec, num_elements, max_error_rate);
	if (status != BLOOM_SUCCESS) {
		return status;
	}

	bloom->filter = (uint8_t *)safe_emalloc(spec.filter_size / BITS_PER_CHAR, sizeof(uint8_t), 0);
	memset(bloom->filter, 0, spec.filter_size / BITS_PER_CHAR * sizeof(uint8_t));
	bloom->spec   = spec;
	bloom->max_error_rate = max_error_rate;

	bloom_gen_salts(bloom);

	return BLOOM_SUCCESS;
}

void bloom_clean(bloom_t *bloom)
{
	if (!bloom) {
		return;
	}

	if (bloom->filter) {
		efree(bloom->filter);
	}
}

bloom_return bloom_add(bloom_t *bloom, const char *data, size_t data_len)
{
	int i;
	uint32_t hash1, hash2;
	uint32_t index;

	if (!bloom || !data || data_len == 0) {
		return BLOOM_BADARG;
	}

	bloom_hash(bloom, data, data_len, &hash1, &hash2);
	for (i = 0; i < bloom->spec.num_hashes; i++) {
		index = (hash1 + i * hash2) % bloom->spec.filter_size;
		bloom->filter[index >> BITS_CHAR_SHIFT] |= 1 << (index % BITS_PER_CHAR);
	}

	bloom->num_elements++;

	return BLOOM_SUCCESS;
}

bloom_return bloom_contains(bloom_t *bloom, const char *data, size_t data_len)
{
	int i;
	uint32_t hash1, hash2;
	uint32_t index;

	if (!bloom || !data || data_len == 0) {
		return BLOOM_BADARG;
	}

	bloom_hash(bloom, data, data_len, &hash1, &hash2);
	for (i = 0; i < bloom->spec.num_hashes; i++) {
		index = (hash1 + i * hash2) % bloom->spec.filter_size;
		if ((bloom->filter[index >> BITS_CHAR_SHIFT] & (1 << (index % BITS_PER_CHAR))) == 0) {
			return BLOOM_NOTFOUND;
		}
	}

	return BLOOM_SUCCESS;
}

bloom_return bloom_calc_optimal(filter_spec_t *spec, size_t num_elements, double max_error_rate)
{
	double h;
	double curr_size = 0.0;
	double opt_size  = (double)SIZE_MAX;
	double opt_h     = 0.0;
	size_t filter_size;
	uint8_t num_hashes;

	if (!spec) {
		return BLOOM_FAILURE;
	}

	for (h = 0.0; h < (double)MAX_HASHES; h++) {
		if ((curr_size = ((-h * num_elements) / log(1 - pow(max_error_rate, 1 / h)))) < opt_size) {
			opt_size = curr_size;
			opt_h    = h;
		}
	}

	if (opt_h == 0.0) {
		return BLOOM_BAD_FILTER_SIZE;
	}

	filter_size = (size_t)opt_size;
	filter_size += ((filter_size % BITS_PER_CHAR) > 0 ? BITS_PER_CHAR - (filter_size % BITS_PER_CHAR) : 0);
	num_hashes  = (size_t)opt_h;

	spec->filter_size = filter_size;
	spec->num_hashes  = num_hashes;
	spec->size_bytes  = filter_size / BITS_PER_CHAR;

	return BLOOM_SUCCESS;
}

