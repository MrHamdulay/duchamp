// -----------------------------------------------------------------------
// growObject.cc: Grow a detected object down to a lower flux threshold.
// -----------------------------------------------------------------------
// Copyright (C) 2006, Matthew Whiting, ATNF
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// Duchamp is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with Duchamp; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
//
// Correspondence concerning Duchamp may be directed to:
//    Internet email: Matthew.Whiting [at] atnf.csiro.au
//    Postal address: Dr. Matthew Whiting
//                    Australia Telescope National Facility, CSIRO
//                    PO Box 76
//                    Epping NSW 1710
//                    AUSTRALIA
// -----------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <vector>
#include <duchamp/Cubes/cubeUtils.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/Statistics.hh>
#include <duchamp/PixelMap/Voxel.hh>
#include <duchamp/Detection/detection.hh>

using namespace PixelInfo;
using namespace Statistics;

namespace duchamp
{

  void growObject(Detection &object, Cube &cube)
  {
    ///  @details
    ///    A function to grow an object (given by the Detection) by
    ///    including neighbouring voxels out to some lower threshold than
    ///    what was previously used in the detection.  
    /// 
    ///    Each pixel has each of its neighbours examined, and if one of
    ///    them is not in the object but above the growth threshold, it
    ///    is added to the object.
    /// 
    ///    \param object Object to be grown.
    ///    \param cube Necessary to see both the Param list, containing
    ///    the growth threshold, and the actual array of pixel fluxes.

    if(cube.pars().getFlagGrowth()){

      bool doGrowing;
      if(cube.pars().getFlagUserGrowthThreshold()) 
	doGrowing = cube.pars().getGrowthThreshold()<cube.pars().getThreshold();
      else
	doGrowing = cube.pars().getGrowthCut()<cube.pars().getCut();

      if(doGrowing){

	std::vector <bool> isInObj(cube.getSize(),false);
	bool flagAdj = cube.pars().getFlagAdjacent();
	int threshS = int(cube.pars().getThreshS());
	if(flagAdj) threshS = 1;
	int threshV = int(cube.pars().getThreshV());

	std::vector<Voxel> voxlist = object.getPixelSet();
	int origSize = voxlist.size();
	long zero = 0;

// 	std::vector<Voxel>::iterator vox;
// 	for(vox=voxlist.begin();vox<voxlist.end();vox++){
// 	  long pos = vox->getX() + vox->getY()*cube.getDimX() + 
// 	    vox->getZ()*cube.getDimX()*cube.getDimY();
	for(size_t i=0; i<voxlist.size(); i++){
	  long pos = voxlist[i].getX() + voxlist[i].getY()*cube.getDimX() + 
	    voxlist[i].getZ()*cube.getDimX()*cube.getDimY();
	  isInObj[pos] = true;
	}
  
	StatsContainer<float> growthStats(cube.stats());

	if(cube.pars().getFlagUserGrowthThreshold())
	  growthStats.setThreshold(cube.pars().getGrowthThreshold());
	else
	  growthStats.setThresholdSNR(cube.pars().getGrowthCut());

	growthStats.setUseFDR(false);

	for(size_t i=0; i<voxlist.size(); i++){
// 	for(vox=voxlist.begin();vox<voxlist.end();vox++){ // for each pixel in the object

	  long xpt=voxlist[i].getX();
	  long ypt=voxlist[i].getY();
	  long zpt=voxlist[i].getZ();

	  int xmin = std::max(xpt - threshS, zero);
	  int xmax = std::min(xpt + threshS, cube.getDimX()-1);
	  int ymin = std::max(ypt - threshS, zero);
	  int ymax = std::min(ypt + threshS, cube.getDimY()-1);
	  int zmin = std::max(zpt - threshV, zero);
	  int zmax = std::min(zpt + threshV, cube.getDimZ()-1);

	  //loop over surrounding pixels.
	  for(int x=xmin; x<=xmax; x++){
	    for(int y=ymin; y<=ymax; y++){
	      for(int z=zmin; z<=zmax; z++){

		if((x!=xpt)||(y!=ypt)||(z!=zpt)){ 
		  // ignore when the current object pixel

		  long pos = x + y * cube.getDimX() + z * cube.getDimX() * cube.getDimY();

		  if(!isInObj[pos] && // pixel not already in object?
		     !cube.isBlank(x,y,z)   &&   // pixel not BLANK?
		     !cube.pars().isInMW(z)       &&   // pixel not MW?
		     (flagAdj || hypot(x-xpt,y-ypt)<threshS)  && // pixel not too far away spatially?
		     (flagAdj || fabs(z-zpt)<threshV) ){  //pixel not too far away spectrally?
	    
		    float flux;
		    if(cube.isRecon()) flux = cube.getReconValue(x,y,z);
		    else               flux = cube.getPixValue(x,y,z);

		    Voxel pixnew(x,y,z,flux);
		    
		    if(  growthStats.isDetection(flux) ){
		      isInObj[pos] = true;
		      voxlist.push_back(pixnew);
		    } // end of if

		  } // end of if clause regarding position OK

		} // end of if clause regarding position not same

	      } // end of z loop
	    } // end of y loop
	  } // end of x loop
      
	} // end of pix loop


	// Add in new pixels to the Detection
	for(size_t i=origSize; i<voxlist.size(); i++){
	  object.addPixel(voxlist[i]);
	}
      
      
	object.calcFluxes(cube.getArray(), cube.getDimArray());

	isInObj.clear();

      }

    }
  }
}
