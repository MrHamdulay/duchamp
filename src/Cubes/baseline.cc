#include <iostream>
#include <iomanip>
#include <param.hh>
#include <ATrous/atrous.hh>
#include <Cubes/cubes.hh>
#include <Utils/feedback.hh>

void Cube::removeBaseline()
{
  /**
   *  Cube::removeBaseline()
   *   A front-end to the getBaseline routine, specialised for the 
   *   Cube data structure. Calls getBaseline on each spectrum individually.
   *   Upon exit, the original array minus its spectral baseline is stored
   *    in this->array and the baseline is in this->baseline.
   *   If the reconstructed array exists, the baseline is subtracted from 
   *    it as well.
   */

  float *spec     = new float[this->axisDim[2]];
  float *thisBaseline = new float[this->axisDim[2]];
  int numSpec = this->axisDim[0]*this->axisDim[1];

  ProgressBar bar;
  if(this->par.isVerbose()) bar.init(numSpec);
  for(int pix=0; pix<numSpec; pix++){ // for each spatial pixel...

    if(this->par.isVerbose() ) bar.update(pix+1);

    for(int z=0; z<this->axisDim[2]; z++)  
      spec[z] = this->array[z*numSpec + pix];

    getBaseline(this->axisDim[2], spec, thisBaseline, this->par);

    for(int z=0; z<this->axisDim[2]; z++) {
      this->baseline[z*numSpec+pix] = thisBaseline[z];
      if(!par.isBlank(this->array[z*numSpec+pix])){
	this->array[z*numSpec+pix] -= thisBaseline[z];
	if(this->reconExists) this->recon[z*numSpec+pix] -= thisBaseline[z];
      }      
    }

  }  

  delete [] spec;
  delete [] thisBaseline;
  
  if(this->par.isVerbose()) bar.remove();
}



void Cube::replaceBaseline()
{
  /**
   *  Cube::replaceBaseline()
   *   A routine to replace the baseline flux on the reconstructed array 
   *    (if it exists) and the fluxes of each of the detected objects (if any).
   */

  if(this->par.getFlagBaseline()){

    for(int i=0;i<this->numPixels;i++){
      if(!(this->par.isBlank(this->array[i])))
	this->array[i] += this->baseline[i];
    }

    if(this->reconExists){ 
      // if we made a reconstruction, we need to add the baseline back in 
      //   for plotting purposes
      for(int i=0;i<this->numPixels;i++){
	if(!(this->par.isBlank(this->array[i])))
	  this->recon[i] += this->baseline[i];
      }
    }
 
    int pos;
    float flux;
    // Now add the baseline to the flux for all the objects.
    for(int obj=0;obj<this->objectList.size();obj++){ // for each detection
      for(int vox=0;vox<this->objectList[obj].getSize();vox++){ 
	// for each of its voxels

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
