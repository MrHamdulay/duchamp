// -----------------------------------------------------------------------
// drawBlankEdges.cc: Draw the edge of the BLANK region of the cube on
//                    a PGPLOT map.
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
#include <cpgplot.h>
#include <duchamp/Cubes/cubeUtils.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/Utils/mycpgplot.hh>
#include <string>

namespace duchamp
{

  void drawBlankEdges(float *dataArray, size_t xdim, size_t ydim, Param &par)
  {
    ///  @details
    ///  A subroutine that is designed to draw the edges of the blank
    ///  region of the cube on a moment/detection map. Uses the same
    ///  procedure as used in the reconstruction subroutines.
    /// 
    ///  Note that it needs a PGPLOT device open. The colour used is the current
    ///   PGPLOT colour.
    /// 
    ///  \param dataArray The array of pixel values
    ///  \param xdim      The size of the array in the x-direction.
    ///  \param ydim      The size of the array in the y-direction.
    ///  \param par       The Param set telling us what a BLANK pixel is.

    if(par.getFlagBlankPix()){

      if(!cpgtest()){
	DUCHAMPERROR("Draw Blank Edges","There is no PGPlot device open.");
      }
      else{

	int colour;
	cpgqci(&colour);
	cpgsci(DUCHAMP_BLANK_EDGE_COLOUR);

	float xoff,x2,yoff,y2;
	cpgqwin(&xoff,&x2,&yoff,&y2);

	size_t size=xdim*ydim;
	std::vector<bool> blank = par.makeBlankMask(dataArray,size);

	for(size_t x=0; x<xdim; x++){// for each column...
	  for(size_t y=1;y<ydim;y++){
	    size_t current = y*xdim + x;
	    size_t previous = (y-1)*xdim + x;
	    if( (blank[current]&&!blank[previous]) || 
		(!blank[current]&&blank[previous])   ){
	      cpgmove(x-0.5, y-0.5);
	      cpgdraw(x+0.5, y-0.5);
	    }
	  }
	}
      
	for(size_t y=0; y<ydim; y++){// for each row...
	  for(size_t x=1;x<xdim;x++){
	    size_t current = y*xdim + x;
	    size_t previous = y*xdim + x-1;
	    if( (blank[current]&&!blank[previous]) || 
		(!blank[current]&&blank[previous])   ){
	      cpgmove(x-0.5, y-0.5);
	      cpgdraw(x-0.5, y+0.5);
	    }
	  }
	}

     
	cpgsci(colour);
 
      }

  
    }

  }

}
