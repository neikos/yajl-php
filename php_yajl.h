/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_YAJL_H
#define PHP_YAJL_H

extern zend_module_entry yajl_module_entry;
#define phpext_yajl_ptr &yajl_module_entry

#ifdef PHP_WIN32
#	define PHP_YAJL_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_YAJL_API __attribute__ ((visibility("default")))
#else
#	define PHP_YAJL_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(yajl);
PHP_MSHUTDOWN_FUNCTION(yajl);
PHP_RINIT_FUNCTION(yajl);
PHP_RSHUTDOWN_FUNCTION(yajl);
PHP_MINFO_FUNCTION(yajl);

PHP_FUNCTION(confirm_yajl_compiled);	/* For testing, remove later. */
PHP_FUNCTION(yajl_parser_create);
PHP_FUNCTION(yajl_parser_set_option);
PHP_FUNCTION(yajl_parser_set_object);
PHP_FUNCTION(yajl_set_null_handler);
PHP_FUNCTION(yajl_set_number_handler);
PHP_FUNCTION(yajl_parse);
PHP_FUNCTION(yajl_parser_free);
PHP_FUNCTION(yajl_get_error);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(yajl)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(yajl)
*/

#include <yajl/yajl_parse.h>

typedef struct
{
    int index;

    yajl_handle yajl_handle;

    int yajl_status;

    unsigned char *json_text;

    zval *nullHandler;
    zval *booleanHandler;
    zval *numberHandler;
    zval *stringHandler;
    zval *startMapHandler;
    zval *mapKeyHandler;
    zval *endMapHandler;
    zval *startArrayHandler;
    zval *endArrayHandler;

    zval *object;
} yajl_parser;

/* In every utility function you add that needs to use variables 
   in php_yajl_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as YAJL_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define YAJL_G(v) TSRMG(yajl_globals_id, zend_yajl_globals *, v)
#else
#define YAJL_G(v) (yajl_globals.v)
#endif

#endif	/* PHP_YAJL_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
