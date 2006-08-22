#include <iostream>
#include <iomanip>
#include <math.h>
#include <ATrous/atrous.hh>
#include <Utils/utils.hh>

using std::endl;
using std::setw;

void atrous1DReconstruct(long &xdim, float *&input,float *&output, Param &par)
{
  /**
   *  atrous1DReconstruct(xdim, input, output, par)
   *
   *  A routine that uses the a trous wavelet method to reconstruct a 
   *   1-dimensional spectrum. 
   *  The Param object "par" contains all necessary info about the filter and 
   *   reconstruction parameters, although a Filter object has to be declared
   *   elsewhere previously.
   *  The input array is in "input", of length "xdim", and the reconstructed
   *   array is in "output".
   */


  extern Filter reconFilter;
  const float SNR_THRESH=par.getAtrousCut();
  const int MIN_SCALE=par.getMinScale();

  bool flagBlank=par.getFlagBlankPix();
  float blankPixValue = par.getBlankPixVal();
  int numScales = reconFilter.getNumScales(xdim);
  double *sigmaFactors = new double[numScales+1];
  for(int i=0;i<=numScales;i++){
    if(i<=reconFilter.maxFactor(1)) sigmaFactors[i] = reconFilter.sigmaFactor(1,i);
    else sigmaFactors[i] = sigmaFactors[i-1] / sqrt(2.);
  }

  float mean,sigma,originalSigma,originalMean,oldsigma,newsigma;
  bool *isGood = new bool[xdim];
  for(int pos=0;pos<xdim;pos++) 
    isGood[pos] = !par.isBlank(input[pos]);

  float *coeffs = new float[xdim];
  float *wavelet = new float[xdim];

  for(int pos=0;pos<xdim;pos++) output[pos]=0.;

  int filterHW = reconFilter.width()/2;
  double *filter = new double[reconFilter.width()];
  for(int i=0;i<reconFilter.width();i++) filter[i] = reconFilter.coeff(i);


  // No trimming done in 1D case.

  int iteration=0;
  newsigma = 1.e9;
  do{
    if(par.isVerbose()) std::cout << "Iteration #"<<++iteration<<":             ";
    // first, get the value of oldsigma and set it to the previous newsigma value
    oldsigma = newsigma;
    // all other times round, we are transforming the residual array
    for(int i=0;i<xdim;i++)  coeffs[i] = input[i] - output[i];
    
    float *array = new float[xdim];
    int goodSize=0;
    for(int i=0;i<xdim;i++) if(isGood[i]) array[goodSize++] = input[i];
    findMedianStats(array,goodSize,originalMean,originalSigma);
    originalSigma /= correctionFactor; // correct from MADFM to sigma estimator.
    delete [] array;

    int spacing = 1;
    for(int scale = 1; scale<=numScales; scale++){

      if(par.isVerbose()) {
	std::cout << "\b\b\b\b\b\b\b\b\b\b\b\bScale ";
	std::cout << setw(2)<<scale<<" /"<<setw(2)<<numScales<<std::flush;
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
	      if(x<0) x = 0 - x;                    // boundary conditions are  
	      else if(x>=xdim) x = 2*(xdim-1) - x;  //    reflection.               
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
	for(int pos=0;pos<xdim;pos++) if(isGood[pos]) array[goodSize++] = wavelet[pos];
	findMedianStats(array,goodSize,mean,sigma);
	delete [] array;
	
	for(int pos=0;pos<xdim;pos++){
	  // preserve the Blank pixel values in the output.
	  if(!isGood[pos]) output[pos] = blankPixValue;
	  else if(fabs(wavelet[pos])>(mean+SNR_THRESH*originalSigma*sigmaFactors[scale]))
	    output[pos] += wavelet[pos];
	}
      }
 
      spacing *= 2;

    } //-> end of scale loop 

    for(int pos=0;pos<xdim;pos++) if(isGood[pos]) output[pos] += coeffs[pos];

    array = new float[xdim];
    goodSize=0;
    for(int i=0;i<xdim;i++) if(isGood[i]) array[goodSize++] = input[i] - output[i];
    findMedianStats(array,goodSize,mean,newsigma);
    newsigma /= correctionFactor; // correct from MADFM to sigma estimator.
    delete [] array;

    if(par.isVerbose())     std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";

  } while( (iteration==1) || 
	   (fabs(oldsigma-newsigma)/newsigma > reconTolerance) );

  if(par.isVerbose()) std::cout << "Completed "<<iteration<<" iterations. ";

  delete [] coeffs;
  delete [] wavelet;
  delete [] isGood;
  delete [] filter;
  delete [] sigmaFactors;
}
