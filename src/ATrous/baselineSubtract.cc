#include <iostream>
#include <iomanip>
#include <math.h>
#include <param.hh>
#include <ATrous/atrous.hh>
#include <Utils/utils.hh>
#include <Utils/feedback.hh>

void baselineSubtract(long numSpec, long specLength, float *originalCube, 
		      float *baselineValues, Param &par)
{
  /** 
   *  baselineSubtract(long numSpec, long specLength, float *originalCube, 
   *                   float *baselineValues, Param &par)
   *    
   *    A routine to find the baseline of spectra in a cube (the spectral 
   *     direction is assumed to be the third dimension) and subtract it off 
   *     the original.
   *    The original cube has numSpec spatial pixels, each containing a 
   *     spectrum of length specLength.
   *    The original cube is read in, and returned with the baseline removed.
   *    This baseline is stored in the array baselineValues.
   *    The Param variable par is needed to test for blank pixels -- these are 
   *     kept as blank.
   */
  extern Filter reconFilter;
  float *spec     = new float[specLength];
  float *thisBaseline = new float[specLength];
  int minscale = par.getMinScale();
  par.setMinScale(reconFilter.getNumScales(specLength));
  float atrouscut = par.getAtrousCut();
  par.setAtrousCut(1);
  bool flagVerb = par.isVerbose();
  par.setVerbosity(false);

  ProgressBar bar;
  if(flagVerb) bar.init(numSpec);
  for(int pix=0; pix<numSpec; pix++){ // for each spatial pixel...

    if(flagVerb) bar.update(pix+1);

    for(int z=0; z<specLength; z++) 
      spec[z] = originalCube[z*numSpec + pix];

    atrous1DReconstruct(specLength,spec,thisBaseline,par);

    for(int z=0; z<specLength; z++) {
      baselineValues[z*numSpec+pix] = thisBaseline[z];
      if(!par.isBlank(originalCube[z*numSpec+pix])) 
	originalCube[z*numSpec+pix] = originalCube[z*numSpec+pix] - 
	  baselineValues[z*numSpec+pix];
    }

  }    
  if(flagVerb) bar.rewind();

  par.setMinScale(minscale);
  par.setAtrousCut(atrouscut);
  par.setVerbosity(flagVerb);
  
  delete [] spec;
  delete [] thisBaseline;
    
}

void getBaseline(long size, float *input, float *baseline, Param &par)
{
  /**
   *  getBaseline(long size, float *input, float *baseline, Param &par)
   *    A function to find the baseline of an input (1-D) spectrum.
   *    Uses the a trous reconstruction, keeping only the highest two scales, 
   *     to reconstruct the baseline.
   *    To avoid contamination by very strong signals, the input spectrum is 
   *     trimmed at 8*MADFM above the median before reconstruction. 
   *     This reduces the strong dips created by the presence of very strong 
   *     signals.
   *    The baseline array is returned -- no change is made to the input array.
   */

  extern Filter reconFilter;
  int minscale = par.getMinScale();
  par.setMinScale(reconFilter.getNumScales(size));
  float atrouscut = par.getAtrousCut();
  par.setAtrousCut(1);
  bool flagVerb = par.isVerbose();
  par.setVerbosity(false);

  float *spec = new float[size];
  float med,sig;
  findMedianStats(input,size,med,sig);
  float threshold = 8. * sig;
  for(int i=0;i<size;i++) {
    if(fabs(input[i]-med)>threshold){
      if(input[i]>med) spec[i] = med + threshold;
      else spec[i] = med - threshold;
    }
    else spec[i] = input[i];
  }

  //    atrous1DReconstruct(size, input, baseline, par);
  atrous1DReconstruct(size, spec, baseline, par);

  par.setMinScale(minscale);
  par.setAtrousCut(atrouscut);
  par.setVerbosity(flagVerb);

  delete [] spec;

}


void getBaseline(long size, float *input, float *baseline)
{
  /**
   *  getBaseline(long size, float *input, float *baseline)
   *    A function to find the baseline of an input (1-D) spectrum.
   *    This version is designed for programs not using Param classes -- it 
   *     keeps that side of things hidden from the user.
   *    Uses the a trous reconstruction, keeping only the highest two scales, 
   *     to reconstruct the baseline.
   *    To avoid contamination by very strong signals, the input spectrum is 
   *     trimmed at 8*MADFM above the median before reconstruction. This 
   *     reduces the strong dips created by the presence of very strong 
   *     signals.
   *    The baseline array is returned -- no change is made to the input array.
   */

  extern Filter reconFilter;
  Param par;
  par.setMinScale(reconFilter.getNumScales(size));
  par.setAtrousCut(1);
  par.setVerbosity(false);

  float *spec = new float[size];
  float med,sig;
  findMedianStats(input,size,med,sig);
  float threshold = 8. * sig;
  for(int i=0;i<size;i++) {
    if(fabs(input[i]-med)>threshold){
      if(input[i]>med) spec[i] = med + threshold;
      else spec[i] = med - threshold;
    }
    else spec[i] = input[i];
  }

  atrous1DReconstruct(size, spec, baseline, par);

  delete [] spec;

}

