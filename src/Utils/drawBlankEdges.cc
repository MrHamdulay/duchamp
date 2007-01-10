#include <cpgplot.h>
#include <duchamp.hh>
#include <param.hh>
#include <Utils/utils.hh>
#include <string>

void drawBlankEdges(float *dataArray, int xdim, int ydim, Param &par)
{
  /** Draw the edge of the BLANK region on a map.
   *     
   *   A subroutine that is designed to draw the
   *    edges of the blank region of the cube on a moment/detection map.
   *   Uses the same procedure as used in the reconstruction subroutines.
   *
   *   Note that it needs a PGPLOT device open. The colour used is the current
   *    PGPLOT colour.
   *
   *   \param dataArray The array of pixel values
   *   \param xdim      The size of the array in the x-direction.
   *   \param ydim      The size of the array in the y-direction.
   *   \param par       The Param set telling us what a BLANK pixel is.
   */ 

  if(par.getFlagBlankPix()){

    if(!cpgtest())
      duchampError("drawBlankEdges","There is no PGPlot device open!\n");
    else{

      float xoff,x2,yoff,y2;
      cpgqwin(&xoff,&x2,&yoff,&y2);

      bool *blank = new bool[xdim*ydim];
      for(int i=0;i<xdim*ydim;i++) blank[i] = par.isBlank(dataArray[i]);

      for(int x=0; x<xdim; x++){// for each column...
	for(int y=1;y<ydim;y++){
	  int current = y*xdim + x;
	  int previous = (y-1)*xdim + x;
	  if( (blank[current]&&!blank[previous]) || 
	      (!blank[current]&&blank[previous])   ){
	    cpgmove(x-0.5, y-0.5);
	    cpgdraw(x+0.5, y-0.5);
	  }
	}
      }
      
      for(int y=0; y<ydim; y++){// for each row...
	for(int x=1;x<xdim;x++){
	  int current = y*xdim + x;
	  int previous = y*xdim + x-1;
	  if( (blank[current]&&!blank[previous]) || 
	      (!blank[current]&&blank[previous])   ){
	    cpgmove(x-0.5, y-0.5);
	    cpgdraw(x-0.5, y+0.5);
	  }
	}
      }

      delete [] blank;
      
    }

  
  }

}
