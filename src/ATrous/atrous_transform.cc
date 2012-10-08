// -----------------------------------------------------------------------
// atrous_transform.cc: Simplified a trous transform functions - only used
//                      for testing purposes.
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
#include <iostream>
#include <math.h>
#include <duchamp/param.hh>
#include <duchamp/ATrous/atrous.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Devel/devel.hh>

namespace duchamp{

  /***********************************************************************/
  /////  1-DIMENSIONAL TRANSFORM
  /***********************************************************************/

  // template <class T>
  // void atrousTransform(long &length, T *&spectrum, T *&coeffs, T *&wavelet)
  // {
  //   int filterHW = filterwidth/2;
  //   int numScales = getNumScales(length);

  //   delete [] coeffs;
  //   coeffs = new T[(numScales+1)*length];
  //   delete [] wavelet;
  //   wavelet = new T[(numScales+1)*length];
  //   for(int i=0;i<length;i++)  coeffs[i] = wavelet[i] = spectrum[i];

  //   int spacing = 1;
  //   for(int scale = 0; scale<numScales; scale++){

  //     for(int pos = 0; pos<length; pos++){
  //       coeffs[(scale+1)*length+pos] = 0;
  //       for(int offset=-filterHW; offset<=filterHW; offset++){
  // 	int x = pos + spacing*offset;
  // 	if(x<0) x = -x;                    // boundary conditions are 
  // 	if(x>=length) x = 2*(length-1) - x;    //    reflection.
  // // 	if(x<0) x = x+length;                // boundary conditions are 
  // // 	if(x>=length) x = x-length;            //    continuous.

  // 	coeffs[(scale+1)*length+pos] += filter1D[offset+filterHW] * 
  // 	  coeffs[scale*length+x];
  //       }
  //       wavelet[(scale+1)*length+pos] = coeffs[scale*length+pos] - 
  // 	coeffs[(scale+1)*length+pos];
  //     }

  //     spacing *= 2;

  //   }

  // }

  
  void atrousTransform(size_t &length, int &numScales, float *spectrum, double *coeffs, double *wavelet, duchamp::Param &par)
  {
    duchamp::Filter reconFilter = par.filter();
    int filterHW = reconFilter.width()/2;

    for(int i=0;i<length;i++)  coeffs[i] = wavelet[i] = spectrum[i];

    int spacing = 1;
    for(int scale = 0; scale<numScales; scale++){

      for(int pos = 0; pos<length; pos++){
	coeffs[(scale+1)*length+pos] = 0;
	for(int offset=-filterHW; offset<=filterHW; offset++){
	  int x = pos + spacing*offset;
	  if(x<0) x = -x;                    // boundary conditions are 
	  if(x>=length) x = 2*(length-1) - x;    //    reflection.
	  // 	if(x<0) x = x+length;                // boundary conditions are 
	  // 	if(x>=length) x = x-length;            //    continuous.

	  // 	coeffs[(scale+1)*length+pos] += filter1D[offset+filterHW] * 
	  // 	  coeffs[scale*length+x];
	  coeffs[(scale+1)*length+pos] += reconFilter.coeff(offset+filterHW) * 
	    coeffs[scale*length+x];
	}
	wavelet[(scale+1)*length+pos] = coeffs[scale*length+pos] - 
	  coeffs[(scale+1)*length+pos];
      }

      spacing *= 2;

    }

  }

  void atrousTransform(size_t &length, float *spectrum, float *coeffs, float *wavelet, duchamp::Param &par)
  {
    duchamp::Filter reconFilter = par.filter();
    int filterHW = reconFilter.width()/2;
    int numScales = reconFilter.getNumScales(length);

    delete [] coeffs;
    coeffs = new float[(numScales+1)*length];
    delete [] wavelet;
    wavelet = new float[(numScales+1)*length];
    for(int i=0;i<length;i++) coeffs[i] = wavelet[i] = spectrum[i];

    int spacing = 1;
    for(int scale = 0; scale<numScales; scale++){

      for(int pos = 0; pos<length; pos++){
	coeffs[(scale+1)*length+pos] = 0;
	for(int offset=-filterHW; offset<=filterHW; offset++){
	  int x = pos + spacing*offset;
	  if(x<0) x = -x;                    // boundary conditions are 
	  if(x>=length) x = 2*(length-1) - x;    //    reflection.
	  // 	if(x<0) x = x+length;                // boundary conditions are 
	  // 	if(x>=length) x = x-length;            //    continuous.

	  // 	coeffs[(scale+1)*length+pos] += filter1D[offset+filterHW] * 
	  // 	  coeffs[scale*length+x];
	  coeffs[(scale+1)*length+pos] += reconFilter.coeff(offset+filterHW) * 
	    coeffs[scale*length+x];
	}
	wavelet[(scale+1)*length+pos] = coeffs[scale*length+pos] - 
	  coeffs[(scale+1)*length+pos];
      }

      spacing *= 2;

    }

  }




  /***********************************************************************/
  /////  2-DIMENSIONAL TRANSFORM
  /***********************************************************************/

  void atrousTransform2D(size_t &xdim, size_t &ydim, int &numScales, float *input, double *coeffs, double *wavelet, duchamp::Param &par)
  {
    duchamp::Filter reconFilter = par.filter();
    float blankPixValue = par.getBlankPixVal();
    int filterHW = reconFilter.width()/2;

    double *filter = new double[reconFilter.width()*reconFilter.width()];
    for(int i=0;i<reconFilter.width();i++){
      for(int j=0;j<reconFilter.width();j++){
	filter[i*reconFilter.width()+j] = reconFilter.coeff(i) * reconFilter.coeff(j);
      }
    }

    size_t size = xdim * ydim;
    float *oldcoeffs = new float[size];

    // locating the borders of the image -- ignoring BLANK pixels
    //   int xLim1=0, yLim1=0, xLim2=xdim-1, yLim2=ydim-1;
    //   for(int row=0;row<ydim;row++){
    //     while((xLim1<xLim2)&&(input[row*xdim+xLim1]==blankPixValue)) xLim1++;
    //     while((xLim2>xLim1)&&(input[row*xdim+xLim1]==blankPixValue)) xLim2--;
    //   }
    //   for(int col=0;col<xdim;col++){
    //     while((yLim1<yLim2)&&(input[col+xdim*yLim1]==blankPixValue)) yLim1++;
    //     while((yLim2>yLim1)&&(input[col+xdim*yLim1]==blankPixValue)) yLim2--;
    //   }
    //   std::cerr << "X Limits: "<<xLim1<<" "<<xLim2<<std::endl;
    //   std::cerr << "Y Limits: "<<yLim1<<" "<<yLim2<<std::endl;
  
    int *xLim1 = new int[ydim];
    int *yLim1 = new int[xdim];
    int *xLim2 = new int[ydim];
    int *yLim2 = new int[xdim];
    for(int row=0;row<ydim;row++){
      int ct1 = 0;
      int ct2 = xdim - 1;
      while((ct1<ct2)&&(input[row*xdim+ct1]==blankPixValue) ) ct1++;
      while((ct2>ct1)&&(input[row*xdim+ct2]==blankPixValue) ) ct2--;
      xLim1[row] = ct1;
      xLim2[row] = ct2;
      std::cerr<<row<<":"<<xLim1[row]<<","<<xLim2[row]<<" ";
    }
    std::cerr<<std::endl;

    for(int col=0;col<xdim;col++){
      int ct1=0;
      int ct2=ydim-1;
      while((ct1<ct2)&&(input[col+xdim*ct1]==blankPixValue) ) ct1++;
      while((ct2>ct1)&&(input[col+xdim*ct2]==blankPixValue) ) ct2--;
      yLim1[col] = ct1;
      yLim2[col] = ct2;
    }


    bool *isGood = new bool[size];
    for(int pos=0;pos<size;pos++) //isGood[pos] = (!flagBlank) || (input[pos]!=blankPixValue);
      isGood[pos] = !par.isBlank(input[pos]);

    for(int i=0;i<size;i++)  coeffs[i] = wavelet[i] = input[i];

    int spacing = 1;
    for(int scale = 0; scale<numScales; scale++){

      for(int i=0;i<size;i++) oldcoeffs[i] = coeffs[i];

      //std::cerr << numScales<<" "<<scale<<" "<<spacing<<" "<<reconFilter.width()*spacing<<std::endl;
      for(int ypos = 0; ypos<ydim; ypos++){
	for(int xpos = 0; xpos<xdim; xpos++){
	  // loops over each pixel in the image
	  int pos = ypos*xdim + xpos;
	  coeffs[pos] = 0;

	  // 	if((par.getFlagBlankPix())&&(oldcoeffs[pos] == blankPixValue) )
	  if(par.isBlank(oldcoeffs[pos]) )
	    coeffs[pos] = oldcoeffs[pos];
	  else{
	    for(int yoffset=-filterHW; yoffset<=filterHW; yoffset++){
	      int y = ypos + spacing*yoffset;
	      int newy;
	      // 	  if(y<0) y = -y;                 // boundary conditions are 
	      // 	  if(y>=ydim) y = 2*(ydim-1) - y; //    reflection.
	      // 	  while((y<yLim1)||(y>yLim2)){
	      // 	    if(y<yLim1) y = 2*yLim1 - y;      // boundary conditions are 
	      // 	    if(y>yLim2) y = 2*yLim2 - y;      //    reflection.
	      // 	  }
	      // boundary conditions are reflection.
	  
	      for(int xoffset=-filterHW; xoffset<=filterHW; xoffset++){
		int x = xpos + spacing*xoffset;
		int newx;
		//if(x<0) x = -x;                 // boundary conditions are 
		// if(x>=xdim) x = 2*(xdim-1) - x; //    reflection.
		//while((x<xLim1)||(x>xLim2)){
		// 	      if(x<xLim1) x = 2*xLim1 - x;      // boundary conditions are 
		// 	      if(x>xLim2) x = 2*xLim2 - x;      //    reflection.
		// 	    }
		// boundary conditions are reflection.
		if(y<yLim1[xpos]) newy = 2*yLim1[xpos] - y;      
		else if(y>yLim2[xpos]) newy = 2*yLim2[xpos] - y;      
		else newy = y;
		if(x<xLim1[ypos]) newx = 2*xLim1[ypos] - x;
		else if(x>xLim2[ypos]) newx = 2*xLim2[ypos] - x;      
		else newx=x;      
	      
		x = newx;
		y = newy;

		int filterpos = (yoffset+filterHW)*reconFilter.width() + (xoffset+filterHW);
		int oldpos = y*xdim + x;

		if(// (x>=0)&&(x<xdim)&&(y>=0)&&(y<ydim)&&
		   (isGood[pos]))
		  coeffs[pos] += filter[filterpos] * oldcoeffs[oldpos];
	      }
	    }
      
	  }
	  wavelet[(scale+1)*size+pos] = oldcoeffs[pos] - coeffs[pos];


	}
      }
 
      spacing *= 2;

    }

    delete [] filter;
    delete [] oldcoeffs;

  }

  // void atrousTransform2D(long &xdim, long &ydim, int &numScales, float *input, double *coeffs, double *wavelet)
  // {
  //   Filter reconFilter = par.filter();
  //   int filterHW = reconFilter.width()/2;

  //   double *filter = new double[reconFilter.width()*reconFilter.width()];
  //   for(int i=0;i<reconFilter.width();i++){
  //     for(int j=0;j<reconFilter.width();j++){
  //       filter[i*reconFilter.width()+j] = reconFilter.coeff(i) * reconFilter.coeff(j);
  //     }
  //   }

  //   long size = xdim * ydim;
  //   float *oldcoeffs = new float[size];

  //   for(int i=0;i<size;i++)  coeffs[i] = wavelet[i] = input[i];

  
  //   int spacing = 1;
  //   for(int scale = 0; scale<numScales; scale++){

  //     for(int i=0;i<size;i++) oldcoeffs[i] = coeffs[i];

  //     std::cerr << numScales<<" "<<scale<<" "<<spacing<<std::endl;
  //     for(int ypos = 0; ypos<ydim; ypos++){
  //       for(int xpos = 0; xpos<xdim; xpos++){
  // 	// loops over each pixel in the image
  // 	int pos = ypos*xdim + xpos;
  // 	coeffs[pos] = 0;

  // 	for(int yoffset=-filterHW; yoffset<=filterHW; yoffset++){
  // 	  int y = ypos + spacing*yoffset;
  // 	  if(y<0) y = -y;                 // boundary conditions are 
  // 	  if(y>=ydim) y = 2*(ydim-1) - y; //    reflection.
	  
  // 	  for(int xoffset=-filterHW; xoffset<=filterHW; xoffset++){
  // 	    int x = xpos + spacing*xoffset;
  // 	    if(x<0) x = -x;                 // boundary conditions are 
  // 	    if(x>=xdim) x = 2*(xdim-1) - x; //    reflection.

  // 	    int filterpos = (yoffset+filterHW)*reconFilter.width() + (xoffset+filterHW);
  // 	    int oldpos = y*xdim + x;
  // 	    coeffs[pos] += filter[filterpos] * oldcoeffs[oldpos];
  // 	  }
  // 	}
       
  // 	wavelet[(scale+1)*size+pos] = oldcoeffs[pos] - coeffs[pos];

  //       }
  //     }
 
  //     spacing *= 2;

  //   }

  //   delete [] filter;
  //   delete [] oldcoeffs;

  // }

  /***********************************************************************/
  /////  3-DIMENSIONAL TRANSFORM
  /***********************************************************************/

  void atrousTransform3D(size_t &xdim, size_t &ydim, size_t &zdim, int &numScales, float *&input, float *&coeffs, float *&wavelet, duchamp::Param &par)
  {
    duchamp::Filter reconFilter = par.filter();
    float blankPixValue = par.getBlankPixVal();
    int filterHW = reconFilter.width()/2;

    size_t size = xdim * ydim * zdim;
    float *oldcoeffs = new float[size];

    std::cerr << "%";
    int fsize = reconFilter.width()*reconFilter.width()*reconFilter.width();
    std::cerr << "%";
    double *filter = new double[fsize];
    for(int i=0;i<reconFilter.width();i++){
      for(int j=0;j<reconFilter.width();j++){
	for(int k=0;k<reconFilter.width();k++){
	  filter[i +j*reconFilter.width() + k*reconFilter.width()*reconFilter.width()] = 
	    reconFilter.coeff(i) * reconFilter.coeff(j) * reconFilter.coeff(k);
	}
      }
    }
    // locating the borders of the image -- ignoring BLANK pixels
    // HAVE NOT DONE THIS FOR Z --> ASSUMING NO TRIMMING IN SPECTRAL DIRECTION
    int xLim1 = 0, yLim1 = 0, xLim2 = xdim-1, yLim2 = ydim-1;
    for(int col=0;col<xdim;col++){
      while((yLim1<yLim2)&&(input[col+xdim*yLim1]==blankPixValue) ) yLim1++;
      while((yLim2>yLim1)&&(input[col+xdim*yLim1]==blankPixValue) ) yLim2--;
    }
    for(int row=0;row<ydim;row++){
      while((xLim1<xLim2)&&(input[row*xdim+xLim1]==blankPixValue) ) xLim1++;
      while((xLim2>xLim1)&&(input[row*xdim+xLim1]==blankPixValue) ) xLim2--;
    }

    for(int i=0;i<size;i++)  coeffs[i] = wavelet[i] = input[i];

    int spacing = 1;
    std::cerr<<xdim<<"x"<<ydim<<"x"<<zdim<<"x"<<numScales;
    for(int scale = 0; scale<numScales; scale++){
      std::cerr << ".";
      for(int i=0;i<size;i++) oldcoeffs[i] = coeffs[i];

      for(int zpos = 0; zpos<zdim; zpos++){
	for(int ypos = 0; ypos<ydim; ypos++){
	  for(int xpos = 0; xpos<xdim; xpos++){
	    // loops over each pixel in the image
	    int pos = zpos*xdim*ydim + ypos*xdim + xpos;
	    coeffs[pos] = 0;

	    if(par.isBlank(oldcoeffs[pos]) )
	      coeffs[pos] = oldcoeffs[pos];
	    else{
	      for(int zoffset=-filterHW; zoffset<=filterHW; zoffset++){
		int z = zpos + spacing*zoffset;
		if(z<0) z = -z;                 // boundary conditions are 
		if(z>=zdim) z = 2*(zdim-1) - z; //    reflection.
	  
		for(int yoffset=-filterHW; yoffset<=filterHW; yoffset++){
		  int y = ypos + spacing*yoffset;
		  //if(y<0) y = -y;                 // boundary conditions are 
		  // if(y>=ydim) y = 2*(ydim-1) - y; //    reflection.
		  if(y<yLim1) y = 2*yLim1 - y;      // boundary conditions are 
		  if(y>yLim2) y = 2*yLim2 - y;      //    reflection.
	  
		  for(int xoffset=-filterHW; xoffset<=filterHW; xoffset++){
		    int x = xpos + spacing*xoffset;
		    //if(x<0) x = -x;                 // boundary conditions are 
		    //if(x>=xdim) x = 2*(xdim-1) - x; //    reflection.
		    if(x<xLim1) x = 2*xLim1 - x;      // boundary conditions are 
		    if(x>xLim2) x = 2*xLim2 - x;      //    reflection.

		    int filterpos = (zoffset+filterHW)*reconFilter.width()*reconFilter.width() + 
		      (yoffset+filterHW)*reconFilter.width() + (xoffset+filterHW);
		    int oldpos = z*xdim*ydim + y*xdim + x;

		    if(!par.isBlank(oldcoeffs[oldpos]))
		      coeffs[pos] += filter[filterpos] * oldcoeffs[oldpos];

		  }
		}
	      }
	    }		
 
	    wavelet[(scale+1)*size+pos] = oldcoeffs[pos] - coeffs[pos];

	  }
	}
      }
 
      spacing *= 2;

    }
    std::cerr << "|";

    delete [] filter;
    delete [] oldcoeffs;

  }


  // void atrousTransform3D(long &xdim, long &ydim, long &zdim, int &numScales, float *input, float *coeffs, float *wavelet)
  // {
  //   extern Filter reconFilter;
  //   int filterHW = reconFilter.width()/2;

  //   long size = xdim * ydim * zdim;
  //   float *oldcoeffs = new float[size];

  //   std::cerr << "%";
  //   int fsize = reconFilter.width()*reconFilter.width()*reconFilter.width();
  //   std::cerr << "%";
  //   double *filter = new double[fsize];
  //   for(int i=0;i<reconFilter.width();i++){
  //     for(int j=0;j<reconFilter.width();j++){
  //       for(int k=0;k<reconFilter.width();k++){
  //       filter[i +j*reconFilter.width() + k*reconFilter.width()*reconFilter.width()] = 
  // 	reconFilter.coeff(i) * reconFilter.coeff(j) * reconFilter.coeff(k);
  //       }
  //     }
  //   }

  //   for(int i=0;i<size;i++)  coeffs[i] = wavelet[i] = input[i];

  //   int spacing = 1;
  //   std::cerr<<xdim<<"x"<<ydim<<"x"<<zdim<<"x"<<numScales;
  //   for(int scale = 0; scale<numScales; scale++){
  //     std::cerr << ".";
  //     for(int i=0;i<size;i++) oldcoeffs[i] = coeffs[i];

  //     for(int zpos = 0; zpos<zdim; zpos++){
  //       for(int ypos = 0; ypos<ydim; ypos++){
  // 	for(int xpos = 0; xpos<xdim; xpos++){
  // 	  // loops over each pixel in the image
  // 	  int pos = zpos*xdim*ydim + ypos*xdim + xpos;
  // 	  coeffs[pos] = 0;

  // 	  for(int zoffset=-filterHW; zoffset<=filterHW; zoffset++){
  // 	    int z = zpos + spacing*zoffset;
  // 	    if(z<0) z = -z;                 // boundary conditions are 
  // 	    if(z>=zdim) z = 2*(zdim-1) - z; //    reflection.
	  
  // 	    for(int yoffset=-filterHW; yoffset<=filterHW; yoffset++){
  // 	      int y = ypos + spacing*yoffset;
  // 	      if(y<0) y = -y;                 // boundary conditions are 
  // 	      if(y>=ydim) y = 2*(ydim-1) - y; //    reflection.
	  
  // 	      for(int xoffset=-filterHW; xoffset<=filterHW; xoffset++){
  // 		int x = xpos + spacing*xoffset;
  // 		if(x<0) x = -x;                 // boundary conditions are 
  // 		if(x>=xdim) x = 2*(xdim-1) - x; //    reflection.

  // 		int filterpos = (zoffset+filterHW)*reconFilter.width()*reconFilter.width() + 
  // 		  (yoffset+filterHW)*reconFilter.width() + (xoffset+filterHW);
  // 		int oldpos = z*xdim*ydim + y*xdim + x;

  // 		coeffs[pos] += filter[filterpos] * oldcoeffs[oldpos];

  // 	      }
  // 	    }
  // 	  }
		
  // 	  wavelet[(scale+1)*size+pos] = oldcoeffs[pos] - coeffs[pos];

  // 	}
  //       }
  //     }
 
  //     spacing *= 2;

  //   }
  //   std::cerr << "|";

  //   delete [] filter;
  //   delete [] oldcoeffs;

  // }


}
