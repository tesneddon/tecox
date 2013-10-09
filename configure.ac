dnl Copyright (c) 2013, Tim Sneddon <tim@sneddon.id.au>
dnl
dnl tecox is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation, either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl tecox is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with tecox.  If not, see <http://www.gnu.org/licenses/>.

# Initialization...
#
AC_PREREQ([2.68])
AC_INIT([tecox], [41], [bugs@teco-lang.org],, [http://teco-lang.org])

AM_INIT_AUTOMAKE
AM_SILENT_RULES([yes])

LT_PREREQ([2.2])
LT_INIT([dlopen])

AC_CONFIG_SRCDIR([lib/teco.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile
		 doc/Makefile
		 include/Makefile
		 lib/Makefile
		 src/Makefile])

# Checks for programs.
AC_PROG_CC
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
