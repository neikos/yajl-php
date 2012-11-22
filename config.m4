dnl $Id$
dnl config.m4 for extension yajl

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(yajl, for yajl support,
Make sure that the comment is aligned:
[  --with-yajl             Include yajl support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(yajl, whether to enable yajl support,
dnl Make sure that the comment is aligned:
dnl [  --enable-yajl           Enable yajl support])

if test "$PHP_YAJL" != "no"; then
  dnl Write more examples of tests here...

  # --with-yajl -> check with-path
  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="/include/yajl/yajl_common.h"  # you most likely want to change this
  if test -r $PHP_YAJL/$SEARCH_FOR; then # path given as parameter
    YAJL_DIR=$PHP_YAJL
  else # search default path list
    AC_MSG_CHECKING([for yajl files in default path])
    for i in $SEARCH_PATH ; do
        if test -r $i/$SEARCH_FOR; then
            YAJL_DIR=$i
            AC_MSG_RESULT(found in $i)
        fi
    done
  fi
  
  if test -z "$YAJL_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the yajl distribution])
  fi

  # --with-yajl -> add include path
  PHP_ADD_INCLUDE($YAJL_DIR/include)

  # --with-yajl -> check for lib and symbol presence
  LIBNAME=yajl # you may want to change this
  LIBSYMBOL=yajl # you most likely want to change this 

  PHP_CHECK_LIBRARY($LIBNAME,yajl_alloc,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $YAJL_DIR/lib, YAJL_SHARED_LIBADD)
    AC_DEFINE(HAVE_YAJLLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong yajl lib version or lib not found])
    ],[
    -L$YAJL_DIR/lib -lm
  ])
  
  PHP_SUBST(YAJL_SHARED_LIBADD)

  PHP_NEW_EXTENSION(yajl, yajl.c, $ext_shared)
fi
