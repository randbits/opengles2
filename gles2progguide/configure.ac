#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([gles2progguide], [0.1], [BUG-REPORT-ADDRESS])

AC_CONFIG_SRCDIR([glesut/esShader.c])
AC_CONFIG_HEADERS([config.h])

AC_GNU_SOURCE

AM_INIT_AUTOMAKE([1.12])

LT_INIT

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for libraries.

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([stdlib.h string.h sys/time.h])

#check for packages
PKG_CHECK_MODULES(X11, [x11])
PKG_CHECK_MODULES(EGL, [egl])
PKG_CHECK_MODULES(GLESV2, [glesv2])
# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gettimeofday memset])

AC_CONFIG_FILES([
Makefile
glesut/Makefile
ch02/Makefile
])

AC_OUTPUT
