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

#define BITS_PER_CHAR 8
#define MAX_HASHES    50

struct _filter_spec_t {
	size_t filter_size;
	uint8_t num_hashes;
};

struct _bloom_t {
	uint8_t  *filter;
	struct _filter_spec_t spec;

	double    max_error_rate;
	size_t    num_elements;

	uint32_t  salt1;
	uint32_t  salt2;
};

static void bloom_calc_salts(bloom_t *bloom)
{
	if (!bloom) {
		return;
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

	bloom->filter = (uint8_t *)ecalloc(spec.filter_size / BITS_PER_CHAR, sizeof(uint8_t));
	bloom->spec   = spec;
	bloom->max_error_rate = max_error_rate;

	return BLOOM_SUCCESS;
}

void bloom_clean(bloom_t *bloom)
{
	if (!bloom) {
		return BLOOM_BADARG;
	}

	efree(bloom->filter);
}

bloom_return bloom_add(bloom_t *bloom, const char *data, size_t data_len)
{
}

bloom_return bloom_contains(bloom_t *bloom, const char *data, size_t data_len)
{
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

	return BLOOM_SUCCESS;
}

int main()
{
	filter_spec_t spec;

	bloom_calc_optimal(&spec, 1000000, 0.01);
	printf("filter size = %d\nhashes = %d\n", spec.filter_size, spec.num_hashes);
	return 0;
}
