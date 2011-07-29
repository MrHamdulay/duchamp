// -----------------------------------------------------------------------
// atrous_3d_reconstruct.cc: 3-dimensional wavelet reconstruction.
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
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/ATrous/atrous.hh>
#include <duchamp/ATrous/filter.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/feedback.hh>
#include <duchamp/Utils/Statistics.hh>
using Statistics::madfmToSigma;

using std::endl;
using std::setw;

namespace duchamp
{

  void atrous3DReconstruct(unsigned long &xdim, unsigned long &ydim, unsigned long &zdim, float *&input, 
			   float *&output, Param &par)
  {
    ///  A routine that uses the a trous wavelet method to reconstruct a 
    ///   3-dimensional image cube.
    ///  The Param object "par" contains all necessary info about the filter and 
    ///   reconstruction parameters.
    /// 
    ///  If there are no non-BLANK pixels (and we are testing for
    ///  BLANKs), the reconstruction cannot be done, so we return the
    ///  input array as the output array and give a warning message.
    /// 
    ///  \param xdim The length of the x-axis.
    ///  \param ydim The length of the y-axis.
    ///  \param zdim The length of the z-axis.
    ///  \param input The input spectrum.
    ///  \param output The returned reconstructed spectrum. This array needs to be declared beforehand.
    ///  \param par The Param set.

    size_t size = xdim * ydim * zdim;
    size_t spatialSize = xdim * ydim;
    unsigned long mindim = xdim;
    if (ydim<mindim) mindim = ydim;
    if (zdim<mindim) mindim = zdim;
    unsigned int numScales = par.filter().getNumScales(mindim);

    double *sigmaFactors = new double[numScales+1];
    for(size_t i=0;i<=numScales;i++){
      if(i<=size_t(par.filter().maxFactor(3)) )
	sigmaFactors[i] = par.filter().sigmaFactor(3,i);
      else sigmaFactors[i] = sigmaFactors[i-1] / sqrt(8.);
    }

    float mean,sigma,originalSigma,originalMean,oldsigma,newsigma;
    bool *isGood = new bool[size];
    size_t goodSize=0;
    for(size_t pos=0;pos<size;pos++){
      isGood[pos] = !par.isBlank(input[pos]);
      if(isGood[pos]) goodSize++;
    }

    if(goodSize == 0){
      // There are no good pixels -- everything is BLANK for some reason.
      // Return the input array as the output, and give a warning message.

      for(size_t pos=0;pos<xdim; pos++) output[pos] = input[pos];

      duchampWarning("3D Reconstruction",
		     "There are no good pixels to be reconstructed -- all are BLANK.\nPerhaps you need to try this with flagTrim=false.\nReturning input array.\n");
    }
    else{
      // Otherwise, all is good, and we continue.


      // findMedianStats(input,goodSize,isGood,originalMean,originalSigma);
      if(par.getFlagRobustStats())
	originalSigma = madfmToSigma(findMADFM(input,isGood,size));
      else
	originalSigma = findStddev(input,isGood,size);

      float *coeffs = new float[size];
      float *wavelet = new float[size];
      // float *residual = new float[size];

      for(size_t pos=0;pos<size;pos++) output[pos]=0.;

      // Define the 3-D (separable) filter, using info from par.filter()
      size_t filterwidth = par.filter().width();
      int filterHW = filterwidth/2;
      size_t fsize = filterwidth*filterwidth*filterwidth;
      double *filter = new double[fsize];
      for(size_t i=0;i<filterwidth;i++){
	for(size_t j=0;j<filterwidth;j++){
	  for(size_t k=0;k<filterwidth;k++){
	    filter[i +j*filterwidth + k*filterwidth*filterwidth] = 
	      par.filter().coeff(i) * par.filter().coeff(j) * par.filter().coeff(k);
	  }
	}
      }

      // Locating the borders of the image -- ignoring BLANK pixels
      //  Only do this if flagBlankPix is true. 
      //  Otherwise use the full range of x and y.
      //  No trimming is done in the z-direction at this point.
      long *xLim1 = new long[ydim];
      for(size_t i=0;i<ydim;i++) xLim1[i] = 0;
      long *xLim2 = new long[ydim];
      for(size_t i=0;i<ydim;i++) xLim2[i] = xdim-1;
      long *yLim1 = new long[xdim];
      for(size_t i=0;i<xdim;i++) yLim1[i] = 0;
      long *yLim2 = new long[xdim];
      for(size_t i=0;i<xdim;i++) yLim2[i] = ydim-1;

      if(par.getFlagBlankPix()){
	float avGapX = 0, avGapY = 0;
	for(size_t row=0;row<ydim;row++){
	  size_t ct1 = 0;
	  size_t ct2 = xdim - 1;
	  while((ct1<ct2)&&(par.isBlank(input[row*xdim+ct1]))) ct1++;
	  while((ct2>ct1)&&(par.isBlank(input[row*xdim+ct2]))) ct2--;
	  xLim1[row] = ct1;
	  xLim2[row] = ct2;
	  avGapX += ct2 - ct1 + 1;
	}
	avGapX /= float(ydim);

	for(size_t col=0;col<xdim;col++){
	  size_t ct1=0;
	  size_t ct2=ydim-1;
	  while((ct1<ct2)&&(par.isBlank(input[col+xdim*ct1]))) ct1++;
	  while((ct2>ct1)&&(par.isBlank(input[col+xdim*ct2]))) ct2--;
	  yLim1[col] = ct1;
	  yLim2[col] = ct2;
	  avGapY += ct2 - ct1 + 1;
	}
	avGapY /= float(xdim);
 
	// if(avGapX < mindim) mindim = int(avGapX);
	// if(avGapY < mindim) mindim = int(avGapY);
	// numScales = par.filter().getNumScales(mindim);
      }

      float threshold;
      int iteration=0;
      newsigma = 1.e9;
      for(size_t i=0;i<size;i++) output[i] = 0;
      do{
	if(par.isVerbose()) std::cout << "Iteration #"<<setw(2)<<++iteration<<": ";
	// first, get the value of oldsigma, set it to the previous newsigma value
	oldsigma = newsigma;
	// we are transforming the residual array (input array first time around)
	for(size_t i=0;i<size;i++)  coeffs[i] = input[i] - output[i];

	int spacing = 1;
	for(unsigned int scale = 1; scale<=numScales; scale++){

	  if(par.isVerbose()){
	    std::cout << "Scale ";
	    std::cout << setw(2)<<scale<<" / "<<setw(2)<<numScales;
	    printBackSpace(13);
	    std::cout << std::flush;
	  }

	  size_t pos = -1;
	  for(unsigned long zpos = 0; zpos<zdim; zpos++){
	    for(unsigned long ypos = 0; ypos<ydim; ypos++){
	      for(unsigned long xpos = 0; xpos<xdim; xpos++){
		// loops over each pixel in the image
		pos++;

		wavelet[pos] = coeffs[pos];
 	    
		if(!isGood[pos] )  wavelet[pos] = 0.;
		else{

		  unsigned int filterpos = -1;
		  for(int zoffset=-filterHW; zoffset<=filterHW; zoffset++){
		    long z = zpos + spacing*zoffset;
		    if(z<0) z = -z;                 // boundary conditions are 
		    if(z>=long(zdim)) z = 2*(zdim-1) - z; //    reflection.
		
		    size_t oldchan = z * spatialSize;
		
		    for(int yoffset=-filterHW; yoffset<=filterHW; yoffset++){
		      long y = long(ypos) + spacing*yoffset;

		      // Boundary conditions -- assume reflection at boundaries.
		      // Use limits as calculated above
		      if(yLim1[xpos]!=yLim2[xpos]){ 
			// if these are equal we will get into an infinite loop
			while((y<yLim1[xpos])||(y>yLim2[xpos])){
			// std::cerr << y << " " <<spacing << " " << yoffset << " " << ypos << " " << xpos << " " << yLim1[xpos] << " " << yLim2[xpos] << "\n";
			  if(y<yLim1[xpos]) y = 2*yLim1[xpos] - y;      
			  else if(y>yLim2[xpos]) y = 2*yLim2[xpos] - y;      
			}
		      }
		      size_t oldrow = y * xdim;
	  
		      for(int xoffset=-filterHW; xoffset<=filterHW; xoffset++){
			long x = long(xpos) + spacing*xoffset;

			// Boundary conditions -- assume reflection at boundaries.
			// Use limits as calculated above
			if(xLim1[ypos]!=xLim2[ypos]){
			  // if these are equal we will get into an infinite loop
			  while((x<xLim1[ypos])||(x>xLim2[ypos])){
			    if(x<xLim1[ypos]) x = 2*xLim1[ypos] - x;      
			    else if(x>xLim2[ypos]) x = 2*xLim2[ypos] - x;      
			  }
			}

			size_t oldpos = oldchan + oldrow + x;

			filterpos++;
		   
			if(isGood[oldpos]) 
			  wavelet[pos] -= filter[filterpos]*coeffs[oldpos];
		      
		      } //-> end of xoffset loop
		    } //-> end of yoffset loop
		  } //-> end of zoffset loop
		} //-> end of else{ ( from if(!isGood[pos])  )
	    
	      } //-> end of xpos loop
	    } //-> end of ypos loop
	  } //-> end of zpos loop

	  // Need to do this after we've done *all* the convolving
	  for(size_t pos=0;pos<size;pos++) coeffs[pos] = coeffs[pos] - wavelet[pos];

	  // Have found wavelet coeffs for this scale -- now threshold
	  if(scale>=par.getMinScale()){
	    if(par.getFlagRobustStats())
	      // findMedianStats(wavelet,size,isGood,mean,sigma);
	      mean = findMedian(wavelet,isGood,size);
	    else
	      //findNormalStats(wavelet,size,isGood,mean,sigma);
	      mean = findMean(wavelet,isGood,size);
	      
	    threshold = mean + 
	      par.getAtrousCut()*originalSigma*sigmaFactors[scale];
	    for(size_t pos=0;pos<size;pos++){
	      if(!isGood[pos]){
		output[pos] = input[pos]; 
		// this preserves the Blank pixel values in the output.
	      }
	      else if( fabs(wavelet[pos]) > threshold ){
		output[pos] += wavelet[pos];
		// only add to the output if the wavelet coefficient is significant
	      }
	    }
	  }
 
	  spacing *= 2;  // double the scale of the filter.

	} //-> end of scale loop 

	for(size_t pos=0;pos<size;pos++) if(isGood[pos]) output[pos] += coeffs[pos];

	// for(size_t i=0;i<size;i++) residual[i] = input[i] - output[i];
	// findMedianStats(residual,goodSize,isGood,mean,newsigma);
	// findMedianStatsDiff(input,output,goodSize,isGood,mean,newsigma);
	// newsigma = madfmToSigma(newsigma);
	if(par.getFlagRobustStats())
	  newsigma = madfmToSigma(findMADFMDiff(input,output,isGood,size));
	else
	  newsigma = findStddevDiff(input,output,isGood,size);

	if(par.isVerbose()) printBackSpace(15);

      } while( (iteration==1) || 
	       (fabs(oldsigma-newsigma)/newsigma > reconTolerance) );

      if(par.isVerbose()) std::cout << "Completed "<<iteration<<" iterations. ";

      delete [] xLim1;
      delete [] xLim2;
      delete [] yLim1;
      delete [] yLim2;
      delete [] filter;
      // delete [] residual;
      delete [] coeffs;
      delete [] wavelet;

    }

    delete [] isGood;
    delete [] sigmaFactors;
  }

}
