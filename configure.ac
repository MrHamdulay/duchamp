#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([Duchamp], [1.1.1], [Matthew.Whiting@csiro.au])
AC_CONFIG_SRCDIR([src/duchamp.hh])
AC_CONFIG_HEADER([src/config.h])
AC_PREFIX_DEFAULT(/usr/local)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LN_S
AC_CHECK_PROGS(INSTALL, install)

# Check for maths library -- will add -lm to $LIBS
AC_CHECK_LIB([m], [log])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h time.h math.h iostream fstream sstream iomanip vector string algorithm functional])
if test "x$ac_cv_header_stdc" = xno; then
  AC_MSG_ERROR([
    -------------------------------------------------------------------
    An ANSI standard C library is required to build Duchamp.  
    One of the ANSI C header files it requires is missing or unusable.

    ERROR: Duchamp configuration failure.
    -------------------------------------------------------------------], [1])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

# Utilities.
AC_PROG_RANLIB

# Checks for library functions.
AC_FUNC_STRTOD
AC_CHECK_FUNCS([floor pow sqrt strtol log atan fabs])

# Extra places to look for third-party include files and libraries.
INCDIRS="$INCDIRS           \
         /usr/include       \
         /usr/local/include \
         /usr/local/pgplot  \
         /usr/local/cfitsio \
         /usr/local/wcslib  \
         /local/pgplot      \
         /local/cfitsio     \
         /opt/local/include"

LIBDIRS="$LIBDIRS           \
         /usr/lib           \
         /usr/local/lib     \
         /usr/local/pgplot  \
         /usr/local/cfitsio \
         /usr/local/wcslib  \
         /local/lib         \
         /local/pgplot      \
         /local/cfitsio     \
         /opt/SUNWspro/lib  \
	 /opt/local/lib"

AC_MSG_NOTICE(LIBDIRS)

for LIBDIR in $LIBDIRS ; do
  AC_CHECK_FILE([$LIBDIR], [LDFLAGS="$LDFLAGS -L$LIBDIR"], [continue])
done

##########################################################################
# Search for PGPLOT
# use --with-cfitsio=directory to specify a particular PGPLOT directory.
# or  --without-pgplot or --with-pgplot=no to do without it
# else do the normal searching for libraries.

AC_MSG_NOTICE([  -- PGPLOT -- ])
AC_ARG_WITH(pgplot,
[
AC_HELP_STRING([--with-pgplot=no],
	       [Compile without PGPLOT graphics capabilities.])
AC_HELP_STRING([--without-pgplot],
	       [Compile without PGPLOT graphics capabilities.])
AC_HELP_STRING([--with-pgplot=<dir>],
	       [Compile with PGPLOT graphics capabilities, using the directory <dir> to specify the PGPLOT library.]),
],
[
if test "x$withval" != xno; then
AC_MSG_NOTICE([Using directory "$withval"])
PGPLOTINCDIRS=$withval
PGPLOTLIBDIRS=$withval
LDFLAGS="$LDFLAGS -L$withval"
fi
],
[
PGPLOTINCDIRS=$INCDIRS
PGPLOTLIBDIRS=$LIBDIRS
])

if test "x$PGPLOTLIBDIRS" = x; then
  #the argument to --with-pgplot was no, so we don't look for it.
  AC_MSG_NOTICE([Not enabling PGPLOT use.])
else

  #Set up the PGPLOT-related libraries and includes.

  for INCDIR in $PGPLOTINCDIRS ; do
    AC_CHECK_FILE([$INCDIR/cpgplot.h], [PGPLOTINC=-I$INCDIR; break])
    INCDIR=$INCDIR/pgplot
    AC_CHECK_FILE([$INCDIR/cpgplot.h], [PGPLOTINC=-I$INCDIR; break])
  done
  
  # PGPLOT compiled with a SUN compiler but linked with something else.
  AC_CHECK_LIB([sunmath], [cosd],      [PGPLOTLIB="-lsunmath $PGPLOTLIB"],
               [], [$PGPLOTLIB $LIBS])
  AC_CHECK_LIB([M77],     [iand_],     [PGPLOTLIB="-lM77 $PGPLOTLIB"],
               [], [$PGPLOTLIB $LIBS])
  AC_CHECK_LIB([F77],     [f77_init],  [PGPLOTLIB="-lF77 $PGPLOTLIB"],
               [], [$PGPLOTLIB $LIBS])
  
  # Checks for extra libraries needed by PGPLOT -- will add -lg2c to $LIBS
  AC_CHECK_LIB([g2c], [gerror_])

  # Search for X11 includes and libraries.
  AC_PATH_X
  if test "x$ac_x_libraries" != x; then
    LDFLAGS="$LDFLAGS -L$ac_x_libraries"
    PGPLOTLIB="-L$ac_x_libraries -lX11 $PGPLOTLIB"
  fi
  
  # It is possible that other libraries may be required depending on what
  # graphics drivers were installed with PGPLOT.
  AC_CHECK_LIB([z],       [deflate],   [PGPLOTLIB="-lz $PGPLOTLIB"],
               [], [$PGPLOTLIB $LIBS])
  AC_CHECK_LIB([png],     [png_error], [PGPLOTLIB="-lpng $PGPLOTLIB"], 
  	     [], [$PGPLOTLIB $LIBS])
  AC_CHECK_LIB([pgplot],  [pgbeg_],    [PGPLOTLIB="-lpgplot $PGPLOTLIB"],
               [], [$PGPLOTLIB $LIBS])
  AC_CHECK_LIB([cpgplot], [cpgopen],   [PGPLOTLIB="-lcpgplot $PGPLOTLIB"],
               [], [$PGPLOTLIB $LIBS])

  PGPLOTLIB="$PGPLOTLIB $LIBS"
  
  # If PGPLOT is not present, we give a warning message but still continue. 
  # The compilation is able to work without the PGPLOT-dependent files.
  if test "x$PGPLOTLIB" = x; then
    AC_MSG_NOTICE([
      -------------------------------------------------------
      WARNING! PGPLOT could not be found.
      Compiling Duchamp without graphics capabilities.               
      -------------------------------------------------------])
  else
    AC_MSG_NOTICE([PGPLOT appears to be available.])
    AC_DEFINE([HAVE_PGPLOT], [1], [Define to 1 if PGPLOT is available.])
    for LIBDIR in $PGPLOTLIBDIRS ; do
      AC_CHECK_FILE([$LIBDIR/libcpgplot.a], [PGPLOTLIB="-L$LIBDIR $PGPLOTLIB"; break])
    done
  fi

fi

AC_SUBST([PGPLOTINC])
AC_SUBST([PGPLOTLIB])
##########################################################################

##########################################################################
# Search for CFITSIO.
# use --with-cfitsio=directory to specify a particular CFITSIO directory.
# else do the normal searching for libraries.

AC_MSG_NOTICE([  -- CFITSIO -- ])
AC_ARG_WITH(cfitsio,
	AC_HELP_STRING([--with-cfitsio=<dir>],
		       [The CFITSIO library and include files are in directory <dir>.]),
[ #if we specify a directory...
if test "x$withval" = xno; then
  AC_MSG_NOTICE([
    The CFITSIO library is required.
    Ignoring the "--with-cfitsio=no" and searching for the library.], [1])
  CFITSIOINCDIRS=$INCDIRS
  CFITSIOLIBDIRS=$LIBDIRS
else
  AC_MSG_NOTICE([Using directory "$withval"])
  CFITSIOINCDIRS="$withval \
              $withval/include"
  CFITSIOLIBDIRS="$withval \
              $withval/lib"
fi
],
[ # if directory not specified, go searching.
CFITSIOINCDIRS=$INCDIRS
CFITSIOLIBDIRS=$LIBDIRS
])

for INCDIR in $CFITSIOINCDIRS ; do
  AC_CHECK_FILE([$INCDIR/fitsio.h], [CFITSIOINC=-I$INCDIR; break])
  INCDIR=$INCDIR/cfitsio
  AC_CHECK_FILE([$INCDIR/fitsio.h], [CFITSIOINC=-I$INCDIR; break])
done

AC_CHECK_LIB([socket],  [recv],   [CFITSIOLIB="-lsocket"], [], [$LIBS])
AC_CHECK_LIB([cfitsio], [ffopen], [CFITSIOLIB="-lcfitsio $CFITSIOLIB"], [],
             [$CFITSIOLIB $LIBS])

# CFITSIO is essential for Duchamp, so exit with error message if it
#   is not present.
if test "x$CFITSIOINC" = x -o "x$CFITSIOLIB" = x; then
  AC_MSG_ERROR([
    -------------------------------------------------------
    Could not find the CFITSIO library.

    ERROR: Duchamp configuration failure.
    -------------------------------------------------------], [1])
else
  for LIBDIR in $CFITSIOLIBDIRS ; do
    AC_CHECK_FILE([$LIBDIR/libcfitsio.a], [CFITSIOLIB="-L$LIBDIR $CFITSIOLIB"; break])
    LIBDIR=$LIBDIR/cfitsio
    AC_CHECK_FILE([$LIBDIR/libcfitsio.a], [CFITSIOLIB="-L$LIBDIR $CFITSIOLIB"; break])
  done
  AC_MSG_NOTICE([CFITSIO appears to be available.])
  AC_DEFINE([HAVE_CFITSIO], [1], [Define to 1 if CFITSIO is available.])
fi

AC_SUBST([CFITSIOINC])
AC_SUBST([CFITSIOLIB])
##########################################################################

##########################################################################
# Search for WCSLIB.
# use --with-cfitsio=directory to specify a particular CFITSIO directory.
# else do the normal searching for libraries.

AC_MSG_NOTICE([  -- WCSLIB -- ])
AC_ARG_WITH(wcslib,
	AC_HELP_STRING([--with-wcslib=<dir>],
		       [The WCSLIB library and include files are in directory <dir>.]),
[ #if we specify a directory...
if test "x$withval" = xno; then
  AC_MSG_NOTICE([
    The WCSLIB library is required.
    Ignoring the "--with-wcslib=no" and searching for the library.], [1])
  CFITSIOINCDIRS=$INCDIRS
  CFITSIOLIBDIRS=$LIBDIRS
else
  AC_MSG_NOTICE([Using directory "$withval"])
  WCSINCDIRS="$withval \
              $withval/include"
  WCSLIBDIRS="$withval \
              $withval/lib"
fi
],
[ # if directory not specified, go searching.
WCSINCDIRS=$INCDIRS
WCSLIBDIRS=$LIBDIRS
])

for INCDIR in $WCSINCDIRS ; do
  AC_CHECK_FILE([$INCDIR/cpgsbox.h], [WCSINC="-I$INCDIR $WCSINC"; break])
  INCDIR=$INCDIR/wcslib
  AC_CHECK_FILE([$INCDIR/cpgsbox.h], [WCSINC="-I$INCDIR $WCSINC"; break])
done
for INCDIR in $WCSINCDIRS ; do
  AC_CHECK_FILE([$INCDIR/wcs.h], [WCSINC="-I$INCDIR $WCSINC"; break])
  INCDIR=$INCDIR/wcslib
  AC_CHECK_FILE([$INCDIR/wcs.h], [WCSINC="-I$INCDIR $WCSINC"; break])
done

AC_CHECK_LIB([pgsbox], [cpgsbox], [WCSLIB="-lpgsbox $WCSLIB"], [],
             [$WCSLIB $LIBS $CFITSIOLIB $PGPLOTLIB])
AC_CHECK_LIB([wcs], [wcss2p], [WCSLIB="-lwcs $WCSLIB"], [],
             [$WCSLIB $LIBS $CFITSIOLIB $PGPLOTLIB])

# WCSLIB is essential for Duchamp, so exit with error message if it
#   is not present. Need at least libwcs to be available -- libpgsbox will
#   depend on PGPLOT being available, but is not critical.
if test "x$WCSINC" = x -o "x$WCSLIB" = x; then
  AC_MSG_ERROR([
    -------------------------------------------------------
    Could not find the WCSLIB library.

    ERROR: Duchamp configuration failure.
    -------------------------------------------------------], [1])
else
  for LIBDIR in $WCSLIBDIRS ; do
    AC_CHECK_FILE([$LIBDIR/libpgsbox.a], [WCSLIB="-L$LIBDIR $WCSLIB"; break])
    LIBDIR=$LIBDIR/wcs
    AC_CHECK_FILE([$LIBDIR/libpgsbox.a], [WCSLIB="-L$LIBDIR $WCSLIB"; break])
  done
  for LIBDIR in $WCSLIBDIRS ; do
    AC_CHECK_FILE([$LIBDIR/libwcs.a], [WCSLIB="-L$LIBDIR $WCSLIB"; break])
    LIBDIR=$LIBDIR/wcs
    AC_CHECK_FILE([$LIBDIR/libwcs.a], [WCSLIB="-L$LIBDIR $WCSLIB"; break])
  done
  AC_MSG_NOTICE([WCSLIB appears to be available.])
  AC_DEFINE([HAVE_WCSLIB], [1], [Define to 1 if WCSLIB is available.])
fi

AC_SUBST([WCSINC])
AC_SUBST([WCSLIB])
##########################################################################

AC_SUBST([LDFLAGS])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
