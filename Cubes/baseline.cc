#include <iostream>
#include <iomanip>
#include <ATrous/atrous.hh>
#include <Cubes/cubes.hh>
#include <param.hh>

void Cube::removeBaseline()
{
  /**
   *  Cube::removeBaseline()
   *   A front-end to the getBaseline routine, specialised for the 
   *   Cube data structure. Calls getBaseline on each spectrum individually.
   *   Upon exit, the original array minus its spectral baseline is stored
   *   If the reconstructed array exists, the baseline is subtracted from it as well.
   *   in this->array and the baseline is in this->baseline.
   */

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
      if(!par.isBlank(this->array[z*numSpec+pix])){
	this->array[z*numSpec+pix] -= thisBaseline[z];
	if(this->reconExists) this->recon[z*numSpec+pix] -= thisBaseline[z];
      }      
    }

  }  

  delete [] spec;
  delete [] thisBaseline;
  
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
