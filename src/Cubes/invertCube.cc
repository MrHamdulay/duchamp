#include <Cubes/cubes.hh>
#include <Detection/detection.hh>

void Cube::invert()
{
  /**
   *  A function that multiplies all non-Blank pixels by -1. 
   *  This is used when searching for negative features.
   */
  for(int i=0; i<this->numPixels; i++)
    if(!this->isBlank(i)){
      this->array[i] *= -1.;
      if(this->reconExists) this->recon[i] *= -1.;
    }
}

void Cube::reInvert()
{
  /**
   *  A function that switches the array back to the original sign.
   *  This is used when searching for negative features.
   */
  for(int i=0; i<this->numPixels; i++){
    if(!this->isBlank(i)){
      this->array[i] *= -1.;
      if(this->reconExists) this->recon[i] *= -1.;
    }
  }

  for(int i=0; i<this->objectList.size(); i++){
    this->objectList[i].setNegative(true);
    this->objectList[i].setTotalFlux(-1. * this->objectList[i].getTotalFlux() );
    this->objectList[i].setIntegFlux(-1. * this->objectList[i].getIntegFlux() );
    this->objectList[i].setPeakFlux(-1. * this->objectList[i].getPeakFlux() );
  }

}
