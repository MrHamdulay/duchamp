#include <Cubes/cubes.hh>
#include <Detection/detection.hh>

enum STATUS { NONOBJECT, OBJECT };

void Image::spectrumDetect() 
{
/**
 *  A detection algorithm that searches in a single 1-D spectrum.  It
 *  simply scans along the spectrum, storing connected sets of
 *  detected pixels, where "detected" means according to the
 *  Image::isDetection(long,long) function.
 *
 *  Upon return, the detected objects are stored in the
 *  Image::objectList vector.
 *
 */

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
	}
	obj->clearDetection();
      }
      status = NONOBJECT;
    }

  }

  // clean up and remove declared memory
  delete obj;
  delete pix;
}
