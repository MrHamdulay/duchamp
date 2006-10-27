#ifndef MYCPGPLOT_H
#define MYCPGPLOT_H
#include <string>
namespace mycpgplot
{

  enum COLOUR {WHITE=0, BLACK, RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW,
	       ORANGE, GREENYELLOW, GREENCYAN, BLUECYAN, BLUEMAGENTA,
	       REDMAGENTA, DARKGREY, LIGHTGREY, DARKGREEN};

  enum INVERSE {XS_BLACK, XS_WHITE};

  inline void setDarkGreen(){cpgscr(DARKGREEN,0.,0.7,0.);};

  enum LINESTYLE {FULL=1, DASHED, DASHDOT, DOTTED, DASHDOT3};

  enum FILLSTYLE {SOLID=1, OUTLINE, HATCHED, CROSSHATCHED};

  enum SYMBOLS {SOLIDPENT=-5, SOLIDDIAMOND, DOT=1, PLUS, ASTERISK, 
		CIRCLE, CROSS, SQUARE, TRIANGLE, OPLUS, ODOT, FOURSTAR,
		DIAMOND, STAR, SOLIDTRIANGLE, OPENPLUS, DAVID, 
		SOLIDSQUARE, SOLIDCIRCLE, SOLIDSTAR, LARGESQUARE,
		CIRCLE1, CIRCLE2, CIRCLE3, CIRCLE4, CIRCLE5, CIRCLE6,
		CIRCLE7, CIRCLE8, CIRCLE9, LEFT, RIGHT, UP, DOWN};

  const std::string degrees="\\(0718)";
  const std::string plusminus="\\(2233)";
  const std::string times="\\(2235)";
  const std::string tick="\\(2267)";
  const std::string odot="\\(2281)";
  const std::string integral="\\(2268)";

}


#endif
