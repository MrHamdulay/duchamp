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
#include <duchamp.hh>
#include <param.hh>
#include <ATrous/atrous.hh>
#include <ATrous/filter.hh>
#include <Utils/utils.hh>
#include <Utils/feedback.hh>
#include <Utils/Statistics.hh>
using Statistics::madfmToSigma;

void atrous2DReconstruct(long &xdim, long &ydim, float *&input, float *&output, Param &par)
{
  /**
   *  A routine that uses the a trous wavelet method to reconstruct a 
   *   2-dimensional image.
   *
   *  If there are no non-BLANK pixels (and we are testing for
   *  BLANKs), the reconstruction cannot be done, so we return the
   *  input array as the output array and give a warning message.
   *
   *  \param xdim The length of the x-axis of the image.
   *  \param ydim The length of the y-axis of the image.
   *  \param input The input spectrum.
   *  \param output The returned reconstructed spectrum. This array
   *  needs to be declared beforehand.
   *  \param par The Param set:contains all necessary info about the
   *  filter and reconstruction parameters.
   */

  long size = xdim * ydim;
  long mindim = xdim;
  if (ydim<mindim) mindim = ydim;
  int numScales = par.filter().getNumScales(mindim);
  double *sigmaFactors = new double[numScales+1];
  for(int i=0;i<=numScales;i++){
    if(i<=par.filter().maxFactor(2)) 
      sigmaFactors[i] = par.filter().sigmaFactor(2,i);
    else sigmaFactors[i] = sigmaFactors[i-1] / 2.;
  }

  float mean,sigma,originalSigma,originalMean,oldsigma,newsigma;
  int goodSize=0;
  bool *isGood = new bool[size];
  for(int pos=0;pos<size;pos++){
    isGood[pos] = !par.isBlank(input[pos]);
    if(isGood[pos]) goodSize++;
  }

  if(goodSize == 0){
    // There are no good pixels -- everything is BLANK for some reason.
    // Return the input array as the output, and give a warning message.

    for(int pos=0;pos<size; pos++) output[pos] = input[pos];

    duchampWarning("2D Reconstruction","\
There are no good pixels to be reconstructed -- all are BLANK.\n\
Returning input array.\n");
  }
  else{
    // Otherwise, all is good, and we continue.

    float *array = new float[size];
    goodSize=0;
    for(int i=0;i<size;i++) if(isGood[i]) array[goodSize++] = input[i];
    findMedianStats(array,goodSize,originalMean,originalSigma);
    originalSigma = madfmToSigma(originalSigma);
    delete [] array;
  
    float *coeffs    = new float[size];
    float *wavelet   = new float[size];

    for(int pos=0;pos<size;pos++) output[pos]=0.;

    int filterwidth = par.filter().width();
    int filterHW = filterwidth/2;
    double *filter = new double[filterwidth*filterwidth];
    for(int i=0;i<filterwidth;i++){
      for(int j=0;j<filterwidth;j++){
	filter[i*filterwidth+j] = par.filter().coeff(i) * par.filter().coeff(j);
      }
    }

    int *xLim1 = new int[ydim];
    for(int i=0;i<ydim;i++) xLim1[i] = 0;
    int *yLim1 = new int[xdim];
    for(int i=0;i<xdim;i++) yLim1[i] = 0;
    int *xLim2 = new int[ydim];
    for(int i=0;i<ydim;i++) xLim2[i] = xdim-1;
    int *yLim2 = new int[xdim];
    for(int i=0;i<xdim;i++) yLim2[i] = ydim-1;

    if(par.getFlagBlankPix()){
      float avGapX = 0, avGapY = 0;
      for(int row=0;row<ydim;row++){
	int ct1 = 0;
	int ct2 = xdim - 1;
	while((ct1<ct2)&&(par.isBlank(input[row*xdim+ct1]))) ct1++;
	while((ct2>ct1)&&(par.isBlank(input[row*xdim+ct2]))) ct2--;
	xLim1[row] = ct1;
	xLim2[row] = ct2;
	avGapX += ct2 - ct1;
      }
      avGapX /= float(ydim);
    
      for(int col=0;col<xdim;col++){
	int ct1=0;
	int ct2=ydim-1;
	while((ct1<ct2)&&(par.isBlank(input[col+xdim*ct1]))) ct1++;
	while((ct2>ct1)&&(par.isBlank(input[col+xdim*ct2]))) ct2--;
	yLim1[col] = ct1;
	yLim2[col] = ct2;
	avGapY += ct2 - ct1;
      }
      avGapY /= float(xdim);
    
      mindim = int(avGapX);
      if(avGapY < avGapX) mindim = int(avGapY);
      numScales = par.filter().getNumScales(mindim);
    }

    float threshold;
    int iteration=0;
    newsigma = 1.e9;
    for(int i=0;i<size;i++) output[i] = 0;
    do{
      if(par.isVerbose()) {
	std::cout << "Iteration #"<<std::setw(2)<<++iteration<<":";
	printBackSpace(13);
      }

      // first, get the value of oldsigma and set it to the previous 
      //   newsigma value
      oldsigma = newsigma;
      // we are transforming the residual array
      for(int i=0;i<size;i++)  coeffs[i] = input[i] - output[i];  

      int spacing = 1;
      for(int scale = 1; scale<numScales; scale++){

	if(par.isVerbose()){
	  std::cout << "Scale ";
	  std::cout << std::setw(2)<<scale<<" / "<<std::setw(2)<<numScales;
	  printBackSpace(13);
	  std::cout <<std::flush;
	}

	for(int ypos = 0; ypos<ydim; ypos++){
	  for(int xpos = 0; xpos<xdim; xpos++){
	    // loops over each pixel in the image
	    int pos = ypos*xdim + xpos;
	  
	    wavelet[pos] = coeffs[pos];

	    if(!isGood[pos]) wavelet[pos] = 0.;
	    else{

	      int filterpos = -1;
	      for(int yoffset=-filterHW; yoffset<=filterHW; yoffset++){
		int y = ypos + spacing*yoffset;
		// Boundary conditions -- assume reflection at boundaries.
		// Use limits as calculated above
		// 	      if(yLim1[xpos]!=yLim2[xpos]){ 
		// 		// if these are equal we will get into an infinite loop here 
		// 		while((y<yLim1[xpos])||(y>yLim2[xpos])){
		// 		  if(y<yLim1[xpos]) y = 2*yLim1[xpos] - y;      
		// 		  else if(y>yLim2[xpos]) y = 2*yLim2[xpos] - y;      
		// 		}
		// 	      }
		int oldrow = y * xdim;
	  
		for(int xoffset=-filterHW; xoffset<=filterHW; xoffset++){
		  int x = xpos + spacing*xoffset;
		  // Boundary conditions -- assume reflection at boundaries.
		  // Use limits as calculated above
		  // 		if(xLim1[ypos]!=xLim2[ypos]){
		  // 		  // if these are equal we will get into an infinite loop here 
		  // 		  while((x<xLim1[ypos])||(x>xLim2[ypos])){
		  // 		    if(x<xLim1[ypos]) x = 2*xLim1[ypos] - x;      
		  // 		    else if(x>xLim2[ypos]) x = 2*xLim2[ypos] - x;      
		  // 		  }
		  // 		}

		  int oldpos = oldrow + x;

		  float oldCoeff;
		  if((y>=yLim1[xpos])&&(y<=yLim2[xpos])&&
		     (x>=xLim1[ypos])&&(x<=xLim2[ypos])  )
		    oldCoeff = coeffs[oldpos];
		  else oldCoeff = 0.;

		  filterpos++;

		  if(isGood[pos]) wavelet[pos] -= filter[filterpos] * oldCoeff;
		  // 		  wavelet[pos] -= filter[filterpos] * coeffs[oldpos];

		} //-> end of xoffset loop
	      } //-> end of yoffset loop
	    } //-> end of else{ ( from if(!isGood[pos])  )
	
	  } //-> end of xpos loop
	} //-> end of ypos loop

	// Need to do this after we've done *all* the convolving
	for(int pos=0;pos<size;pos++) coeffs[pos] = coeffs[pos] - wavelet[pos];

	// Have found wavelet coeffs for this scale -- now threshold    
	if(scale>=par.getMinScale()){
	  array = new float[size];
	  goodSize=0;
	  for(int pos=0;pos<size;pos++){
	    if(isGood[pos]) array[goodSize++] = wavelet[pos];
	  }
	  findMedianStats(array,goodSize,mean,sigma);
	  delete [] array;

	  threshold = mean + 
	    par.getAtrousCut() * originalSigma * sigmaFactors[scale];
	  for(int pos=0;pos<size;pos++){
	    if(!isGood[pos]) output[pos] = input[pos];
	    // preserve the Blank pixel values in the output.
	    else if( fabs(wavelet[pos]) > threshold ) 
	      output[pos] += wavelet[pos];
	  }
	}
	spacing *= 2;

      } // END OF LOOP OVER SCALES

      for(int pos=0;pos<size;pos++) if(isGood[pos]) output[pos] += coeffs[pos];

      array = new float[size];
      goodSize=0;
      for(int i=0;i<size;i++){
	if(isGood[i]) array[goodSize++] = input[i] - output[i];
      }
      findMedianStats(array,goodSize,mean,newsigma);
      newsigma = madfmToSigma(newsigma); 
      delete [] array;
    
      if(par.isVerbose()) printBackSpace(15);

    } while( (iteration==1) || 
	     (fabs(oldsigma-newsigma)/newsigma > reconTolerance) );

    if(par.isVerbose()) std::cout << "Completed "<<iteration<<" iterations. ";

    delete [] xLim1;
    delete [] xLim2;
    delete [] yLim1;
    delete [] yLim2;
    delete [] filter;
    delete [] coeffs;
    delete [] wavelet;

  }

  delete [] isGood;
  delete [] sigmaFactors;
}
    
    
