# Initialization...
#
#AC_PREREQ([2.68])
AC_INIT([tecox], [41], [bugs@teco-lang.org],, [http://teco-lang.org])

AM_INIT_AUTOMAKE
AM_SILENT_RULES([yes])

#LT_PREREQ([2.2])
#LT_INIT([dlopen])

AC_CONFIG_SRCDIR([lib/teco.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([Makefile
		 doc/Makefile
		 include/Makefile
		 lib/Makefile
		 src/Makefile
                 tools/Makefile])

# Checks for libraries.
AX_WITH_CURSES

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_INSTALL
AM_PROG_CC_C_O

AC_OUTPUT


echo \
"-----------------------------------------------------------------

${PACKAGE_NAME} Version ${PACKAGE_VERSION}

Prefix: '${prefix}'
Compiler: '${CC} ${CFLAGS} ${CPPFLAGS}'

Package features:
  None.

Now type 'make @<:@<target>@:>@'
  where the optional <target> is:
    all                - build all binaries
    install            - install everything

-----------------------------------------------------------------"
