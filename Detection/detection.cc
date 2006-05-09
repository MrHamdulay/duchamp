#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <wcs.h>
#include <math.h>
#include <param.hh>
#include <Utils/utils.hh>
#include <Detection/detection.hh>

using std::setw;
using std::setprecision;
using std::endl;

std::ostream& operator<< ( std::ostream& theStream, Voxel& vox)
{
  /**
   * << operator for Voxel class
   * A convenient way of printing the coordinate & flux values of a voxel.
   */  

  theStream << setw(4) << vox.itsX ;
  theStream << " " << setw(4) << vox.itsY;
  theStream << " " << setw(4) << vox.itsZ;
  theStream << setprecision(4);
  theStream << "  " << vox.itsF;

}


void Detection::calcParams()
{
  /**
   * Detection::calcParams()
   *  A function that calculates centroid positions, minima & maxima of coordinates,
   *   and total & peak fluxes for a detection.
   */

  if(this->pix.size()>0){
    this->xcentre = 0;
    this->ycentre = 0;
    this->zcentre = 0;
    this->totalFlux = 0;
    this->peakFlux = this->pix[0].itsF;
    this->xmin = 0;
    this->xmax = 0;
    this->ymin = 0;
    this->ymax = 0;
    this->zmin = 0;
    this->zmax = 0;
    int *ctr = new int;
    for((*ctr)=0;(*ctr)<this->pix.size();(*ctr)++){
      this->xcentre   += this->pix[*ctr].itsX;
      this->ycentre   += this->pix[*ctr].itsY;
      this->zcentre   += this->pix[*ctr].itsZ;
      this->totalFlux += this->pix[*ctr].itsF;
      if(((*ctr)==0)||(this->pix[*ctr].itsX<this->xmin))     this->xmin     = this->pix[*ctr].itsX;
      if(((*ctr)==0)||(this->pix[*ctr].itsX>this->xmax))     this->xmax     = this->pix[*ctr].itsX;
      if(((*ctr)==0)||(this->pix[*ctr].itsY<this->ymin))     this->ymin     = this->pix[*ctr].itsY;
      if(((*ctr)==0)||(this->pix[*ctr].itsY>this->ymax))     this->ymax     = this->pix[*ctr].itsY;
      if(((*ctr)==0)||(this->pix[*ctr].itsZ<this->zmin))     this->zmin     = this->pix[*ctr].itsZ;
      if(((*ctr)==0)||(this->pix[*ctr].itsZ>this->zmax))     this->zmax     = this->pix[*ctr].itsZ;
      if(this->negativeSource){
	// if negative feature, peakFlux is most negative flux
	if(((*ctr)==0)||(this->pix[*ctr].itsF < this->peakFlux)){
	  this->peakFlux = this->pix[*ctr].itsF;
	  this->xpeak = this->pix[*ctr].itsX;
	  this->ypeak = this->pix[*ctr].itsY;
	  this->zpeak = this->pix[*ctr].itsZ;
	}
      }
      else{
	// otherwise, it's a regular detection, and peakFlux is most positive flux
	if(((*ctr)==0)||(this->pix[*ctr].itsF > this->peakFlux)){
	  this->peakFlux = this->pix[*ctr].itsF;
	  this->xpeak = this->pix[*ctr].itsX;
	  this->ypeak = this->pix[*ctr].itsY;
	  this->zpeak = this->pix[*ctr].itsZ;
	}
      }
    }
    delete ctr;
    this->xcentre /= this->pix.size();
    this->ycentre /= this->pix.size();
    this->zcentre /= this->pix.size();
  }
}

void Detection::calcWCSparams(wcsprm *wcs)
{
  /**
   * Detection::calcWCSparams(wcsprm *)
   *  Use the input wcs to calculate the position and velocity information for the Detection.
   *  Makes no judgement as to whether the WCS is good -- this needs to be done beforehand.
   *  Quantities calculated:
   *     RA: ra [deg], ra (string), ra width.
   *     Dec: dec [deg], dec (string), dec width.
   *     Vel: vel [km/s], min & max vel, vel width.
   *     Other: coord type for all three axes, nuRest, name (IAU-style, in equatorial or Galactic)
   *     Uses getIntegFlux to calculate the integrated flux in (say) [Jy km/s]
   */

  this->calcParams(); // make sure this is up to date.

  double *pixcrd = new double[15];
  double *world  = new double[15];
  /*
  define a five-point array in 3D:
     (x,y,z), (x,y,z1), (x,y,z2), (x1,y1,z), (x2,y2,z)
   note x = central point, x1 = minimum x, x2 = maximum x etc.
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
  int flag = pixToWCSMulti(wcs, pixcrd, world, 5);
  delete [] pixcrd;

  // world now has the WCS coords for the five points -- use this to work out WCS params.
  
  this->lngtype = wcs->lngtyp;
  this->lattype = wcs->lattyp;
  this->ztype   = wcs->ctype[2];
  this->nuRest  = wcs->restfrq;
  this->ra   = world[0];
  this->dec  = world[1];
  this->raS = decToDMS(this->ra,this->lngtype);
  this->decS = decToDMS(this->dec,this->lattype);
  this->raWidth   = angularSeparation(world[9],world[1],world[12],world[1]) * 60.;
  this->decWidth  = angularSeparation(world[0],world[10],world[0],world[13]) * 60.;
  if(this->lngtype=="RA") this->name = getIAUNameEQ(this->ra,this->dec,wcs->equinox);
  else this->name = getIAUNameGAL(this->ra,this->dec);
  this->vel    = setVel_kms(wcs, world[2]);
  this->velMin = setVel_kms(wcs, world[5]);
  this->velMax = setVel_kms(wcs, world[8]);
  this->velWidth = fabs(this->velMax - this->velMin);

  this->getIntegFlux(wcs);

  this->flagWCS = true;

  delete [] world;

}

float Detection::getIntegFlux(wcsprm *wcs)
{
  /**
   * Detection::getIntegFlux(wcsprm *)
   *  Uses the input wcs to calculate the velocity-integrated flux, 
   *  putting velocity in units of km/s.
   *  Integrates over full spatial and velocity range as given by extrema calculated 
   *  by calcWCSparams.
   *  Units are (Jy/beam) . km/s  (no correction for the beam is made here...)
   */

  // include one pixel either side in each direction
  int xsize = (this->xmax-this->xmin+3);
  int ysize = (this->ymax-this->ymin+3);
  int zsize = (this->zmax-this->zmin+3); 
  vector <bool> isObj(xsize*ysize*zsize,false);
  vector <float> fluxArray(xsize*ysize*zsize,0.);
  // work out which pixels are object pixels
  for(int p=0;p<this->pix.size();p++){
    int pos = (this->pix[p].getX()-this->xmin+1) + (this->pix[p].getY()-this->ymin+1)*xsize + 
      (this->pix[p].getZ()-this->zmin+1)*xsize*ysize;
    fluxArray[pos] = this->pix[p].getF();
    isObj[pos] = true;
  }
  
  // work out the WCS coords for each pixel
  double *world  = new double[xsize*ysize*zsize*3];
  double *pix = new double[xsize*ysize*zsize*3];
  for(int i=0;i<xsize*ysize*zsize;i++){
    pix[i*3+0] = this->xmin -1 + i%xsize;
    pix[i*3+1] = this->ymin -1 + (i/xsize)%ysize;
    pix[i*3+2] = this->zmin -1 + i/(xsize*ysize);
  }
  int flag = pixToWCSMulti(wcs, pix, world, xsize*ysize*zsize);
  delete [] pix;

  // put velocity coords into km/s
  for(int i=0;i<xsize*ysize*zsize;i++)
    world[3*i+2] = setVel_kms(wcs,world[3*i+2]);

  this->intFlux = 0.;
  for(int pix=0; pix<xsize*ysize; pix++){ // loop over each spatial pixel.
    for(int z=0; z<zsize; z++){
      int pos =  z*xsize*ysize + pix;
      if(isObj[pos]){ // if it's an object pixel...
	float deltaVel = (world[3*(pos+xsize*ysize)+2] - world[ 3*(pos-xsize*ysize)+2 ]) / 2.;
	this->intFlux += fluxArray[pos] * fabsf(deltaVel);
      }
    }
  }
  delete [] world;
}


// void Detection::addAnObject(Detection &toAdd)
// {
  /**
   * Detection::addAnObject(Detection &)
   *  Combines two objects by adding all the pixels of the argument to the instigator.
   *  All pixel & flux parameters are recalculated (so that calcParams does not need to be 
   *   called a second time), but WCS parameters are not.
   */
/*
  int *ctr = new int;
  this->xcentre *= this->pix.size();
  this->ycentre *= this->pix.size();
  this->zcentre *= this->pix.size();
  for((*ctr)=0;(*ctr)<toAdd.getSize();(*ctr)++){
    this->pix.push_back(toAdd.getPixel(*ctr));
    this->xcentre += toAdd.getX(*ctr);
    this->ycentre += toAdd.getY(*ctr);
    this->zcentre += toAdd.getZ(*ctr);
    this->totalFlux += toAdd.getF(*ctr);
    if(toAdd.getX(*ctr)<this->xmin) this->xmin=toAdd.getX(*ctr);
    if(toAdd.getX(*ctr)>this->xmax) this->xmax=toAdd.getX(*ctr);
    if(toAdd.getY(*ctr)<this->ymin) this->ymin=toAdd.getY(*ctr);
    if(toAdd.getY(*ctr)>this->ymax) this->ymax=toAdd.getY(*ctr);
    if(toAdd.getZ(*ctr)<this->zmin) this->zmin=toAdd.getZ(*ctr);
    if(toAdd.getZ(*ctr)>this->zmax) this->zmax=toAdd.getZ(*ctr);
    if(toAdd.getF(*ctr)>this->peakFlux) this->peakFlux=toAdd.getF(*ctr);
  }
  this->xcentre /= this->pix.size();
  this->ycentre /= this->pix.size();
  this->zcentre /= this->pix.size();
    
  delete ctr;

}
*/


void Detection::addAnObject(Detection &toAdd)
{
  /**
   * Detection::addAnObject(Detection &)
   *  Combines two objects by adding all the pixels of the argument to the instigator.
   *  All pixel & flux parameters are recalculated (so that calcParams does not need to be 
   *   called a second time), but WCS parameters are not.
   */

  int *ctr = new int;
  int *ctr2 = new int;
  bool *isNewPix = new bool;
  this->xcentre *= this->pix.size();
  this->ycentre *= this->pix.size();
  this->zcentre *= this->pix.size();
  for((*ctr)=0;(*ctr)<toAdd.getSize();(*ctr)++){
    // For each pixel in the new object, test to see if it already appears in the object
    long *x = new long;
    *x = toAdd.getX(*ctr);
    long *y = new long;
    *y = toAdd.getY(*ctr);
    long *z = new long;
    *z = toAdd.getZ(*ctr);
    float *f = new float;
    *f = toAdd.getF(*ctr);
    *isNewPix = true;
    *ctr2 = 0;
    while( *isNewPix && (*ctr2<this->pix.size()) ){
      *isNewPix = *isNewPix && (( this->pix[*ctr2].itsX != *x ) || 
				( this->pix[*ctr2].itsY != *y ) ||
				( this->pix[*ctr2].itsZ != *z ) );
      *ctr2 += 1;
    }
    if(*isNewPix){
      // If the pixel is new, add it to the object and re-calculate the parameters.
      this->pix.push_back(toAdd.getPixel(*ctr));
      this->xcentre += *x;
      this->ycentre += *y;
      this->zcentre += *z;
      this->totalFlux += *f;
      if(*x<this->xmin) this->xmin=*x;
      if(*x>this->xmax) this->xmax=*x;
      if(*y<this->ymin) this->ymin=*y;
      if(*y>this->ymax) this->ymax=*y;
      if(*z<this->zmin) this->zmin=*z;
      if(*z>this->zmax) this->zmax=*z;
      if(*f>this->peakFlux) this->peakFlux=*f;
    }
    delete x,y,z,f;
  }
  this->xcentre /= this->pix.size();
  this->ycentre /= this->pix.size();
  this->zcentre /= this->pix.size();
    
  delete ctr;
  delete ctr2;
  delete isNewPix;
}


void Detection::addOffsets(Param &par)
{
  this->xSubOffset = par.getXOffset();
  this->ySubOffset = par.getYOffset();
  this->zSubOffset = par.getZOffset();
}

bool Detection::hasEnoughChannels(int minNumber)
{
  /**
   *  bool hasEnoughChannels(int)
   *    A function to determine if the Detection has enough
   *    contiguous channels to meet the minimum requirement
   *    given as the argument. 
   *    Needs to have at least one occurence of minNumber consecutive
   *    channels present to return true. Otherwise returns false.
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

    if( numChannels == minNumber) result = true;
  }
  return result;
  
}

int Detection::getSpatialSize()
{
  /**
   *  int getSpatialSize()
   *    A function that returns the number of distinct spatial
   *     pixels in a Detection.
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

std::ostream& operator<< ( std::ostream& theStream, Detection& obj)
{
  /**
   * << operator for Detection class
   *  A convenient way of printing the coordinate & flux 
   *  values for each pixel in the Detection
   */  

  for(int i=0;i<obj.pix.size();i++) theStream << obj.pix[i] << endl;
  theStream<<"---"<<endl;
}

Detection combineObjects(Detection &first, Detection &second)
{
  // make the new object
  int *ctr = new int;
  Detection *newObject = new Detection;
  for(*ctr=0;(*ctr)<first.getSize();(*ctr)++){
    newObject->addPixel(first.getPixel(*ctr));
  }
  for(*ctr=0;(*ctr)<second.getSize();(*ctr)++){
    newObject->addPixel(second.getPixel(*ctr));
  }
  delete ctr;
  newObject->calcParams();
  return *newObject;
}

vector <Detection> combineLists(vector <Detection> &first, vector <Detection> &second)
{
  // make the new object
  vector <Detection> newList(first.size()+second.size());
  for(int i=0;i<first.size();i++) newList[i] = first[i];
  for(int i=0;i<second.size();i++) newList[i+first.size()] = second[i];
  
  return newList;
}

