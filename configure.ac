# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([Duchamp], [1.2], [Matthew.Whiting@csiro.au])
AC_CONFIG_SRCDIR([src/duchamp.hh])
AC_CONFIG_HEADER([src/config.h])
AC_PREFIX_DEFAULT(/usr/local)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_F77
AC_PROG_LN_S
AC_CHECK_PROGS(INSTALL, install)

# Check for maths library -- will add -lm to $LIBS
AC_CHECK_LIB([m], [log])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h time.h math.h values.h iostream fstream sstream iomanip vector string algorithm functional])
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
# use --with-pgplot=directory to specify a particular PGPLOT directory.
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
  #This is the command to do the linking stage of the compilation
  #If no pgplot, we can just link with g++ (or equivalent)
  LINKER=$CXX 
#  AC_CHECK_LIB([stdc++], [main], [PGPLOTLIB="-lstdc++"])
#  if test "x$F77" = xgfortran; then
#    PGPLOTLIB="$PGPLOTLIB -bind_at_load"
#  fi
else

  #Set up the PGPLOT-related libraries and includes.

  for INCDIR in $PGPLOTINCDIRS ; do
    AC_CHECK_FILE([$INCDIR/cpgplot.h], [PGPLOTINC=-I$INCDIR; break])
    AC_CHECK_FILE([$INCDIR/pgplot/cpgplot.h], [PGPLOTINC=-I$INCDIR/pgplot; break])
    AC_CHECK_FILE([$INCDIR/include/cpgplot.h], [PGPLOTINC=-I$INCDIR/include; break])
  done
  
  # PGPLOT compiled with a SUN compiler but linked with something else.
  AC_CHECK_LIB([sunmath], [cosd],      [EXTRAPGLIB="-lsunmath $EXTRAPGLIB"],
               [], [$EXTRAPGLIB $LIBS])
  AC_CHECK_LIB([M77],     [iand_],     [EXTRAPGLIB="-lM77 $EXTRAPGLIB"],
               [], [$EXTRAPGLIB $LIBS])
  AC_CHECK_LIB([F77],     [f77_init],  [EXTRAPGLIB="-lF77 $EXTRAPGLIB"],
               [], [$EXTRAPGLIB $LIBS])
  
  # Search for X11 includes and libraries.
  AC_PATH_XTRA()
#  X_LIBS_TMP=$X_LIBS
#  X_LIBS=""
#  for LIB in $X_LIBS_TMP; do
##      LIB2=substr($LIB,0,2)
##      if test "$LIB2" != "x-R"; then
#       AC_MSG_NOTICE([lib = $LIB, m4_substr($LIB,0,2)])
#       if test "m4_substr([$LIB],0,2)" != "-R"; then
#      	 X_LIBS="$X_LIBS $LIB"
#      fi
#  done
##  AC_PATH_X
  AC_MSG_NOTICE([X-library search yielded X_CFLAGS=$X_CFLAGS, X_LIBS=$X_LIBS, X_EXTRA_LIBS=$X_EXTRA_LIBS, X_PRE_LIBS=$X_PRE_LIBS])
  # returns C compiler flags needed for X to X_CFLAGS and the X linker flags to X_LIBS
  if test "x$X_LIBS" != x; then
    LDFLAGS="$LDFLAGS $X_LIBS"
    EXTRAPGLIB="$X_LIBS $X_PRE_LIBS -lX11 $EXTRAPGLIB"
  fi

#  # Add pgplot libraries, if they exist to the lib path
#  for LIBDIR in $PGPLOTLIBDIRS ; do
#    AC_CHECK_FILE([$LIBDIR/libcpgplot.a], [PGPLOTLIB="-L$LIBDIR $PGPLOTLIB"; break])
#  done
  
  # It is possible that other libraries may be required depending on what
  # graphics drivers were installed with PGPLOT.
  AC_CHECK_LIB([z],       [deflate],   [EXTRAPGLIB="$EXTRAPGLIB -lz"],
               [], [$EXTRAPGLIB $LIBS])
  AC_CHECK_LIB([png],     [png_error], [EXTRAPGLIB="$EXTRAPGLIB -lpng"], 
  	     [], [$EXTRAPGLIB $LIBS])

  AC_LANG([Fortran 77])
  AC_CHECK_LIB([pgplot],  [pgopen],    [PGPLOTLIB="-lpgplot"],
               [], [$PGPLOTLIB $EXTRAPGLIB])
  AC_LANG(C)
  # Checks for extra libraries needed by PGPLOT -- will add -lg2c -lstdc++ to $LIBS
  AC_CHECK_LIB([g2c], [gerror_])
  AC_CHECK_LIB([stdc++], [main])
  if test "x$F77" = xgfortran; then
    PGPLOTLIB="-lcpgplot $PGPLOTLIB"
    AC_MSG_NOTICE([ Using gfortran, so manually adding -lcpgplot to PGPLOTLIB string.])
  else
    AC_CHECK_LIB([cpgplot], [cpgopen],   [PGPLOTLIB="-lcpgplot $PGPLOTLIB"],
                 [], [$PGPLOTLIB $LIBS $EXTRAPGLIB])
  fi
  
  # If PGPLOT is not present, we give a warning message but still continue. 
  # The compilation is able to work without the PGPLOT-dependent files.
  if test "x$PGPLOTLIB" = x; then
    AC_MSG_NOTICE([
      -------------------------------------------------------
      WARNING! PGPLOT could not be found.
      Compiling Duchamp without graphics capabilities.               
      -------------------------------------------------------])
    #We have no pgplot, so we just link with g++ (or equivalent)
    LINKER=$CXX 
  else
    #We are using pgplot, so link with the fortran compiler.
    LINKER=$F77
    PGPLOTLIB="$PGPLOTLIB $EXTRAPGLIB $LIBS"
    AC_MSG_NOTICE([PGPLOT appears to be available.])
    AC_DEFINE([HAVE_PGPLOT], [1], [Define to 1 if PGPLOT is available.])
    for LIBDIR in $PGPLOTLIBDIRS ; do
     AC_CHECK_FILE([$LIBDIR/libcpgplot.a], [PGPLOTLIB="-L$LIBDIR $PGPLOTLIB"; break])
     AC_CHECK_FILE([$LIBDIR/lib/libcpgplot.a], [PGPLOTLIB="-L$LIBDIR/lib $PGPLOTLIB"; break])
     AC_CHECK_FILE([$LIBDIR/pgplot/libcpgplot.a], [PGPLOTLIB="-L$LIBDIR/pgplot $PGPLOTLIB"; break])
    done
#    if test "x$F77" = xgfortran; then
#      PGPLOTLIB="$PGPLOTLIB -bind_at_load"
#    fi
  fi

fi

AC_SUBST([LINKER])
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
  for LIBDIR in $CFITSIOLIBDIRS ; do
    AC_CHECK_FILE([$LIBDIR], [LDFLAGS="$LDFLAGS -L$LIBDIR"], [continue])
  done
  for INCDIR in $CFITSIOINCDIRS ; do
    AC_CHECK_FILE([$INCDIR], [CPPFLAGS="$CPPFLAGS -I$INCDIR"], [continue])
  done
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

AC_CHECK_LIB([socket],  [recv],   [CFITSIOLIB="$CFITSIOLIB -lsocket"], [], [$LIBS])
AC_CHECK_LIB([cfitsio], [ffopen], [CFITSIOLIB="$CFITSIOLIB -lcfitsio"], [],
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
  AC_MSG_NOTICE([CFITSIO appears to be available.])
  AC_DEFINE([HAVE_CFITSIO], [1], [Define to 1 if CFITSIO is available.])
for LIBDIR in $CFITSIOLIBDIRS ; do
  AC_CHECK_FILE([$LIBDIR/libcfitsio.a], [CFITSIOLIB="-L$LIBDIR $CFITSIOLIB"; break])
  LIBDIR=$LIBDIR/cfitsio
  AC_CHECK_FILE([$LIBDIR/libcfitsio.a], [CFITSIOLIB="-L$LIBDIR $CFITSIOLIB"; break])
done

fi

AC_SUBST([CFITSIOINC])
AC_SUBST([CFITSIOLIB])
##########################################################################

##########################################################################
# Search for WCSLIB.
# use --with-wcslib=directory to specify a particular WCSLIB directory.
# else do the normal searching for libraries.

AC_MSG_NOTICE([  -- WCSLIB -- ])
WCSINCDIR=""
AC_ARG_WITH(wcslib,
	AC_HELP_STRING([--with-wcslib=<dir>],
		       [The WCSLIB library and include files are in directory <dir>.]),
[ #if we specify a directory...
if test "x$withval" = xno; then
  AC_MSG_NOTICE([
    The WCSLIB library is required.
    Ignoring the "--with-wcslib=no" and searching for the library.], [1])
  WCSINCDIRS=$INCDIRS
  WCSLIBDIRS=$LIBDIRS
else
  AC_MSG_NOTICE([Using directory "$withval"])
  WCSINCDIRS="$withval \
              $withval/include"
  WCSLIBDIRS="$withval \
              $withval/lib"
  for LIBDIR in $WCSLIBDIRS ; do
    AC_CHECK_FILE([$LIBDIR], [LDFLAGS="$LDFLAGS -L$LIBDIR"], [continue])
  done
  for INCDIR in $WCSINCDIRS ; do
    AC_CHECK_FILE([$INCDIR/wcslib], 
                  [CPPFLAGS="$CPPFLAGS -I$INCDIR"; WCSINCDIR=$INCDIR; break], 
		  [continue])
  done
fi
],
[ # if directory not specified, go searching.
WCSINCDIRS=$INCDIRS
WCSLIBDIRS=$LIBDIRS
])


if test "x$WCSINCDIR" = x; then
  for INCBASE in $WCSINCDIRS ; do
    AC_CHECK_FILE([$INCBASE/wcslib/wcs.h], [WCSINC="-I$INCBASE -I$INCBASE/wcslib $WCSINC"; break])
  done
else
  AC_CHECK_FILE([$WCSINCDIR/wcslib/wcs.h], [WCSINC="-I$WCSINCDIR -I$WCSINCDIR/wcslib $WCSINC"])
fi


# Look for libwcs.a, and libpgsbox.a
AC_CHECK_LIB([wcs], [wcss2p], [WCSLIB="$WCSLIB -lwcs"], [],
             [$WCSLIB $LIBS $CFITSIOLIB $PGPLOTLIB])

# Only use libpgsbox if we have pgplot enabled - determine this by testing the PGPLOTINCDIRS variable
if test "x$PGPLOTINCDIRS" != x; then 
   if test "x$F77" = xgfortran; then
     WCSLIB="$WCSLIB -lpgsbox"
       AC_MSG_NOTICE([ Using gfortran, so manually adding -lpgsbox to WCSLIB string.])
   else
     AC_CHECK_LIB([pgsbox], [cpgsbox], [WCSLIB="$WCSLIB -lpgsbox"], [],
                 [$WCSLIB $LIBS $CFITSIOLIB $PGPLOTLIB])
   fi
fi

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
  AC_MSG_NOTICE([WCSLIB appears to be available.])
  AC_DEFINE([HAVE_WCSLIB], [1], [Define to 1 if WCSLIB is available.])
# Now add the -L statements to start of WCSLIB.
# Search for libwcs.a, and, if we find it, search for libpgsbox in same directory and
#   then in other directories, keeping the appropriate order of -L calls.

  DIRLIST=". \
           wcs \
           wcslib"

  for LIBBASE in $WCSLIBDIRS ; do
    for DIR in $DIRLIST; do
      if test "x$DIR" = x.; then
       LIBDIR=$LIBBASE
      else
       LIBDIR="$LIBBASE/$DIR"
      fi
      AC_CHECK_FILE([$LIBDIR/libwcs.a], 
             [WCSLIBFRONT="-L$LIBDIR"; 
             AC_CHECK_FILE([$LIBDIR/libpgsbox.a], [], [
               for LIBBASE2 in $WCSLIBDIRS ; do
                for DIR2 in $DIRLIST; do
                 if test "x$DIR" = x.; then
                  LIBDIR=$LIBBASE
                 else
                  LIBDIR="$LIBBASE/$DIR"
                 fi
                 AC_CHECK_FILE([$LIBDIR2/libpgsbox.a],
                               [WCSLIBFRONT="$WCSLIB -L$LIBDIR2"; break])
                done
               done])
              break])
    done
  done
  WCSLIB="$WCSLIBFRONT $WCSLIB"
fi

AC_SUBST([WCSINC])
AC_SUBST([WCSLIB])
##########################################################################

AC_SUBST([LDFLAGS])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
