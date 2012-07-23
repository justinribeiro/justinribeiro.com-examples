dnl $Id: config.m4,v 1.4 2007/02/02 15:38:53 dsr Exp $
dnl config.m4 for extension sam

PHP_ARG_WITH(sam, for sam support,
 [  --with-sam             Include sam support])

PHP_ARG_WITH(sam_xms, for sam xms support,
 [  --with-sam_xms[=DIR]   Include sam_xms support])

if test "$PHP_SAM" != "no"; then

  PHP_NEW_EXTENSION(sam, php_sam.c, $ext_shared, , -DLINUX)

  if test "$PHP_SAM_XMS" != "no"; then

    dnl # --with-sam_xms -> check with-path
    SEARCH_PATH="/opt"
    SEARCH_FOR="/IBM/XMS/tools/c/include/xms.h"
    if test -r $PHP_SAM_XMS/; then # path given as parameter
      SAM_DIR=$PHP_SAM_XMS
    else # search default path list
      AC_MSG_CHECKING([for sam xms files in default path])
      for i in $SEARCH_PATH ; do
        if test -r $i/$SEARCH_FOR; then
          SAM_DIR=$i
          AC_MSG_RESULT(found in $i)
        fi
      done
    fi

    if test -z "$SAM_DIR"; then
      AC_MSG_RESULT([not found])
      AC_MSG_ERROR([Please reinstall the XMS distribution])
    fi

    dnl # --with-sam_xms -> add include path
    PHP_ADD_INCLUDE($SAM_DIR/IBM/XMS/tools/c/include)

    dnl # --with-sam_xms -> check for lib and symbol presence
    LIBNAME=gxi
    LIBSYMBOL=xmsConnFactCreate

    PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
     [
     PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $SAM_DIR/IBM/XMS/lib, SAM_XMS_SHARED_LIBADD)
     AC_DEFINE(HAVE_SAMLIB,1,[ ])
    ],[
     AC_MSG_ERROR([wrong sam XMS lib version or lib not found])
    ],[
      -L$SAM_DIR/IBM/XMS/lib -lm -ldl
    ])

    PHP_SUBST(SAM_XMS_SHARED_LIBADD)

    PHP_NEW_EXTENSION(sam_xms, php_sam_xms.c SAM_XMS_Utils.c SAM_XMS_Connection.c, $ext_shared, , -DLINUX)

  fi
fi