#ifndef PGHEADER_HH
#define PGHEADER_HH

#ifdef  HAVE_PGPLOT
#include <cpgplot.h>

const bool USE_PGPLOT=true;

void endPGPLOT(){cpgend();};

#else

const bool USE_PGPLOT=false;
void endPGPLOT(){};

#endif // matches #ifdef HAVE_PGPLOT

#endif // matches #ifndef PGHEADER_HH
