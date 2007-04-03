#include <Cubes/cubes.hh>
#include <PixelMap/Scan.hh>

using namespace PixelInfo;

enum STATUS { NONOBJECT, OBJECT };

std::vector<Scan> Image::spectrumDetect() 
{
/**
 *  A detection algorithm that searches in a single 1-D spectrum.  It
 *  simply scans along the spectrum, storing connected sets of
 *  detected pixels as Scans, where "detected" means according to the
 *  Image::isDetection(long,long) function.
 *
 *  When finished a vector of the detected scans is returned.
 *
 */

  STATUS status;
  Scan obj;
  std::vector<Scan> outputlist;
  bool isObject;

  status = NONOBJECT;
  for(int pos=0;pos<(this->axisDim[0]+1);pos++){

    if(pos<this->axisDim[0]){
      isObject = this->isDetection(pos,0);
    }
    else isObject=false;

    if(isObject){
      if(status != OBJECT){
	status = OBJECT;
	obj.define(0, pos, 1);
      }
      else obj.growRight();
    }
    else{
      if(status == OBJECT){ // if we were on an object and have left
	if(obj.getXlen() >= this->minSize){ // if it's big enough
	  outputlist.push_back(obj);  // add to list.
	}
	obj.clear();
      }
      status = NONOBJECT;
    }

  }

  return outputlist;
  
}
