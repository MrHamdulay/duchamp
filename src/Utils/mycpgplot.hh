#ifndef MYCPGPLOT_H
#define MYCPGPLOT_H
#include <Utils/utils.hh>
#include <string>

/**
 * A namespace that holds definitions and basic functions to aid the
 * use of PGPLOT.
 */

namespace mycpgplot
{
  /** Enumerated PGPLOT colour names. */
  enum COLOUR {BACKGND=0, FOREGND, RED, GREEN, BLUE, CYAN, 
	       MAGENTA, YELLOW, ORANGE, GREENYELLOW, GREENCYAN, BLUECYAN, 
	       BLUEMAGENTA, REDMAGENTA, DARKGREY, LIGHTGREY, DARKGREEN};

  /** Enumerated colour names for the inverse case, such as for /xs devices.*/
  enum INVERSE {XS_BLACK, XS_WHITE};

  /** 
   * Define the DARKGREEN colour, with RGB value of (0,0.7,0).
   */
  inline void setDarkGreen(){cpgscr(DARKGREEN,0.,0.7,0.);};

  /**
   * A device independent way to set the colour to white.
   *
   * Uses cpgIsPS() to determine whether a device is a postscript
   * one. If so, we use the BACKGND colour (ie. 0), otherwise use
   * FOREGND (1).
   */
  inline void setWhite(){
    if(cpgIsPS()) cpgsci(BACKGND);
    else cpgsci(FOREGND);
  };

  /**
   * A device independent way to set the colour to black.
   *
   * Uses cpgIsPS() to determine whether a device is a postscript
   * one. If so, we use the FOREGND colour (ie. 1), otherwise use
   * BACKGND (0).
   */
  inline void setBlack(){
    if(cpgIsPS()) cpgsci(FOREGND);
    else cpgsci(BACKGND);
  };

  /** Enumerated PGPLOT line styles. */
  enum LINESTYLE {FULL=1, DASHED, DASHDOT, DOTTED, DASHDOT3};

  /** Enumerated PGPLOT area fill styles. */
  enum FILLSTYLE {SOLID=1, OUTLINE, HATCHED, CROSSHATCHED};

  /** Enumerated PGPLOT plotting symbols. */
  enum SYMBOLS {SOLIDPENT=-5, SOLIDDIAMOND, DOT=1, PLUS, ASTERISK, 
		CIRCLE, CROSS, SQUARE, TRIANGLE, OPLUS, ODOT, FOURSTAR,
		DIAMOND, STAR, SOLIDTRIANGLE, OPENPLUS, DAVID, 
		SOLIDSQUARE, SOLIDCIRCLE, SOLIDSTAR, LARGESQUARE,
		CIRCLE1, CIRCLE2, CIRCLE3, CIRCLE4, CIRCLE5, CIRCLE6,
		CIRCLE7, CIRCLE8, CIRCLE9, LEFT, RIGHT, UP, DOWN};

  const std::string degrees="\\(0718)";   ///< The degrees symbol: \f$^\circ\f$
  const std::string plusminus="\\(2233)"; ///< The plus-minus symbol: \f$\pm\f$
  const std::string times="\\(2235)";     ///< Similar to \f$\times\f$
  const std::string tick="\\(2267)";      ///< A tick, which is good for square roots: similar to \f$\surd\f$
  const std::string odot="\\(2281)";      ///< \f$\odot\f$
  const std::string integral="\\(2268)";  ///< The integral symbol \f$\int\f$

}


#endif
