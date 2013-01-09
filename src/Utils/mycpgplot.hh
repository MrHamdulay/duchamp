// -----------------------------------------------------------------------
// mycpgplot.hh: Enumerations, shortcuts and other useful functions
//               for use with PGPLOT.
// -----------------------------------------------------------------------
// Copyright (C) 2006, Matthew Whiting, ATNF
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// Duchamp is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with Duchamp; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
//
// Correspondence concerning Duchamp may be directed to:
//    Internet email: Matthew.Whiting [at] atnf.csiro.au
//    Postal address: Dr. Matthew Whiting
//                    Australia Telescope National Facility, CSIRO
//                    PO Box 76
//                    Epping NSW 1710
//                    AUSTRALIA
// -----------------------------------------------------------------------
#ifndef MYCPGPLOT_H
#define MYCPGPLOT_H
#include <string>

#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef HAVE_PGPLOT
#include <duchamp/config.h>

/// A namespace that holds definitions and basic functions to aid the
/// use of PGPLOT.

namespace mycpgplot
{
  /// @brief Enumerated PGPLOT colour names. 
  enum COLOUR {BACKGND=0, FOREGND, RED, GREEN, BLUE, CYAN, 
	       MAGENTA, YELLOW, ORANGE, GREENYELLOW, GREENCYAN, BLUECYAN, 
	       BLUEMAGENTA, REDMAGENTA, DARKGREY, LIGHTGREY, DARKGREEN,
	       WCSGREEN};

  /// @brief Enumerated colour names for the inverse case, such as for /xs devices.
  enum INVERSE {XS_BLACK, XS_WHITE};

  /// @brief Enumerated PGPLOT line styles. 
  enum LINESTYLE {FULL=1, DASHED, DASHDOT, DOTTED, DASHDOT3};

  /// @brief Enumerated PGPLOT area fill styles. 
  enum FILLSTYLE {SOLID=1, OUTLINE, HATCHED, CROSSHATCHED};

  /// @brief Enumerated PGPLOT plotting symbols. 
  enum SYMBOLS {SOLIDPENT=-5, SOLIDDIAMOND, DOT=1, PLUS, ASTERISK, 
		CIRCLE, CROSS, SQUARE, TRIANGLE, OPLUS, ODOT, FOURSTAR,
		DIAMOND, STAR, SOLIDTRIANGLE, OPENPLUS, DAVID, 
		SOLIDSQUARE, SOLIDCIRCLE, SOLIDSTAR, LARGESQUARE,
		CIRCLE1, CIRCLE2, CIRCLE3, CIRCLE4, CIRCLE5, CIRCLE6,
		CIRCLE7, CIRCLE8, LEFT, RIGHT, UP, DOWN};

  enum FONTS {NORMAL=1, ROMAN, ITALIC, SCRIPT};

  const std::string degrees="\\(0718)";   ///< The degrees symbol: \f$^\circ\f$
  const std::string plusminus="\\(2233)"; ///< The plus-minus symbol: \f$\pm\f$
  const std::string times="\\(2235)";     ///< Similar to \f$\times\f$
  const std::string tick="\\(2267)";      ///< A tick, which is good for square roots: similar to \f$\surd\f$
  const std::string odot="\\(2281)";      ///< \f$\odot\f$
  const std::string integral="\\(2268)";  ///< The integral symbol \f$\int\f$

}

//////////////////////

#ifdef HAVE_PGPLOT

#include <cpgplot.h>

// The following are in pgplot_related.c
//
/// @brief Is a PGPLOT device open? 
extern "C" int  cpgtest();

/// @brief Do a logarithmic-scaled wedge, as in PGWEDG 
extern "C" void cpgwedglog(const char* side, float disp, float width, 
			   float fg, float bg, const char *label);

/// @brief Do CPGHIST but with the y-axis logarithmic 
extern "C" void cpghistlog(int npts, float *data, float datamin, 
			   float datamax, int nbin, int pgflag);

/// @brief Do a PGPLOT cumulative distribution 
extern "C" void cpgcumul(int npts, float *data, float datamin, 
			 float datamax, int pgflag);

extern "C" void cpgellipse(float x0, float y0, float maj, float min, float pa);

namespace mycpgplot {

  /// @brief Is a PGPLOT device a postscript (hardcopy) device?  
  int  cpgIsPS(); 

  /// @brief A front-end to the cpgopen function, with other necessary definitions.
  int mycpgopen(std::string device);

  /// @brief Define the DARKGREEN colour, with RGB value of (0,0.7,0).
  void setDarkGreen();

  /// @brief Define the WCSGREEN colour, with RGB value of (0.3,0.5,0.3). 
  void setWCSGreen();

  /// @brief A device-independent way to set the colour to white. 
  void setWhite();

  /// @brief A device-independent way to set the colour to black. 
   void setBlack();

}

#else // from ifdef HAVE_PGPLOT
// PGPLOT has not been defined.
// extern "C" int  cpgtest(){return 0;};
// extern "C" int  cpgIsPS(){return 0;}; 
// extern "C" void cpgwedglog(const char* side, float disp, float width, 
// 			   float fg, float bg, const char *label){ };
// extern "C" void cpghistlog(int npts, float *data, float datamin, 
// 			   float datamax, int nbin, int pgflag){ };
// extern "C" void cpgcumul(int npts, float *data, float datamin, 
// 			 float datamax, int pgflag){ };
// namespace mycpgplot {
//   int mycpgopen(std::string device){return 0;};
//   inline void setDarkGreen(){ };
//   inline void setWCSGreen(){ };
//   inline void setWhite(){ };
//   inline void setBlack(){ };
// }

#endif // from ifdef HAVE_PGPLOT

#endif
