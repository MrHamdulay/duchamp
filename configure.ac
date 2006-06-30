#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([Duchamp], [1.0], [matthew.whiting@csiro.au])
AC_CONFIG_SRCDIR([src/param.cc])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lX11':
#AC_CHECK_LIB([X11], [main])
# FIXME: Replace `main' with a function in `-lcfitsio':
#AC_CHECK_LIB([cfitsio], [main])
# FIXME: Replace `main' with a function in `-lcpgplot':
#AC_CHECK_LIB([cpgplot], [main])
# FIXME: Replace `main' with a function in `-lg2c':
AC_CHECK_LIB([g2c], [gerror_])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [log])
# FIXME: Replace `main' with a function in `-lpgplot':
#AC_CHECK_LIB([pgplot], [main])
# FIXME: Replace `main' with a function in `-lpgsbox':
#AC_CHECK_LIB([pgsbox], [main])
# FIXME: Replace `main' with a function in `-lwcs':
#AC_CHECK_LIB([wcs], [main])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h time.h math.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

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
         /local/cfitsio"

LIBDIRS="$LIBDIRS           \
         /usr/lib           \
         /usr/local/lib     \
         /usr/local/pgplot  \
         /usr/local/cfitsio \
         /usr/local/wcslib  \
         /local/lib         \
         /local/pgplot      \
         /local/cfitsio     \
         /u/whi550/CubeFinder\
         /opt/SUNWspro/lib"

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
AC_CHECK_LIB([m],       [log],       [PGPLOTLIB="-lm $PGPLOTLIB"])
AC_CHECK_LIB([g2c],     [gerror_],   [PGPLOTLIB="-lg2c $PGPLOTLIB"],
             [], [$PGPLOTLIB $LIBS])

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
  # Make sure that ffhdr2str is in the library -- exit if not
#  AC_CHECK_LIB([cfitsio], [ffhdr2str], [CFITSIOLIB="$CFITSIOLIB"], 
#               [AC_MSG_ERROR([
#    -------------------------------------------------------
#    CFITSIO library does not have function ffhdr2str.
#    Please upgrade your CFITSIO library.
#
#    ERROR: Duchamp configuration failure.
#    -------------------------------------------------------])],
#               [$CFITSIOLIB $LIBS])
#
  AC_MSG_NOTICE([CFITSIO appears to be available.])
  AC_DEFINE([HAVE_CFITSIO], [1], [Define to 1 if CFITSIO is available.])
fi

AC_SUBST([CFITSIOINC])
AC_SUBST([CFITSIOLIB])
##########################################################################

##########################################################################
# Search for WCSLIB.
for INCDIR in $INCDIRS ; do
  AC_CHECK_FILE([$INCDIR/wcs.h], [WCSINC="-I$INCDIR $WCSINC"; break])
  INCDIR=$INCDIR/wcslib
  AC_CHECK_FILE([$INCDIR/wcs.h], [WCSINC="-I$INCDIR $WCSINC"; break])
done
for INCDIR in $INCDIRS ; do
  AC_CHECK_FILE([$INCDIR/cpgsbox.h], [WCSINC="-I$INCDIR $WCSINC"; break])
  INCDIR=$INCDIR/wcslib
  AC_CHECK_FILE([$INCDIR/cpgsbox.h], [WCSINC="-I$INCDIR $WCSINC"; break])
done

AC_CHECK_LIB([wcs], [wcss2p], [WCSLIB="-lwcs $WCSLIB"], [],
             [$WCSLIB $LIBS $CFITSIOLIB $PGPLOTLIB])
AC_CHECK_LIB([pgsbox], [cpgsbox], [WCSLIB="-lpgsbox $WCSLIB"], [],
             [$WCSLIB $LIBS $CFITSIOLIB $PGPLOTLIB])

if test "x$WCSINC" = x -o "x$WCSLIB" = x; then
  AC_MSG_ERROR([
    -------------------------------------------------------
    Could not find the WCSLIB library.

    ERROR: Duchamp configuration failure.
    -------------------------------------------------------], [1])
else
  AC_MSG_NOTICE([WCSLIB appears to be available.])
  AC_DEFINE([HAVE_WCSLIB], [1], [Define to 1 if WCSLIB is available.])
fi

AC_SUBST([WCSINC])
AC_SUBST([WCSLIB])
##########################################################################

AC_SUBST([LDFLAGS])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
