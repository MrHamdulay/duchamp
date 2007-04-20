#include <iostream>
#include <iomanip>
#include <math.h>
#include <duchamp.hh>
#include <param.hh>
#include <Utils/utils.hh>
#include <Utils/feedback.hh>
#include <ATrous/atrous.hh>
#include <ATrous/filter.hh>
#include <Utils/Statistics.hh>
using Statistics::madfmToSigma;

void atrous1DReconstruct(long &xdim, float *&input, float *&output, Param &par)
{
  /**
   *  A routine that uses the a trous wavelet method to reconstruct a 
   *   1-dimensional spectrum. 
   *  The Param object "par" contains all necessary info about the filter and 
   *   reconstruction parameters.
   *
   *  If all pixels are BLANK (and we are testing for BLANKs), the
   *  reconstruction will simply give BLANKs back, so we return the
   *  input array as the output array.
   *
   *  \param xdim The length of the spectrum.
   *  \param input The input spectrum.
   *  \param output The returned reconstructed spectrum. This array needs to 
   *    be declared beforehand.
   *  \param par The Param set.
   */

  const float SNR_THRESH=par.getAtrousCut();
  const int MIN_SCALE=par.getMinScale();

  float blankPixValue = par.getBlankPixVal();
  int numScales = par.filter().getNumScales(xdim);
  double *sigmaFactors = new double[numScales+1];
  for(int i=0;i<=numScales;i++){
    if(i<=par.filter().maxFactor(1)) 
      sigmaFactors[i] = par.filter().sigmaFactor(1,i);
    else sigmaFactors[i] = sigmaFactors[i-1] / sqrt(2.);
  }

  float mean,sigma,originalSigma,originalMean,oldsigma,newsigma;
  bool *isGood = new bool[xdim];
  int goodSize=0;
  for(int pos=0;pos<xdim;pos++) {
    isGood[pos] = !par.isBlank(input[pos]);
    if(isGood[pos]) goodSize++;
  }

  if(goodSize == 0){
    // There are no good pixels -- everything is BLANK for some reason.
    // Return the input array as the output.

    for(int pos=0;pos<xdim; pos++) output[pos] = input[pos];

  }
  else{
    // Otherwise, all is good, and we continue.


    float *coeffs = new float[xdim];
    float *wavelet = new float[xdim];

    for(int pos=0;pos<xdim;pos++) output[pos]=0.;

    int filterHW = par.filter().width()/2;
    double *filter = new double[par.filter().width()];
    for(int i=0;i<par.filter().width();i++) filter[i] = par.filter().coeff(i);


    // No trimming done in 1D case.

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
      for(int i=0;i<xdim;i++)  coeffs[i] = input[i] - output[i];
    
      float *array = new float[xdim];
      goodSize=0;
      for(int i=0;i<xdim;i++) if(isGood[i]) array[goodSize++] = input[i];
      findMedianStats(array,goodSize,originalMean,originalSigma);
      originalSigma = madfmToSigma(originalSigma); 
      delete [] array;

      int spacing = 1;
      for(int scale = 1; scale<=numScales; scale++){

	if(par.isVerbose()) {
	  printBackSpace(12);
	  std::cout << "Scale " << std::setw(2) << scale
		    << " /"     << std::setw(2) << numScales <<std::flush;
	}

	for(int xpos = 0; xpos<xdim; xpos++){
	  // loops over each pixel in the image
	  int pos = xpos;

	  wavelet[pos] = coeffs[pos];
	
	  if(!isGood[pos] )  wavelet[pos] = 0.;
	  else{

	    for(int xoffset=-filterHW; xoffset<=filterHW; xoffset++){
	      int x = xpos + spacing*xoffset;

	      while((x<0)||(x>=xdim)){
		// boundary conditions are reflection. 
		if(x<0) x = 0 - x;
		else if(x>=xdim) x = 2*(xdim-1) - x;
	      }

	      int filterpos = (xoffset+filterHW);
	      int oldpos = x;

	      if(isGood[oldpos]) 
		wavelet[pos] -= filter[filterpos]*coeffs[oldpos];
	      
	    } //-> end of xoffset loop
	  } //-> end of else{ ( from if(!isGood[pos])  )
	    
	} //-> end of xpos loop

	// Need to do this after we've done *all* the convolving
	for(int pos=0;pos<xdim;pos++) coeffs[pos] = coeffs[pos] - wavelet[pos];

	// Have found wavelet coeffs for this scale -- now threshold
	if(scale>=MIN_SCALE){
	  array = new float[xdim];
	  goodSize=0;
	  for(int pos=0;pos<xdim;pos++) 
	    if(isGood[pos]) array[goodSize++] = wavelet[pos];
	  findMedianStats(array,goodSize,mean,sigma);
	  delete [] array;
	
	  for(int pos=0;pos<xdim;pos++){
	    // preserve the Blank pixel values in the output.
	    if(!isGood[pos]) output[pos] = blankPixValue;
	    else if( fabs(wavelet[pos]) > 
		     (mean+SNR_THRESH*originalSigma*sigmaFactors[scale]) )
	      output[pos] += wavelet[pos];
	  }
	}
 
	spacing *= 2;

      } //-> end of scale loop 

      for(int pos=0;pos<xdim;pos++) if(isGood[pos]) output[pos] += coeffs[pos];

      array = new float[xdim];
      goodSize=0;
      for(int i=0;i<xdim;i++)
	if(isGood[i]) array[goodSize++] = input[i] - output[i];
      findMedianStats(array,goodSize,mean,newsigma);
      newsigma = madfmToSigma(newsigma); 
      delete [] array;

      if(par.isVerbose()) printBackSpace(26);

    } while( (iteration==1) || 
	     (fabs(oldsigma-newsigma)/newsigma > reconTolerance) );

    if(par.isVerbose()) std::cout << "Completed "<<iteration<<" iterations. ";

    delete [] filter;
    delete [] coeffs;
    delete [] wavelet;

  }

  delete [] isGood;
  delete [] sigmaFactors;
}
