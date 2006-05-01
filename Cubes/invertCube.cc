#include <Cubes/cubes.hh>

void Cube::invert()
{
  /**
   *  Cube::invert()
   *   A function that multiplies all non-Blank pixels by -1. 
   *   This is used when searching for negative features.
   */
  for(int i=0; i<this->numPixels; i++)
    if(!this->isBlank(i)) this->array[i] *= -1.;
}

void Cube::reInvert()
{
  /**
   *  Cube::reInvert()
   *   A function that switches the array back to the original sign.
   *   Any objects will have the flux of each pixel inverted as well.
   *   This is used when searching for negative features.
   */
  for(int i=0; i<this->numPixels; i++){
    if(!this->isBlank(i)){
      this->array[i] *= -1.;
      if(this->reconExists) this->recon[i] *= -1.;
    }
  }

  for(int i=0; i<this->objectList.size(); i++){
    for(int pix=0; pix<this->objectList[i].getSize(); pix++){
      this->objectList[i].setF(pix, -1. * this->objectList[i].getF(pix) );
    }
    this->objectList[i].calcParams();
  }

}
