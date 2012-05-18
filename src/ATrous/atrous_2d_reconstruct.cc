// -----------------------------------------------------------------------
// atrous_2d_reconstruct.cc: 2-dimensional wavelet reconstruction.
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

namespace duchamp
{

  void atrous2DReconstruct(size_t &xdim, size_t &ydim, float *&input, float *&output, Param &par)
  {
    ///  A routine that uses the a trous wavelet method to reconstruct a 
    ///   2-dimensional image.
    /// 
    ///  If there are no non-BLANK pixels (and we are testing for
    ///  BLANKs), the reconstruction cannot be done, so we return the
    ///  input array as the output array and give a warning message.
    /// 
    ///  \param xdim The length of the x-axis of the image.
    ///  \param ydim The length of the y-axis of the image.
    ///  \param input The input spectrum.
    ///  \param output The returned reconstructed spectrum. This array
    ///  needs to be declared beforehand.
    ///  \param par The Param set:contains all necessary info about the
    ///  filter and reconstruction parameters.

    size_t size = xdim * ydim;
    unsigned long mindim = xdim;
    if (ydim<mindim) mindim = ydim;
    unsigned int numScales = par.filter().getNumScales(mindim);
    double *sigmaFactors = new double[numScales+1];
    for(size_t i=0;i<=numScales;i++){
      if(i<=par.filter().maxFactor(2)) 
	sigmaFactors[i] = par.filter().sigmaFactor(2,i);
      else sigmaFactors[i] = sigmaFactors[i-1] / 2.;
    }

    float mean,originalSigma,oldsigma,newsigma;
    size_t goodSize=0;
    bool *isGood = new bool[size];
    for(size_t pos=0;pos<size;pos++){
      isGood[pos] = !par.isBlank(input[pos]);
      if(isGood[pos]) goodSize++;
    }

    if(goodSize == 0){
      // There are no good pixels -- everything is BLANK for some reason.
      // Return the input array as the output, and give a warning message.

      for(size_t pos=0;pos<size; pos++) output[pos] = input[pos];

      DUCHAMPWARN("2D Reconstruction","There are no good pixels to be reconstructed -- all are BLANK. Returning input array.");
    }
    else{
      // Otherwise, all is good, and we continue.

      //      findMedianStats(input,goodSize,isGood,originalMean,originalSigma);
      // originalSigma = madfmToSigma(originalSigma);
      if(par.getFlagRobustStats())
	originalSigma = madfmToSigma(findMADFM(input,isGood,size));
      else
	originalSigma = findStddev<float>(input,isGood,size);
  
      float *coeffs    = new float[size];
      float *wavelet   = new float[size];
      // float *residual  = new float[size];

      for(size_t pos=0;pos<size;pos++) output[pos]=0.;

      unsigned int filterwidth = par.filter().width();
      int filterHW = filterwidth/2;
      double *filter = new double[filterwidth*filterwidth];
      for(size_t i=0;i<filterwidth;i++){
	for(size_t j=0;j<filterwidth;j++){
	  filter[i*filterwidth+j] = par.filter().coeff(i) * par.filter().coeff(j);
	}
      }

      // long *xLim1 = new long[ydim];
      // for(size_t i=0;i<ydim;i++) xLim1[i] = 0;
      // long *yLim1 = new long[xdim];
      // for(size_t i=0;i<xdim;i++) yLim1[i] = 0;
      // long *xLim2 = new long[ydim];
      // for(size_t i=0;i<ydim;i++) xLim2[i] = xdim-1;
      // long *yLim2 = new long[xdim];
      // for(size_t i=0;i<xdim;i++) yLim2[i] = ydim-1;

      // if(par.getFlagBlankPix()){
      // 	float avGapX = 0, avGapY = 0;
      // 	for(size_t row=0;row<ydim;row++){
      // 	  size_t ct1 = 0;
      // 	  size_t ct2 = xdim - 1;
      // 	  while((ct1<ct2)&&(par.isBlank(input[row*xdim+ct1]))) ct1++;
      // 	  while((ct2>ct1)&&(par.isBlank(input[row*xdim+ct2]))) ct2--;
      // 	  xLim1[row] = ct1;
      // 	  xLim2[row] = ct2;
      // 	  avGapX += ct2 - ct1;
      // 	}
      // 	avGapX /= float(ydim);
    
      // 	for(size_t col=0;col<xdim;col++){
      // 	  size_t ct1=0;
      // 	  size_t ct2=ydim-1;
      // 	  while((ct1<ct2)&&(par.isBlank(input[col+xdim*ct1]))) ct1++;
      // 	  while((ct2>ct1)&&(par.isBlank(input[col+xdim*ct2]))) ct2--;
      // 	  yLim1[col] = ct1;
      // 	  yLim2[col] = ct2;
      // 	  avGapY += ct2 - ct1;
      // 	}
      // 	avGapY /= float(xdim);
    
      // 	// if(avGapX < mindim) mindim = int(avGapX);
      // 	// if(avGapY < mindim) mindim = int(avGapY);
      // 	// numScales = par.filter().getNumScales(mindim);
      // }

      float threshold;
      int iteration=0;
      newsigma = 1.e9;
      for(size_t i=0;i<size;i++) output[i] = 0;
      do{
	if(par.isVerbose()) {
	  std::cout << "Iteration #"<<std::setw(2)<<++iteration<<":";
	  printBackSpace(13);
	}

	// first, get the value of oldsigma and set it to the previous 
	//   newsigma value
	oldsigma = newsigma;
	// we are transforming the residual array
	for(size_t i=0;i<size;i++)  coeffs[i] = input[i] - output[i];  

	int spacing = 1;
	for(unsigned int scale = 1; scale<numScales; scale++){

	  if(par.isVerbose()){
	    std::cout << "Scale ";
	    std::cout << std::setw(2)<<scale<<" / "<<std::setw(2)<<numScales;
	    printBackSpace(13);
	    std::cout <<std::flush;
	  }

	  for(unsigned long ypos = 0; ypos<ydim; ypos++){
	    for(unsigned long xpos = 0; xpos<xdim; xpos++){
	      // loops over each pixel in the image
	      size_t pos = ypos*xdim + xpos;
	  
	      wavelet[pos] = coeffs[pos];

	      if(!isGood[pos]) wavelet[pos] = 0.;
	      else{

		size_t filterpos = 0;
		for(int yoffset=-filterHW; yoffset<=filterHW; yoffset++){
		  long y = long(ypos) + spacing*yoffset;
		  while((y<0)||(y>=long(ydim))){
		    // boundary conditions are reflection. 
		    if(y<0) y = 0 - y;
		    else if(y>=long(ydim)) y = 2*(ydim-1) - y;
		  }
		  // Boundary conditions -- assume reflection at boundaries.
		  // Use limits as calculated above
		  // 	      if(yLim1[xpos]!=yLim2[xpos]){ 
		  // 		// if these are equal we will get into an infinite loop here 
		  // 		while((y<yLim1[xpos])||(y>yLim2[xpos])){
		  // 		  if(y<yLim1[xpos]) y = 2*yLim1[xpos] - y;      
		  // 		  else if(y>yLim2[xpos]) y = 2*yLim2[xpos] - y;      
		  // 		}
		  // 	      }
		  size_t oldrow = y * xdim;
	  
		  for(int xoffset=-filterHW; xoffset<=filterHW; xoffset++){
		    long x = long(xpos) + spacing*xoffset;
		    while((x<0)||(x>=long(xdim))){
		      // boundary conditions are reflection. 
		      if(x<0) x = 0 - x;
		      else if(x>=long(xdim)) x = 2*(xdim-1) - x;
		    }
		    // Boundary conditions -- assume reflection at boundaries.
		    // Use limits as calculated above
		    // 		if(xLim1[ypos]!=xLim2[ypos]){
		    // 		  // if these are equal we will get into an infinite loop here 
		    // 		  while((x<xLim1[ypos])||(x>xLim2[ypos])){
		    // 		    if(x<xLim1[ypos]) x = 2*xLim1[ypos] - x;      
		    // 		    else if(x>xLim2[ypos]) x = 2*xLim2[ypos] - x;      
		    // 		  }
		    // 		}

		    size_t oldpos = oldrow + x;

		    // float oldCoeff;
		    // if((y>=yLim1[xpos])&&(y<=yLim2[xpos])&&
		    //    (x>=xLim1[ypos])&&(x<=xLim2[ypos])  )
		    //   oldCoeff = coeffs[oldpos];
		    // else oldCoeff = 0.;

		    // if(isGood[pos]) wavelet[pos] -= filter[filterpos] * oldCoeff;
		    // // 		  wavelet[pos] -= filter[filterpos] * coeffs[oldpos];
		    if(isGood[pos]) 
		      wavelet[pos] -= filter[filterpos] * coeffs[oldpos];

		    filterpos++;

		  } //-> end of xoffset loop
		} //-> end of yoffset loop
	      } //-> end of else{ ( from if(!isGood[pos])  )
	
	    } //-> end of xpos loop
	  } //-> end of ypos loop

	  // Need to do this after we've done *all* the convolving
	  for(size_t pos=0;pos<size;pos++) coeffs[pos] = coeffs[pos] - wavelet[pos];

	  // Have found wavelet coeffs for this scale -- now threshold    
	  if(scale>=par.getMinScale()){
	    //	    findMedianStats(wavelet,goodSize,isGood,mean,sigma);
	    if(par.getFlagRobustStats())
	      mean = findMedian<float>(wavelet,isGood,size);
	    else
	      mean= findMean<float>(wavelet,isGood,size);

	    threshold = mean + 
	      par.getAtrousCut() * originalSigma * sigmaFactors[scale];
	    for(size_t pos=0;pos<size;pos++){
	      if(!isGood[pos]) output[pos] = input[pos];
	      // preserve the Blank pixel values in the output.
	      else if( fabs(wavelet[pos]) > threshold ) 
		output[pos] += wavelet[pos];
	    }
	  }
	  spacing *= 2;

	} // END OF LOOP OVER SCALES

	for(size_t pos=0;pos<size;pos++) if(isGood[pos]) output[pos] += coeffs[pos];

	// for(size_t i=0;i<size;i++) residual[i] = input[i] - output[i];
	// findMedianStats(residual,goodSize,isGood,mean,newsigma);
	// findMedianStatsDiff(input,output,size,isGood,mean,newsigma);
	// newsigma = madfmToSigma(newsigma); 
	if(par.getFlagRobustStats())
	  newsigma = madfmToSigma(findMADFMDiff(input,output,isGood,size));
	else
	  newsigma = findStddevDiff<float>(input,output,isGood,size);

	if(par.isVerbose()) printBackSpace(15);

      } while( (iteration==1) || 
	       (fabs(oldsigma-newsigma)/newsigma > reconTolerance) );

      if(par.isVerbose()) std::cout << "Completed "<<iteration<<" iterations. ";

      // delete [] xLim1;
      // delete [] xLim2;
      // delete [] yLim1;
      // delete [] yLim2;
      delete [] filter;
      delete [] coeffs;
      delete [] wavelet;
      // delete [] residual;

    }

    delete [] isGood;
    delete [] sigmaFactors;
  }
    
}
