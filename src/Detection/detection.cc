#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <wcs.h>
#include <math.h>
#include <param.hh>
#include <duchamp.hh>
#include <Utils/utils.hh>
#include <PixelMap/Voxel.hh>
#include <PixelMap/Object3D.hh>
#include <Detection/detection.hh>

using std::setw;
using std::setprecision;
using std::endl;
using std::vector;

using namespace PixelInfo;

Detection::Detection(const Detection& d)
{
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
}

//--------------------------------------------------------------------

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
    ChanMap tempmap = this->pixelArray.getChanMap(m);
    z = tempmap.getZ();
    for(int s=0; s<tempmap.getNumScan(); s++){
      Scan tempscan = tempmap.getScan(s);
      y = tempscan.getY();
      for(long x=tempscan.getX(); x<=tempscan.getXmax(); x++){

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
    }
  }

  this->xCentroid /= this->totalFlux;
  this->yCentroid /= this->totalFlux;
  this->zCentroid /= this->totalFlux;
}
//--------------------------------------------------------------------

void Detection::calcWCSparams(float *fluxArray, long *dim, FitsHeader &head)
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
   *  Uses Detection::calcIntegFlux() to calculate the
   *  integrated flux in (say) [Jy km/s]
   *
   *  Note that the regular parameters are NOT recalculated!
   *
   *  \param fluxArray The array of flux values to calculate the
   *  integrated flux from.
   *  \param dim The dimensions of the flux array.
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
			     "Error in calculating the WCS for this object.");
    else{

      // world now has the WCS coords for the five points 
      //    -- use this to work out WCS params
  
      this->lngtype = head.getWCS()->lngtyp;
      this->lattype = head.getWCS()->lattyp;
      this->specUnits = head.getSpectralUnits();
      this->fluxUnits = head.getFluxUnits();
      // if fluxUnits are eg. Jy/beam, make intFluxUnits = Jy km/s
      this->intFluxUnits = head.getIntFluxUnits();
      this->ra   = world[0];
      this->dec  = world[1];
      this->raS  = decToDMS(this->ra, this->lngtype);
      this->decS = decToDMS(this->dec,this->lattype);
      this->raWidth = angularSeparation(world[9],world[1],world[12],world[1])*60.;
      this->decWidth  = angularSeparation(world[0],world[10],world[0],world[13]) * 60.;
      this->name = head.getIAUName(this->ra, this->dec);
      this->vel    = head.specToVel(world[2]);
      this->velMin = head.specToVel(world[5]);
      this->velMax = head.specToVel(world[8]);
      this->velWidth = fabs(this->velMax - this->velMin);

      this->calcIntegFlux(fluxArray,dim,head);
    
      this->flagWCS = true;
    }
    delete [] world;

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
   *  If the flux units end in "/beam" (eg. Jy/beam), then the
   *   flux is corrected by the beam size (in pixels).
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
    vector <float> localFlux(xsize*ysize*zsize,0.);
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

    this->intFlux = 0.;
    for(int pix=0; pix<xsize*ysize; pix++){ // loop over each spatial pixel.
      for(int z=0; z<zsize; z++){
	int pos =  z*xsize*ysize + pix;
	if(isObj[pos]){ // if it's an object pixel...
	  float deltaVel;
	  if(z==0) 
	    deltaVel = (world[pos+xsize*ysize] - world[pos]);
	  else if(z==(zsize-1)) 
	    deltaVel = (world[pos] - world[pos-xsize*ysize]);
	  else 
	    deltaVel = (world[pos+xsize*ysize] - world[pos-xsize*ysize]) / 2.;
	  this->intFlux += localFlux[pos] * fabs(deltaVel);
	}
      }
    }

    // correct for the beam size if the flux units string ends in "/beam"
    int size = this->fluxUnits.size();
    std::string tailOfFluxUnits = this->fluxUnits.substr(size-5,size);
    if(tailOfFluxUnits == "/beam") this->intFlux /= head.getBeamSize();

    delete [] world;

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
   *  to calling pixels().calcParams()), but WCS parameters
   *  are not.
   */
  Detection output;
  output.pixelArray = lhs.pixelArray + rhs.pixelArray;
//   output.totalFlux  = lhs.totalFlux  + rhs.totalFlux;
//   if(lhs.peakFlux > rhs.peakFlux){
//     output.peakFlux = lhs.peakFlux;
//     output.xpeak    = lhs.xpeak;
//     output.ypeak    = lhs.ypeak;
//     output.zpeak    = lhs.zpeak;
//     output.peakSNR  = lhs.peakSNR;
//   }
//   else{
//     output.peakFlux = rhs.peakFlux;
//     output.xpeak    = rhs.xpeak;
//     output.ypeak    = rhs.ypeak;
//     output.zpeak    = rhs.zpeak;
//     output.peakSNR  = rhs.peakSNR;
//   }
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

