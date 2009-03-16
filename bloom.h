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

#ifndef _BLOOM_H_
#define _BLOOM_H_

#include <stdlib.h>
#include <stdint.h>

typedef struct _filter_spec_t {
	size_t filter_size;
	size_t size_bytes;
	uint8_t num_hashes;
} filter_spec_t;

typedef struct _bloom_t {
	uint8_t  *filter;
	struct _filter_spec_t spec;

	double max_error_rate;
	size_t num_elements;

	uint32_t  salt1;
	uint32_t  salt2;
} bloom_t;

typedef enum {
	BLOOM_SUCCESS,
	BLOOM_FAILURE,
	BLOOM_BADARG,
	BLOOM_NOTFOUND,
	BLOOM_BAD_FILTER_SIZE
} bloom_return;

bloom_return bloom_init(bloom_t *bloom, size_t num_elements, double max_error_rate);
void bloom_clean(bloom_t *bloom);

bloom_return bloom_add(bloom_t *bloom, const char *data, size_t data_len);
bloom_return bloom_contains(bloom_t *blook, const char *data, size_t data_len);

bloom_return bloom_calc_optimal(filter_spec_t *spec, size_t num_elements, double max_error_rate);

#endif /* _BLOOM_H_ */
