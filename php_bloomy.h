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

#ifndef PHP_BLOOMY_H
#define PHP_BLOOMY_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <ext/standard/info.h>

extern zend_module_entry bloomy_module_entry;
#define phpext_bloomy_ptr &bloomy_module_entry

#ifdef PHP_WIN32
#define PHP_BLOOMY_API __declspec(dllexport)
#else
#define PHP_BLOOMY_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define PHP_BLOOMY_VERSION "0.1.0"

PHP_MINIT_FUNCTION(bloomy);
PHP_MINFO_FUNCTION(bloomy);

#endif /* PHP_BLOOMY_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
