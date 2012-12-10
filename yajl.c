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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_yajl.h"

#include <yajl/yajl_parse.h>

/* If you declare any globals in php_yajl.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(yajl)
*/

/* True global resources - no need for thread safety here */
static int le_yajl_parser;

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_parser_create, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_parser_set_option, 0, 0, 2)
    ZEND_ARG_INFO(0, parser)
    ZEND_ARG_INFO(0, option)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

/* {{{ yajl_functions[]
 *
 * Every user visible function must have an entry in yajl_functions[].
 */
const zend_function_entry yajl_functions[] = {
	PHP_FE(confirm_yajl_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(yajl_parser_create,	arginfo_yajl_parser_create)
	PHP_FE(yajl_parser_set_option,	arginfo_yajl_parser_set_option)
	PHP_FE_END	/* Must be the last line in yajl_functions[] */
};
/* }}} */

/* {{{ yajl_module_entry
 */
zend_module_entry yajl_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"yajl",
	yajl_functions,
	PHP_MINIT(yajl),
	PHP_MSHUTDOWN(yajl),
	PHP_RINIT(yajl),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(yajl),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(yajl),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_YAJL
ZEND_GET_MODULE(yajl)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("yajl.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_yajl_globals, yajl_globals)
    STD_PHP_INI_ENTRY("yajl.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_yajl_globals, yajl_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_yajl_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_yajl_init_globals(zend_yajl_globals *yajl_globals)
{
	yajl_globals->global_value = 0;
	yajl_globals->global_string = NULL;
}
*/
/* }}} */

static void yajl_parser_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    yajl_handle hand = (yajl_handle)rsrc->ptr;

    yajl_free(hand);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(yajl)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/

    le_yajl_parser = zend_register_list_destructors_ex(yajl_parser_dtor, NULL, 
                                                       "yajl parser", module_number);

    REGISTER_LONG_CONSTANT("YAJL_ALLOW_COMMENTS", yajl_allow_comments,
                                CONST_CS | CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("YAJL_DONT_VALIDATE_STRINGS", yajl_dont_validate_strings,
                                CONST_CS | CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("YAJL_ALLOW_TRAILING_GARBAGE", yajl_allow_trailing_garbage,
                                CONST_CS | CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("YAJL_ALLOW_MULTIPLE_VALUES", yajl_allow_multiple_values,
                                CONST_CS | CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("YAJL_ALLOW_PARTIAL_VALUES", yajl_allow_partial_values,
                                CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(yajl)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(yajl)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(yajl)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(yajl)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "yajl support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* Going to use PHP's volatile alloc functions for the yajl parser.
 */
static void * yajl_internal_malloc(void *ctx, size_t sz)
{
    return emalloc(sz);
}

static void * yajl_internal_realloc(void *ctx, void * previous,
                                    size_t sz)
{
    return erealloc(previous, sz);
}

static void yajl_internal_free(void *ctx, void * ptr)
{
    efree(ptr);
}

/* Not going to use ctx. Set it to NULL.
 */
static yajl_alloc_funcs alloc_funcs = 
{
    .malloc = yajl_internal_malloc,
    .realloc = yajl_internal_realloc,
    .free = yajl_internal_free,
    .ctx = NULL
};

/* Now craft the callbacks such that they call user-supplied PHP functions.
 */
static int yajl_null(void *ctx)
{
    return yajl_status_ok;
}

static int yajl_boolean(void *ctx, int boolVal)
{
    return yajl_status_ok;
}

static int yajl_number(void *ctx, const char *numberVal, size_t numberLen)
{
    return yajl_status_ok;
}

static int yajl_string(void *ctx, const unsigned char *stringVal, size_t stringLen)
{
    return yajl_status_ok;
}

static int yajl_start_map(void *ctx)
{
    return yajl_status_ok;
}

static int yajl_map_key(void *ctx, const unsigned char * key, size_t stringLen)
{
    return yajl_status_ok;
}

static int yajl_end_map(void *ctx)
{
    return yajl_status_ok;
}

static int yajl_start_array(void *ctx)
{
    return yajl_status_ok;
}

static int yajl_end_array(void *ctx)
{
    return yajl_status_ok;
}

static yajl_callbacks callbacks =
{
    .yajl_null = yajl_null,
    .yajl_boolean = yajl_boolean,
    .yajl_integer = NULL,
    .yajl_double = NULL,
    .yajl_number = yajl_number,
    .yajl_string = yajl_string,
    .yajl_start_map = yajl_start_map,
    .yajl_map_key = yajl_map_key,
    .yajl_end_map = yajl_end_map,
    .yajl_start_array = yajl_start_array,
    .yajl_end_array = yajl_end_array
};

/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_yajl_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_yajl_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "yajl", arg);
	RETURN_STRINGL(strg, len, 0);
}

/* {{{ proto int yajl_parser_create() 
   Creates a yajl parser resource */
PHP_FUNCTION(yajl_parser_create)
{
    yajl_handle hand;

    hand = yajl_alloc(&callbacks, &alloc_funcs, NULL); /* NULL for ctx. Not using it. */

    ZEND_REGISTER_RESOURCE(return_value, hand, le_yajl_parser);
}
/* }}} */

/* {{{ proto int yajl_parser_set_option(resource parser, int option, int value) 
   Set options in an yajl parser */
PHP_FUNCTION(yajl_parser_set_option)
{
    yajl_handle hand;
    zval *parser_resource_id, **val;
    long opt;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll",
                                &parser_resource_id, &opt, &val) == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(hand, yajl_handle, &parser_resource_id,
                            -1, "yajl parser", le_yajl_parser);

    switch(opt)
    {
        case yajl_allow_comments:
            break;
        case yajl_dont_validate_strings:
            break;
        case yajl_allow_trailing_garbage:
            break;
        case yajl_allow_multiple_values:
            break;
        case yajl_allow_partial_values:
            break;
        default:
            php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown option");
            RETURN_FALSE;
            break;
    }

    if ( yajl_config(hand, opt, Z_LVAL_PP(val)) == 0 )
    {
        php_error_docref(NULL TSRMLS_CC, E_WARNING,
            "Could not set yajl parser config option %lx to value %ld",
                                                        opt, Z_LVAL_PP(val));

        RETURN_FALSE;
    }

    RETVAL_TRUE;
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
