#include <iostream>
#include <string.h>
#include <duchamp/duchamp.hh>
#include <duchamp/FitsIO/DuchampBeam.hh>
#include <duchamp/FitsIO/Beam.hh>
#include <duchamp/param.hh>
#include <sstream>

namespace duchamp 
{

  DuchampBeam::DuchampBeam():
    Beam()
  {
    this->itsOrigin = EMPTY;
  }

  DuchampBeam::DuchampBeam(float maj, float min, float pa):
    Beam(maj,min,pa)
  {
    this->itsOrigin  = EMPTY;
  }

  DuchampBeam::DuchampBeam(const Beam &b):
    Beam(b)
  {
    this->itsOrigin = EMPTY;
  }

  DuchampBeam::DuchampBeam(const DuchampBeam &b):
    Beam(b)
  {
    operator=(b);
  }

  DuchampBeam& DuchampBeam::operator=(const DuchampBeam &b)
  {
    ((Beam &) *this) = b;
    this->itsOrigin = b.itsOrigin;
    return *this;
  }
    
  void DuchampBeam::define(float maj, float min, float pa, BEAM_ORIGIN origin)
  {
    this->Beam::define(maj,min,pa);
    this->itsOrigin = origin;
  }
  
  void DuchampBeam::setFWHM(float fwhm, BEAM_ORIGIN origin)
  {
    this->Beam::setFWHM(fwhm);
    this->itsOrigin = origin;
  }

  void DuchampBeam::setArea(float area, BEAM_ORIGIN origin)
  {
    this->Beam::setArea(area);
    this->itsOrigin = origin;
  }

  void DuchampBeam::empty()
  {
    this->itsMaj = this->itsMin = this->itsPA = this->itsArea = 0.;
    this->itsOrigin = EMPTY;
  }

  std::string DuchampBeam::originString()
  {
    std::string output;
    switch(this->itsOrigin)
      {
      case HEADER:
	output="HEADER";
	break;
      case PARAM:
	output="PARAM";
	break;
      case EMPTY:
	output="EMPTY";
	break;
      default:
	output="ERROR";
	break;
      }
    return output;

  }

  void DuchampBeam::define(Param &par, bool warn)
  {
    std::string paramName;
    bool doWarning=warn;
    if(par.getBeamFWHM()>0.){
      this->setFWHM(par.getBeamFWHM(),PARAM);
      par.setBeamSize(this->itsArea);
      paramName = "beamFWHM";
      par.setBeamAsUsed(*this);
    }
    else if(par.getBeamSize()>0.){
      this->setArea(par.getBeamSize(),PARAM);
      paramName = "beamArea";
      par.setBeamAsUsed(*this);
    }
    else{
      this->empty();
      doWarning = false;
    }

    if(doWarning){
      std::stringstream errmsg;
      errmsg << "Header beam keywords not present. Using parameter "<< paramName <<" to determine size of beam.\n";
      duchampWarning("Beam definition",errmsg.str());
    }
  }
    

  void DuchampBeam::readFromFITS(fitsfile *fptr, Param &par, float pixelScale) // read from file a la headerIO.cc
  {
    char *comment = new char[80];
    std::string keyword[3]={"BMAJ","BMIN","BPA"};
    float bmaj,bmin,bpa;

    // Read the Keywords. If they are present, read the
    //   others, and calculate the beam size.
    // If it is not, give warning and set beam size to nominal value.
    int bstatus[3]={0,0,0};
    fits_read_key(fptr, TFLOAT, (char *)keyword[0].c_str(), &bmaj, comment, &bstatus[0]);
    fits_read_key(fptr, TFLOAT, (char *)keyword[1].c_str(), &bmin, comment, &bstatus[1]);
    fits_read_key(fptr, TFLOAT, (char *)keyword[2].c_str(), &bpa, comment,  &bstatus[2]);

    if(bstatus[0]||bstatus[1]||bstatus[2]){ // error
      this->define(par);
    }
    else{ // all keywords present
      this->define(bmaj/pixelScale, bmin/pixelScale, bpa, HEADER);
      par.setBeamAsUsed(*this);
    }
  }

  
  void DuchampBeam::writeToFITS(fitsfile *fptr) // write to file, but only if itsOrigin==HEADER. Use cfitsio commands directly.
  {
   
    if(this->itsOrigin == HEADER){
      char *keyword = new char[FLEN_KEYWORD];
      int status = 0;
      strcpy(keyword,"BMAJ");
      if (fits_update_key(fptr, TFLOAT, keyword, &this->itsMaj, NULL, &status)){
	duchampError("Writing beam","Error writing beam info:");
	fits_report_error(stderr, status);
      }
      status = 0;
      strcpy(keyword,"BMIN");
      if (fits_update_key(fptr, TFLOAT, keyword, &this->itsMin, NULL, &status)){
	duchampError("Writing beam","Error writing beam info:");
	fits_report_error(stderr, status);
      }
      status = 0;
      strcpy(keyword,"BPA");
      if (fits_update_key(fptr, TFLOAT, keyword, &this->itsPA, NULL, &status)){
	duchampError("Writing beam","Error writing beam info:");
	fits_report_error(stderr, status);
      }
    }
  }



}

