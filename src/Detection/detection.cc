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

using std::setw;
using std::setprecision;
using std::endl;
using std::vector;

using namespace PixelInfo;

namespace duchamp
{


  Detection::Detection()
  {
    this->flagWCS=false; 
    this->negSource = false; 
    this->flagText="";
    this->totalFlux = peakFlux = 0.;
    this->centreType="centroid";
  }

  Detection::Detection(const Detection& d)
  {
    operator=(d);
  }

  Detection& Detection::operator= (const Detection& d)
  {
    if(this == &d) return *this;
    this->pixelArray   = d.pixelArray;
    this->xSubOffset   = d.xSubOffset;
    this->ySubOffset   = d.ySubOffset;
    this->zSubOffset   = d.zSubOffset;
    this->totalFlux    = d.totalFlux;
    this->intFlux	     = d.intFlux;
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
    this->dec	     = d.dec;
    this->raWidth	     = d.raWidth;
    this->decWidth     = d.decWidth;
    this->specUnits    = d.specUnits;
    this->fluxUnits    = d.fluxUnits;
    this->intFluxUnits = d.intFluxUnits;
    this->lngtype	     = d.lngtype;
    this->lattype	     = d.lattype;
    this->vel          = d.vel;
    this->velWidth     = d.velWidth;
    this->velMin       = d.velMin;
    this->velMax       = d.velMax;
    this->posPrec      = d.posPrec;
    this->xyzPrec      = d.xyzPrec;
    this->fintPrec     = d.fintPrec;
    this->fpeakPrec    = d.fpeakPrec;
    this->velPrec	     = d.velPrec;
    this->snrPrec      = d.snrPrec;
    return *this;
  }

  //--------------------------------------------------------------------

  bool Detection::voxelListsMatch(std::vector<Voxel> voxelList)
  {
    /**
     *  A test to see whether there is a 1-1 correspondence between
     *  the given list of Voxels and the voxel positions contained in
     *  this Detection's pixel list. No testing of the fluxes of the
     *  Voxels is done.
     *
     * \param voxelList The std::vector list of Voxels to be tested.
     */

    bool listsMatch = true;
    // compare sizes
    listsMatch = listsMatch && (voxelList.size() == this->getSize());
    if(!listsMatch) return listsMatch;

    // make sure all voxels are in Detection
    for(int i=0;i<voxelList.size();i++)
      listsMatch = listsMatch && this->pixelArray.isInObject(voxelList[i]);
    // make sure all Detection pixels are in voxel list
    int v1=0, mysize=this->getSize();
    while(listsMatch && v1<mysize){
      bool inList = false;
      int v2=0;
      Voxel test = this->getPixel(v1);
      while(!inList && v2<voxelList.size()){
	inList = inList || test.match(voxelList[v2]);
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
    /**
     *  A function that calculates total & peak fluxes (and the location
     *  of the peak flux) for a Detection.
     *
     *  \param fluxArray The array of flux values to calculate the
     *  flux parameters from.
     *  \param dim The dimensions of the flux array.
     */

    this->totalFlux = this->peakFlux = 0;
    this->xCentroid = this->yCentroid = this->zCentroid = 0.;

    // first check that the voxel list and the Detection's pixel list
    // have a 1-1 correspondence

    if(!this->voxelListsMatch(voxelList)){
      duchampError("Detection::calcFluxes","Voxel list provided does not match");
      return;
    }

    for(int i=0;i<voxelList.size();i++) {
      long x = voxelList[i].getX();
      long y = voxelList[i].getY();
      long z = voxelList[i].getZ();
      float f = voxelList[i].getF();
      this->totalFlux += f;
      this->xCentroid += x*f;
      this->yCentroid += y*f;
      this->zCentroid += z*f;
      if( (i==0) ||  //first time round
	  (this->negSource&&(f<this->peakFlux)) || 
	  (!this->negSource&&(f>this->peakFlux))   )
	{
	  this->peakFlux = f;
	  this->xpeak =    x;
	  this->ypeak =    y;
	  this->zpeak =    z;
	}
    }

    this->xCentroid /= this->totalFlux;
    this->yCentroid /= this->totalFlux;
    this->zCentroid /= this->totalFlux;
  }
  //--------------------------------------------------------------------

  void Detection::calcFluxes(float *fluxArray, long *dim)
  {
    /**
     *  A function that calculates total & peak fluxes (and the location
     *  of the peak flux) for a Detection.
     *
     *  \param fluxArray The array of flux values to calculate the
     *  flux parameters from.
     *  \param dim The dimensions of the flux array.
     */

    this->totalFlux = this->peakFlux = 0;
    this->xCentroid = this->yCentroid = this->zCentroid = 0.;
    long y,z,count=0;

    for(int m=0; m<this->pixelArray.getNumChanMap(); m++){
      ChanMap *tempmap = new ChanMap;
      *tempmap = this->pixelArray.getChanMap(m);
      z = tempmap->getZ();
      for(int s=0; s<tempmap->getNumScan(); s++){
	Scan *tempscan = new Scan;
	*tempscan = tempmap->getScan(s);
	y = tempscan->getY();
	for(long x=tempscan->getX(); x<=tempscan->getXmax(); x++){

	  float f = fluxArray[x + y*dim[0] + z*dim[0]*dim[1]];
	  this->totalFlux += f;
	  this->xCentroid += x*f;
	  this->yCentroid += y*f;
	  this->zCentroid += z*f;
	  if( (count==0) ||  //first time round
	      (this->negSource&&(f<this->peakFlux)) || 
	      (!this->negSource&&(f>this->peakFlux))   )
	    {
	      this->peakFlux = f;
	      this->xpeak =    x;
	      this->ypeak =    y;
	      this->zpeak =    z;
	    }
	  count++;
	}
	delete tempscan;
      }
      delete tempmap;
    }

    this->xCentroid /= this->totalFlux;
    this->yCentroid /= this->totalFlux;
    this->zCentroid /= this->totalFlux;
  }
  //--------------------------------------------------------------------

  //  void Detection::calcWCSparams(float *fluxArray, long *dim, FitsHeader &head)
  void Detection::calcWCSparams(FitsHeader &head)
  {
    /**
     *  Use the input wcs to calculate the position and velocity 
     *    information for the Detection.
     *  Quantities calculated:
     *  <ul><li> RA: ra [deg], ra (string), ra width.
     *      <li> Dec: dec [deg], dec (string), dec width.
     *      <li> Vel: vel [km/s], min & max vel, vel width.
     *      <li> coord type for all three axes, nuRest, 
     *      <li> name (IAU-style, in equatorial or Galactic) 
     *  </ul>
     *
     *  Note that the regular parameters are NOT recalculated!
     *
     *  \param head FitsHeader object that contains the WCS information.
     */

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
	this->name = head.getIAUName(this->ra, this->dec);
	this->vel    = head.specToVel(world[2]);
	this->velMin = head.specToVel(world[5]);
	this->velMax = head.specToVel(world[8]);
	this->velWidth = fabs(this->velMax - this->velMin);

	//	this->calcIntegFlux(fluxArray,dim,head);
    
	this->flagWCS = true;
      }
      delete [] world;

    }
  }
  //--------------------------------------------------------------------

  void Detection::calcIntegFlux(std::vector<Voxel> voxelList, FitsHeader &head)
  {
    /**
     *  Uses the input WCS to calculate the velocity-integrated flux, 
     *   putting velocity in units of km/s.
     *  The fluxes used are taken from the Voxels, rather than an
     *   array of flux values.
     *  Integrates over full spatial and velocity range as given 
     *   by the extrema calculated by calcWCSparams.
     *
     *  If the flux units end in "/beam" (eg. Jy/beam), then the flux is
     *  corrected by the beam size (in pixels). This is done by
     *  multiplying the integrated flux by the number of spatial pixels,
     *  and dividing by the beam size in pixels (e.g. Jy/beam * pix /
     *  pix/beam --> Jy)
     *
     *  \param voxelList The list of Voxels with flux information
     *  \param head FitsHeader object that contains the WCS information.
     */

    if(!voxelListsMatch(voxelList)){
      duchampError("Detection::calcIntegFlux","Voxel list provided does not match");
      return;
    }

    if(head.getNumAxes() > 2) {

      // include one pixel either side in each direction
      long xsize = (this->getXmax()-this->getXmin()+3);
      long ysize = (this->getYmax()-this->getYmin()+3);
      long zsize = (this->getZmax()-this->getZmin()+3); 
      vector <bool> isObj(xsize*ysize*zsize,false);
      double *localFlux = new double[xsize*ysize*zsize];
      for(int i=0;i<xsize*ysize*zsize;i++) localFlux[i]=0.;

      for(int i=0;i<voxelList.size();i++){
	long x = voxelList[i].getX();
	long y = voxelList[i].getY();
	long z = voxelList[i].getZ();
	long pos = (x-this->getXmin()+1) + (y-this->getYmin()+1)*xsize
	  + (z-this->getZmin()+1)*xsize*ysize;
	localFlux[pos] = voxelList[i].getF();
	isObj[pos] = true;
      }
  
      // work out the WCS coords for each pixel
      double *world  = new double[xsize*ysize*zsize];
      double xpt,ypt,zpt;
      for(int i=0;i<xsize*ysize*zsize;i++){
	xpt = double( this->getXmin() -1 + i%xsize );
	ypt = double( this->getYmin() -1 + (i/xsize)%ysize );
	zpt = double( this->getZmin() -1 + i/(xsize*ysize) );
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

    }
    else // in this case there is just a 2D image.
      this->intFlux = this->totalFlux;

    if(head.isWCS()){
      // correct for the beam size if the flux units string ends in "/beam"
      if(head.needBeamSize()) this->intFlux  /= head.getBeamSize();
    }

  }
  //--------------------------------------------------------------------

  void Detection::calcIntegFlux(float *fluxArray, long *dim, FitsHeader &head)
  {
    /**
     *  Uses the input WCS to calculate the velocity-integrated flux, 
     *   putting velocity in units of km/s.
     *  Integrates over full spatial and velocity range as given 
     *   by the extrema calculated by calcWCSparams.
     *
     *  If the flux units end in "/beam" (eg. Jy/beam), then the flux is
     *  corrected by the beam size (in pixels). This is done by
     *  multiplying the integrated flux by the number of spatial pixels,
     *  and dividing by the beam size in pixels (e.g. Jy/beam * pix /
     *  pix/beam --> Jy)
     *
     *  \param fluxArray The array of flux values.
     *  \param dim The dimensions of the flux array.
     *  \param head FitsHeader object that contains the WCS information.
     */

    if(head.getNumAxes() > 2) {

      // include one pixel either side in each direction
      long xsize = (this->getXmax()-this->getXmin()+3);
      long ysize = (this->getYmax()-this->getYmin()+3);
      long zsize = (this->getZmax()-this->getZmin()+3); 
      vector <bool> isObj(xsize*ysize*zsize,false);
      double *localFlux = new double[xsize*ysize*zsize];
      for(int i=0;i<xsize*ysize*zsize;i++) localFlux[i]=0.;
      // work out which pixels are object pixels
      for(int m=0; m<this->pixelArray.getNumChanMap(); m++){
	ChanMap tempmap = this->pixelArray.getChanMap(m);
	long z = this->pixelArray.getChanMap(m).getZ();
	for(int s=0; s<this->pixelArray.getChanMap(m).getNumScan(); s++){
	  long y = this->pixelArray.getChanMap(m).getScan(s).getY();
	  for(long x=this->pixelArray.getChanMap(m).getScan(s).getX(); 
	      x<=this->pixelArray.getChanMap(m).getScan(s).getXmax(); 
	      x++){
	    long pos = (x-this->getXmin()+1) + (y-this->getYmin()+1)*xsize
	      + (z-this->getZmin()+1)*xsize*ysize;
	    localFlux[pos] = fluxArray[x + y*dim[0] + z*dim[0]*dim[1]];
	    isObj[pos] = true;
	  }
	}
      }
  
      // work out the WCS coords for each pixel
      double *world  = new double[xsize*ysize*zsize];
      double xpt,ypt,zpt;
      for(int i=0;i<xsize*ysize*zsize;i++){
	xpt = double( this->getXmin() -1 + i%xsize );
	ypt = double( this->getYmin() -1 + (i/xsize)%ysize );
	zpt = double( this->getZmin() -1 + i/(xsize*ysize) );
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

    }
    else // in this case there is just a 2D image.
      this->intFlux = this->totalFlux;

    if(head.isWCS()){
      // correct for the beam size if the flux units string ends in "/beam"
      if(head.needBeamSize()) this->intFlux  /= head.getBeamSize();
    }

  }
  //--------------------------------------------------------------------

  Detection operator+ (Detection lhs, Detection rhs)
  {
    /**
     *  Combines two objects by adding all the pixels using the Object3D
     *  operator.
     *
     *  The pixel parameters are recalculated in the process (equivalent
     *  to calling pixels().calcParams()), but WCS parameters are not.
     */
    Detection output = lhs;
    output.pixelArray = lhs.pixelArray + rhs.pixelArray;
    return output;
  }
  //--------------------------------------------------------------------

  void Detection::setOffsets(Param &par)
  {
    /**
     * This function stores the values of the offsets for each cube axis.
     * The offsets are the starting values of the cube axes that may differ from
     *  the default value of 0 (for instance, if a subsection is being used).
     * The values will be used when the detection is outputted.
     */
    this->xSubOffset = par.getXOffset();
    this->ySubOffset = par.getYOffset();
    this->zSubOffset = par.getZOffset();
  }
  //--------------------------------------------------------------------

  bool Detection::hasEnoughChannels(int minNumber)
  {
    /**
     * A function to determine if the Detection has enough
     * contiguous channels to meet the minimum requirement
     * given as the argument. 
     * \param minNumber How many channels is the minimum acceptable number?
     * \return True if there is at least one occurence of minNumber consecutive
     * channels present to return true. False otherwise.
     */

    // Preferred method -- need a set of minNumber consecutive channels present.
    this->pixelArray.order();
    int numChannels = 0;
    bool result = false;
    int size = this->pixelArray.getNumChanMap();
    if(size>0) numChannels++;
    if( numChannels >= minNumber) result = true;
    for(int i=1;(i<size && !result);i++) {
      if( (this->pixelArray.getZ(i) - this->pixelArray.getZ(i-1)) == 1) 
	numChannels++;
      else if( (this->pixelArray.getZ(i) - this->pixelArray.getZ(i-1)) >= 2) 
	numChannels = 1;

      if( numChannels >= minNumber) result = true;
    }
    return result;
  
  }
  //--------------------------------------------------------------------

  std::vector<int> Detection::getVertexSet()
  {
    /**
     * Gets a list of points being the end-points of 1-pixel long
     * segments drawing a border around the spatial extend of a
     * detection. The vector is a series of 4 integers, being: x_0,
     * y_0, x_1, y_1.
     * \return The vector of vertex positions.
     */
    std::vector<int> vertexSet;

    int xmin = this->getXmin() - 1;
    int xmax = this->getXmax() + 1;
    int ymin = this->getYmin() - 1;
    int ymax = this->getYmax() + 1;
    int xsize = xmax - xmin + 1;
    int ysize = ymax - ymin + 1;

    std::vector<Voxel> voxlist = this->pixelArray.getPixelSet();
    std::vector<bool> isObj(xsize*ysize,false);
    for(int i=0;i<voxlist.size();i++){
      int pos = (voxlist[i].getX()-xmin) + 
	(voxlist[i].getY()-ymin)*xsize;
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
  //--------------------------------------------------------------------

  std::ostream& operator<< ( std::ostream& theStream, Detection& obj)
  {
    /**
     *  A convenient way of printing the coordinate values for each
     *  pixel in the Detection.  
     *
     *  NOTE THAT THERE IS CURRENTLY NO FLUX INFORMATION BEING PRINTED!
     *
     *  Use as front end to the Object3D::operator<< function.
     */  

    theStream << obj.pixelArray << "---\n";
    return theStream;
  }
  //--------------------------------------------------------------------

}
