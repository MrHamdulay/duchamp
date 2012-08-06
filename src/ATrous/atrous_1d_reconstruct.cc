// -----------------------------------------------------------------------
// atrous_1d_reconstruct.cc: 1-dimensional wavelet reconstruction.
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
#include <sstream>
#include <iomanip>
#include <math.h>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/feedback.hh>
#include <duchamp/ATrous/atrous.hh>
#include <duchamp/ATrous/filter.hh>
#include <duchamp/Utils/Statistics.hh>
using Statistics::madfmToSigma;

namespace duchamp
{

  void atrous1DReconstruct(size_t &xdim, float *&input, float *&output, Param &par)
  {
    ///  A routine that uses the a trous wavelet method to reconstruct a 
    ///   1-dimensional spectrum. 
    ///  The Param object "par" contains all necessary info about the filter and 
    ///   reconstruction parameters.
    /// 
    ///  If all pixels are BLANK (and we are testing for BLANKs), the
    ///  reconstruction will simply give BLANKs back, so we return the
    ///  input array as the output array.
    /// 
    ///  \param xdim The length of the spectrum.
    ///  \param input The input spectrum.
    ///  \param output The returned reconstructed spectrum. This array needs to 
    ///    be declared beforehand.
    ///  \param par The Param set.

    const float SNR_THRESH=par.getAtrousCut();
    unsigned int MIN_SCALE=par.getMinScale();
    unsigned int MAX_SCALE=par.getMaxScale();
    static bool firstTime = true;  // need this static in case we do two reconstructions - e.g. baseline subtraction

    unsigned int numScales = par.filter().getNumScales(xdim);
    if((MAX_SCALE>0)&&(MAX_SCALE<=numScales))
      MAX_SCALE = std::min(MAX_SCALE,numScales);
    else{
      if((firstTime)&&(MAX_SCALE!=0)){
	firstTime=false;
	DUCHAMPWARN("Reading parameters","The requested value of the parameter scaleMax, \"" << par.getMaxScale() << "\" is outside the allowed range (1-"<< numScales <<") -- setting to " << numScales);
      } 
      MAX_SCALE = numScales;
      par.setMaxScale(MAX_SCALE);
    }
    double *sigmaFactors = new double[numScales+1];
    for(size_t i=0;i<=numScales;i++){
      if(i<=par.filter().maxFactor(1)) 
	sigmaFactors[i] = par.filter().sigmaFactor(1,i);
      else sigmaFactors[i] = sigmaFactors[i-1] / sqrt(2.);
    }

    float mean,originalSigma,oldsigma,newsigma;
    bool *isGood = new bool[xdim];
    size_t goodSize=0;
    for(size_t pos=0;pos<xdim;pos++) {
      isGood[pos] = !par.isBlank(input[pos]);
      if(isGood[pos]) goodSize++;
    }

    if(goodSize == 0){
      // There are no good pixels -- everything is BLANK for some reason.
      // Return the input array as the output.

      for(size_t pos=0;pos<xdim; pos++) output[pos] = input[pos];

    }
    else{
      // Otherwise, all is good, and we continue.


      float *coeffs = new float[xdim];
      float *wavelet = new float[xdim];
      // float *residual = new float[xdim];

      for(size_t pos=0;pos<xdim;pos++) output[pos]=0.;

      int filterHW = par.filter().width()/2;
      double *filter = new double[par.filter().width()];
      for(size_t i=0;i<par.filter().width();i++) filter[i] = par.filter().coeff(i);


      // No trimming done in 1D case.

      float threshold;
      int iteration=0;
      newsigma = 1.e9;
      do{
	if(par.isVerbose()) {
	  std::cout << "Iteration #"<<++iteration<<":";
	  printSpace(13);
	}
	// first, get the value of oldsigma and set it to the previous 
	//   newsigma value
	oldsigma = newsigma;
	// all other times round, we are transforming the residual array
	for(size_t i=0;i<xdim;i++)  coeffs[i] = input[i] - output[i];
    
	// findMedianStats(input,xdim,isGood,originalMean,originalSigma);
	// originalSigma = madfmToSigma(originalSigma); 
	if(par.getFlagRobustStats())
	  originalSigma = madfmToSigma(findMADFM(input,isGood,xdim));
	else
	  originalSigma = findStddev<float>(input,isGood,xdim);

	int spacing = 1;
	for(unsigned int scale = 1; scale<=numScales; scale++){

	  if(par.isVerbose()) {
	    std::cout << "Scale " << std::setw(2) << scale
		      << " /"     << std::setw(2) << numScales <<std::flush;
	  }

	  for(size_t xpos = 0; xpos<xdim; xpos++){
	    // loops over each pixel in the image

	    wavelet[xpos] = coeffs[xpos];
	
	    if(!isGood[xpos] )  wavelet[xpos] = 0.;
	    else{

	      for(int xoffset=-filterHW; xoffset<=filterHW; xoffset++){
		long x = xpos + spacing*xoffset;

		while((x<0)||(x>=long(xdim))){
		  // boundary conditions are reflection. 
		  if(x<0) x = 0 - x;
		  else if(x>=long(xdim)) x = 2*(xdim-1) - x;
		}

		size_t filterpos = (xoffset+filterHW);
		size_t oldpos = x;

		if(isGood[oldpos]) 
		  wavelet[xpos] -= filter[filterpos]*coeffs[oldpos];
	      
	      } //-> end of xoffset loop
	    } //-> end of else{ ( from if(!isGood[xpos])  )
	    
	  } //-> end of xpos loop

	  // Need to do this after we've done *all* the convolving
	  for(size_t pos=0;pos<xdim;pos++) coeffs[pos] = coeffs[pos] - wavelet[pos];

	  // Have found wavelet coeffs for this scale -- now threshold
	  if(scale>=MIN_SCALE && scale <=MAX_SCALE){
	    // 	    findMedianStats(wavelet,xdim,isGood,mean,sigma);
	    if(par.getFlagRobustStats())
	      mean = findMedian<float>(wavelet,isGood,xdim);
	    else
	      mean = findMean<float>(wavelet,isGood,xdim);

	    threshold = mean+SNR_THRESH*originalSigma*sigmaFactors[scale];
	    for(size_t pos=0;pos<xdim;pos++){
	      // preserve the Blank pixel values in the output.
	      if(!isGood[pos]) output[pos] = input[pos];
	      else if( fabs(wavelet[pos]) > threshold )
		output[pos] += wavelet[pos];
	    }
	  }
 
	  spacing *= 2;

	} //-> end of scale loop 

	// Only add the final smoothed array if we are doing *all* the scales. 
	if(numScales == par.filter().getNumScales(xdim))
	  for(size_t pos=0;pos<xdim;pos++) 
	    if(isGood[pos]) output[pos] += coeffs[pos];

 	// for(size_t pos=0;pos<xdim;pos++) residual[pos]=input[pos]-output[pos];
  	// findMedianStats(residual,xdim,isGood,mean,newsigma);
	// newsigma = madfmToSigma(newsigma); 
	if(par.getFlagRobustStats())
	  newsigma = madfmToSigma(findMADFMDiff(input,output,isGood,xdim));
	else
	  newsigma = findStddevDiff<float>(input,output,isGood,xdim);

	if(par.isVerbose()) printBackSpace(26);

      } while( (iteration==1) || 
	       (fabs(oldsigma-newsigma)/newsigma > par.getReconConvergence()) );

      if(par.isVerbose()) std::cout << "Completed "<<iteration<<" iterations. ";

      delete [] filter;
      // delete [] residual;
      delete [] wavelet;
      delete [] coeffs;

    }

    delete [] isGood;
    delete [] sigmaFactors;
  }

}
