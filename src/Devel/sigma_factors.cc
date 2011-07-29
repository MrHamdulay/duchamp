// -----------------------------------------------------------------------
// sigma_factors.cc: Functions to calculate the scaling factors for
//                   the rms in the case of wavelet reconstruction.
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
#include <iomanip>
#include <math.h>
#include <duchamp/ATrous/atrous.hh>
#include <duchamp/ATrous/filter.hh>
#define DEBUG true
//#define DEBUG false

void getSigmaFactors1DNew(duchamp::Filter &reconFilter, int &numScales)
{
  std::cerr<<"-->"<<reconFilter.getName()<<"<--"<<std::endl;  
//  std::cerr << "-->"<<Filter::getName()<<"<--"<<std::endl;
//  std::cerr << "-->"<<getNumScales(125)<<"<--"<<std::endl;
  
  int filterwidth = reconFilter.width();
   // int filterHW = reconFilter.width()/2;
//  int filterwidth = Filter::width();
//  int filterHW = Filter::width()/2;
//  int filterwidth=1;
  int filterHW=filterwidth/2;

  // starting values:
  long xdim = 1;
  long size = xdim;
  double *c = new double[size];
  double *w = new double[size];
  w[0] = 1.;

  double *filter = new double[filterwidth];
  for(int i=0;i<filterwidth;i++) filter[i] = reconFilter.coeff(i);
//  for(int i=0;i<filterwidth;i++) filter[i] = Filter::coeff(i);
  
  int spacing = 1;
//   const float FACTOR = 16.;
  const float FACTOR = reconFilter.coeff(0)==0?1.:1./reconFilter.coeff(0);
  // const float FACTOR = 1.;
  const float FACFAC = FACTOR*FACTOR;

  if(DEBUG){
    std::cout << "divide by " << pow(FACTOR,0) << std::endl;
    for(int i=0;i<size;i++){
      std::cout<<i<<" "<<w[i]<<std::endl;
    }
    std::cout<<"===============\n";
  }

  for(int scale = 1; scale<=numScales; scale++){

    // re-size and incorporate previous w into new c
    long oldsize = size;
    long oldxdim = xdim;
//     xdim = 2 * xdim + 3;
//     xdim = reconFilter.getMaxSize(scale);
    xdim += filterHW*long(pow(2,scale));
    size = xdim;
    double *newc = new double[size];
    for(int i=0;i<size;i++){
      int oldpos = i - xdim/2;
      if(// (oldpos<oldsize)&&(oldpos>=0)
	 abs(oldpos)<=oldsize/2) newc[i] = w[oldpos+oldsize/2] * FACTOR;
      else newc[i] = 0.;
      
      if(DEBUG) std::cout<<i<<" "<<oldpos<<" "<<size<<" "<<oldsize<<" "<<oldsize/2<<"   "
			 <<newc[i]// *pow(16,scale-1)
			 <<std::endl;
    }
    if(DEBUG) std::cout<<"---------------\nspacing = " << spacing << std::endl;
    delete [] c;
    c = newc;
    double *neww = new double[size];
    delete [] w;
    w = neww;
    for(int i=0;i<size;i++) w[i] = 0.;

    long double sum = 0.;

    for(int xpos = 0; xpos<xdim; xpos++){
      // loops over each pixel in the image
      w[xpos] = 0;

      for(int xoffset=-filterHW; xoffset<=filterHW; xoffset++){
	int x = xpos + spacing*xoffset;
	int filterpos = (xoffset+filterHW);
	if((x>=0)&&(x<xdim))  // if outside boundary --> set to 0.
	  w[xpos] += filter[filterpos] * c[x];
      } //-> end of xoffset loop
       
      sum += (c[xpos]-w[xpos])*(c[xpos]-w[xpos]) / pow(FACFAC,scale);

    } //-> end of xpos loop

    if(DEBUG){
      std::cout << "divide by " << pow(FACTOR,scale) << std::endl;
      for(int i=0;i<size;i++){
	std::cout<<i<<" "<<w[i]// *pow(16,scale)
		 <<std::endl;
      }
      std::cout<<"===============\n";
    }

    std::cerr << "Scale "<<std::setw(2)<<scale<<":";
    std::cerr.setf(std::ios::scientific);
    std::cerr.setf(std::ios::left);
    std::cerr <<"\t1D = "<<std::setw(15)<<std::setprecision(9)<<sqrt(sum)<<std::endl;
    std::cerr.unsetf(std::ios::left);
      

    spacing *= 2;

  } //-> end of scale loop 

}

void getSigmaFactors2DNew(duchamp::Filter &reconFilter, int &numScales)
{

  int filterHW = reconFilter.width()/2;
  int fsize = reconFilter.width()*reconFilter.width();
  double *filter = new double[fsize];
  for(int i=0;i<reconFilter.width();i++){
    for(int j=0;j<reconFilter.width();j++){
      filter[i +j*reconFilter.width()] = reconFilter.coeff(i) * reconFilter.coeff(j);
    }
  }

  
  // starting values:
  size_t xdim = 1;
  size_t size = xdim*xdim;
  double *c = new double[size];
  double *w = new double[size];
  w[0] = 1.;

  int spacing = 1;
   const float FACTOR = reconFilter.coeff(0)==0?1.:1./(reconFilter.coeff(0)*reconFilter.coeff(0));
   // const float FACTOR = 16.*16.;
  // const float FACTOR = 1.;
  const float FACFAC = FACTOR * FACTOR;

  for(int scale = 1; scale<=numScales; scale++){

    // re-size and incorporate previous w into new c
    size_t oldsize = size;
    size_t oldxdim = xdim;
//     xdim = 2 * oldxdim + 3;
    xdim += filterHW*long(pow(2,scale));
    size = xdim * xdim;
    //    std::cerr << xdim << " " << size << " " << size*sizeof(float) << "\n";
    double *newc = new double[size];
    for(int i=0;i<size;i++){
      int x = i%xdim;
      int y = i/xdim;
      int centrepos=0;
      if((abs(x-xdim/2)<=oldxdim/2)&&(abs(y-xdim/2)<=oldxdim/2)){
	centrepos = x-xdim/2+oldxdim/2 + oldxdim*(y-xdim/2+oldxdim/2);
	newc[i] = w[centrepos] * FACTOR;
      }
      else newc[i] = 0.;
       // std::cout<<i<<" "<<x<<" "<<y<<" "<<centrepos<<" "<<xdim<<" "<<size
       // 	       <<" "<<oldxdim<<" "<<oldsize<<"   "<<newc[i]*pow(256,scale-1)<<std::endl;
    }
//     std::cout<<"---------------\n";
    delete [] c;
    c = newc;
//     std::cerr<<".";
    double *neww = new double[size];
    delete [] w;
    w = neww;
    for(int i=0;i<size;i++) w[i] = 0.;
//     std::cerr<<".";

    long double sum = 0.;

    for(int ypos = 0; ypos<xdim; ypos++){
      for(int xpos = 0; xpos<xdim; xpos++){
	// loops over each pixel in the image
	int pos = ypos*xdim + xpos;
	w[pos] = 0;

	for(int yoffset=-filterHW; yoffset<=filterHW; yoffset++){
	  int y = ypos + spacing*yoffset;

	  for(int xoffset=-filterHW; xoffset<=filterHW; xoffset++){
	    int x = xpos + spacing*xoffset;

	    int filterpos = (yoffset+filterHW)*reconFilter.width() + (xoffset+filterHW);
	    int oldpos = y*xdim + x;
		    
	    if((y>=0)&&(y<xdim)&&(x>=0)&&(x<xdim))
	      w[pos] += filter[filterpos] * c[oldpos];

	  } //-> end of xoffset loop
	} //-> end of yoffset loop
 
	sum += (c[pos]-w[pos])*(c[pos]-w[pos]) / pow(FACFAC,scale);
	    
      } //-> end of xpos loop
    } //-> end of ypos loop

//     for(int i=0;i<size;i++){
//       std::cout<<i<<" "<<i%xdim<<" "<<i/xdim<<" "<<w[i]*pow(256,scale)<<std::endl;
//     }
//     std::cout<<"===============\n";

    std::cerr << "Scale "<<std::setw(2)<<scale<<":";
    std::cerr.setf(std::ios::scientific);
    std::cerr.setf(std::ios::left);
    std::cerr <<"\t2D = "<<std::setw(11)<<std::setprecision(9)<<sqrt(sum)<<std::endl;
    std::cerr.unsetf(std::ios::left);
      
    spacing *= 2;

  } //-> end of scale loop 


}


void getSigmaFactors3DNew(duchamp::Filter &reconFilter, int &numScales)
{
  
  int filterHW = reconFilter.width()/2;
  int fsize = reconFilter.width()*reconFilter.width()*reconFilter.width();
  double *filter = new double[fsize];
  for(int i=0;i<reconFilter.width();i++){
    for(int j=0;j<reconFilter.width();j++){
      for(int k=0;k<reconFilter.width();k++){
	filter[i +j*reconFilter.width() + k*reconFilter.width()*reconFilter.width()] = 
	  reconFilter.coeff(i) * reconFilter.coeff(j) * reconFilter.coeff(k);
      }
    }
  }

  // starting values:
  long xdim = 1;
  long size = xdim*xdim*xdim;
  double *c = new double[size];
  double *w = new double[size];
  int x,y,z,centrepos;
  w[0] = 1.;
  
  int spacing = 1;
//   const float FACTOR = 16.*16.*16.;
  const float FACTOR = 1.;
  const float FACFAC = FACTOR * FACTOR;

  for(int scale = 1; scale<=numScales; scale++){

    // re-size and incorporate previous w into new c
    long oldsize = size;
    long oldxdim = xdim;
//     xdim = 2 * xdim + 3;
    xdim += filterHW*long(pow(2,scale));
    size = xdim * xdim * xdim;
    double *newc = new double[size];
    for(int i=0;i<size;i++){
      x = i%xdim;
      y = (i/xdim) % xdim;
      z = i / (xdim*xdim);
      centrepos = 0;
      if((abs(x-xdim/2)<=oldxdim/2)&&
	 (abs(y-xdim/2)<=oldxdim/2)&&
	 (abs(z-xdim/2)<=oldxdim/2)){
	centrepos = x-xdim/2+oldxdim/2 + oldxdim*(y-xdim/2+oldxdim/2)
	  + oldxdim*oldxdim*(z-xdim/2+oldxdim/2);
	newc[i] = w[centrepos] * FACTOR;
      }	
      else newc[i] = 0.;
//       std::cout<<i<<" "<<x<<" "<<y<<" "<<z<<" "<<oldpos<<" "<<size<<" "<<oldsize
// 	       <<"   "<<newc[i]*pow(16,3*(scale-1))<<std::endl;
    }
//     std::cout<<"---------------\n";
    delete [] c;
    c = newc;
    double *neww = new double[size];
    delete [] w;
    w = neww;
    for(int i=0;i<size;i++) w[i] = 0.;
    long double sum = 0.;

    for(int zpos = 0; zpos<xdim; zpos++){
      for(int ypos = 0; ypos<xdim; ypos++){
	for(int xpos = 0; xpos<xdim; xpos++){
	  // loops over each pixel in the image
	  int pos = zpos*xdim*xdim + ypos*xdim + xpos;
	  w[pos] = 0;

	  for(int zoffset=-filterHW; zoffset<=filterHW; zoffset++){
	    z = zpos + spacing*zoffset;
	    for(int yoffset=-filterHW; yoffset<=filterHW; yoffset++){
	      y = ypos + spacing*yoffset;
	      for(int xoffset=-filterHW; xoffset<=filterHW; xoffset++){
		x = xpos + spacing*xoffset;

		int filterpos = (zoffset+filterHW)*reconFilter.width()*reconFilter.width() + 
		  (yoffset+filterHW)*reconFilter.width() + (xoffset+filterHW);
		int oldpos = z*xdim*xdim + y*xdim + x;

		// only consider points inside boundary -- ie. =0 outside
		if((z>=0)&&(z<xdim)&&(y>=0)&&(y<xdim)&&(x>=0)&&(x<xdim))
		  w[pos] += filter[filterpos] * c[oldpos];

	      } //-> end of xoffset loop
	    } //-> end of yoffset loop
	  } //-> end of zoffset loop
 
	  sum += (c[pos]-w[pos])*(c[pos]-w[pos]) / pow(FACFAC,scale);
	    
	} //-> end of xpos loop
      } //-> end of ypos loop
    } //-> end of zpos loop

//     for(int i=0;i<size;i++){
//       std::cout<<i<<" "<<w[i]*pow(16,3*scale)<<std::endl;
//     }
//     std::cout<<"===============\n";

    std::cerr << "Scale "<<std::setw(2)<<scale<<":";
    std::cerr.setf(std::ios::scientific);
    std::cerr.setf(std::ios::left);
    std::cerr <<"\t3D = "<<std::setw(11)<<std::setprecision(9)<<sqrt(sum)<<std::endl;
    std::cerr.unsetf(std::ios::left);

    spacing *= 2;

  } //-> end of scale loop 




}
