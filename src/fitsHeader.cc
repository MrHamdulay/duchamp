#include <iostream>
#include <sstream>
#include <string>
#include <wcs.h>
#include <wcsunits.h>
#include <duchamp.hh>
#include <param.hh>
#include <fitsHeader.hh>
#include <Utils/utils.hh>

FitsHeader::FitsHeader()
{
  this->wcs = (struct wcsprm *)calloc(1,sizeof(struct wcsprm));
  this->wcs->flag=-1;
  wcsini(true, 3, this->wcs); 
  this->wcsIsGood = false;
  this->nwcs = 0;
  this->scale=1.;
  this->offset=0.;
  this->power=1.;
  this->fluxUnits="counts";
}

FitsHeader::~FitsHeader()
{
  /**
   *  Uses the WCSLIB function wcsvfree to clear the wcsprm struct.
   */
  wcsvfree(&nwcs,&wcs);
}

FitsHeader::FitsHeader(const FitsHeader& h)
{
  this->wcs = (struct wcsprm *)calloc(1,sizeof(struct wcsprm));
  this->wcs->flag     = -1;
  wcsini(true, h.wcs->naxis, this->wcs); 
  wcscopy(true, h.wcs, this->wcs); 
  wcsset(this->wcs);
  this->nwcs          = h.nwcs;
  this->wcsIsGood     = h.wcsIsGood;
  this->spectralUnits = h.spectralUnits;
  this->fluxUnits     = h.fluxUnits;
  this->intFluxUnits  = h.intFluxUnits;
  this->beamSize      = h.beamSize;
  this->bmajKeyword   = h.bmajKeyword;
  this->bminKeyword   = h.bminKeyword;
  this->bpaKeyword    = h.bpaKeyword;
  this->blankKeyword  = h.blankKeyword;
  this->bzeroKeyword  = h.bzeroKeyword;
  this->bscaleKeyword = h.bscaleKeyword;
  this->scale         = h.scale;
  this->offset        = h.offset;
  this->power         = h.power;
}

FitsHeader& FitsHeader::operator= (const FitsHeader& h)
{
  if(this == &h) return *this;
  this->wcs = (struct wcsprm *)calloc(1,sizeof(struct wcsprm));
  this->wcs->flag     = -1;
  wcsini(true, h.wcs->naxis, this->wcs); 
  wcscopy(true, h.wcs, this->wcs); 
  wcsset(this->wcs);
  this->nwcs          = h.nwcs;
  this->wcsIsGood     = h.wcsIsGood;
  this->spectralUnits = h.spectralUnits;
  this->fluxUnits     = h.fluxUnits;
  this->intFluxUnits  = h.intFluxUnits;
  this->beamSize      = h.beamSize;
  this->bmajKeyword   = h.bmajKeyword;
  this->bminKeyword   = h.bminKeyword;
  this->bpaKeyword    = h.bpaKeyword;
  this->blankKeyword  = h.blankKeyword;
  this->bzeroKeyword  = h.bzeroKeyword;
  this->bscaleKeyword = h.bscaleKeyword;
  this->scale         = h.scale;
  this->offset        = h.offset;
  this->power         = h.power;
  return *this;
}

void FitsHeader::setWCS(struct wcsprm *w)
{
  /** 
   *  A function that assigns the wcs parameters, and runs
   *   wcsset to set it up correctly.
   *  Performs a check to see if the WCS is good (by looking at 
   *   the lng and lat wcsprm parameters), and sets the wcsIsGood 
   *   flag accordingly.
   * \param w A WCSLIB wcsprm struct with the correct parameters.
   */
  wcscopy(true, w, this->wcs);
  wcsset(this->wcs);
  if( (w->lng!=-1) && (w->lat!=-1) ) this->wcsIsGood = true;
}

struct wcsprm *FitsHeader::getWCS()
{
  /** 
   *  A function that returns a properly initilized wcsprm object
   *  corresponding to the WCS.
   */
  struct wcsprm *wNew = (struct wcsprm *)calloc(1,sizeof(struct wcsprm));
  wNew->flag=-1;
  wcsini(true, this->wcs->naxis, wNew); 
  wcscopy(true, this->wcs, wNew); 
  wcsset(wNew);
  return wNew;
}

int FitsHeader::wcsToPix(const double *world, double *pix)
{      
  return wcsToPixSingle(this->wcs, world, pix);  
};
int FitsHeader::wcsToPix(const double *world, double *pix, const int npts)
{ 
  return wcsToPixMulti(this->wcs, world, pix, npts);  
};
int FitsHeader::pixToWCS(const double *pix, double *world)
{    
  return pixToWCSSingle(this->wcs, pix, world);  
};
int FitsHeader::pixToWCS(const double *pix, double *world, const int npts)
{
  return pixToWCSMulti(this->wcs, pix,world, npts);  
};


double FitsHeader::pixToVel(double &x, double &y, double &z)
{
  double vel;
  if(this->wcsIsGood){
    double *pix   = new double[3]; 
    double *world = new double[3];
    pix[0] = x; pix[1] = y; pix[2] = z;
    pixToWCSSingle(this->wcs,pix,world);
    vel = this->specToVel(world[2]);
    delete [] pix;
    delete [] world;
  }
  else vel = z;
  return vel;
}

double* FitsHeader::pixToVel(double &x, double &y, double *zarray, int size)
{
  double *newzarray = new double[size];
  if(this->wcsIsGood){
    double *pix   = new double[size*3];
    for(int i=0;i<size;i++){
      pix[3*i]   = x; 
      pix[3*i+1] = y; 
      pix[3*i+2] = zarray[i];
    }
    double *world = new double[size*3];
    pixToWCSMulti(this->wcs,pix,world,size);
    delete [] pix;
    for(int i=0;i<size;i++) newzarray[i] = this->specToVel(world[3*i+2]);
    delete [] world;
  }
  else{
    for(int i=0;i<size;i++) newzarray[i] = zarray[i];
  }
  return newzarray;
}

double FitsHeader::specToVel(const double &coord)
{
  double vel;
  if(power==1.0) vel =  coord*this->scale + this->offset;
  else vel = pow( (coord*this->scale + this->offset), this->power);
  return vel;
}

double FitsHeader::velToSpec(const float &velocity)
{
//   return velToCoord(this->wcs,velocity,this->spectralUnits);};
  return (pow(velocity, 1./this->power) - this->offset) / this->scale;
}

std::string FitsHeader::getIAUName(double ra, double dec)
{
  if(strcmp(this->wcs->lngtyp,"RA")==0) 
    return getIAUNameEQ(ra, dec, this->wcs->equinox);
  else 
    return getIAUNameGAL(ra, dec);
}

void FitsHeader::fixUnits(Param &par)
{
  // define spectral units from the param set
  this->spectralUnits = par.getSpectralUnits();

  double sc=1.;
  double of=0.;
  double po=1.;
  if(this->wcsIsGood){
    int status = wcsunits( this->wcs->cunit[this->wcs->spec], 
			   this->spectralUnits.c_str(), 
			   &sc, &of, &po);
    if(status > 0){
      std::stringstream errmsg;
      errmsg << "WCSUNITS Error, Code = " << status
	     << ": " << wcsunits_errmsg[status];
      if(status == 10) 
	errmsg << "\nTried to get conversion from \"" 
	       << this->wcs->cunit[this->wcs->spec] << "\" to \"" 
	       << this->spectralUnits.c_str() << "\".\n";
      this->spectralUnits = this->wcs->cunit[this->wcs->spec];
      if(this->spectralUnits==""){
	errmsg << "Spectral units not specified. "
	       << "For data presentation, we will use dummy units of \"SPC\".\n"
	       << "Please report this occurence -- it should not happen now!"
	       << "In the meantime, you may want to set the CUNIT"
	       << this->wcs->spec + 1 <<" keyword to make this work.\n";
	this->spectralUnits = "SPC";
      }
      duchampError("fixUnits", errmsg.str());
      
    }
  }
  this->scale = sc;
  this->offset= of;
  this->power = po;

  // Work out the integrated flux units, based on the spectral units.
  // If flux is per beam, trim the /beam from the flux units and multiply 
  //  by the spectral units.
  // Otherwise, just muliply by the spectral units.
  if(this->fluxUnits.size()>0){
    if(this->fluxUnits.substr(this->fluxUnits.size()-5,
			      this->fluxUnits.size()   ) == "/beam"){
      this->intFluxUnits = this->fluxUnits.substr(0,this->fluxUnits.size()-5)
	+" " +this->spectralUnits;
    }
    else this->intFluxUnits = this->fluxUnits + " " + this->spectralUnits;
  }

}
