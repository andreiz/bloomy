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

/* $ Id: $ */

#include "php_bloomy.h"
#include "bloom.h"

#include "ext/standard/php_lcg.h"
#include "ext/standard/php_rand.h"
#include "ext/standard/php_smart_str.h"
#include "ext/standard/php_var.h"

/****************************************
  Helper macros
****************************************/

#define BLOOM_METHOD_INIT_VARS             \
    zval*             object  = getThis(); \
    php_bloom_t*      obj     = NULL;      \

#define BLOOM_METHOD_FETCH_OBJECT                                             \
    obj = (php_bloom_t *) zend_object_store_get_object( object TSRMLS_CC );   \
	if (!obj->bloom) {	\
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "BloomFilter constructor was not called");	\
		return;	\
	}


/****************************************
  Structures and definitions
****************************************/

typedef struct {
	zend_object zo;

	bloom_t *bloom;
} php_bloom_t;

static zend_class_entry *bloom_ce = NULL;
static const double DEFAULT_ERROR_RATE = 0.01;


/****************************************
  Forward declarations
****************************************/

static void php_bloom_destroy(php_bloom_t *obj TSRMLS_DC);


/****************************************
  Method implementations
****************************************/

/* {{{ BloomFilter::__construct(int capacity [, double error_rate [, int random_seed ] ])
   Creates a new filter with the specified capacity */
static PHP_METHOD(BloomFilter, __construct)
{
	zval *object = getThis();
	php_bloom_t *obj;
	long capacity, seed = 0;
	double error_rate = DEFAULT_ERROR_RATE;
	bloom_return status;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|dl", &capacity, &error_rate, &seed) == FAILURE) {
		ZVAL_NULL(object);
		return;
	}

    if (capacity == 0       ||
        capacity > SIZE_MAX ||
        error_rate <= 0.0   ||
        error_rate >= 1.0) {

		ZVAL_NULL(object);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "bad filter parameters");
		return;
    }

	if (seed == 0) {
		seed = GENERATE_SEED();
	}
	srand(seed);

	obj = (php_bloom_t *) zend_object_store_get_object(object TSRMLS_CC);

	obj->bloom = (bloom_t *) emalloc(sizeof(bloom_t));
	status = bloom_init(obj->bloom, capacity, error_rate);
	if (status != BLOOM_SUCCESS) {
		ZVAL_NULL(object);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "could not create filter");
		return;
	}

}
/* }}} */

/* {{{ BloomFilter::add(string item)
   Adds an item to the filter */
static PHP_METHOD(BloomFilter, add)
{
	char *data = NULL;
	int   data_len;
	BLOOM_METHOD_INIT_VARS;
	bloom_return status;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &data, &data_len) == FAILURE) {
		return;
	}

	BLOOM_METHOD_FETCH_OBJECT;

	status = bloom_add(obj->bloom, data, data_len);
	if (status != BLOOM_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "could not add data to filter");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ BloomFilter::has(string item)
   Checks if the filter has the specified item */
static PHP_METHOD(BloomFilter, has)
{
	char *data = NULL;
	int   data_len;
	BLOOM_METHOD_INIT_VARS;
	bloom_return status;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &data, &data_len) == FAILURE) {
		return;
	}

	BLOOM_METHOD_FETCH_OBJECT;

	status = bloom_contains(obj->bloom, data, data_len);

	if (status == BLOOM_NOTFOUND) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

/* {{{ BloomFilter::getInfo()
   Returns array with filter information */
static PHP_METHOD(BloomFilter, getInfo)
{
	BLOOM_METHOD_INIT_VARS;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	BLOOM_METHOD_FETCH_OBJECT;

	array_init(return_value);
	add_assoc_double_ex(return_value, ZEND_STRS("error_rate"), obj->bloom->max_error_rate);
	add_assoc_long_ex(return_value, ZEND_STRS("num_hashes"), obj->bloom->spec.num_hashes);
	add_assoc_long_ex(return_value, ZEND_STRS("filter_size"), obj->bloom->spec.filter_size);
	add_assoc_long_ex(return_value, ZEND_STRS("filter_size_in_bytes"), obj->bloom->spec.size_bytes);
	add_assoc_long_ex(return_value, ZEND_STRS("num_items"), obj->bloom->num_elements);
}
/* }}} */


/****************************************
  Internal support code
****************************************/

/* {{{ constructor/destructor */
static void php_bloom_destroy(php_bloom_t *obj TSRMLS_DC)
{
	if (obj->bloom) {
		bloom_clean(obj->bloom);
		efree(obj->bloom);
	}
}

static void php_bloom_free_storage(php_bloom_t *obj TSRMLS_DC)
{
	zend_object_std_dtor(&obj->zo TSRMLS_CC);

	php_bloom_destroy(obj TSRMLS_CC);
	efree(obj);
}

zend_object_value php_bloom_new(zend_class_entry *ce TSRMLS_DC)
{
    zend_object_value retval;
    php_bloom_t *obj;
    zval *tmp;

    obj = (php_bloom_t *) emalloc(sizeof(*obj));
	memset(obj, 0, sizeof(*obj));
	zend_object_std_init(&obj->zo, ce TSRMLS_CC);
#if PHP_VERSION_ID < 50399
    zend_hash_copy(obj->zo.properties, &ce->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
#else
    object_properties_init(&(obj->std), class_type);
#endif

    retval.handle = zend_objects_store_put(obj, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t)php_bloom_free_storage, NULL TSRMLS_CC);
    retval.handlers = zend_get_std_object_handlers();

    return retval;
}
/* }}} */

/* {{{ internal API functions */
int php_bloom_serialize(zval *object, unsigned char **buffer, zend_uint *buf_len,
						zend_serialize_data *data TSRMLS_DC)
{
	zval value, *value_p = &value;
	smart_str buf = { 0 };
	php_serialize_data_t *var_hash = (php_serialize_data_t *)data;
	php_bloom_t *obj = (php_bloom_t *) zend_object_store_get_object(object TSRMLS_CC);

	smart_str_appendl(&buf, "p:", 2);
	smart_str_append_unsigned(&buf, obj->bloom->spec.filter_size);
	smart_str_appendc(&buf, ',');
	smart_str_append_unsigned(&buf, obj->bloom->spec.size_bytes);
	smart_str_appendc(&buf, ',');
	smart_str_append_unsigned(&buf, obj->bloom->spec.num_hashes);
	smart_str_appendc(&buf, ',');
	smart_str_append_unsigned(&buf, obj->bloom->num_elements);
	smart_str_appendc(&buf, ',');
	smart_str_append_unsigned(&buf, obj->bloom->salt1);
	smart_str_appendc(&buf, ',');
	smart_str_append_unsigned(&buf, obj->bloom->salt2);
	smart_str_appendc(&buf, ';');

	INIT_PZVAL(&value);
	ZVAL_DOUBLE(&value, obj->bloom->max_error_rate);
	php_var_serialize(&buf, &value_p, var_hash TSRMLS_CC);

	ZVAL_STRINGL(&value, (char*)obj->bloom->filter, obj->bloom->spec.size_bytes, 0);
	php_var_serialize(&buf, &value_p, var_hash TSRMLS_CC);

	*buffer = (unsigned char*)estrndup(buf.c, buf.len);
	*buf_len = buf.len;
	efree(buf.c);

	return SUCCESS;
}

int php_bloom_unserialize(zval **object, zend_class_entry *ce, const unsigned char *buf,
						  zend_uint buf_len, zend_unserialize_data *data TSRMLS_DC)
{
#define PARSE_NEXT_NUM() \
	num = (size_t)strtol((const char *)p, &e, 10); \
	if (num == 0 || errno == ERANGE || (*e != ',' && *e != ';') || (e+1 >= (char *)buf_end)) { \
		goto err_cleanup; \
	} \
	p = (const unsigned char *)++e;

	const unsigned char *p, *buf_end;
	char *e;
	long num;
	zval *value = NULL;
	php_bloom_t *obj;
	php_unserialize_data_t *var_hash = (php_unserialize_data_t *)data;

	object_init_ex(*object, ce);
	obj = (php_bloom_t *) zend_object_store_get_object(*object TSRMLS_CC);

	p = buf;
	buf_end = buf + buf_len;

	obj->bloom = (bloom_t *) emalloc(sizeof(bloom_t));
	memset(obj->bloom, 0, sizeof(php_bloom_t));

	if (*p != 'p' || *++p != ':') {
		goto err_cleanup;
	}
	++p;

	PARSE_NEXT_NUM();
	obj->bloom->spec.filter_size = (size_t)num;

	PARSE_NEXT_NUM();
	obj->bloom->spec.size_bytes = (size_t)num;

	PARSE_NEXT_NUM();
	if (num > UCHAR_MAX) {
		goto err_cleanup;
	}
	obj->bloom->spec.num_hashes = (uint8_t)num;

	PARSE_NEXT_NUM();
	obj->bloom->num_elements = (size_t)num;

	PARSE_NEXT_NUM();
	obj->bloom->salt1 = (size_t)num;

	PARSE_NEXT_NUM();
	obj->bloom->salt2 = (size_t)num;

	ALLOC_INIT_ZVAL(value);

	if (!php_var_unserialize(&value, &p, buf_end, var_hash TSRMLS_CC)
			|| Z_TYPE_P(value) != IS_DOUBLE) {
		zval_ptr_dtor(&value);
		goto err_cleanup;
	}

	--p; /* for ':' */
	obj->bloom->max_error_rate = Z_DVAL_P(value);
	if (*p != ';') {
		goto err_cleanup;
	}

	++p;
	if (!php_var_unserialize(&value, &p, buf_end, var_hash TSRMLS_CC)
			|| Z_TYPE_P(value) != IS_STRING
			|| Z_STRLEN_P(value) != obj->bloom->spec.size_bytes) {
		zval_ptr_dtor(&value);
		goto err_cleanup;
	}

	/*
	 * To avoid unnecessarily copying the string, we just point the filter to the
	 * unserialized string and simply free the zval container instead of destroying it
	 * with zval_ptr_dtor().
	 */
	obj->bloom->filter = (uint8_t *)Z_STRVAL_P(value);
	FREE_ZVAL(value);

	return SUCCESS;

err_cleanup:
	if (value) {
		zval_ptr_dtor(&value);
	}

	return FAILURE;
#undef PARSE_NEXT_NUM
}
/* }}} */

/* {{{ methods arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 0, 0, 1)
	ZEND_ARG_INFO(0, capacity)
	ZEND_ARG_INFO(0, error_rate)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_add, 0)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_has, 0)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_getInfo, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ bloom_class_methods */
static zend_function_entry bloom_class_methods[] = {
    PHP_ME(BloomFilter, __construct,        arginfo___construct, ZEND_ACC_PUBLIC)
    PHP_ME(BloomFilter, add,                arginfo_add,         ZEND_ACC_PUBLIC)
    PHP_ME(BloomFilter, has,                arginfo_has,         ZEND_ACC_PUBLIC)

    PHP_ME(BloomFilter, getInfo,           arginfo_getInfo,    ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};
/* }}} */

/* {{{ bloomy_module_entry
 */
zend_module_entry bloomy_module_entry = {
	STANDARD_MODULE_HEADER,
	"bloomy",
	NULL,
	PHP_MINIT(bloomy),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(bloomy),
	PHP_BLOOMY_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(bloomy)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "BloomFilter", bloom_class_methods);
	bloom_ce = zend_register_internal_class(&ce TSRMLS_CC);
	bloom_ce->create_object = php_bloom_new;
	bloom_ce->serialize     = php_bloom_serialize;
	bloom_ce->unserialize   = php_bloom_unserialize;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(bloomy)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "bloomy support", "enabled");
	php_info_print_table_row(2, "Version", PHP_BLOOMY_VERSION);
	php_info_print_table_end();
}
/* }}} */

#ifdef COMPILE_DL_BLOOMY
ZEND_GET_MODULE(bloomy)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
