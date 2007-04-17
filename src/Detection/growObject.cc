#include <iostream>
#include <iomanip>
#include <vector>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>
#include <Utils/Statistics.hh>
#include <PixelMap/Voxel.hh>
#include <Detection/detection.hh>

using std::vector;
using std::setw;

using namespace PixelInfo;

void growObject(Detection &object, Cube &cube)
{
  /** 
   *    A function to grow an object (given by the Detection) by
   *    including neighbouring voxels out to some lower threshold than
   *    what was previously used in the detection.  
   *
   *    Each pixel has each of its neighbours examined, and if one of
   *    them is not in the object but above the growth threshold, it
   *    is added to the object.
   *
   *    \param object Object to be grown.
   *    \param cube Necessary to see both the Param list, containing
   *    the growth threshold, and the actual array of pixel fluxes.
   */


  vector <bool> isInObj(cube.getSize(),false);
  bool flagAdj = cube.pars().getFlagAdjacent();
  int threshS = int(cube.pars().getThreshS());
  if(flagAdj) threshS = 1;
  int threshV = int(cube.pars().getThreshV());

  for(int i=0;i<object.getSize();i++) {
    Voxel vox = object.getPixel(i);
    long pos = vox.getX() + vox.getY()*cube.getDimX() + 
      vox.getZ()*cube.getDimX()*cube.getDimY();
    isInObj[pos] = true;
  }
  
  StatsContainer<float> growthStats = cube.getStats();

  growthStats.setThresholdSNR(cube.pars().getGrowthCut());
  growthStats.setUseFDR(false);
  
  for(int pix=0; pix<object.getSize(); pix++){ // for each pixel in the object

    for(int xnbr=-1*threshS; xnbr<=1*threshS; xnbr++){
      for(int ynbr=-1*threshS; ynbr<=1*threshS; ynbr++){
	for(int znbr=-1*threshV; znbr<=1*threshV; znbr++){

	  if((xnbr!=0)||(ynbr!=0)||(znbr!=0)){ 
	    // ignore when all=0 ie. the current object pixel

	    Voxel pixnew = object.getPixel(pix);
	    long newx = pixnew.getX() + xnbr;
	    long newy = pixnew.getY() + ynbr;
	    long newz = pixnew.getZ() + znbr;
	  
	    if((newx<cube.getDimX())&&(newx>=0)&&   // x in cube?
	       (newy<cube.getDimY())&&(newy>=0)&&   // y in cube?
	       (newz<cube.getDimZ())&&(newz>=0)&&   // z in cube?
	       !cube.isBlank(newx,newy,newz)   &&   // pixel not BLANK?
	       !cube.pars().isInMW(newz)       &&   // pixel not MW?
	       (flagAdj || hypot(xnbr,ynbr))     ){ // pixel not too far?
	    
	      pixnew.setX(newx);
	      pixnew.setY(newy);
	      pixnew.setZ(newz);

	      float flux;
	      if(cube.isRecon()) flux = cube.getReconValue(newx,newy,newz);
	      else               flux = cube.getPixValue(newx,newy,newz);
	      pixnew.setF(flux);

	      long pos = newx + newy * cube.getDimX() + 
		newz * cube.getDimX() * cube.getDimY();
	      if( (!isInObj[pos]) && growthStats.isDetection(flux) ){
		isInObj[pos] = true;
		object.addPixel(pixnew);
	      } // end of if

	    } // end of if clause regarding newx, newy, newz

	  } // end of if clause regarding xnbr, ynbr, znbr

	} // end of znbr loop
      } // end of ynbr loop
    } // end of xnbr loop
      
  } // end of pix loop

  object.calcFluxes(cube.getArray(), cube.getDimArray());

  isInObj.clear();

}

