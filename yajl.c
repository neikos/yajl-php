/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012, Nick Tolomiczenko                           |
  |                          <nick.tolomiczenko@gmail.com>               |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Nick Tolomiczenko <nick.tolomiczenko@gmail.com>              |
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

/* {{{ prototypes for forward declarations */
static zval *yajl_call_handler(yajl_parser *, zval *, int, zval **);
/* }}} */

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

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_set_object, 0, 0, 2)
    ZEND_ARG_INFO(0, parser)
    ZEND_ARG_INFO(1, obj)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_set_null_handler, 0, 0, 1)
    ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_set_boolean_handler, 0, 0, 1)
    ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_set_number_handler, 0, 0, 1)
    ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_set_string_handler, 0, 0, 1)
    ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_set_start_map_handler, 0, 0, 1)
    ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_set_map_key_handler, 0, 0, 1)
    ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_set_end_map_handler, 0, 0, 1)
    ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_set_start_array_handler, 0, 0, 1)
    ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_set_end_array_handler, 0, 0, 1)
    ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_parse, 0, 0, 2)
    ZEND_ARG_INFO(0, parser)
    ZEND_ARG_INFO(0, data)
    ZEND_ARG_INFO(0, verbose_error)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_complete_parse, 0, 0, 1)
    ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_parser_free, 0, 0, 1)
    ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_get_error, 0, 0, 1)
    ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yajl_get_bytes_consumed, 0, 0, 1)
    ZEND_ARG_INFO(0, parser)
ZEND_END_ARG_INFO()

/* {{{ yajl_functions[]
 *
 * Every user visible function must have an entry in yajl_functions[].
 */
const zend_function_entry yajl_functions[] = {
	PHP_FE(yajl_parser_create,	arginfo_yajl_parser_create)
	PHP_FE(yajl_parser_set_option,	arginfo_yajl_parser_set_option)
	PHP_FE(yajl_set_object,	arginfo_yajl_set_object)
	PHP_FE(yajl_set_null_handler,	arginfo_yajl_set_null_handler)
	PHP_FE(yajl_set_boolean_handler,	arginfo_yajl_set_boolean_handler)
	PHP_FE(yajl_set_number_handler,	arginfo_yajl_set_number_handler)
	PHP_FE(yajl_set_string_handler,	arginfo_yajl_set_string_handler)
	PHP_FE(yajl_set_start_map_handler,	arginfo_yajl_set_start_map_handler)
	PHP_FE(yajl_set_map_key_handler,	arginfo_yajl_set_map_key_handler)
	PHP_FE(yajl_set_end_map_handler,	arginfo_yajl_set_end_map_handler)
	PHP_FE(yajl_set_start_array_handler,	arginfo_yajl_set_start_array_handler)
	PHP_FE(yajl_set_end_array_handler,	arginfo_yajl_set_end_array_handler)
	PHP_FE(yajl_parse,	arginfo_yajl_parse)
	PHP_FE(yajl_complete_parse,	arginfo_yajl_complete_parse)
	PHP_FE(yajl_parser_free,	arginfo_yajl_parser_free)
	PHP_FE(yajl_get_error,	arginfo_yajl_get_error)
	PHP_FE(yajl_get_bytes_consumed,	arginfo_yajl_get_bytes_consumed)
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

/* {{{ yajl_parser_rsrc_dtor() */
static void yajl_parser_rsrc_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    yajl_parser *parser = (yajl_parser *)rsrc->ptr;

    if ( parser->yajl_handle )
    {
        yajl_free(parser->yajl_handle);
    }

    if ( parser->json_text )
    {
        efree(parser->json_text);
    }

    if ( parser->nullHandler )
	{
		zval_ptr_dtor(&parser->nullHandler);
	}

    if ( parser->booleanHandler )
	{
		zval_ptr_dtor(&parser->booleanHandler);
	}

    if ( parser->numberHandler )
	{
		zval_ptr_dtor(&parser->numberHandler);
	}

    if ( parser->stringHandler )
	{
		zval_ptr_dtor(&parser->stringHandler);
	}

    if ( parser->startMapHandler )
	{
		zval_ptr_dtor(&parser->startMapHandler);
	}

    if ( parser->mapKeyHandler )
	{
		zval_ptr_dtor(&parser->mapKeyHandler);
	}

    if ( parser->endMapHandler )
	{
		zval_ptr_dtor(&parser->endMapHandler);
	}

    if ( parser->startArrayHandler )
	{
		zval_ptr_dtor(&parser->startArrayHandler);
	}

    if ( parser->endArrayHandler )
	{
		zval_ptr_dtor(&parser->endArrayHandler);
	}

    if ( parser->object )
	{
		zval_ptr_dtor(&parser->object);
	}

    efree(parser);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(yajl)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/

    le_yajl_parser = zend_register_list_destructors_ex(yajl_parser_rsrc_dtor, NULL, 
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

/* {{{ extension convenience functions */
static zval *create_resource_zval(long value)
{
    zval *ret;
    TSRMLS_FETCH();

    MAKE_STD_ZVAL(ret);
    ZVAL_RESOURCE(ret,value);

    zend_list_addref(value);

    return ret;
}

static zval *create_bool_zval(int value)
{
    zval *ret;

    MAKE_STD_ZVAL(ret);
    ZVAL_BOOL(ret,value);

    return ret;
}

static zval *create_long_zval(long value)
{
    zval *ret;

    MAKE_STD_ZVAL(ret);
    ZVAL_LONG(ret,value);

    return ret;
}

static zval *create_double_zval(double value)
{
    zval *ret;

    MAKE_STD_ZVAL(ret);
    ZVAL_DOUBLE(ret,value);

    return ret;
}

static zval *create_string_zval(const char *stringVal, size_t stringLen)
{
    zval *ret;

    MAKE_STD_ZVAL(ret);
    ZVAL_STRINGL(ret,stringVal,stringLen,1);

    return ret;
}
/* }}} */

/* Going to use PHP's volatile alloc functions for the yajl parser */
/* {{{ Setting yajl's alloc functions
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
/* }}} */

/* Now craft the callbacks such that they call user-supplied PHP functions */
/* {{{ Setting yajl's callback functions
 */
static int yajl_null(void *ctx)
{
    yajl_parser *parser = (yajl_parser *)ctx;
    zval *retval, *args[1];

    if ( parser )
    {
        if ( parser->nullHandler )
        {
            args[0] = create_resource_zval(parser->index);

            if ((retval = yajl_call_handler(parser, parser->nullHandler,1, args)))
            {
                zval_ptr_dtor(&retval);
            }
        }
    }

    return 1;
}

static int yajl_boolean(void *ctx, int boolVal)
{
    yajl_parser *parser = (yajl_parser *)ctx;
    zval *retval, *args[2];

    if ( parser )
    {
        if ( parser->booleanHandler )
        {
            args[0] = create_resource_zval(parser->index);
            args[1] = create_bool_zval(boolVal);

            if ((retval = yajl_call_handler(parser, parser->booleanHandler,2, args)))
            {
                zval_ptr_dtor(&retval);
            }
        }
    }

    return 1;
}

static int yajl_integer(void *ctx, long long integerVal)
{
    yajl_parser *parser = (yajl_parser *)ctx;
    zval *retval, *args[2];

    TSRMLS_FETCH();

    if ( parser )
    {
        if ( parser->numberHandler )
        {
			if ((long)integerVal != integerVal) /* If truncation would occur, as it
												   might in 32 bit archs where 'long'
												   type is 4 bytes in size and a
												   'long long' is 8 bytes in size,
												   cause a handler failure */
			{
                php_error_docref(NULL TSRMLS_CC, E_WARNING,
					"integer could not be represented as a php int. Try parsing with 'numbers_are_strings' set to true: e.g., $parser = yajl_parser_create(true).");
				return 0;
			}

            args[0] = create_resource_zval(parser->index);
            args[1] = create_long_zval(integerVal);

            if ((retval = yajl_call_handler(parser, parser->numberHandler,2, args)))
            {
                zval_ptr_dtor(&retval);
            }
        }
    }

    return 1;
}

static int yajl_double(void *ctx, double doubleVal)
{
    yajl_parser *parser = (yajl_parser *)ctx;
    zval *retval, *args[2];

    if ( parser )
    {
        if ( parser->numberHandler )
        {
            args[0] = create_resource_zval(parser->index);
            args[1] = create_double_zval(doubleVal);

            if ((retval = yajl_call_handler(parser, parser->numberHandler,2, args)))
            {
                zval_ptr_dtor(&retval);
            }
        }
    }

    return 1;
}

static int yajl_string(void *ctx, const unsigned char *stringVal, size_t stringLen)
{
    yajl_parser *parser = (yajl_parser *)ctx;
    zval *retval, *args[2];

    if ( parser )
    {
        if ( parser->stringHandler )
        {
            args[0] = create_resource_zval(parser->index);
            args[1] = create_string_zval((const char *)stringVal, stringLen);

            if ((retval = yajl_call_handler(parser, parser->stringHandler,2, args)))
            {
                zval_ptr_dtor(&retval);
            }
        }
    }

    return 1;
}

static int yajl_number(void *ctx, const char *numberVal, size_t numberLen)
{
    yajl_parser *parser = (yajl_parser *)ctx;
    zval *retval, *args[2];

    if ( parser )
    {
        if ( parser->numberHandler )
        {
            args[0] = create_resource_zval(parser->index);
            args[1] = create_string_zval((const char *)numberVal, numberLen);

            if ((retval = yajl_call_handler(parser, parser->numberHandler,2, args)))
            {
                zval_ptr_dtor(&retval);
            }
        }
    }

    return 1;
}

static int yajl_start_map(void *ctx)
{
    yajl_parser *parser = (yajl_parser *)ctx;
    zval *retval, *args[1];

    if ( parser )
    {
        if ( parser->startMapHandler )
        {
            args[0] = create_resource_zval(parser->index);

            if ((retval = yajl_call_handler(parser, parser->startMapHandler,1, args)))
            {
                zval_ptr_dtor(&retval);
            }
        }
    }

    return 1;
}

static int yajl_map_key(void *ctx, const unsigned char * key, size_t keyLen)
{
    yajl_parser *parser = (yajl_parser *)ctx;
    zval *retval, *args[2];

    if ( parser )
    {
        if ( parser->mapKeyHandler )
        {
            args[0] = create_resource_zval(parser->index);
            args[1] = create_string_zval((const char *)key, keyLen);

            if ((retval = yajl_call_handler(parser, parser->mapKeyHandler,2, args)))
            {
                zval_ptr_dtor(&retval);
            }
        }
    }

    return 1;
}

static int yajl_end_map(void *ctx)
{
    yajl_parser *parser = (yajl_parser *)ctx;
    zval *retval, *args[1];

    if ( parser )
    {
        if ( parser->endMapHandler )
        {
            args[0] = create_resource_zval(parser->index);

            if ((retval = yajl_call_handler(parser, parser->endMapHandler,1, args)))
            {
                zval_ptr_dtor(&retval);
            }
        }
    }

    return 1;
}

static int yajl_start_array(void *ctx)
{
    yajl_parser *parser = (yajl_parser *)ctx;
    zval *retval, *args[1];

    if ( parser )
    {
        if ( parser->startArrayHandler )
        {
            args[0] = create_resource_zval(parser->index);

            if ((retval = yajl_call_handler(parser, parser->startArrayHandler,1, args)))
            {
                zval_ptr_dtor(&retval);
            }
        }
    }

    return 1;
}

static int yajl_end_array(void *ctx)
{
    yajl_parser *parser = (yajl_parser *)ctx;
    zval *retval, *args[1];

    if ( parser )
    {
        if ( parser->endArrayHandler )
        {
            args[0] = create_resource_zval(parser->index);

            if ((retval = yajl_call_handler(parser, parser->endArrayHandler,1, args)))
            {
                zval_ptr_dtor(&retval);
            }
        }
    }

    return 1;
}

static yajl_callbacks callbacks =
{
    .yajl_null = yajl_null,
    .yajl_boolean = yajl_boolean,
    .yajl_integer = yajl_integer,
    .yajl_double = yajl_double,
    .yajl_number = NULL,
    .yajl_string = yajl_string,
    .yajl_start_map = yajl_start_map,
    .yajl_map_key = yajl_map_key,
    .yajl_end_map = yajl_end_map,
    .yajl_start_array = yajl_start_array,
    .yajl_end_array = yajl_end_array
};

static yajl_callbacks numbers_are_strings_callbacks =
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
/* }}} */

/* {{{ yajl_set_handler() */
static void yajl_set_handler(zval **handler, zval **data)
{
    /* Release any original handler */
    if ( *handler )
    {
        zval_ptr_dtor(handler);
    }

    /* data could point to an array of array($obj,'method') */
    if (Z_TYPE_PP(data) != IS_ARRAY && Z_TYPE_PP(data) != IS_OBJECT)
    {
        convert_to_string_ex(data);

        if (Z_STRLEN_PP(data) == 0)
        {
            *handler = NULL;
            return;
        }
    }

    zval_add_ref(data);

    *handler = *data;
}
/* }}} */

/* {{{ yajl_call_handler() */
static zval *yajl_call_handler(yajl_parser *parser, zval *handler,int argc, zval **argv)
{
    int i, result;
    zval ***args;
    zval *retval;
    zend_fcall_info fci;
    zval **method;
    zval **obj;

    TSRMLS_FETCH();

    if ( parser && handler && ! EG(exception) )
    {
        args = safe_emalloc(sizeof(zval **), argc, 0);
    
        for (i = 0; i < argc; i++)
        {
            args[i] = &argv[i];
        }
    
        fci.size = sizeof(fci);
        fci.function_table = EG(function_table);
        fci.function_name = handler;
        fci.symbol_table = NULL;
        fci.object_ptr = parser->object;
        fci.retval_ptr_ptr = &retval;
        fci.param_count = argc;
        fci.params = args;
        fci.no_separation = 0;

        result = zend_call_function(&fci, NULL TSRMLS_CC);

        if (result == FAILURE)
        {
            if (Z_TYPE_P(handler) == IS_STRING)
            {
                php_error_docref(NULL TSRMLS_CC, E_WARNING,
                    "Unable to call handler %s()", Z_STRVAL_P(handler));
            }
            else if ( zend_hash_index_find(Z_ARRVAL_P(handler), 0, (void **) &obj)
                        == SUCCESS
                     &&
                      zend_hash_index_find(Z_ARRVAL_P(handler), 1, (void **) &method)
                        == SUCCESS
                     &&
                      Z_TYPE_PP(obj) == IS_OBJECT && Z_TYPE_PP(method) == IS_STRING )
            {
                php_error_docref(NULL TSRMLS_CC, E_WARNING,
                                 "Unable to call handler %s::%s()",
                                 Z_OBJCE_PP(obj)->name, Z_STRVAL_PP(method));
            }
            else
            {
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call handler");
            }
        }

        for (i = 0; i < argc; i++)
        {
            zval_ptr_dtor(args[i]);
        }

        efree(args);

        if (result == FAILURE)
        {
            return NULL;
        }
        else
        {
            return EG(exception) ? NULL : retval;
        }
    }
    else
    {
        for (i = 0; i < argc; i++)
        {
            zval_ptr_dtor(&argv[i]);
        }

        return NULL;
    }
}
/* }}} */

/* {{{ proto int yajl_parser_create( [ bool numbers_are_strings ]) 
   Creates a yajl parser resource */
PHP_FUNCTION(yajl_parser_create)
{
    yajl_parser *parser;
    zend_bool numbers_are_strings = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b",
                                &numbers_are_strings) == FAILURE)
    {
        return;
    }

    parser = ecalloc(1, sizeof(yajl_parser));
    if ( numbers_are_strings )
    {
        parser->yajl_handle = yajl_alloc(&numbers_are_strings_callbacks,
                                            &alloc_funcs, parser);
    }
    else
    {
        parser->yajl_handle = yajl_alloc(&callbacks,
                                            &alloc_funcs, parser);
    }
                                                                    
    parser->object = NULL;

    ZEND_REGISTER_RESOURCE(return_value, parser, le_yajl_parser);
    parser->index = Z_LVAL_P(return_value);
}
/* }}} */

/* {{{ proto int yajl_parser_set_option(resource parser, int option [, int value]) 
   Set options in an yajl parser */
PHP_FUNCTION(yajl_parser_set_option)
{
    yajl_parser *parser;
    zval *parser_index;
    long opt, val = 0;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|l",
                                &parser_index, &opt, &val) == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(parser, yajl_parser *, &parser_index,
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

    if ( yajl_config(parser->yajl_handle, opt, val) == 0 )
    {
        php_error_docref(NULL TSRMLS_CC, E_WARNING,
            "Could not set yajl parser config option %lx to value %ld",
                                                        opt, val);

        RETURN_FALSE;
    }

    RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int yajl_set_object(resource parser, object &obj) 
   Set an object that will have the callbacks as methods */
PHP_FUNCTION(yajl_set_object)
{
    yajl_parser *parser;
    zval *parser_index, *mythis;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ro", &parser_index, &mythis)
            == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(parser, yajl_parser *, &parser_index,
                            -1, "yajl parser", le_yajl_parser);

    if ( parser->object )
    {
        zval_ptr_dtor(&parser->object);
    }

    ALLOC_ZVAL(parser->object);
    MAKE_COPY_ZVAL(&mythis, parser->object);

    RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int yajl_set_null_handler(resource parser, string nhdl) 
   Set a handler to be called when a json 'null' is parsed */
PHP_FUNCTION(yajl_set_null_handler)
{
    yajl_parser *parser;
    zval *parser_index, **nhdl;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rZ", &parser_index, &nhdl)
            == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(parser, yajl_parser *, &parser_index,
                            -1, "yajl parser", le_yajl_parser);

    yajl_set_handler(&parser->nullHandler, nhdl);

    RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int yajl_set_boolean_handler(resource parser, string bhdl) 
   Set a handler to be called when a json 'boolean' is parsed */
PHP_FUNCTION(yajl_set_boolean_handler)
{
    yajl_parser *parser;
    zval *parser_index, **bhdl;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rZ", &parser_index, &bhdl)
            == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(parser, yajl_parser *, &parser_index,
                            -1, "yajl parser", le_yajl_parser);

    yajl_set_handler(&parser->booleanHandler, bhdl);

    RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int yajl_set_number_handler(resource parser, string nhdl) 
   Set a handler to be called when a json number is parsed */
PHP_FUNCTION(yajl_set_number_handler)
{
    yajl_parser *parser;
    zval *parser_index, **nhdl;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rZ", &parser_index, &nhdl)
            == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(parser, yajl_parser *, &parser_index,
                            -1, "yajl parser", le_yajl_parser);

    yajl_set_handler(&parser->numberHandler, nhdl);

    RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int yajl_set_string_handler(resource parser, string shdl) 
   Set a handler to be called when a json string is parsed */
PHP_FUNCTION(yajl_set_string_handler)
{
    yajl_parser *parser;
    zval *parser_index, **shdl;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rZ", &parser_index, &shdl)
            == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(parser, yajl_parser *, &parser_index,
                            -1, "yajl parser", le_yajl_parser);

    yajl_set_handler(&parser->stringHandler, shdl);

    RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int yajl_set_start_map_handler(resource parser, string smhdl) 
   Set a handler to be called when the start of a json map is encountered */
PHP_FUNCTION(yajl_set_start_map_handler)
{
    yajl_parser *parser;
    zval *parser_index, **smhdl;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rZ", &parser_index, &smhdl)
            == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(parser, yajl_parser *, &parser_index,
                            -1, "yajl parser", le_yajl_parser);

    yajl_set_handler(&parser->startMapHandler, smhdl);

    RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int yajl_set_map_key_handler(resource parser, string mkhdl) 
   Set a handler to be called when a map key is encountered */
PHP_FUNCTION(yajl_set_map_key_handler)
{
    yajl_parser *parser;
    zval *parser_index, **mkhdl;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rZ", &parser_index, &mkhdl)
            == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(parser, yajl_parser *, &parser_index,
                            -1, "yajl parser", le_yajl_parser);

    yajl_set_handler(&parser->mapKeyHandler, mkhdl);

    RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int yajl_set_end_map_handler(resource parser, string emhdl) 
   Set a handler to be called when the end of a json map is encountered */
PHP_FUNCTION(yajl_set_end_map_handler)
{
    yajl_parser *parser;
    zval *parser_index, **emhdl;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rZ", &parser_index, &emhdl)
            == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(parser, yajl_parser *, &parser_index,
                            -1, "yajl parser", le_yajl_parser);

    yajl_set_handler(&parser->endMapHandler, emhdl);

    RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int yajl_set_start_array_handler(resource parser, string sahdl) 
   Set a handler to be called when the start of a json array is encountered */
PHP_FUNCTION(yajl_set_start_array_handler)
{
    yajl_parser *parser;
    zval *parser_index, **sahdl;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rZ", &parser_index, &sahdl)
            == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(parser, yajl_parser *, &parser_index,
                            -1, "yajl parser", le_yajl_parser);

    yajl_set_handler(&parser->startArrayHandler, sahdl);

    RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int yajl_set_end_array_handler(resource parser, string eahdl) 
   Set a handler to be called when the end of a json array is encountered */
PHP_FUNCTION(yajl_set_end_array_handler)
{
    yajl_parser *parser;
    zval *parser_index, **eahdl;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rZ", &parser_index, &eahdl)
            == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(parser, yajl_parser *, &parser_index,
                            -1, "yajl parser", le_yajl_parser);

    yajl_set_handler(&parser->endArrayHandler, eahdl);

    RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int yajl_parse(resource parser, string data [, bool verbose_error]) 
   Parse some json text */
PHP_FUNCTION(yajl_parse)
{
    yajl_parser *parser;
    zval *parser_index;
    char *data;
    int data_len;
    zend_bool verbose_error = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|b",
        &parser_index, &data, &data_len, &verbose_error) == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(parser, yajl_parser *, &parser_index,
                            -1, "yajl parser", le_yajl_parser);

    if ( parser->json_text ) efree(parser->json_text);

    parser->yajl_status = yajl_status_ok;

    /* If a verbose error message is desired, hold onto the current json chunk.
     */
    if ( verbose_error ) parser->json_text = (unsigned char *)estrndup(data,data_len);

    parser->yajl_status = yajl_parse(parser->yajl_handle,
                                        (unsigned char *)data, data_len);

    RETVAL_LONG(parser->yajl_status==yajl_status_ok?1:0);
}
/* }}} */

/* {{{ proto int yajl_complete_parse(resource parser) 
   Parse any reamining buffered json text */
PHP_FUNCTION(yajl_complete_parse)
{
    yajl_parser *parser;
    zval *parser_index;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &parser_index) == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(parser, yajl_parser *, &parser_index,
                            -1, "yajl parser", le_yajl_parser);

    parser->yajl_status = yajl_complete_parse(parser->yajl_handle);

    RETVAL_LONG(parser->yajl_status==yajl_status_ok?1:0);
}
/* }}} */

/* {{{ proto int yajl_parser_free(resource parser) 
   Free the JSON parser */
PHP_FUNCTION(yajl_parser_free)
{
    zval *parser_index;
    yajl_parser *parser;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &parser_index) == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(parser, yajl_parser *, &parser_index,
                            -1, "yajl parser", le_yajl_parser);

    if (zend_list_delete(parser->index) == FAILURE)
    {
        RETURN_FALSE;
    }

    RETVAL_TRUE;
}
/* }}} */

/* {{{ proto int yajl_get_error(resource parser) 
   Get the JSON parser's current error string */
PHP_FUNCTION(yajl_get_error)
{
    yajl_parser *parser;
    zval *parser_index;
    char *error_str;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &parser_index) == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(parser, yajl_parser *, &parser_index,
                            -1, "yajl parser", le_yajl_parser);

    /* The string interpretation of 'yajl_status_ok' is more meaningful than
     * yajl_get_error()'s 'unknown error'
     */
    if ( parser->yajl_status == yajl_status_ok )
    {
        error_str = (char *)yajl_status_to_string(parser->yajl_status);
        RETVAL_STRING(error_str, 1);
    }
    else
    {
        if ( parser->json_text )
        {
            /* Indicates parser was created with verbose flag true. */
            error_str = (char *)yajl_get_error(parser->yajl_handle, 1,
                                parser->json_text, strlen((char *)parser->json_text));
        }
        else
        {
            error_str = (char *)yajl_get_error(parser->yajl_handle, 0, NULL, 0);
        }

        if ( error_str )
        {
            RETVAL_STRING(error_str, 1);
            yajl_free_error(parser->yajl_handle, (unsigned char *)error_str);
        }
    }
}
/* }}} */

/* {{{ proto int yajl_get_bytes_consumed(resource parser) 
   Get the amount of data consumed from the last parsed chunk */
PHP_FUNCTION(yajl_get_bytes_consumed)
{
    yajl_parser *parser;
    zval *parser_index;
    long num_bytes_consumed;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &parser_index) == FAILURE)
    {
        return;
    }

    ZEND_FETCH_RESOURCE(parser, yajl_parser *, &parser_index,
                            -1, "yajl parser", le_yajl_parser);

    /* The string interpretation of 'yajl_status_ok' is more meaningful than
     * yajl_get_error()'s 'unknown error'
     */

    num_bytes_consumed = yajl_get_bytes_consumed(parser->yajl_handle);

    RETVAL_LONG(num_bytes_consumed);
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
