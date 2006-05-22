#include <iostream>
#include <iomanip>
#include <vector>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>

using std::vector;
using std::setw;

void growObject(Detection &object, Cube &cube)
{
  /** 
   *  growObject(Detection, Cube)
   *    A function to grow an object (given by the Detection)
   *     out to some lower threshold.
   *    The Cube is necessary to see both the Param list, containing
   *     the growth threshold, and the actual array of pixel fluxes.
   *    Each pixel has each of its neighbours examined, and if one of 
   *     them is not in the object but above the growth threshold, it
   *     is added to the object.
   */


  vector <bool> isInObj(cube.getSize(),false);
  float thresh1;
  float thresh2;
  long  chanpos;

  for(int i=0;i<object.getSize();i++) {
    long pos = object.getX(i) + object.getY(i)*cube.getDimX() + 
      object.getZ(i)*cube.getDimX()*cube.getDimY();
    isInObj[pos] = true;
  }
  float cut = cube.pars().getGrowthCut();
  
  for(int pix=0; pix<object.getSize(); pix++){ // for each pixel in the object

    for(int xnbr=-1; xnbr<=1; xnbr++){
      for(int ynbr=-1; ynbr<=1; ynbr++){
	for(int znbr=-1; znbr<=1; znbr++){

	  if((xnbr!=0)||(ynbr!=0)||(znbr!=0)){ 
	    // ignore when all=0 ie. the current object pixel

	    Voxel pixnew = object.getPixel(pix);
	    long newx = object.getX(pix) + xnbr;
	    long newy = object.getY(pix) + ynbr;
	    long newz = object.getZ(pix) + znbr;
	  
	    if((newx<cube.getDimX())&&(newx>=0)&&
	       (newy<cube.getDimY())&&(newy>=0)&&
	       (newz<cube.getDimZ())&&(newz>=0)){
	    
	      pixnew.setX(newx);
	      pixnew.setY(newy);
	      pixnew.setZ(newz);
	      // In following R=recon, B=baseline
	      // Four cases: i) B+R -- use recon
	      //            ii) R   -- use recon
	      //           iii) B   -- use array
	      //            iv) none-- use array
	      float flux;
	      if(cube.isRecon()) flux = cube.getReconValue(newx,newy,newz);
	      else  flux = cube.getPixValue(newx,newy,newz);
	      pixnew.setF(flux);

	      long chanpos = newx + newy * cube.getDimX();
	      long pos = newx + newy * cube.getDimX() + 
		newz * cube.getDimX() * cube.getDimY();
	      // thresh1 = spectral growth threshold
	      float thresh1 = cube.getSpecMean(chanpos) + cut * cube.getSpecSigma(chanpos);
	      // thresh2 = channel map growth threshold
	      float thresh2 = cube.getChanMean(newz) + cut * cube.getChanSigma(newz);
	      if( (!isInObj[pos]) && ( (flux > thresh1) || (flux > thresh2) ) ){
		isInObj[pos] = true;
		object.addPixel(pixnew);
	      } // end of if

	    } // end of if clause regarding newx, newy, newz

	  } // end of if clause regarding xnbr, ynbr, znbr

	} // end of znbr loop
      } // end of ynbr loop
    } // end of xnbr loop
      
  } // end of pix loop

  object.calcParams();

  isInObj.clear();

}



void growObject(Detection &object, Image &image)
{
  vector <bool> isInObj(image.getSize(),false);
  for(int i=0;i<object.getSize();i++) 
    isInObj[object.getX(i) + object.getY(i)*image.getDimX()] = true;

  for(int pix=0; pix<object.getSize(); pix++){ // for each pixel in the object

    for(int xnbr=-1; xnbr<=1; xnbr++){
      for(int ynbr=-1; ynbr<=1; ynbr++){
	if((xnbr!=0)||(ynbr!=0)){ // ignore when both=0 ie. the current object pixel

	  Voxel *pixnew = new Voxel;
	  *pixnew = object.getPixel(pix);
	  long newx = object.getX(pix) + xnbr;
	  long newy = object.getY(pix) + ynbr;
	  
	  if((newx<image.getDimX())&&(newy<image.getDimY())&&(newx>=0)&&(newy>=0)){
	    
	    pixnew->setX(newx);
	    pixnew->setY(newy);
	    pixnew->setF(image.getPixValue(newx,newy));
	    
	    if( (!isInObj[newx+newy*image.getDimX()]) && 
		(isDetection(pixnew->getF(), image.getMean(), 
			     image.getSigma(), image.pars().getGrowthCut())) ){
	      isInObj[newx+newy*image.getDimX()] = true;
	      object.addPixel(*pixnew);
	    }
	  }
	  delete pixnew;
	}
      }
    }

  }

  isInObj.clear();

}
