/**
 *  spectrumDetect.cc
 *
 * A detection algorithm that searches in a single 1-D spectrum.
 *
 * INPUTS: 
 *    image     -- an Image object, containing a 1-D image that has been 
 *                 processed such that its pValue array is defined.
 * OUTPUTS:
 *   The detection and mask arrays in image will be filled, according to 
 *   the location of the objects in the image.
 *
 */

#include <Cubes/cubes.hh>
#include <Detection/detection.hh>

enum STATUS { NONOBJECT, OBJECT };

void Image::spectrumDetect() 
{
  STATUS status;
  Detection *obj = new Detection;
  Pixel *pix = new Pixel;
  bool isObject;

  status = NONOBJECT;
  for(int pos=0;pos<(this->axisDim[0]+1);pos++){

    isObject=false;
    if(pos<this->axisDim[0]){
      pix->setXYF(pos, 0, this->array[pos] );
      isObject = this->isDetection(pos,0);
    }

    if(isObject){
      if(status != OBJECT) status = OBJECT;
      obj->addPixel(*pix);
    }
    else{
      if(status == OBJECT){ // if we were on an object and have left
	if(obj->getSize() >= this->minSize){ // if it's big enough
	  obj->calcParams(); // work out midpoints, fluxes etc
	  this->addObject(*obj);  // add to list.
	  this->maskObject(*obj);
	}
	obj->clearDetection();
      }
      status = NONOBJECT;
    }

  }

  // clean up and remove declared memory
  delete obj,pix;
}
