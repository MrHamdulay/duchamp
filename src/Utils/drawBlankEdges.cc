#include <cpgplot.h>
#include <duchamp.hh>
#include <param.hh>
#include <Utils/utils.hh>
#include <string>

void drawBlankEdges(float *dataArray, int xdim, int ydim, Param &par)
{
  /** 
   *  drawBlankEdges(float *dataArray, int xdim, int ydim, Param &par)
   * 
   *   A subroutine that is designed to draw (in tasteful purple) the
   *    edges of the blank region of the cube on a moment/detection map.
   *   Uses the same procedure as used in the reconstruction subroutines.
   *   Needs a pgplot device open!
   *   INPUTS: dataArray  -- the array of pixel values
   *           xdim, ydim -- the sizes of the array in the x- and y- directions
   *           par        -- how we know what a BLANK pixel value is.
   */ 

  if(par.getFlagBlankPix()){

//     char *status = new char[10];
//     int statusLength = sizeof(status);
//     cpgqinf("STATE",status,&statusLength);
//     string stat = status;
//     delete [] status;
//     if( stat == "CLOSED" ) 
//       duchampError("drawBlankEdges","There is no PGPlot device open!\n");
//     else{

    if(!cpgtest())
      duchampError("drawBlankEdges","There is no PGPlot device open!\n");
    else{

      float xoff,x2,yoff,y2;
      cpgqwin(&xoff,&x2,&yoff,&y2);

      for(int x=0; x<xdim; x++){// for each column...
	for(int y=1;y<ydim;y++){
	  int current = y*xdim + x;
	  int previous = (y-1)*xdim + x;
	  if(((par.isBlank(dataArray[current]))&&!par.isBlank(dataArray[previous])) || 
	     (!par.isBlank(dataArray[current])&&par.isBlank(dataArray[previous]))){
	    cpgmove(x+xoff,   y+yoff);
	    cpgdraw(x+xoff+1, y+yoff);
	  }
	}
      }
      
      for(int y=0; y<ydim; y++){// for each row...
	for(int x=1;x<xdim;x++){
	  int current = y*xdim + x;
	  int previous = y*xdim + x-1;
	  if(((par.isBlank(dataArray[current]))&&!par.isBlank(dataArray[previous])) || 
	     (!par.isBlank(dataArray[current])&&par.isBlank(dataArray[previous]))){
	    cpgmove(x+xoff, y+yoff);
	    cpgdraw(x+xoff, y+yoff+1);
	  }
	}
      }
      
    }

  
  }

}
