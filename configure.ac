dnl  Process this file with autoconf to produce a configure script
AC_REVISION($Revision$)

dnl   This configure file is known to work with autoconf-2.57,
dnl   automake versions 1.6.3 and 1.7.5, and libtool versions 1.4.2 and
dnl   1.5.  It should work with autoconf versions 2.50 or better, and
dnl   automake 1.6 or better.

dnl   Initialisation: package name and version number
m4_define([v_maj], [9])
m4_define([v_min], [2])
m4_define([v_mic], [13])
m4_define([project_version], [v_maj.v_min.v_mic])
AC_INIT([ast],[project_version],[starlink@jiscmail.ac.uk])
AC_CONFIG_AUX_DIR([build-aux])

dnl   Shared library versioning
m4_define([lt_cur], [m4_eval(v_maj + v_min)])
m4_define([lt_rev], [v_mic])
m4_define([lt_age], [v_min])
version_info="lt_cur:lt_rev:lt_age"
AC_SUBST([version_info])

dnl   Require autoconf-2.50 at least
AC_PREREQ([2.69])
dnl   Require Starlink automake
AM_INIT_AUTOMAKE([1.8.2-starlink subdir-objects])

dnl   Sanity-check: name a file in the source directory
AC_CONFIG_SRCDIR([ast_link.in])

#   Checks for programs
AC_PROG_CC
AC_PROG_CPP
LT_INIT
AC_PROG_LN_S

#   Include defaults for Starlink configurations
STAR_DEFAULTS

#   See if the --without-topinclude option has been provided. This sets the
#   automake conditional TOPINCLUDE. By default header files are installed
#   into both includedir and includedir/star. If --without-topinclude is
#   specified when running configure, then the header files are only
#   installed into includedir/star.
AC_ARG_WITH(
      [topinclude],
      AS_HELP_STRING([--without-topinclude],[install headers only in includedir/star - not includedir]),
      if test "$withval" = "yes"; then
         topinclude="1"
      else
         topinclude="0"
      fi,
      topinclude="1")
AM_CONDITIONAL(TOPINCLUDE, test x$topinclude = x1)

#   See if the --with-starmem option has been provided. This sets the
#   preprocesor macro HAVE_STAR_MEM_H.
AC_ARG_WITH(
      [starmem],
      AS_HELP_STRING([--with-starmem],[use starmem library for memory management]),
      AC_DEFINE([HAVE_STAR_MEM_H],[1],[Use the starmem library for memory management]),
)

#   See if the --with-memdebug option has been provided. This sets the
#   preprocesor macro MEM_DEBUG which enables facilities used to track
#   down memory leaks, etc.
AC_ARG_WITH(
      [memdebug],
      AS_HELP_STRING([--with-memdebug],[enable AST memory leak debugging functions]),
      AC_DEFINE([MEM_DEBUG],[1],[enable AST memory leak debugging functions in memory.c]),
)

#   See if the --with-external_pal option has been provided. This sets the
#   preprocesor macro EXTERNAL_PAL which prevents use of the PAL & ERFA
#   library functions that are included in the AST distribution. Suitable
#   link options are used within ast_link(_adam) scripts to pull in libpal.
AC_ARG_WITH([external_pal],
            [ --with-external_pal   Use external PAL and ERFA libraries],
        if test "$withval" = "yes"; then
           external_pal="1"
        else
           external_pal="0"
        fi,
        external_pal="0")
AC_SUBST( EXTERNAL_PAL, $external_pal )
if test "$external_pal" = "1"; then
   AC_SUBST( LIBPAL, "-lpal" )
   AC_DEFINE([EXTERNAL_PAL],[1],[use external PAL and ERFA libraries]),
else
   AC_SUBST( LIBPAL, "" )
fi
AM_CONDITIONAL(EXTERNAL_PAL, test x$external_pal = x1)

#   See if the --with-external_cminpack option has been provided. This sets the
#   preprocesor macro EXTERNAL_CMINPACK which prevents use of the CMINPACK
#   library functions that are included in the AST distribution. Suitable
#   link options are used within ast_link(_adam) scripts to pull in libcminpack.
AC_ARG_WITH([external_cminpack],
            [ --with-external_cminpack   Use external CMINPACK libraries],
        if test "$withval" = "yes"; then
           external_cminpack="1"
        else
           external_cminpack="0"
        fi,
        external_cminpack="0")
AC_SUBST( EXTERNAL_CMINPACK, $external_cminpack )
if test "$external_cminpack" = "1"; then
   AC_SUBST( LIBCMINPACK, "-lcminpack" )
   AC_DEFINE([EXTERNAL_CMINPACK],[1],[use external CMINPACK libraries]),
else
   AC_SUBST( LIBCMINPACK, "" )
fi
AM_CONDITIONAL(EXTERNAL_CMINPACK, test x$external_cminpack = x1)

#  Conditional defining whether we want to support Fortran libraries
#  (e.g. pgplot) and applications. It seems that this must come before
#  any uses of AC_CHECK_LIB as otherwise the library checked using
#  AC_CHECK_LIB is automatically added to the list of linker options
#  created by AC_FC_LIBRARY_LDFLAGS
AC_ARG_WITH([fortran],
            [ --without-fortran   Build package without Fortran support],
        if test "$withval" = "yes"; then
           use_fortran="1"
        else
           use_fortran="0"
        fi,
        use_fortran="1")
AM_CONDITIONAL(NOFORTRAN, test x$use_fortran = x0)
AC_SUBST(FORTRAN, $use_fortran)

#   ast_link needs to be able to link against the Fortran runtime if
#   necessary
if test "$use_fortran" = "1"; then
AC_PROG_FC
AC_FC_LIBRARY_LDFLAGS
fi

#  Prepare a version of STAR_FFLAGS with -I... removed
#  for use with ast_tester (which supplies these itself).
star_fflags_test=""
for flag in $STAR_FFLAGS; do
   case "$flag" in
      -I*) ;;
      *) star_fflags_test="$star_fflags_test $flag" ;;
   esac
done
AC_SUBST( STAR_FFLAGS_TEST, $star_fflags_test )

#   If --with-pic=no is set we should honour that.
AM_CONDITIONAL(NOPIC, test x$pic_mode = xno)

#  Conditional defining whether we build with POSIX thread support.
AC_ARG_WITH([pthreads],
            [ --without-pthreads   Build package without POSIX threads support],
        if test "$withval" = "yes"; then
           use_pthreads="1"
        else
           use_pthreads="0"
        fi,
        use_pthreads="1")
if test "$use_pthreads" = "1"; then
AC_CHECK_LIB([pthread], [pthread_create], ,[use_pthreads="0"])
fi
AM_CONDITIONAL(NOTHREADS, test x$use_pthreads = x0)
AC_SUBST(THREADS, $use_pthreads)

#  Conditional defining whether we build with YAML support.
AC_ARG_WITH([yaml],
            [ --without-yaml   Build package without YAML support],
        if test "$withval" = "yes"; then
           use_yaml="1"
        else
           use_yaml="0"
        fi,
        use_yaml="1")
if test "$use_yaml" = "1"; then
AC_CHECK_LIB([yaml], [yaml_parser_initialize], ,[use_yaml="0"])
fi
AM_CONDITIONAL(NOYAML, test x$use_yaml = x0)
AC_SUBST(YAML, $use_yaml)

# See which variadic function API to use
AC_CHECK_HEADERS(stdarg.h varargs.h, break)

# Can we use backtrace?
AC_CHECK_HEADERS([execinfo.h])
AC_CHECK_FUNCS([backtrace])

# Do we have reentrant strerror and strtok?
AC_CHECK_FUNCS([strerror_r strtok_r])

# Do we have vsnprintf?
AC_CHECK_FUNCS([vsnprintf])

#  See if we have long doubles (used by the Mapping and Region classes)
AC_CHECK_TYPES([long double])

#  See if we have 64 bit integers.
AC_CHECK_TYPES([int64_t, uint64_t])

# Calculate alternative 64 bit integer sizes
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)


# Decide how to get the name of the curently executing function.
# -------------------------------------------------------------
AC_DEFUN([CIT_FUNCTIONSTRING], [
  set_function_name=no



  AC_MSG_CHECKING([whether C compiler defines __func__])
  AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM([[]], [[const char* name = __func__;]])],
    [AC_MSG_RESULT(yes)
     set_function_name=yes
     AC_DEFINE([FUNCTION_NAME], [__func__], [Variable holding current function name.])],
    [AC_MSG_RESULT(no)])
  if test "$set_function_name" == no; then
    AC_MSG_CHECKING([whether C compiler defines __FUNCTION__])
    AC_COMPILE_IFELSE(
      [AC_LANG_PROGRAM([[]], [[const char* name = __FUNCTION__;]])],
      [AC_MSG_RESULT(yes)
       set_function_name=yes
       AC_DEFINE([FUNCTION_NAME], [__FUNCTION__], [Variable holding current function name.])],
      [AC_MSG_RESULT(no)])
  fi
])

CIT_FUNCTIONSTRING

#   Find an absolute path to the Perl binary, augmenting the path with the
#   location of the Starlink Perl build.  If this fails, then set @PERL@
#   to the backup `/usr/bin/env perl', which will find Perl if it exists
#   in the path at runtime.
AC_PATH_PROG(PERL, perl, [/usr/bin/env perl], [$STARLINK/Perl/bin:$PATH])

#   Function and declaration checks
AC_CHECK_FUNCS([isnan])
AC_CHECK_DECLS([isnan],,,[#include <math.h>
                         ])
AC_CHECK_FUNCS([isfinite])
AC_CHECK_DECLS([isfinite],,,[#include <math.h>
                         ])
STAR_DECLARE_DEPENDENCIES(sourceset, [sst htx])

#  Perform the check that configures f77.h.in for the return type of REAL
#  Fortran functions. On 64-bit g77 with f2c compatibility this is double
#  not float.
if test "$use_fortran" = "1"; then
STAR_CNF_F2C_COMPATIBLE

# Determine the type of Fortran character string lengths.
STAR_CNF_TRAIL_TYPE
fi

#   Declare the message file.
STAR_MESSGEN(ast_err.msg)

#   Test for non-ANSI behaviour in sscanf on some platforms, reported by
#   Bill Joye. Also check for bad MINGW sscanf. That returns nc=0 in the
#   System test.
AC_MSG_CHECKING([whether the sscanf function is ANSI-compatible])
AC_RUN_IFELSE([AC_LANG_SOURCE([[

#include <stdio.h>
#include <stdlib.h>

int main() {
  char foo[] = " name 1111.1 ";
  char mingw[] = "system= FK4_NO_E";

  char bar[8];
  float ff;
  int system;
  int nc;
  int r;

  nc = 0;
  r = sscanf(foo, " %s %f %n", bar, &ff, &nc);

  if ( nc == 13 ) {
     nc = 0;
     r = sscanf( mingw, "system= %n%*s %n", &system, &nc );
     if ( nc != 0 ) nc = 13;
  }
  exit( ( nc != 13 ) ? 0 : 1 );
}

]])],[
   AC_MSG_RESULT([no]);AC_DEFINE([HAVE_NONANSI_SSCANF],[1],[The sscanf shows the non-ANSI behaviour reported by Bill Joye])
],[AC_MSG_RESULT([yes])],[
   AC_DEFINE([HAVE_NONANSI_SSCANF],[1],[The sscanf may show the non-ANSI behaviour reported by Bill Joye])
])

#   Declare the documentation.  We need to do complicated things to
#   satisfy these targets, so give a non-null value
#   for the second argument, to suppress automatic generation of
#   rules.
STAR_LATEX_DOCUMENTATION([sun210 sun211], [sun210.pdf sun210.tex sun211.pdf sun211.tex sun210.htx_tar sun211.htx_tar])
STAR_PREDIST_SOURCES(sun_master.tex)
STAR_CHECK_PROGS(star2html)
STAR_CHECK_PROGS(prolat, sst)   # prolat is part of SST

AC_CONFIG_HEADERS(config.h)

AC_CONFIG_FILES(Makefile component.xml ast_link ast_link_adam src/object.h)
if test "$use_fortran" = "1"; then
AC_CONFIG_FILES(src/f77.h)
fi

AC_CONFIG_FILES([ast_cpp], [chmod +x ast_cpp])
#   Following are files which are substituted by the Makefile at
#   distribution time, rather than by configure.  They are not distributed.
STAR_PREDIST_SOURCES(src/version.h.in builddocs.in addversion.in)

AC_OUTPUT
