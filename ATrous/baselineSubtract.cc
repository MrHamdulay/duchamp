#include <iostream>
#include <iomanip>
#include <ATrous/atrous.hh>
#include <Cubes/cubes.hh>
#include <param.hh>

void baselineSubtract(long numSpec, long specLength, float *originalCube, float *baselineValues, Param &par)
{
  /** 
   *  baselineSubtract(long numSpec, long specLength, float *originalCube, float *baselineValues, Param &par)
   *    
   *    A routine to find the baseline of spectra in a cube (spectral direction assumed 
   *    to be the third dimension) and subtract it off the original.
   *    The original cube has numSpec spatial pixels, each containing a spectrum of length specLength.
   *    The original cube is read in, and returned with the baseline removed.
   *    This baseline is stored in the array baselineValues.
   *    The Param variable par is needed to test for blank pixels -- these are kept as blank.
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

  std::cout << "|                    |" << std::flush;
  for(int pix=0; pix<numSpec; pix++){ // for each spatial pixel...

//     if(flagVerb) std::cout << std::setw(6) << pix << "\b\b\b\b\b\b" << std::flush;

    if(flagVerb && ((100*(pix+1)/numSpec)%5 == 0) ){
      std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b|";
      for(int i=0;i<(100*(pix+1)/numSpec)/5;i++) std::cout << "#";
      for(int i=(100*(pix+1)/numSpec)/5;i<20;i++) std::cout << " ";
      std::cout << "|" << std::flush;
    }

    for(int z=0; z<specLength; z++) 
      spec[z] = originalCube[z*numSpec + pix];

    atrous1DReconstruct(specLength,spec,thisBaseline,par);

    for(int z=0; z<specLength; z++) {
      baselineValues[z*numSpec+pix] = thisBaseline[z];
      if(!par.isBlank(originalCube[z*numSpec+pix])) 
	originalCube[z*numSpec+pix] = originalCube[z*numSpec+pix] - baselineValues[z*numSpec+pix];
    }

  }    
  if(flagVerb) std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";

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
   *    Uses the a trous reconstruction, keeping only the highest two scales, to
   *       reconstruct the baseline.
   *    To avoid contamination by very strong signals, the input spectrum is trimmed
   *       at 8*MADFM above the median before reconstruction. This reduces the strong
   *       dips created by the presence of very strong signals.
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
    if(fabsf(input[i]-med)>threshold){
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


void Cube::removeBaseline()
{
  /**
   *  Cube::removeBaseline()
   *   A front-end to the getBaseline routine, specialised for the 
   *   Cube data structure. Calls getBaseline on each spectrum individually.
   *   Upon exit, the original array minus its spectral baseline is stored
   *   in this->array and the baseline is in this->baseline.
   */

//   baselineSubtract(this->axisDim[0]*this->axisDim[1], this->axisDim[2],
// 		   this->array, this->baseline, this->par);


  float *spec     = new float[this->axisDim[2]];
  float *thisBaseline = new float[this->axisDim[2]];
  int numSpec = this->axisDim[0]*this->axisDim[1];

  std::cout << "|                    |" << std::flush;
  for(int pix=0; pix<numSpec; pix++){ // for each spatial pixel...

    if(this->par.isVerbose() && ((100*(pix+1)/numSpec)%5 == 0) ){
      std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b|";
      for(int i=0;i<(100*(pix+1)/numSpec)/5;i++) std::cout << "#";
      for(int i=(100*(pix+1)/numSpec)/5;i<20;i++) std::cout << " ";
      std::cout << "|" << std::flush;
    }

    for(int z=0; z<this->axisDim[2]; z++)  spec[z] = this->array[z*numSpec + pix];

    getBaseline(this->axisDim[2], spec, thisBaseline, this->par);

    for(int z=0; z<this->axisDim[2]; z++) {
      this->baseline[z*numSpec+pix] = thisBaseline[z];
      if(!par.isBlank(this->array[z*numSpec+pix])) this->array[z*numSpec+pix] -= thisBaseline[z];
    }

  }  
  
  if(this->par.isVerbose()) std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
}

void Cube::replaceBaseline()
{
  /**
   *  Cube::replaceBaseline()
   *   A routine to replace the baseline flux on the reconstructed array (if it exists)
   *   and the fluxes of each of the detected objects (if any).
   */

  if(this->par.getFlagBaseline()){

    for(int i=0;i<this->numPixels;i++){
      if(!(this->par.isBlank(this->array[i])))
	this->array[i] += this->baseline[i];
    }

    if(this->reconExists){ 
      // if we made a reconstruction, we need to add the baseline back in for plotting purposes
      for(int i=0;i<this->numPixels;i++){
	if(!(this->par.isBlank(this->array[i])))
	  this->recon[i] += this->baseline[i];
      }
    }
 
    int pos;
    float flux;
    // Now add the baseline to the flux for all the objects.
    for(int obj=0;obj<this->objectList.size();obj++){ // for each detection
      for(int vox=0;vox<this->objectList[obj].getSize();vox++){ // for each of its voxels

	pos = this->objectList[obj].getX(vox) + 
	  this->axisDim[0]*this->objectList[obj].getY(vox) + 
	  this->axisDim[0]*this->axisDim[1]*this->objectList[obj].getZ(vox);

	flux = this->objectList[obj].getF(vox) + this->baseline[pos];

	this->objectList[obj].setF(vox, flux);

      }
      this->objectList[obj].calcParams();  // correct the flux calculations.

    }
  
  }

}
