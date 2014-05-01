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

  void getSpecAbscissae(Detection &object, FitsHeader &head, size_t zdim, float *abscissae)
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

  void getSpecAbscissae(FitsHeader &head, float xpt, float ypt, size_t zdim, float *abscissae)
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

    void getIntSpec(Detection &object, float *fluxArray, size_t *dimArray, std::vector<bool> mask, 
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

    for(size_t i=0;i<dimArray[2];i++) spec[i] = 0.;
    size_t xySize = dimArray[0]*dimArray[1];
    std::vector<bool> done(xySize,false); 
    std::vector<Voxel> voxlist = object.getPixelSet();
    std::vector<Voxel>::iterator vox;
    for(vox=voxlist.begin();vox<voxlist.end();vox++){
      size_t pos = vox->getX() + dimArray[0] * vox->getY();
      if(!done[pos]){
	done[pos] = true;
	for(size_t z=0;z<dimArray[2];z++){
	  if(mask[pos+z*xySize]){
	    spec[z] += fluxArray[pos + z*xySize] / beamCorrection;
	  }	    
	}
      }
    }

  }
  //--------------------------------------------------------------------

    void getPeakSpec(Detection &object, float *fluxArray, size_t *dimArray, std::vector<bool> mask, float *spec)
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

    if((object.getXPeak()<0 || object.getXPeak()>=int(dimArray[0])) || (object.getYPeak()<0 || object.getYPeak()>=int(dimArray[1]))){
      DUCHAMPWARN("getPeakSpec","Object peak outside array boundaries");
      for (size_t z=0;z<dimArray[2];z++) spec[z]=0.;
    }
    else{
      size_t xySize = dimArray[0]*dimArray[1];
      size_t pos = object.getXPeak() + dimArray[0]*object.getYPeak();
      for(size_t z=0;z<dimArray[2];z++){
	if(mask[pos + z*xySize])
	  spec[z] = fluxArray[pos + z*xySize];
      }
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
    ///  \param objNum The number of the object under
    ///         consideration. If negative, we extract the single
    ///         spectrum at (x,y)=(0,0) (useful for the 1D case).
    ///  \param specx The array of frequency/velocity/channel/etc
    ///         values (the x-axis on the spectral plot).
    ///  \param specy The array of flux values, matching the specx
    ///         array.
    ///  \param specRecon The reconstructed or smoothed array, done in
    ///         the same way as specy.
    ///  \param specBase The fitted baseline values, done in the same
    ///         way as specy.

    size_t xdim = this->axisDim[0];
    size_t ydim = this->axisDim[1];
    size_t zdim = this->axisDim[2];
	
    for(size_t i=0;i<zdim;i++){
      specy[i]     = 0.;
      specRecon[i] = 0.;
      specBase[i]  = 0.;
    }

    double xloc,yloc;
    size_t spatpos=0;
    std::vector<Voxel> voxlist;
    if(objNum>=0){
      if(this->par.getSpectralMethod()=="sum"){
	xloc=double(this->objectList->at(objNum).getXcentre());
	yloc=double(this->objectList->at(objNum).getYcentre());
	voxlist = this->objectList->at(objNum).getPixelSet();
      }
      else{
	spatpos = this->objectList->at(objNum).getXPeak() +
	xdim*this->objectList->at(objNum).getYPeak();
      }
    }

    if(this->head.isWCS()){
//       double xval = double(this->objectList->at(objNum).getXcentre());
//       double yval = double(this->objectList->at(objNum).getYcentre());
//       for(double zval=0;zval<zdim;zval++)
// 	specx[int(zval)] = this->head.pixToVel(xval,yval,zval);
      for(double zval=0;zval<zdim;zval++)
	specx[int(zval)] = this->head.pixToVel(xloc,yloc,zval);
    }
    else
      for(double zval=0;zval<zdim;zval++) specx[int(zval)] = zval;
	
    float beamCorrection;
    if(this->header().needBeamSize())
      beamCorrection = this->head.beam().area();
    else beamCorrection = 1.;
	
    if(objNum>=0 && this->par.getSpectralMethod()=="sum"){
      std::vector<bool> done(xdim*ydim,false);
      std::vector<Voxel>::iterator vox;
      for(vox=voxlist.begin();vox<voxlist.end();vox++){
	spatpos = vox->getX() + xdim * vox->getY();
	if(!done[spatpos]){
	  done[spatpos] = true;
	  for(size_t z=0;z<zdim;z++){
	    if(!(this->isBlank(spatpos+z*xdim*ydim))){
	      specy[z] += this->array[spatpos + z*xdim*ydim] / beamCorrection;
	      if(this->reconExists)
		specRecon[z] += this->recon[spatpos + z*xdim*ydim] / beamCorrection;
	      if(this->par.getFlagBaseline())
		specBase[z] += this->baseline[spatpos + z*xdim*ydim] / beamCorrection;
	    }       
	  }
	}
      }
    }
    else {// if(par.getSpectralMethod()=="peak"){
//       size_t spatpos = this->objectList->at(objNum).getXPeak() +
// 	xdim*this->objectList->at(objNum).getYPeak();
      for(size_t z=0;z<zdim;z++){
	specy[z] = this->array[spatpos + z*xdim*ydim];
	if(this->reconExists)
	  specRecon[z] = this->recon[spatpos + z*xdim*ydim];
	if(this->par.getFlagBaseline())
	  specBase[z] = this->baseline[spatpos + z*xdim*ydim];
      }
    }

  }
  //--------------------------------------------------------------------

  void getSmallVelRange(Detection &obj, FitsHeader &head, 
			double *minvel, double *maxvel)
  {
    ///  @details
    ///  Routine to calculate the velocity range for the zoomed-in region.
    ///  This range should be the maximum of 20 pixels, or 3x the wdith of 
    ///   the detection.
    ///  Need to :
    ///      Calculate pixel width of a 3x-detection-width region.
    ///      If smaller than 20, calculate velocities of central vel +- 10 pixels
    ///      If not, use the 3x-detection-width
    ///  Range returned via "minvel" and "maxvel" parameters.
    ///  \param obj Detection under examination.
    ///  \param head FitsHeader, containing the WCS information.
    ///  \param minvel Returned value of minimum velocity
    ///  \param maxvel Returned value of maximum velocity

    double *pixcrd = new double[3];
    double *world  = new double[3];
    float minpix,maxpix;
    // define new velocity extrema 
    //    -- make it 3x wider than the width of the detection.
    *minvel = 0.5*(obj.getVelMin()+obj.getVelMax()) - 1.5*obj.getVelWidth();
    *maxvel = 0.5*(obj.getVelMin()+obj.getVelMax()) + 1.5*obj.getVelWidth();
    // Find velocity range in number of pixels:
    world[0] = obj.getRA();
    world[1] = obj.getDec();
    world[2] = head.velToSpec(*minvel);
    head.wcsToPix(world,pixcrd);
    minpix = pixcrd[2];
    world[2] = head.velToSpec(*maxvel);
    head.wcsToPix(world,pixcrd);
    maxpix = pixcrd[2];
    if(maxpix<minpix) std::swap(maxpix,minpix);
    
    if((maxpix - minpix + 1) < 20){
      pixcrd[0] = double(obj.getXcentre());
      pixcrd[1] = double(obj.getYcentre());
      pixcrd[2] = obj.getZcentre() - 10.;
      head.pixToWCS(pixcrd,world);
      //    *minvel = setVel_kms(wcs,world[2]);
      *minvel = head.specToVel(world[2]);
      pixcrd[2] = obj.getZcentre() + 10.;
      head.pixToWCS(pixcrd,world);
      //     *maxvel = setVel_kms(wcs,world[2]);
      *maxvel = head.specToVel(world[2]);
      if(*maxvel<*minvel) std::swap(*maxvel,*minvel);
    }
    delete [] pixcrd;
    delete [] world;

  }
  //--------------------------------------------------------------------

  void getSmallZRange(Detection &obj, double *minz, double *maxz)
  {
    ///  @details
    ///  Routine to calculate the pixel range for the zoomed-in spectrum.
    ///  This range should be the maximum of 20 pixels, or 3x the width 
    ///   of the detection.
    ///  Need to :
    ///      Calculate pixel width of a 3x-detection-width region.
    ///       If smaller than 20, use central pixel +- 10 pixels
    ///  Range returned via "minz" and "maxz" parameters.
    ///  \param obj Detection under examination.
    ///  \param minz Returned value of minimum z-pixel coordinate
    ///  \param maxz Returned value of maximum z-pixel coordinate

    *minz = 2.*obj.getZmin() - obj.getZmax();
    *maxz = 2.*obj.getZmax() - obj.getZmin();
    
    if((*maxz - *minz + 1) < 20){
      *minz = obj.getZcentre() - 10.;
      *maxz = obj.getZcentre() + 10.;
    }

  }
  //--------------------------------------------------------------------

}
