dnl
dnl $ Id: $
dnl vim:se ts=2 sw=2 et:

PHP_ARG_ENABLE(bloomy, whether to enable bloomy functions,
[  --enable-bloomy         Enable bloomy support])

if test "$PHP_BLOOMY" != "no"; then
  export OLD_CPPFLAGS="$CPPFLAGS"
  export CPPFLAGS="$CPPFLAGS $INCLUDES -DHAVE_BLOOMY"

  export CPPFLAGS="$OLD_CPPFLAGS"

  PHP_SUBST(BLOOMY_SHARED_LIBADD)
  AC_DEFINE(HAVE_BLOOMY, 1, [ ])

  PHP_NEW_EXTENSION(bloomy, php_bloomy.c bloom.c lookup3.c, $ext_shared)

fi

