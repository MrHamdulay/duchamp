#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([Duchamp], [1.1], [Matthew.Whiting@csiro.au])
AC_CONFIG_SRCDIR([src/duchamp.hh])
AC_CONFIG_HEADER([src/config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP

# Checks for libraries.
AC_CHECK_LIB([g2c], [gerror_])
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

for INCDIR in $INCDIRS ; do
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
# PGPLOT compiled with a GNU compiler but linked with something else.
#AC_CHECK_LIB([m],       [log],       [PGPLOTLIB="-lm $PGPLOTLIB"])
#AC_CHECK_LIB([g2c],     [gerror_],   [PGPLOTLIB="-lg2c $PGPLOTLIB"],
#             [], [$PGPLOTLIB $LIBS])

# Search for X11 includes and libraries.
AC_PATH_X
if test "x$ac_x_libraries" != x; then
  LDFLAGS="$LDFLAGS -L$ac_x_libraries"
  PGPLOTLIB="-lX11 $PGPLOTLIB"
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
             [PGPLOTLIB=], [$PGPLOTLIB $LIBS])

if test "x$PGPLOTLIB" = x; then
  AC_MSG_ERROR([
    -------------------------------------------------------
    Could not find the PGPLOT library.

    ERROR: Duchamp configuration failure.
    -------------------------------------------------------], [1])
else
  AC_MSG_NOTICE([PGPLOT appears to be available.])
  AC_DEFINE([HAVE_PGPLOT], [1], [Define to 1 if PGPLOT is available.])
fi

AC_SUBST([PGPLOTINC])
AC_SUBST([PGPLOTLIB])
##########################################################################

##########################################################################
# Search for CFITSIO.
for INCDIR in $INCDIRS ; do
  AC_CHECK_FILE([$INCDIR/fitsio.h], [CFITSIOINC=-I$INCDIR; break])
  INCDIR=$INCDIR/cfitsio
  AC_CHECK_FILE([$INCDIR/fitsio.h], [CFITSIOINC=-I$INCDIR; break])
done

AC_CHECK_LIB([socket],  [recv],   [CFITSIOLIB="-lsocket"], [], [$LIBS])
AC_CHECK_LIB([cfitsio], [ffopen], [CFITSIOLIB="-lcfitsio $CFITSIOLIB"], [],
             [$CFITSIOLIB $LIBS])

if test "x$CFITSIOINC" = x -o "x$CFITSIOLIB" = x; then
  AC_MSG_ERROR([
    -------------------------------------------------------
    Could not find the CFITSIO library.

    ERROR: Duchamp configuration failure.
    -------------------------------------------------------], [1])
else
  for LIBDIR in $LIBDIRS ; do
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
for INCDIR in $INCDIRS ; do
  AC_CHECK_FILE([$INCDIR/cpgsbox.h], [WCSINC="-I$INCDIR $WCSINC"; break])
  INCDIR=$INCDIR/wcslib
  AC_CHECK_FILE([$INCDIR/cpgsbox.h], [WCSINC="-I$INCDIR $WCSINC"; break])
done
for INCDIR in $INCDIRS ; do
  AC_CHECK_FILE([$INCDIR/wcs.h], [WCSINC="-I$INCDIR $WCSINC"; break])
  INCDIR=$INCDIR/wcslib
  AC_CHECK_FILE([$INCDIR/wcs.h], [WCSINC="-I$INCDIR $WCSINC"; break])
done

AC_CHECK_LIB([pgsbox], [cpgsbox], [WCSLIB="-lpgsbox $WCSLIB"], [],
             [$WCSLIB $LIBS $CFITSIOLIB $PGPLOTLIB])
AC_CHECK_LIB([wcs], [wcss2p], [WCSLIB="-lwcs $WCSLIB"], [],
             [$WCSLIB $LIBS $CFITSIOLIB $PGPLOTLIB])

if test "x$WCSINC" = x -o "x$WCSLIB" = x; then
  AC_MSG_ERROR([
    -------------------------------------------------------
    Could not find the WCSLIB library.

    ERROR: Duchamp configuration failure.
    -------------------------------------------------------], [1])
else
  for LIBDIR in $LIBDIRS ; do
    AC_CHECK_FILE([$LIBDIR/libpgsbox.a], [WCSLIB="-L$LIBDIR $WCSLIB"; break])
    LIBDIR=$LIBDIR/wcs
    AC_CHECK_FILE([$LIBDIR/libpgsbox.a], [WCSLIB="-L$LIBDIR $WCSLIB"; break])
  done
  for LIBDIR in $LIBDIRS ; do
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
