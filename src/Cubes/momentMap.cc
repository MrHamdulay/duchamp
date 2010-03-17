// -----------------------------------------------------------------------
// momentMap.cc : functions to calculate the momment-0 map of the cube
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
#include <sstream>
#include <math.h>
#include <string.h>
#include <cpgplot.h>
#include <wcslib/cpgsbox.h>
#include <wcslib/pgwcsl.h>
#include <wcslib/wcs.h>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/PixelMap/Object3D.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Utils/utils.hh>

using namespace PixelInfo;

namespace duchamp
{

  void Cube::getMomentMapForPlot(float *momentMap, float &z1, float &z2)
  {
    
    /// @details This returns the moment-0 map in a form suitable for
    /// plotting via pgplot. The map is transformed to log space, and
    /// the minimum & maximum values are also returned.
    /// @param momentMap The values of log10(momentMap). This array needs to be declared beforehand.
    /// @param z1 The minimum value of momentMap
    /// @param z2 The maximum value of momentMap


    long xdim=this->axisDim[0];
    long ydim=this->axisDim[1];

    std::vector<bool> detectedPixels;
    this->getMomentMap(momentMap, detectedPixels);

    int count=0;
    for(int i=0;i<xdim*ydim;i++) {
      if(momentMap[i]>0.){
	float logmm = log10(momentMap[i]);
	bool addPixel = detectedPixels[i];
	if(addPixel){
	  if(count==0) z1 = z2 = logmm;
	  else{
	    if(logmm < z1) z1 = logmm;
	    if(logmm > z2) z2 = logmm;
	  }
	  count++;
	}
      }
    }
    
    for(int i=0;i<xdim*ydim;i++) {
      bool addPixel = detectedPixels[i];
      addPixel = addPixel && (momentMap[i]>0.);
      if(!addPixel) momentMap[i] = z1-1.;
      else momentMap[i] = log10(momentMap[i]);
    }

  }

  void Cube::getMomentMap(float *momentMap, std::vector<bool> &detectedPixels)
  {

    /// @details This returns the moment-0 map (ie. the map of
    /// integrated flux) for the cube. The momentMap array needs to be
    /// allocated before calling this function - it should be of the
    /// same spatial dimensions as the cube. The function also returns
    /// a map showing which spatial pixels are detected. This provides
    /// a way of telling whether a given pixel is affected, as
    /// although the momentMap is initialised to zero, it is
    /// conceivable that a pixel with detections in its spectrum could
    /// yield a value of zero. The detection map is stored as a
    /// vector<bool> for compactness.
    /// @param momentMap The array holding the moment map values. Needs to be allocated beforehand.
    /// @param detectedPixel The map showing which spatial pixels contain an object.

    long xdim=this->axisDim[0];
    long ydim=this->axisDim[1];
    long zdim=this->axisDim[2];

    // get the list of objects that should be plotted. 
    std::vector<bool> objectChoice = this->par.getObjectChoices(this->objectList->size());

    bool *isObj = new bool[xdim*ydim*zdim];
    for(int i=0;i<xdim*ydim*zdim;i++) isObj[i] = false;
    for(size_t i=0;i<this->objectList->size();i++){
      if(objectChoice[i]){
	std::vector<Voxel> voxlist = this->objectList->at(i).getPixelSet();
	std::vector<Voxel>::iterator vox;
	for(vox=voxlist.begin();vox<voxlist.end();vox++){
	  int pixelpos = vox->getX() + xdim*vox->getY() + xdim*ydim*vox->getZ();
	  isObj[pixelpos] = true;
	}
      }
    }

    // Array containing the moment map, initialised to zero
    for(int i=0;i<xdim*ydim;i++) momentMap[i] = 0.;

    // Bool vector containing yes/no on whether a given spatial pixel has a detected object.
    // Initialise this to false everywhere.
    detectedPixels = std::vector<bool>(xdim*ydim,false);

    // if we are looking for negative features, we need to invert the 
    //  detected pixels for the moment map
    float sign = 1.;
    if(this->pars().getFlagNegative()) sign = -1.;

    float deltaVel;
    double x,y;

    double *zArray  = new double[zdim];
    for(int z=0; z<zdim; z++) zArray[z] = double(z);
    
    for(int pix=0; pix<xdim*ydim; pix++){ 

      x = double(pix%xdim);
      y = double(pix/xdim);

      if(!this->isBlank(pix) &&   // only do this for non-blank pixels. Judge this by the first pixel of the channel.
	 this->detectMap[pix] > 0 ){  // and only where something was detected in the spectrum.

	double * world = this->head.pixToVel(x,y,zArray,zdim);
      
	for(int z=0; z<zdim; z++){      
	  int pos =  z*xdim*ydim + pix;  // the voxel in the cube
	  if(isObj[pos]){ // if it's an object pixel...
	    // delta-vel is half the distance between adjacent channels.
	    // if at end, then just use 0-1 or (zdim-1)-(zdim-2) distance
	    if(z==0){
	      if(zdim==1) deltaVel=1.; // pathological case -- if 2D image.
	      else deltaVel = world[z+1] - world[z];
	    }
	    else if(z==(zdim-1)) deltaVel = world[z-1] - world[z];
	    else deltaVel = (world[z+1] - world[z-1]) / 2.;

	    momentMap[pix] += sign * this->array[pos] * fabs(deltaVel);
	    detectedPixels[pix] = true;

	  }
	}
	delete [] world;
      }

    }
    
    delete [] zArray;
    delete [] isObj;

  }


}
