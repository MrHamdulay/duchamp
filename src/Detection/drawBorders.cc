// -----------------------------------------------------------------------
// detection.cc : Member functions for the Detection class.
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
#include <vector>
#include <cpgplot.h>
#include <duchamp/duchamp.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/PixelMap/Voxel.hh>

using namespace PixelInfo;

namespace duchamp
{
  void Detection::drawBorders(int xoffset, int yoffset)
  {
    ///  @details
    /// For a given object, draw borders around the spatial extent of the object.
    /// \param xoffset The offset from 0 of the x-axis of the plotting window
    /// \param yoffset The offset from 0 of the y-axis of the plotting window

    if(!cpgtest()){
      DUCHAMPERROR("Draw Borders","There is no PGPlot device open.");
    }
    else{

      float x1,x2,y1,y2;
      cpgqwin(&x1,&x2,&y1,&y2);
      int xsize = int(x2 - x1) + 1;
      int ysize = int(y2 - y1) + 1;

      cpgswin(0,xsize-1,0,ysize-1);

      std::vector<std::vector<Voxel> > vertexSets = this->getVertexSet();

      for(size_t n=0;n<vertexSets.size();n++){
	  // for each set of vertices

	  cpgmove(vertexSets[n][0].getX()-xoffset,vertexSets[n][0].getY()-yoffset);
	  for(size_t i=1;i<vertexSets[n].size();i++)
	      cpgdraw(vertexSets[n][i].getX()-xoffset,vertexSets[n][i].getY()-yoffset);

      }

      cpgswin(x1,x2,y1,y2);
  
    }    

  }

}
