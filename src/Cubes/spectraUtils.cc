// -----------------------------------------------------------------------
// spectraUtils.cc: Utility functions to obtain & manipulate spectra
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
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <math.h>
#include <wcslib/wcs.h>
#include <duchamp/Cubes/cubeUtils.hh>
#include <duchamp/param.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/PixelMap/Object3D.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Utils/utils.hh>

using namespace PixelInfo;

namespace duchamp
{

  void getSpecAbscissae(Detection &object, FitsHeader &head, long zdim, float *abscissae)
  {
    /// @details
    ///  A function that returns an array of
    ///  frequency/velocity/channel/etc values (that can be used as the
    ///  abscissae on the spectral plot).
    ///  \param object The object on which our spectrum is centered (in
    ///  case the spectral value changes with x & y
    ///  \param head The FitsHeader set of parameters that determine the coordinate transformation. 
    ///  \param zdim The length of the spectral axis
    ///  \param abscissae The array of spectral values -- must be allocated first

    getSpecAbscissae(head,object.getXcentre(),object.getYcentre(),zdim, abscissae);
  }

  void getSpecAbscissae(FitsHeader &head, float xpt, float ypt, long zdim, float *abscissae)
  {
    /// @details
    ///  A function that returns an array of
    ///  frequency/velocity/channel/etc values (that can be used as the
    ///  horizontal axis on the spectral plot).
    ///  \param head The FitsHeader set of parameters that determine the coordinate transformation. 
    ///  \param xpt The x-value of the spatial position on which our spectrum is centred.
    ///  \param ypt The y-value of the spatial position on which our spectrum is centred.
    ///  \param zdim The length of the spectral axis
    ///  \param abscissae The array of spectral values -- must be allocated first.

    if(head.isWCS()){
      double xval = double(xpt);
      double yval = double(ypt);
      for(double zval=0;zval<zdim;zval++) 
	abscissae[int(zval)] = head.pixToVel(xval,yval,zval);
    }
    else 
      for(double zval=0;zval<zdim;zval++) abscissae[int(zval)] = zval;

  }
  //--------------------------------------------------------------------

  void getIntSpec(Detection &object, float *fluxArray, long *dimArray, std::vector<bool> mask, 
		  float beamCorrection, float *spec)
  {
    /// @details
    ///  The base function that extracts an integrated spectrum for a
    ///  given object from a pixel array. The spectrum is returned as
    ///  the integrated flux, corrected for the beam using the given
    ///  correction factor.
    ///   \param object The Detection in question
    ///   \param fluxArray The full array of pixel values.
    ///   \param dimArray The axis dimensions for the fluxArray
    ///   \param mask A mask array indicating whether given pixels are valid
    ///   \param beamCorrection How much to divide the summed spectrum
    ///   by to return the integrated flux.
    ///   \param spec The integrated spectrum for the object -- must be allocated first.

    for(int i=0;i<dimArray[2];i++) spec[i] = 0.;
    long xySize = dimArray[0]*dimArray[1];
    bool *done = new bool[xySize]; 
    for(int i=0;i<xySize;i++) done[i]=false;
    std::vector<Voxel> voxlist = object.getPixelSet();
    std::vector<Voxel>::iterator vox;
    for(vox=voxlist.begin();vox<voxlist.end();vox++){
      int pos = vox->getX() + dimArray[0] * vox->getY();
      if(!done[pos]){
	done[pos] = true;
	for(int z=0;z<dimArray[2];z++){
	  if(mask[pos+z*xySize]){
	    spec[z] += fluxArray[pos + z*xySize] / beamCorrection;
	  }	    
	}
      }
    }
    delete [] done;

  }
  //--------------------------------------------------------------------

  void getPeakSpec(Detection &object, float *fluxArray, long *dimArray, bool *mask, float *spec)
  {
    /// @details
    ///  The base function that extracts an peak spectrum for a
    ///  given object from a pixel array. The spectrum is returned as
    ///  the integrated flux, corrected for the beam using the given
    ///  correction factor.
    ///   \param object The Detection in question
    ///   \param fluxArray The full array of pixel values.
    ///   \param dimArray The axis dimensions for the fluxArray
    ///   \param mask A mask array indicating whether given pixels are valid
    ///   \param spec The peak spectrum for the object -- must be allocated first

    long xySize = dimArray[0]*dimArray[1];
    int pos = object.getXPeak() + dimArray[0]*object.getYPeak();
    for(int z=0;z<dimArray[2];z++){
      if(mask[pos + z*xySize])
	spec[z] = fluxArray[pos + z*xySize];
    }
  }
  //--------------------------------------------------------------------


  void Cube::getSpectralArrays(int objNum, float *specx, float *specy, 
			       float *specRecon, float *specBase)
  {
    /// @details
    ///  A utility function that goes and calculates, for a given
    ///  Detection, the spectral arrays, according to whether we want
    ///  the peak or integrated flux. The arrays can be used by
    ///  Cube::plotSpectrum() and Cube::writeSpectralData(). The arrays
    ///  calculated are listed below. Their length is given by the
    ///  length of the Cube's spectral dimension.
    /// 
    ///  Note that the arrays need to be allocated prior to calling
    ///  this function.
    /// 
    ///  \param objNum The number of the object under consideration
    ///  \param specx The array of frequency/velocity/channel/etc
    ///         values (the x-axis on the spectral plot).
    ///  \param specy The array of flux values, matching the specx
    ///         array.
    ///  \param specRecon The reconstructed or smoothed array, done in
    ///         the same way as specy.
    ///  \param specBase The fitted baseline values, done in the same
    ///         way as specy.

    long xdim = this->axisDim[0];
    long ydim = this->axisDim[1];
    long zdim = this->axisDim[2];
	
    for(int i=0;i<zdim;i++) specy[i]     = 0.;
    for(int i=0;i<zdim;i++) specRecon[i] = 0.;
    for(int i=0;i<zdim;i++) specBase[i]  = 0.;
	
    if(this->head.isWCS()){
      double xval = double(this->objectList->at(objNum).getXcentre());
      double yval = double(this->objectList->at(objNum).getYcentre());
      for(double zval=0;zval<zdim;zval++)
	specx[int(zval)] = this->head.pixToVel(xval,yval,zval);
    }
    else
      for(double zval=0;zval<zdim;zval++) specx[int(zval)] = zval;
	
    float beamCorrection;
    if(this->header().needBeamSize())
      beamCorrection = this->head.beam().area();
    else beamCorrection = 1.;
	
    if(this->par.getSpectralMethod()=="sum"){
      bool *done = new bool[xdim*ydim];
      for(int i=0;i<xdim*ydim;i++) done[i]=false;
      std::vector<Voxel> voxlist = this->objectList->at(objNum).getPixelSet();
      std::vector<Voxel>::iterator vox;
      for(vox=voxlist.begin();vox<voxlist.end();vox++){
	int pos = vox->getX() + xdim * vox->getY();
	if(!done[pos]){
	  done[pos] = true;
	  for(int z=0;z<zdim;z++){
	    if(!(this->isBlank(pos+z*xdim*ydim))){
	      specy[z] += this->array[pos + z*xdim*ydim] / beamCorrection;
	      if(this->reconExists)
		specRecon[z] += this->recon[pos + z*xdim*ydim] / beamCorrection;
	      if(this->par.getFlagBaseline())
		specBase[z] += this->baseline[pos + z*xdim*ydim] / beamCorrection;
	    }       
	  }
	}
      }
      delete [] done;
    }
    else {// if(par.getSpectralMethod()=="peak"){
      int pos = this->objectList->at(objNum).getXPeak() +
	xdim*this->objectList->at(objNum).getYPeak();
      for(int z=0;z<zdim;z++){
	specy[z] = this->array[pos + z*xdim*ydim];
	if(this->reconExists)
	  specRecon[z] = this->recon[pos + z*xdim*ydim];
	if(this->par.getFlagBaseline())
	  specBase[z] = this->baseline[pos + z*xdim*ydim];
      }
    }

//     long zdim = this->axisDim[2];
//     Detection obj = this->objectList->at(objNum);
//     getSpecAbscissae(obj, this->head, zdim, specx);

//     float beamCorrection;
//     if(this->header().needBeamSize())
//       beamCorrection = this->par.getBeamSize();
//     else beamCorrection = 1.;

//     bool *mask = this->makeBlankMask();
//     if(!this->reconExists) 
//       for(int i=0;i<this->axisDim[2];i++) specRecon[i] = 0.;
//     if(!this->par.getFlagBaseline())
//       for(int i=0;i<this->axisDim[2];i++) specBase[i]  = 0.;

//     if(this->par.getSpectralMethod()=="sum"){
//       getIntSpec(obj, this->array, this->axisDim, mask, beamCorrection, specy);
//       if(this->reconExists){
// 	getIntSpec(obj, this->recon, this->axisDim, mask, beamCorrection, specRecon);
//       }
//       if(this->par.getFlagBaseline()){
// 	getIntSpec(obj, this->baseline, this->axisDim, mask, beamCorrection, specBase);
//       }
//     }
//     else{ // if(.getSpectralMethod()=="peak"){
//       getPeakSpec(obj, this->array, this->axisDim, mask, specy);
//       if(this->reconExists)
// 	getPeakSpec(obj, this->recon, this->axisDim, mask, specRecon);
//       if(this->par.getFlagBaseline())
// 	getPeakSpec(obj, this->baseline, this->axisDim, mask, specBase);
//     }

  }
  //--------------------------------------------------------------------

}
