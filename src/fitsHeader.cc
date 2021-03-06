// -----------------------------------------------------------------------
// fitsHeader.cc: Information about the FITS file's header.
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
#include <sstream>
#include <string>
#include <stdlib.h>
#include <wcslib/wcs.h>
#include <wcslib/wcsunits.h>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Utils/utils.hh>

namespace duchamp
{

  FitsHeader::FitsHeader()
  {
    this->fptr = 0;
    this->wcs = (struct wcsprm *)calloc(1,sizeof(struct wcsprm));
    this->wcs->flag=-1;
    wcsini(true, 3, this->wcs); 
    this->wcsIsGood = false;
    this->nwcs = 0;
    this->flag2D=false;
    this->spectralUnits="";
    this->spectralDescription=duchamp::duchampSpectralDescription[FREQUENCY];
    this->originalFluxUnits="";
    this->fluxUnits="";
    this->intFluxUnits="";
    this->scale=1.;
    this->offset=0.;
    this->power=1.;
    this->itsBeam.empty();
  }

  FitsHeader::~FitsHeader()
  {
    /// @details Uses the WCSLIB function wcsvfree to clear the wcsprm struct.
    wcsvfree(&nwcs,&wcs);
  }

  FitsHeader::FitsHeader(const FitsHeader& h)
  {
    operator=(h);
  }

  FitsHeader& FitsHeader::operator= (const FitsHeader& h)
  {
    if(this == &h) return *this;
    this->fptr = h.fptr;
    this->wcs = (struct wcsprm *)calloc(1,sizeof(struct wcsprm));
    this->wcs->flag     = -1;
    wcsini(true, h.wcs->naxis, this->wcs); 
    wcscopy(true, h.wcs, this->wcs); 
    wcsset(this->wcs);
    this->nwcs          = h.nwcs;
    this->wcsIsGood     = h.wcsIsGood;
    this->flag2D        = h.flag2D;
    this->spectralUnits = h.spectralUnits;
    this->spectralType  = h.spectralType;
    this->spectralDescription = h.spectralDescription;
    this->originalFluxUnits = h.originalFluxUnits;
    this->fluxUnits     = h.fluxUnits;
    this->intFluxUnits  = h.intFluxUnits;
    this->itsBeam       = h.itsBeam;
    this->blankKeyword  = h.blankKeyword;
    this->bzeroKeyword  = h.bzeroKeyword;
    this->bscaleKeyword = h.bscaleKeyword;
    this->scale         = h.scale;
    this->offset        = h.offset;
    this->power         = h.power;
    return *this;
  }

  int FitsHeader::openFITS(std::string name)
  {
    int status = 0;
    if( fits_open_file(&this->fptr,name.c_str(),READONLY,&status) ){
      fits_report_error(stderr, status);
      // if(((status==URL_PARSE_ERROR)||(status==BAD_NAXIS))
      // 	 &&(this->pars().getFlagSubsection()))
      // 	DUCHAMPERROR("Cube Reader", "It may be that the subsection you've entered is invalid. Either it has the wrong number of axes, or one axis has too large a range.");
    }
    return status;
  }

  int FitsHeader::closeFITS()
  {
    int status=0;
    fits_close_file(this->fptr, &status);
    if (status){
      DUCHAMPWARN("Cube Reader","Error closing file: ");
      fits_report_error(stderr, status);
    }
    return status;

  }

  void FitsHeader::setWCS(struct wcsprm *w)
  {
    ///  A function that assigns the wcs parameters, and runs
    ///   wcsset to set it up correctly.
    ///  Performs a check to see if the WCS is good (by looking at 
    ///   the lng and lat wcsprm parameters), and sets the wcsIsGood 
    ///   flag accordingly.
    /// \param w A WCSLIB wcsprm struct with the correct parameters.

    wcscopy(true, w, this->wcs);
    wcsset(this->wcs);
    if( (w->lng!=-1) && (w->lat!=-1) ) this->wcsIsGood = true;
  }

  struct wcsprm *FitsHeader::getWCScopy()
  {
    ///  A function that returns a properly initilized wcsprm object
    ///  corresponding to the WCS.

    struct wcsprm *wNew = (struct wcsprm *)calloc(1,sizeof(struct wcsprm));
    wNew->flag=-1;
    wcsini(true, this->wcs->naxis, wNew); 
    wcscopy(true, this->wcs, wNew); 
    wcsset(wNew);
    return wNew;
  }

  int FitsHeader::wcsToPix(double xWorld, double yWorld, double zWorld, double &xPix, double &yPix, double &zPix)
  {
      double world[3],pix[3];
      world[0]=xWorld;
      world[1]=yWorld;
      world[2]=zWorld;
      int returnval = wcsToPixSingle(this->wcs, world, pix); 
      if(returnval==0){
	  xPix=pix[0];
	  yPix=pix[1];
	  zPix=pix[2];
      }
      else {
	  xPix=-1.;
	  yPix=-1.;
	  zPix=-1.;
      }
      return returnval;
  }
  int FitsHeader::wcsToPix(const double *world, double *pix)
  {      
    return wcsToPixSingle(this->wcs, world, pix);  
  }
  int FitsHeader::wcsToPix(const double *world, double *pix, const int npts)
  { 
    return wcsToPixMulti(this->wcs, world, pix, npts);  
  }
  int FitsHeader::pixToWCS(double xPix, double yPix, double zPix, double &xWorld, double &yWorld, double &zWorld)
  {
      double world[3],pix[3];
      pix[0]=xPix;
      pix[1]=yPix;
      pix[2]=zPix;
      int returnval = pixToWCSSingle(this->wcs, pix, world); 
      if(returnval==0){
	  xWorld=world[0];
	  yWorld=world[1];
	  zWorld=world[2];
      }
      else {
	  xWorld=-1.;
	  yWorld=-1.;
	  zWorld=-1.;
      }
      return returnval;
  }
  int FitsHeader::pixToWCS(const double *pix, double *world)
  {    
    return pixToWCSSingle(this->wcs, pix, world);  
  }
  int FitsHeader::pixToWCS(const double *pix, double *world, const int npts)
  {
    return pixToWCSMulti(this->wcs, pix,world, npts);  
  }


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
    if(std::string(this->wcs->lngtyp)=="RA") 
      return getIAUNameEQ(ra, dec, this->wcs->equinox);
    else 
      return getIAUNameGAL(ra, dec);
  }

  void FitsHeader::fixSpectralUnits(std::string units)
  {
    ///  Put the units for the FITS header into some sort of standard form.
    /// 
    ///  We first get the desired spectral units from the Parameter set,
    ///  and then transform the spectral units of the wcsprm struct to
    ///  those units. If this doesn't work, we leave them as they are. If
    ///  they are blank, we make them SPC and give an error message --
    ///  this should hopefully NOT happen.
    /// 
    ///  We also work out the units for the integrated flux. If there are
    ///  three axes, we just append the spectral units to the flux units
    ///  (removing "/beam" if it is present). If there are just two, we
    ///  simply keep it the same, removing the "/beam".
    /// 
    ///  \param par The parameter set telling us what the desired
    ///             spectral units are.


    double sc=1.;
    double of=0.;
    double po=1.;
  
    this->spectralUnits = this->wcs->cunit[this->wcs->spec];

    if(units != ""){

      if(this->wcsIsGood){
 
	int status = wcsunits( this->wcs->cunit[this->wcs->spec], units.c_str(), &sc, &of, &po);

	if(status > 0){
	  DUCHAMPERROR("fixSpectralUnits","Conversion of spectral units from '" << this->wcs->cunit[this->wcs->spec] << "' to '" << units 
		       << "' failed, wcslib code = " << status << ": " << wcsunits_errmsg[status]);
	  if(this->spectralUnits==""){
	    DUCHAMPERROR("fixSpectralUnits", "Spectral units not specified. For data presentation, we will use dummy units of \"SPC\".\n"
			 << "Please report this occurence -- it should not happen! In the meantime, you may want to set the CUNIT"
			 << this->wcs->spec + 1 <<" keyword to make this work.");
	    this->spectralUnits = "SPC";
	  }
	}
	else this->spectralUnits = units;
      }
    }
    this->scale = sc;
    this->offset= of;
    this->power = po;
    
  }

  bool FitsHeader::needBeamSize()
  {
    ///  A function that tells you whether the beam correction is
    ///  needed. It checks to see whether the flux units string ends in
    ///  "/beam" (in which case the beam size etc is needed and
    ///  integrated fluxes need to be corrected). If we don't have any beam
    ///  information, this will return false.
    ///  /return True if FitsHeader::fluxUnits ends in "/beam". False
    ///  otherwise.

    int size = this->fluxUnits.size();
    if(this->itsBeam.origin()==EMPTY) return false; // we have no beam to correct with!
    else if(size<6) return false;
    else {
      std::string tailOfFluxUnits = makelower(this->fluxUnits.substr(size-5,size));
      return (tailOfFluxUnits == "/beam");
    }
  }

  void FitsHeader::setIntFluxUnits()
  {

    /// Work out the integrated flux units, based on the spectral
    /// units. If flux is per beam, trim the /beam from the flux
    /// units. Then, if as long as the image is 3D, multiply by the
    /// spectral units.
    
    if(this->needBeamSize())
      this->intFluxUnits = this->fluxUnits.substr(0,this->fluxUnits.size()-5);
    else 
      this->intFluxUnits = this->fluxUnits;

    if(!this->flag2D) this->intFluxUnits += std::string(" ") + this->spectralUnits;

  }

  std::string FitsHeader::lngtype()
  {
    std::string lngtyp(this->wcs->lngtyp);
    if(removeLeadingBlanks(lngtyp)==""){
      lngtyp = this->wcs->ctype[this->wcs->lng];
      return lngtyp.substr(0,4);
    }
    else return lngtyp;
  }

  std::string FitsHeader::lattype()
  {
    std::string lattyp(this->wcs->lattyp);
    if(removeLeadingBlanks(lattyp)==""){
      lattyp = this->wcs->ctype[this->wcs->lat];
      return lattyp.substr(0,4);
    }
    else return lattyp;
  }

  float FitsHeader::getShapeScale()
  {
    // Returns a scale factor that will scale the reported size of the fitted ellipses to numbers that are sensible.
      float scale;
      float cdelt = fabs(this->wcs->cdelt[this->wcs->lng]);
      if(cdelt>0.01) scale =1.;
      else if(cdelt<1.e-3) scale=3600.;
      else scale = 60.;
      return scale;
  }

  std::string FitsHeader::getShapeUnits()
  {
      std::string units="deg";
      float cdelt = fabs(this->wcs->cdelt[this->wcs->lng]);
      if(cdelt>0.01) units="deg";
      else if(cdelt<1.e-3) units="arcsec";
      else units="arcmin";
      return units;
  }


}
