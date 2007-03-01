#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <wcs.h>
#include <math.h>
#include <param.hh>
#include <Utils/utils.hh>
#include <Detection/voxel.hh>
#include <Detection/detection.hh>

using std::setw;
using std::setprecision;
using std::endl;
using std::vector;

Detection::Detection(const Detection& d)
{
  pix             = d.pix;
  xcentre	  = d.xcentre;
  ycentre	  = d.ycentre;
  zcentre	  = d.zcentre;
  xmin   	  = d.xmin;
  xmax	          = d.xmax;
  ymin   	  = d.ymin;
  ymax	          = d.ymax;
  zmin   	  = d.zmin;
  zmax	          = d.zmax;
  xSubOffset	  = d.xSubOffset;
  ySubOffset	  = d.ySubOffset;
  zSubOffset	  = d.zSubOffset;
  totalFlux	  = d.totalFlux;
  intFlux	  = d.intFlux;
  peakFlux	  = d.peakFlux;
  xpeak		  = d.xpeak;
  ypeak		  = d.ypeak;
  zpeak		  = d.zpeak;
  peakSNR	  = d.peakSNR;
  negativeSource  = d.negativeSource;
  flagText	  = d.flagText;
  id		  = d.id;
  name		  = d.name;
  flagWCS	  = d.flagWCS;
  raS		  = d.raS;
  decS		  = d.decS;
  ra		  = d.ra;
  dec		  = d.dec;
  raWidth	  = d.raWidth;
  decWidth	  = d.decWidth;
  specUnits	  = d.specUnits;
  fluxUnits	  = d.fluxUnits;
  intFluxUnits	  = d.intFluxUnits;
  lngtype	  = d.lngtype;
  lattype	  = d.lattype;
  vel		  = d.vel;
  velWidth	  = d.velWidth;
  velMin	  = d.velMin;
  velMax	  = d.velMax;
  posPrec	  = d.posPrec;
  xyzPrec	  = d.xyzPrec;
  fintPrec	  = d.fintPrec;
  fpeakPrec	  = d.fpeakPrec;
  velPrec	  = d.velPrec;
  snrPrec         = d.snrPrec;
}

//--------------------------------------------------------------------

  Detection& Detection::operator= (const Detection& d)
{
  pix             = d.pix;
  xcentre	  = d.xcentre;
  ycentre	  = d.ycentre;
  zcentre	  = d.zcentre;
  xmin   	  = d.xmin;
  xmax	          = d.xmax;
  ymin   	  = d.ymin;
  ymax	          = d.ymax;
  zmin   	  = d.zmin;
  zmax	          = d.zmax;
  xSubOffset	  = d.xSubOffset;
  ySubOffset	  = d.ySubOffset;
  zSubOffset	  = d.zSubOffset;
  totalFlux	  = d.totalFlux;
  intFlux	  = d.intFlux;
  peakFlux	  = d.peakFlux;
  xpeak		  = d.xpeak;
  ypeak		  = d.ypeak;
  zpeak		  = d.zpeak;
  peakSNR	  = d.peakSNR;
  negativeSource  = d.negativeSource;
  flagText	  = d.flagText;
  id		  = d.id;
  name		  = d.name;
  flagWCS	  = d.flagWCS;
  raS		  = d.raS;
  decS		  = d.decS;
  ra		  = d.ra;
  dec		  = d.dec;
  raWidth	  = d.raWidth;
  decWidth	  = d.decWidth;
  specUnits	  = d.specUnits;
  fluxUnits	  = d.fluxUnits;
  intFluxUnits	  = d.intFluxUnits;
  lngtype	  = d.lngtype;
  lattype	  = d.lattype;
  vel		  = d.vel;
  velWidth	  = d.velWidth;
  velMin	  = d.velMin;
  velMax	  = d.velMax;
  posPrec	  = d.posPrec;
  xyzPrec	  = d.xyzPrec;
  fintPrec	  = d.fintPrec;
  fpeakPrec	  = d.fpeakPrec;
  velPrec	  = d.velPrec;
  snrPrec         = d.snrPrec;
}

//--------------------------------------------------------------------

void Detection::calcParams()
{
  /**
   *  A function that calculates centroid positions, minima & maxima
   *  of coordinates, and total & peak fluxes for a Detection.
   */

  this->xcentre = 0;
  this->ycentre = 0;
  this->zcentre = 0;
  this->totalFlux = 0;
  this->peakFlux = 0;
  this->xmin = 0;
  this->xmax = 0;
  this->ymin = 0;
  this->ymax = 0;
  this->zmin = 0;
  this->zmax = 0;
  if(this->pix.size()>0){
    this->peakFlux = this->pix[0].itsF;
    for(int ctr=0;ctr<this->pix.size();ctr++){
      this->xcentre   += this->pix[ctr].itsX;
      this->ycentre   += this->pix[ctr].itsY;
      this->zcentre   += this->pix[ctr].itsZ;
      this->totalFlux += this->pix[ctr].itsF;
      if((ctr==0)||(this->pix[ctr].itsX<this->xmin)) 
	this->xmin = this->pix[ctr].itsX;
      if((ctr==0)||(this->pix[ctr].itsX>this->xmax)) 
	this->xmax = this->pix[ctr].itsX;
      if((ctr==0)||(this->pix[ctr].itsY<this->ymin)) 
	this->ymin = this->pix[ctr].itsY;
      if((ctr==0)||(this->pix[ctr].itsY>this->ymax)) 
	this->ymax = this->pix[ctr].itsY;
      if((ctr==0)||(this->pix[ctr].itsZ<this->zmin)) 
	this->zmin = this->pix[ctr].itsZ;
      if((ctr==0)||(this->pix[ctr].itsZ>this->zmax)) 
	this->zmax = this->pix[ctr].itsZ;
      if(this->negativeSource){
	// if negative feature, peakFlux is most negative flux
	if((ctr==0)||(this->pix[ctr].itsF < this->peakFlux)){
	  this->peakFlux = this->pix[ctr].itsF;
	  this->xpeak = this->pix[ctr].itsX;
	  this->ypeak = this->pix[ctr].itsY;
	  this->zpeak = this->pix[ctr].itsZ;
	}
      }
      else{
	// otherwise, it's a regular detection, 
	//   and peakFlux is most positive flux
	if((ctr==0)||(this->pix[ctr].itsF > this->peakFlux)){
	  this->peakFlux = this->pix[ctr].itsF;
	  this->xpeak = this->pix[ctr].itsX;
	  this->ypeak = this->pix[ctr].itsY;
	  this->zpeak = this->pix[ctr].itsZ;
	}
      }
    }
    this->xcentre /= this->pix.size();
    this->ycentre /= this->pix.size();
    this->zcentre /= this->pix.size();
  }
}
//--------------------------------------------------------------------

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
   *  Uses Detection::getIntegFlux(FitsHeader &) to calculate the
   *  integrated flux in (say) [Jy km/s]
   *
   *  \param head FitsHeader object that contains the WCS information.
   */

  if(head.isWCS()){

    this->calcParams(); // make sure this is up to date.

    double *pixcrd = new double[15];
    double *world  = new double[15];
    /*
      define a five-point array in 3D:
      (x,y,z), (x,y,z1), (x,y,z2), (x1,y1,z), (x2,y2,z)
      [note: x = central point, x1 = minimum x, x2 = maximum x etc.]
      and convert to world coordinates.   
    */
    pixcrd[0]  = pixcrd[3] = pixcrd[6] = this->xcentre;
    pixcrd[9]  = this->xmin-0.5;
    pixcrd[12] = this->xmax+0.5;
    pixcrd[1]  = pixcrd[4] = pixcrd[7] = this->ycentre;
    pixcrd[10] = this->ymin-0.5;
    pixcrd[13] = this->ymax+0.5;
    pixcrd[2] = pixcrd[11] = pixcrd[14] = this->zcentre;
    pixcrd[5] = this->zmin;
    pixcrd[8] = this->zmax;
    int flag = head.pixToWCS(pixcrd, world, 5);
    delete [] pixcrd;

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

    this->getIntegFlux(head);
    
    this->flagWCS = true;

    delete [] world;

  }
}
//--------------------------------------------------------------------

float Detection::getIntegFlux(FitsHeader &head)
{
  /**
   *  Uses the input WCS to calculate the velocity-integrated flux, 
   *   putting velocity in units of km/s.
   *  Integrates over full spatial and velocity range as given 
   *   by the extrema calculated by calcWCSparams.
   *  If the flux units end in "/beam" (eg. Jy/beam), then the
   *   flux is corrected by the beam size (in pixels).
   *
   *  \param head FitsHeader object that contains the WCS information.
   */

  // include one pixel either side in each direction
  int xsize = (this->xmax-this->xmin+3);
  int ysize = (this->ymax-this->ymin+3);
  int zsize = (this->zmax-this->zmin+3); 
  vector <bool> isObj(xsize*ysize*zsize,false);
  vector <float> fluxArray(xsize*ysize*zsize,0.);
  // work out which pixels are object pixels
  for(int p=0;p<this->pix.size();p++){
    int pos = (this->pix[p].getX()-this->xmin+1) 
      + (this->pix[p].getY()-this->ymin+1)*xsize 
      + (this->pix[p].getZ()-this->zmin+1)*xsize*ysize;
    fluxArray[pos] = this->pix[p].getF();
    isObj[pos] = true;
  }
  
  // work out the WCS coords for each pixel
  double *world  = new double[xsize*ysize*zsize];
  double x,y,z;
  for(int i=0;i<xsize*ysize*zsize;i++){
    x = double( this->xmin -1 + i%xsize );
    y = double( this->ymin -1 + (i/xsize)%ysize );
    z = double( this->zmin -1 + i/(xsize*ysize) );
    world[i] = head.pixToVel(x,y,z);
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
	this->intFlux += fluxArray[pos] * fabs(deltaVel);
      }
    }
  }

  // correct for the beam size if the flux units string ends in "/beam"
  int size = this->fluxUnits.size();
  std::string tailOfFluxUnits = this->fluxUnits.substr(size-5,size);
  if(tailOfFluxUnits == "/beam") this->intFlux /= head.getBeamSize();

  delete [] world;
}
//--------------------------------------------------------------------

void Detection::addAnObject(Detection &toAdd)
{
  /**
   *  Combines two objects by adding all the pixels of the argument 
   *   to the instigator.
   *  All pixel & flux parameters are recalculated (so that 
   *   calcParams does not need to be called a second time), 
   *   but WCS parameters are not.
   *  If the instigator is empty (pix.size()==0) then we just make it 
   *   equal to the argument, and call calcParams to initialise the 
   *   necessary parameters
   */

  int size = this->pix.size();
  if(size==0){
    *this = toAdd;
    this->calcParams();
  }
  else if(size>0){

    this->xcentre *= size;
    this->ycentre *= size;
    this->zcentre *= size;
  
    for(int ctr=0;ctr<toAdd.getSize();ctr++){
      long x  = toAdd.getX(ctr);
      long y  = toAdd.getY(ctr);
      long z  = toAdd.getZ(ctr);
      float f = toAdd.getF(ctr);
      bool isNewPix = true;
      int ctr2 = 0;
      // For each pixel in the new object, test to see if it already 
      //  appears in the object
      while( isNewPix && (ctr2<this->pix.size()) ){
	isNewPix = isNewPix && (( this->pix[ctr2].itsX != x ) || 
				( this->pix[ctr2].itsY != y ) ||
				( this->pix[ctr2].itsZ != z ) );
	ctr2++;
      }
      if(isNewPix){
	// If the pixel is new, add it to the object and 
	//   re-calculate the parameters.
	this->pix.push_back(toAdd.getPixel(ctr));
	this->xcentre += x;
	this->ycentre += y;
	this->zcentre += z;
	this->totalFlux += f;
	if     (x < this->xmin) this->xmin = x;
	else if(x > this->xmax) this->xmax = x;
	if     (y < this->ymin) this->ymin = y;
	else if(y > this->ymax) this->ymax = y;
	if     (z < this->zmin) this->zmin = z;
	else if(z > this->zmax) this->zmax = z;
	if(f > this->peakFlux) this->peakFlux = f;
      }
    }
    size = this->pix.size();
    this->xcentre /= size;
    this->ycentre /= size;
    this->zcentre /= size;

  }
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

  // Original requirement -- based on total span
// int numChannels = this->getZmax() - this->getZmin() + 1;

// Alternative -- number of distinct channels detected
//   int numChannels = 0;
//   this->SortByZ();
//   if(this->getSize()>0) numChannels++;
//   for(int i=1;i<this->getSize();i++)
//     if(this->getZ(i)>this->getZ(i-1)) numChannels++;  
//   return (numChannels < minNumber);

// Preferred method -- need a set of minNumber consecutive channels present.
  this->SortByZ();
  int numChannels = 0;
  bool result = false;
  if(this->getSize()>0) numChannels++;
  for(int i=1;i<this->getSize();i++) {
    if( (this->getZ(i) - this->getZ(i-1)) == 1) numChannels++;
    else if( (this->getZ(i) - this->getZ(i-1)) >= 2) numChannels = 1;

    if( numChannels >= minNumber) result = true;
  }
  return result;
  
}
//--------------------------------------------------------------------

int Detection::getSpatialSize()
{
  /**
   *  A function that returns the number of distinct spatial pixels in
   *  a Detection.
   */

  vector<Pixel> spatialPix;
  Pixel newpix;
  bool addThisOne;
  newpix.setXY(this->getX(0),this->getY(0));
  spatialPix.push_back(newpix);
  for(int i=1;i<this->pix.size();i++){
    newpix.setXY(this->getX(i),this->getY(i));
    addThisOne = true;
    for(int j=0;(j<spatialPix.size())&&addThisOne;j++) { 
      // do whole list or until addThisOne=false
      addThisOne = ( (newpix.getX()!=spatialPix[j].getX()) || 
		     (newpix.getY()!=spatialPix[j].getY())   );
      // ie. if one of X or Y is different, addThisOne is true.
    }
    if(addThisOne) spatialPix.push_back(newpix);
  }
  return spatialPix.size();
}
//--------------------------------------------------------------------

std::ostream& operator<< ( std::ostream& theStream, Detection& obj)
{
  /**
   *  A convenient way of printing the coordinate & flux 
   *  values for each pixel in the Detection.
   *  Use as front end to the Voxel::operator<< function.
   */  

  for(int i=0;i<obj.pix.size();i++) theStream << obj.pix[i] << endl;
  theStream<<"---"<<endl;
}
//--------------------------------------------------------------------

Detection combineObjects(Detection &first, Detection &second)
{
  /** 
   * Function that combines two Detection objects and returns the
   * combination. Basic parameters are recalculated using
   * Detection::calcParams(), but WCS ones are not.
   *
   * \param first,second The two Detection objects to be combined.
   * \return A Detection object that includes all voxels in the two
   * input objects.
   */ 
  Detection *newObject = new Detection;
  for(int ctr=0;ctr<first.getSize();ctr++){
    newObject->addPixel(first.getPixel(ctr));
  }
  for(int ctr=0;ctr<second.getSize();ctr++){
    newObject->addPixel(second.getPixel(ctr));
  }
  newObject->calcParams();
  return *newObject;
}
//--------------------------------------------------------------------

vector <Detection> combineLists(vector <Detection> &first, 
				vector <Detection> &second)
{
  /** 
   * Function that combines two lists of Detection objects and returns
   * the combination. 
   * \param first,second The two lists of Detection objects to be combined.
   * \return A vector list Detection objects that includes all objects
   * in the two input lists.
   */ 

  vector <Detection> newList(first.size()+second.size());
  for(int i=0;i<first.size();i++) newList[i] = first[i];
  for(int i=0;i<second.size();i++) newList[i+first.size()] = second[i];
  
  return newList;
}

