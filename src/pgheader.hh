#ifndef PGHEADER_HH
#define PGHEADER_HH

#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#include <duchamp/config.h>

#ifdef  HAVE_PGPLOT
#include <cpgplot.h>

const bool USE_PGPLOT=true;

#else

const bool USE_PGPLOT=false;

#endif // matches #ifdef HAVE_PGPLOT

#endif // matches #ifndef PGHEADER_HH
