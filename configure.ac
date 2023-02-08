#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([taralli], [[1.1]], [bf@abenteuerland.at])
#AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_FILES([Makefile])
AC_SUBST([CFLAGS], [["$CFLAGS -Wall -Wextra -O2"]])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([X11],[XOpenDisplay],[],[
         echo "X11 library is required for this program"
         exit -1])
AC_CHECK_LIB([Xi],[XGrabDevice],[],[
      echo "XInput is required for this program"
      exit -1])
AC_CHECK_LIB([Xrandr],[XRRGetOutputInfo],[],[
      echo "X11 RandR extension library is required for this program"
      exit -1])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h X11/X.h X11/extensions/XInput2.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([memset])

AC_OUTPUT
