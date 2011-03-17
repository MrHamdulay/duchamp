// -----------------------------------------------------------------------
// detection.cc : Member functions for the Detection class.
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
#include <string>
#include <wcslib/wcs.h>
#include <math.h>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/PixelMap/Voxel.hh>
#include <duchamp/PixelMap/Object3D.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Cubes/cubeUtils.hh>
#include <duchamp/Detection/columns.hh>

using namespace PixelInfo;

namespace duchamp
{

  void Detection::defaultDetection()
  {
    this->xSubOffset = 0;
    this->ySubOffset = 0;
    this->zSubOffset = 0;
    this->haveParams = false;
    this->totalFlux = 0.;
    this->peakFlux = 0.;
    this->intFlux = 0.;
    this->xpeak = 0;
    this->ypeak = 0;
    this->zpeak = 0;
    this->peakSNR = 0.;
    this->xCentroid = 0.;
    this->yCentroid = 0.;
    this->zCentroid = 0.;
    this->centreType="centroid";
    this->negSource = false; 
    this->flagText="";
    this->id = -1;
    this->name = "";
    this->flagWCS=false; 
    this->specOK = true;
    this->raS = "";
    this->decS = "";
    this->ra = 0.;
    this->dec = 0.;
    this->raWidth = 0.;
    this->decWidth = 0.;
    this->majorAxis = 0.;
    this->minorAxis = 0.;
    this->posang = 0.;
    this->specUnits = "";
    this->fluxUnits = "";
    this->intFluxUnits = "";
    this->lngtype = "RA";
    this->lattype = "DEC";
    this->vel = 0.;
    this->velWidth = 0.;
    this->velMin = 0.;
    this->velMax = 0.;
    this->w20 = 0.;
    this->v20min = 0.;
    this->v20max = 0.;
    this->w50 = 0.;
    this->v50min = 0.;
    this->v50max = 0.;
    this->posPrec = Column::prPOS;
    this->xyzPrec = Column::prXYZ;
    this->fintPrec = Column::prFLUX;
    this->fpeakPrec = Column::prFLUX;
    this->velPrec = Column::prVEL;
    this->snrPrec = Column::prSNR;
  }

  Detection::Detection():
    Object3D()
  {
    this->defaultDetection();
  }

  Detection::Detection(const Object3D& o):
    Object3D(o)
  {
    this->defaultDetection();
  }

  Detection::Detection(const Detection& d):
    Object3D(d)
  {
    operator=(d);
  }

  Detection& Detection::operator= (const Detection& d)
  {
    ((Object3D &) *this) = d;
    this->xSubOffset   = d.xSubOffset;
    this->ySubOffset   = d.ySubOffset;
    this->zSubOffset   = d.zSubOffset;
    this->haveParams   = d.haveParams;
    this->totalFlux    = d.totalFlux;
    this->intFlux      = d.intFlux;
    this->peakFlux     = d.peakFlux;
    this->xpeak        = d.xpeak;
    this->ypeak        = d.ypeak;
    this->zpeak        = d.zpeak;
    this->peakSNR      = d.peakSNR;
    this->xCentroid    = d.xCentroid;
    this->yCentroid    = d.yCentroid;
    this->zCentroid    = d.zCentroid;
    this->centreType   = d.centreType;
    this->negSource    = d.negSource;
    this->flagText     = d.flagText;
    this->id           = d.id;
    this->name         = d.name;
    this->flagWCS      = d.flagWCS;
    this->specOK       = d.specOK;
    this->raS          = d.raS;
    this->decS         = d.decS;
    this->ra           = d.ra;
    this->dec	       = d.dec;
    this->raWidth      = d.raWidth;
    this->decWidth     = d.decWidth;
    this->majorAxis    = d.majorAxis;
    this->minorAxis    = d.minorAxis;
    this->posang       = d.posang;
    this->specUnits    = d.specUnits;
    this->fluxUnits    = d.fluxUnits;
    this->intFluxUnits = d.intFluxUnits;
    this->lngtype      = d.lngtype;
    this->lattype      = d.lattype;
    this->vel          = d.vel;
    this->velWidth     = d.velWidth;
    this->velMin       = d.velMin;
    this->velMax       = d.velMax;
    this->w20          = d.w20;
    this->v20min       = d.v20min;
    this->v20max       = d.v20max;
    this->w50          = d.w50;
    this->v50min       = d.v50min;
    this->v50max       = d.v50max;
    this->posPrec      = d.posPrec;
    this->xyzPrec      = d.xyzPrec;
    this->fintPrec     = d.fintPrec;
    this->fpeakPrec    = d.fpeakPrec;
    this->velPrec      = d.velPrec;
    this->snrPrec      = d.snrPrec;
    return *this;
  }

  //--------------------------------------------------------------------
  float Detection::getXcentre()
  {
    if(this->centreType=="peak") return this->xpeak;
    else if(this->centreType=="average") return this->getXaverage();
    else return this->xCentroid;
  }

  float Detection::getYcentre()
  {
    if(this->centreType=="peak") return this->ypeak;
    else if(this->centreType=="average") return this->getYaverage();
    else return this->yCentroid;
  }

  float Detection::getZcentre()
  {
    if(this->centreType=="peak") return this->zpeak;
    else if(this->centreType=="average") return this->getZaverage();
    else return this->zCentroid;
  }

  //--------------------------------------------------------------------

  bool Detection::voxelListsMatch(std::vector<Voxel> voxelList)
  {
    /// @details
    ///  A test to see whether there is a 1-1 correspondence between
    ///  the given list of Voxels and the voxel positions contained in
    ///  this Detection's pixel list. No testing of the fluxes of the
    ///  Voxels is done.
    /// 
    /// \param voxelList The std::vector list of Voxels to be tested.

    bool listsMatch = true;
    // compare sizes
    listsMatch = listsMatch && (voxelList.size() == this->getSize());
    if(!listsMatch) return listsMatch;

    // make sure all Detection pixels are in voxel list
    listsMatch = listsMatch && this->voxelListCovered(voxelList);

    // make sure all voxels are in Detection
    std::vector<Voxel>::iterator vox;
    for(vox=voxelList.begin();vox<voxelList.end();vox++)
      listsMatch = listsMatch && this->isInObject(*vox);

    return listsMatch;

  }
  //--------------------------------------------------------------------

  bool Detection::voxelListCovered(std::vector<Voxel> voxelList)
  {
    ///  @details
    ///  A test to see whether the given list of Voxels contains each
    ///  position in this Detection's pixel list. It does not look for
    ///  a 1-1 correspondence: the given list can be a super-set of the
    ///  Detection. No testing of the fluxes of the Voxels is done.
    /// 
    /// \param voxelList The std::vector list of Voxels to be tested.

    bool listsMatch = true;

    // make sure all Detection pixels are in voxel list
    size_t v1=0;
    std::vector<Voxel> detpixlist = this->getPixelSet();
    while(listsMatch && v1<detpixlist.size()){
      bool inList = false;
      size_t v2=0;
      while(!inList && v2<voxelList.size()){
	inList = inList || detpixlist[v1].match(voxelList[v2]);
	v2++;
      }
      listsMatch = listsMatch && inList;
      v1++;
    }

    return listsMatch;

  }
  //--------------------------------------------------------------------

  void Detection::calcFluxes(std::vector<Voxel> voxelList)
  {
    ///  @details
    ///  A function that calculates total & peak fluxes (and the location
    ///  of the peak flux) for a Detection.
    /// 
    ///  \param fluxArray The array of flux values to calculate the
    ///  flux parameters from.
    ///  \param dim The dimensions of the flux array.
    
    //    this->haveParams = true;

    this->totalFlux = this->peakFlux = 0;
    this->xCentroid = this->yCentroid = this->zCentroid = 0.;

    // first check that the voxel list and the Detection's pixel list
    // have a 1-1 correspondence

    if(!this->voxelListCovered(voxelList)){
      duchampError("Detection::calcFluxes","Voxel list provided does not match");
      return;
    }

    std::vector<Voxel>::iterator vox;
    for(vox=voxelList.begin();vox<voxelList.end();vox++){
      if(this->isInObject(*vox)){
	long x = vox->getX();
	long y = vox->getY();
	long z = vox->getZ();
	float f = vox->getF();
	this->totalFlux += f;
	this->xCentroid += x*f;
	this->yCentroid += y*f;
	this->zCentroid += z*f;
	if( (vox==voxelList.begin()) ||  //first time round
	    (this->negSource&&(f<this->peakFlux)) || 
	    (!this->negSource&&(f>this->peakFlux))   )
	  {
	    this->peakFlux = f;
	    this->xpeak =    x;
	    this->ypeak =    y;
	    this->zpeak =    z;
	  }
      }
    }

    this->xCentroid /= this->totalFlux;
    this->yCentroid /= this->totalFlux;
    this->zCentroid /= this->totalFlux;
  }
  //--------------------------------------------------------------------

  void Detection::calcFluxes(float *fluxArray, long *dim)
  {
    ///  @details
    ///  A function that calculates total & peak fluxes (and the location
    ///  of the peak flux) for a Detection.
    /// 
    ///  \param fluxArray The array of flux values to calculate the
    ///  flux parameters from.
    ///  \param dim The dimensions of the flux array.

    //    this->haveParams = true;

    this->totalFlux = this->peakFlux = 0;
    this->xCentroid = this->yCentroid = this->zCentroid = 0.;

    std::vector<Voxel> voxList = this->getPixelSet();
    std::vector<Voxel>::iterator vox=voxList.begin();
    for(;vox<voxList.end();vox++){

      long x=vox->getX();
      long y=vox->getY();
      long z=vox->getZ();
      long ind = vox->arrayIndex(dim);
      float f = fluxArray[ind];
      this->totalFlux += f;
      this->xCentroid += x*f;
      this->yCentroid += y*f;
      this->zCentroid += z*f;
      if( (vox==voxList.begin()) ||
	  (this->negSource&&(f<this->peakFlux)) || 
	  (!this->negSource&&(f>this->peakFlux))   )
	{
	  this->peakFlux = f;
	  this->xpeak = x;
	  this->ypeak = y;
	  this->zpeak = z;
	}
 
    }

    this->xCentroid /= this->totalFlux;
    this->yCentroid /= this->totalFlux;
    this->zCentroid /= this->totalFlux;
  }
  //--------------------------------------------------------------------

  void Detection::calcWCSparams(FitsHeader &head)
  {
    ///  @details
    ///  Use the input wcs to calculate the position and velocity 
    ///    information for the Detection.
    ///  Quantities calculated:
    ///  <ul><li> RA: ra [deg], ra (string), ra width.
    ///      <li> Dec: dec [deg], dec (string), dec width.
    ///      <li> Vel: vel [km/s], min & max vel, vel width.
    ///      <li> coord type for all three axes, nuRest, 
    ///      <li> name (IAU-style, in equatorial or Galactic) 
    ///  </ul>
    /// 
    ///  Note that the regular parameters are NOT recalculated!
    /// 
    ///  \param head FitsHeader object that contains the WCS information.

    if(head.isWCS()){

      double *pixcrd = new double[15];
      double *world  = new double[15];
      /*
	define a five-point array in 3D:
	(x,y,z), (x,y,z1), (x,y,z2), (x1,y1,z), (x2,y2,z)
	[note: x = central point, x1 = minimum x, x2 = maximum x etc.]
	and convert to world coordinates.   
      */
      pixcrd[0]  = pixcrd[3] = pixcrd[6] = this->getXcentre();
      pixcrd[9]  = this->getXmin()-0.5;
      pixcrd[12] = this->getXmax()+0.5;
      pixcrd[1]  = pixcrd[4] = pixcrd[7] = this->getYcentre();
      pixcrd[10] = this->getYmin()-0.5;
      pixcrd[13] = this->getYmax()+0.5;
      pixcrd[2] = pixcrd[11] = pixcrd[14] = this->getZcentre();
      pixcrd[5] = this->getZmin();
      pixcrd[8] = this->getZmax();
      int flag = head.pixToWCS(pixcrd, world, 5);
      delete [] pixcrd;
      if(flag!=0) duchampError("calcWCSparams",
			       "Error in calculating the WCS for this object.\n");
      else{

	// world now has the WCS coords for the five points 
	//    -- use this to work out WCS params
  
	this->haveParams = true;

	this->specOK = head.canUseThirdAxis();
	this->lngtype = head.WCS().lngtyp;
	this->lattype = head.WCS().lattyp;
	this->specUnits = head.getSpectralUnits();
	this->fluxUnits = head.getFluxUnits();
	// if fluxUnits are eg. Jy/beam, make intFluxUnits = Jy km/s
	this->intFluxUnits = head.getIntFluxUnits();
	this->ra   = world[0];
	this->dec  = world[1];
	this->raS  = decToDMS(this->ra, this->lngtype);
	this->decS = decToDMS(this->dec,this->lattype);
	this->raWidth = angularSeparation(world[9],world[1],
					  world[12],world[1]) * 60.;
	this->decWidth  = angularSeparation(world[0],world[10],
					    world[0],world[13]) * 60.;

	Object2D spatMap = this->getSpatialMap();
	std::pair<double,double> axes = spatMap.getPrincipleAxes();
	this->majorAxis = std::max(axes.first,axes.second) * head.getAvPixScale();
	this->minorAxis = std::min(axes.first,axes.second) * head.getAvPixScale();
	this->posang = spatMap.getPositionAngle() * 180. / M_PI;

	this->name = head.getIAUName(this->ra, this->dec);
	this->vel    = head.specToVel(world[2]);
	this->velMin = head.specToVel(world[5]);
	this->velMax = head.specToVel(world[8]);
	this->velWidth = fabs(this->velMax - this->velMin);

	this->flagWCS = true;
      }
      delete [] world;

    }
  }
  //--------------------------------------------------------------------

  void Detection::calcIntegFlux(long zdim, std::vector<Voxel> voxelList, FitsHeader &head)
  {
    ///  @details
    ///  Uses the input WCS to calculate the velocity-integrated flux, 
    ///   putting velocity in units of km/s.
    ///  The fluxes used are taken from the Voxels, rather than an
    ///   array of flux values.
    ///  Integrates over full spatial and velocity range as given 
    ///   by the extrema calculated by calcWCSparams.
    /// 
    ///  If the flux units end in "/beam" (eg. Jy/beam), then the flux is
    ///  corrected by the beam size (in pixels). This is done by
    ///  multiplying the integrated flux by the number of spatial pixels,
    ///  and dividing by the beam size in pixels (e.g. Jy/beam * pix /
    ///  pix/beam --> Jy)
    /// 
    ///  \param zdim The size of the spectral axis (needed to find the velocity widths)
    ///  \param voxelList The list of Voxels with flux information
    ///  \param head FitsHeader object that contains the WCS information.

    const int border = 1;

    if(!this->voxelListCovered(voxelList)){
      duchampError("Detection::calcIntegFlux","Voxel list provided does not match");
      return;
    }

    if(!head.is2D()){

      this->haveParams = true;

      // include one pixel either side in each direction
      long xsize = (this->getXmax()-this->getXmin()+border*2+1);
      long ysize = (this->getYmax()-this->getYmin()+border*2+1);
      long zsize = (this->getZmax()-this->getZmin()+border*2+1); 
      long size = xsize*ysize*zsize;
      std::vector <bool> isObj(size,false);
      double *localFlux = new double[size];
      for(int i=0;i<size;i++) localFlux[i]=0.;

      std::vector<Voxel>::iterator vox;
      for(vox=voxelList.begin();vox<voxelList.end();vox++){
	if(this->isInObject(*vox)){
	  long x = vox->getX();
	  long y = vox->getY();
	  long z = vox->getZ();
	  long pos = (x-this->getXmin()+border) + (y-this->getYmin()+border)*xsize
	    + (z-this->getZmin()+border)*xsize*ysize;
	  localFlux[pos] = vox->getF();
	  isObj[pos] = true;
	}
      }
  
      // work out the WCS coords for each pixel
      double *world  = new double[size];
      double xpt,ypt,zpt;
      for(int i=0;i<xsize*ysize*zsize;i++){
	xpt = double( this->getXmin() - border + i%xsize );
	ypt = double( this->getYmin() - border + (i/xsize)%ysize );
	zpt = double( this->getZmin() - border + i/(xsize*ysize) );
	world[i] = head.pixToVel(xpt,ypt,zpt);
      }

      double integrated = 0.;
      for(int pix=0; pix<xsize*ysize; pix++){ // loop over each spatial pixel.
	for(int z=0; z<zsize; z++){
	  int pos =  z*xsize*ysize + pix;
	  if(isObj[pos]){ // if it's an object pixel...
	    double deltaVel;
	    if(z==0) 
	      deltaVel = (world[pos+xsize*ysize] - world[pos]);
	    else if(z==(zsize-1)) 
	      deltaVel = (world[pos] - world[pos-xsize*ysize]);
	    else 
	      deltaVel = (world[pos+xsize*ysize] - world[pos-xsize*ysize]) / 2.;
	    integrated += localFlux[pos] * fabs(deltaVel);
	  }
	}
      }
      this->intFlux = integrated;

      delete [] world;
      delete [] localFlux;

      calcVelWidths(zdim,voxelList,head);

    }
    else // in this case there is just a 2D image.
      this->intFlux = this->totalFlux;

    if(head.isWCS()){
      // correct for the beam size if the flux units string ends in "/beam"
      if(head.needBeamSize()) this->intFlux  /= head.beam().area();
    }

  }
  //--------------------------------------------------------------------

  void Detection::calcIntegFlux(float *fluxArray, long *dim, FitsHeader &head)
  {
    ///  @details
    ///  Uses the input WCS to calculate the velocity-integrated flux, 
    ///   putting velocity in units of km/s.
    ///  Integrates over full spatial and velocity range as given 
    ///   by the extrema calculated by calcWCSparams.
    /// 
    ///  If the flux units end in "/beam" (eg. Jy/beam), then the flux is
    ///  corrected by the beam size (in pixels). This is done by
    ///  multiplying the integrated flux by the number of spatial pixels,
    ///  and dividing by the beam size in pixels (e.g. Jy/beam * pix /
    ///  pix/beam --> Jy)
    /// 
    ///  \param fluxArray The array of flux values.
    ///  \param dim The dimensions of the flux array.
    ///  \param head FitsHeader object that contains the WCS information.

    if(!head.is2D()){

      this->haveParams = true;

      // include one pixel either side in each direction
      long xsize = (this->xmax-this->xmin+3);
      long ysize = (this->ymax-this->ymin+3);
      long zsize = (this->zmax-this->zmin+3); 
      long size = xsize*ysize*zsize;
      std::vector <bool> isObj(size,false);
      double *localFlux = new double[size];
      for(int i=0;i<size;i++) localFlux[i]=0.;
      // work out which pixels are object pixels
      std::vector<Voxel> voxlist = this->getPixelSet();
      for(std::vector<Voxel>::iterator v=voxlist.begin();v<voxlist.end();v++){
	long pos=(v->getX()-this->xmin+1) + (v->getY()-this->ymin+1)*xsize
	  + (v->getZ()-this->zmin+1)*xsize*ysize;
	localFlux[pos] = fluxArray[v->arrayIndex(dim)];
	isObj[pos] = true;
      }
  
      // work out the WCS coords for each pixel
      double *world  = new double[size];
      double xpt,ypt,zpt;
      int i=0;
      for(int z=0;z<zsize;z++){
	for(int y=0;y<ysize;y++){
	  for(int x=0;x<xsize;x++){
	    xpt=double(this->xmin - 1 + x);
	    ypt=double(this->ymin - 1 + y);
	    zpt=double(this->zmin - 1 + z);
	    world[i++] = head.pixToVel(xpt,ypt,zpt);
	  }
	}
      }

      double integrated = 0.;
      for(int pix=0; pix<xsize*ysize; pix++){ // loop over each spatial pixel.
	for(int z=0; z<zsize; z++){
	  int pos =  z*xsize*ysize + pix;
	  if(isObj[pos]){ // if it's an object pixel...
	    double deltaVel;
	    if(z==0) 
	      deltaVel = (world[pos+xsize*ysize] - world[pos]);
	    else if(z==(zsize-1)) 
	      deltaVel = (world[pos] - world[pos-xsize*ysize]);
	    else 
	      deltaVel = (world[pos+xsize*ysize] - world[pos-xsize*ysize]) / 2.;
	    integrated += localFlux[pos] * fabs(deltaVel);
	  }
	}
      }
      this->intFlux = integrated;

      delete [] world;
      delete [] localFlux;

      calcVelWidths(fluxArray, dim, head);

    }
    else // in this case there is just a 2D image.
      this->intFlux = this->totalFlux;

    if(head.isWCS()){
      // correct for the beam size if the flux units string ends in "/beam" and we have beam info
      if(head.needBeamSize()) this->intFlux  /= head.beam().area();
    }

  }
  //--------------------------------------------------------------------

  void Detection::calcVelWidths(long zdim, std::vector<Voxel> voxelList, FitsHeader &head)
  {
    ///  @details
    /// Calculates the widths of the detection at 20% and 50% of the
    /// peak integrated flux. The procedure is as follows: first
    /// generate an integrated flux spectrum (using all given voxels
    /// that lie in the object's spatial map); find the peak; starting
    /// at the spectral edges of the detection, move in or out until
    /// you reach the 20% or 50% peak flux level. Linear interpolation
    /// between points is done.
    /// 
    ///  \param zdim The size of the spectral axis in the cube
    ///  \param voxelList The list of Voxels with flux information
    ///  \param head FitsHeader object that contains the WCS information.

    float *intSpec = new float[zdim];
    for(int i=0;i<zdim;i++) intSpec[i]=0;
       
    Object2D spatMap = this->getSpatialMap();
    for(int s=0;s<spatMap.getNumScan();s++){
      std::vector<Voxel>::iterator vox;
      for(vox=voxelList.begin();vox<voxelList.end();vox++){
	if(spatMap.isInObject(*vox)){
	  intSpec[vox->getZ()] += vox->getF();
	}
      }
    }
    
    calcVelWidths(zdim, intSpec, head);

    delete [] intSpec;

  }

  //--------------------------------------------------------------------

  void Detection::calcVelWidths(long zdim, float *intSpec, FitsHeader &head)
  {

      // finding the 20% & 50% points.  Start at the velmin & velmax
      //  points. Then, if the int flux there is above the 20%/50%
      //  limit, go out, otherwise go in. This is to deal with the
      //  problems from double- (or multi-) peaked sources.

    this->haveParams = true;

    int z=this->getZmin();
    double zpt,xpt=double(this->getXcentre()),ypt=double(this->getXcentre());
    bool goLeft;
    
    float peak=0.;
    int peakLoc=0;
    for(int z=this->getZmin();z<=this->getZmax();z++) {
      if(z==0 || peak<intSpec[z]){
	peak = intSpec[z];
	peakLoc = z;
      }
    }
    
    goLeft = intSpec[z]>peak*0.5;
    if(goLeft) while(z>0 && intSpec[z]>peak*0.5) z--;
    else       while(z<peakLoc && intSpec[z]<peak*0.5) z++;
    if(z==0) this->v50min = this->velMin;
    else{
      if(goLeft) zpt = z + (peak*0.5-intSpec[z])/(intSpec[z+1]-intSpec[z]);
      else       zpt = z - (peak*0.5-intSpec[z])/(intSpec[z-1]-intSpec[z]);
      this->v50min = head.pixToVel(xpt,ypt,zpt);
    }
    z=this->getZmax();
    goLeft = intSpec[z]<peak*0.5;
    if(goLeft) while(z>peakLoc && intSpec[z]<peak*0.5) z--;
    else       while(z<zdim    && intSpec[z]>peak*0.5) z++;
    if(z==zdim) this->v50max = this->velMax;
    else{
      if(goLeft) zpt = z + (peak*0.5-intSpec[z])/(intSpec[z+1]-intSpec[z]);
      else       zpt = z - (peak*0.5-intSpec[z])/(intSpec[z-1]-intSpec[z]);
      this->v50max = head.pixToVel(xpt,ypt,zpt);
    }
    z=this->getZmin();
    goLeft = intSpec[z]>peak*0.2;
    if(goLeft) while(z>0 && intSpec[z]>peak*0.2) z--;
    else       while(z<peakLoc && intSpec[z]<peak*0.2) z++;
    if(z==0) this->v20min = this->velMin;
    else{
      if(goLeft) zpt = z + (peak*0.2-intSpec[z])/(intSpec[z+1]-intSpec[z]);
      else       zpt = z - (peak*0.2-intSpec[z])/(intSpec[z-1]-intSpec[z]);
      this->v20min = head.pixToVel(xpt,ypt,zpt);
    }
    z=this->getZmax();
    goLeft = intSpec[z]<peak*0.2;
    if(goLeft) while(z>peakLoc && intSpec[z]<peak*0.2) z--;
    else       while(z<zdim    && intSpec[z]>peak*0.2) z++;
    if(z==zdim) this->v20max = this->velMax;
    else{
      if(goLeft) zpt = z + (peak*0.2-intSpec[z])/(intSpec[z+1]-intSpec[z]);
      else       zpt = z - (peak*0.2-intSpec[z])/(intSpec[z-1]-intSpec[z]);
      this->v20max = head.pixToVel(xpt,ypt,zpt);
    }

    this->w20 = fabs(this->v20min - this->v20max);
    this->w50 = fabs(this->v50min - this->v50max);


  }
  //--------------------------------------------------------------------

  void Detection::calcVelWidths(float *fluxArray, long *dim, FitsHeader &head)
  {
    ///  @details
    /// Calculates the widths of the detection at 20% and 50% of the
    /// peak integrated flux. The procedure is as follows: first
    /// generate an integrated flux spectrum (summing each spatial
    /// pixel's spectrum); find the peak; starting at the spectral
    /// edges of the detection, move in or out until you reach the 20%
    /// or 50% peak flux level. Linear interpolation between points is
    /// done. 
    /// 
    ///  \param fluxArray The array of flux values.
    ///  \param dim The dimensions of the flux array.
    ///  \param head FitsHeader object that contains the WCS information.

    if(dim[2] > 2){

      float *intSpec = new float[dim[2]];
      long size=dim[0]*dim[1]*dim[2];
      std::vector<bool> mask(size,true); 
      getIntSpec(*this,fluxArray,dim,mask,1.,intSpec);

      this->calcVelWidths(dim[2],intSpec,head);

      delete [] intSpec;

    }
    else{
      this->v50min = this->v20min = this->velMin;
      this->v50max = this->v20max = this->velMax;
      this->w20 = fabs(this->v20min - this->v20max);
      this->w50 = fabs(this->v50min - this->v50max);
    }

  }
  //--------------------------------------------------------------------

  void Detection::setOffsets(Param &par)
  {
    ///  @details
    /// This function stores the values of the offsets for each cube axis.
    /// The offsets are the starting values of the cube axes that may differ from
    ///  the default value of 0 (for instance, if a subsection is being used).
    /// The values will be used when the detection is outputted.

    this->xSubOffset = par.getXOffset();
    this->ySubOffset = par.getYOffset();
    this->zSubOffset = par.getZOffset();
  }
  //--------------------------------------------------------------------

  bool Detection::hasEnoughChannels(int minNumber)
  {
    ///  @details
    /// A function to determine if the Detection has enough
    /// contiguous channels to meet the minimum requirement
    /// given as the argument. 
    /// \param minNumber How many channels is the minimum acceptable number?
    /// \return True if there is at least one occurence of minNumber consecutive
    /// channels present to return true. False otherwise.

    // Preferred method -- need a set of minNumber consecutive channels present.

    int numChan = this->getMaxAdjacentChannels();
    bool result = (numChan >= minNumber);

    return result;
  
  }
  //--------------------------------------------------------------------

  std::vector<int> Detection::getVertexSet()
  {
    ///  @details
    /// Gets a list of points being the end-points of 1-pixel long
    /// segments drawing a border around the spatial extend of a
    /// detection. The vector is a series of 4 integers, being: x_0,
    /// y_0, x_1, y_1.
    /// \return The vector of vertex positions.

    std::vector<int> vertexSet;

    int xmin = this->getXmin() - 1;
    int xmax = this->getXmax() + 1;
    int ymin = this->getYmin() - 1;
    int ymax = this->getYmax() + 1;
    int xsize = xmax - xmin + 1;
    int ysize = ymax - ymin + 1;

    std::vector<Voxel> voxlist = this->getPixelSet();
    std::vector<bool> isObj(xsize*ysize,false);
    std::vector<Voxel>::iterator vox;
    for(vox=voxlist.begin();vox<voxlist.end();vox++){
      int pos = (vox->getX()-xmin) + 
	(vox->getY()-ymin)*xsize;
      isObj[pos] = true;
    }
    voxlist.clear();
    
    for(int x=xmin; x<=xmax; x++){
      // for each column...
      for(int y=ymin+1;y<=ymax;y++){
	int current  = (y-ymin)*xsize + x-xmin;
	int previous = (y-ymin-1)*xsize + x-xmin;
	if((isObj[current]&&!isObj[previous])   ||
	   (!isObj[current]&&isObj[previous])){
	  vertexSet.push_back(x);
	  vertexSet.push_back(y);
	  vertexSet.push_back(x+1);
	  vertexSet.push_back(y);
	}
      }
    }
    for(int y=ymin; y<=ymax; y++){
      // now for each row...
      for(int x=xmin+1;x<=xmax;x++){
	int current  = (y-ymin)*xsize + x-xmin;
	int previous = (y-ymin)*xsize + x-xmin - 1;
	if((isObj[current]&&!isObj[previous])   ||
	   (!isObj[current]&&isObj[previous])){
	  vertexSet.push_back(x);
	  vertexSet.push_back(y);
	  vertexSet.push_back(x);
	  vertexSet.push_back(y+1);
	}
      }
    }

    return vertexSet;
  
  }

  
  void Detection::addDetection(Detection &other)
  {
    for(std::map<long, Object2D>::iterator it = other.chanlist.begin(); it!=other.chanlist.end();it++)
      //      this->addChannel(*it);
      this->addChannel(it->first, it->second);
    this->haveParams = false; // make it appear as if the parameters haven't been calculated, so that we can re-calculate them  
  }

  Detection operator+ (Detection &lhs, Detection &rhs)
  {
    Detection output = lhs;
    for(std::map<long, Object2D>::iterator it = rhs.chanlist.begin(); it!=rhs.chanlist.end();it++)
      output.addChannel(it->first, it->second);
    output.haveParams = false; // make it appear as if the parameters haven't been calculated, so that we can re-calculate them
    return output;
  }
    

  bool Detection::canMerge(Detection &other, Param &par)
  {
    bool near = this->isNear(other,par);
    if(near) return this->isClose(other,par);
    else return near;
  }

  bool Detection::isNear(Detection &other, Param &par)
  {

    bool flagAdj = par.getFlagAdjacent();
    float threshS = par.getThreshS();
    float threshV = par.getThreshV();

    long gap;
    if(flagAdj) gap = 1;
    else gap = long( ceil(threshS) );

    bool areNear;
    // Test X ranges
    if((this->xmin-gap)<other.xmin) areNear=((this->xmax+gap)>=other.xmin);
    else areNear=(other.xmax>=(this->xmin-gap));
    // Test Y ranges
    if(areNear){
      if((this->ymin-gap)<other.ymin) areNear=areNear&&((this->ymax+gap)>=other.ymin);
      else areNear=areNear&&(other.ymax>=(this->ymin-gap));
    }
    // Test Z ranges
    if(areNear){
      gap = long(ceil(threshV));
      if((this->zmin-gap)<other.zmin) areNear=areNear&&((this->zmax+gap)>=other.zmin);
      else areNear=areNear&&(other.zmax>=(this->zmin-gap));
    }
    
    return areNear;

  }

  bool Detection::isClose(Detection &other, Param &par)
  {
    bool close = false;   // this will be the value returned
    
    bool flagAdj = par.getFlagAdjacent();
    float threshS = par.getThreshS();
    float threshV = par.getThreshV();

    // 
    // If we get to here, the pixel ranges overlap -- so we do a
    // pixel-by-pixel comparison to make sure they are actually
    // "close" according to the thresholds.  Otherwise, close=false,
    // and so don't need to do anything else before returning.
    // 

    std::vector<long> zlist1 = this->getChannelList();
    std::vector<long> zlist2 = other.getChannelList();
    Scan test1,test2;

    for(size_t ct1=0; (!close && (ct1<zlist1.size())); ct1++){
      for(size_t ct2=0; (!close && (ct2<zlist2.size())); ct2++){
	
	if(abs(zlist1[ct1]-zlist2[ct2])<=threshV){
	      
	  Object2D temp1 = this->getChanMap(zlist1[ct1]);
	  Object2D temp2 = other.getChanMap(zlist2[ct2]);

	  close = temp1.canMerge(temp2,threshS,flagAdj);

	}

      }
    }
       
    return close;
    
  }



}
