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

#if HAVE_BLOOMY

/* {{{ bloomy_functions[] */
function_entry bloomy_functions[] = {
	{ NULL, NULL, NULL }
};
/* }}} */


/* {{{ bloomy_module_entry
 */
zend_module_entry bloomy_module_entry = {
	STANDARD_MODULE_HEADER,
	"bloomy",
	bloomy_functions,
	PHP_MINIT(bloomy),     /* Replace with NULL if there is nothing to do at php startup   */ 
	PHP_MSHUTDOWN(bloomy), /* Replace with NULL if there is nothing to do at php shutdown  */
	PHP_RINIT(bloomy),     /* Replace with NULL if there is nothing to do at request start */
	PHP_RSHUTDOWN(bloomy), /* Replace with NULL if there is nothing to do at request end   */
	PHP_MINFO(bloomy),
	"0.0.1dev", 
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_BLOOMY
ZEND_GET_MODULE(bloomy)
#endif


/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(bloomy)
{

	/* add your stuff here */

	return SUCCESS;
}
/* }}} */


/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(bloomy)
{

	/* add your stuff here */

	return SUCCESS;
}
/* }}} */


/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(bloomy)
{
	/* add your stuff here */

	return SUCCESS;
}
/* }}} */


/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(bloomy)
{
	/* add your stuff here */

	return SUCCESS;
}
/* }}} */


/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(bloomy)
{
	php_info_print_box_start(0);
	php_printf("<p>The unknown extension</p>\n");
	php_printf("<p>Version 0.0.1devdevel (2009-03-16)</p>\n");
	php_info_print_box_end();
	/* add your stuff here */

}
/* }}} */

#endif /* HAVE_BLOOMY */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
