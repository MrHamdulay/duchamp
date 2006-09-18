#include <iostream>
#include <string>
#include <string.h>
#include <wcsunits.h>
#define WCSLIB_GETWCSTAB // define this so that we don't try and redefine 
                         //  wtbarr (this is a problem when using gcc v.4+
#include <fitsio.h>
#include <longnam.h>
#include <math.h>
#include <duchamp.hh>
#include <Cubes/cubes.hh>

int FitsHeader::readHeaderInfo(string fname, Param &par)
{

  this->getBUNIT(fname);
  
  this->getBLANKinfo(fname, par);
  
  this->getBeamInfo(fname);

  return SUCCESS;
}


//////////////////////////////////////////////////

int FitsHeader::getBUNIT(string fname)
{
  /**
   *  FitsHeader::getBUNIT(string fname)
   *   Read the BUNIT header keyword, to store the units
   *    of brightness (flux).
   */
  fitsfile *fptr;         
  char *comment = new char[FLEN_COMMENT];
  char *unit = new char[FLEN_VALUE];
  int returnStatus = 0, status = 0;
  status = 0;
  if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
    fits_report_error(stderr, status);
    return FAILURE;
  }
  fits_read_key(fptr, TSTRING, "BUNIT", unit, comment, &returnStatus);
  if (returnStatus){
    duchampWarning("getBUNIT","Error reading BUNIT keyword: ");
    fits_report_error(stderr, returnStatus);
  }
  else{
    wcsutrn(0,unit);
    this->fluxUnits = unit;
  }
  
  status = 0;
  fits_close_file(fptr, &status);
  if (status){
    duchampWarning("getCube","Error closing file: ");
    fits_report_error(stderr, status);
  }

  delete [] comment, unit;

  return returnStatus;
}

//////////////////////////////////////////////////

int FitsHeader::getBLANKinfo(string fname, Param &par)
{
  /**
   *   FitsHeader::getBLANKinfo(string fname, Param &par)
   *    Reading in the Blank pixel value keywords.
   *    If the BLANK keyword is in the header, use that and store the relevant 
   *     values.
   *    If not, use the default value (either the default from param.cc or 
   *     from the param file) and assume simple values for the keywords 
   *        --> the scale keyword is the same as the blank value, 
   *            the blank keyword (which is an int) is 1 and 
   *            the bzero (offset) is 0.
   */
  int returnStatus = 0, status = 0;
  if(par.getFlagBlankPix()){
    fitsfile *fptr;         
    char *comment = new char[FLEN_COMMENT];
    int blank;
    float bscale, bzero;
    if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
      fits_report_error(stderr, status);
      return FAILURE;
    }
    if(fits_read_key(fptr, TINT, "BLANK", &blank, comment, &returnStatus)){
      duchampWarning("getBLANKinfo","Error reading BLANK keyword: ");
      fits_report_error(stderr, returnStatus);
      std::stringstream errmsg;
      errmsg << "Using default BLANK value (" 
	     << par.getBlankPixVal() << ").\n";
      duchampWarning("getBLANKinfo", errmsg.str());
      this->setBlankKeyword(1);
      this->setBscaleKeyword(par.getBlankPixVal());
      this->setBzeroKeyword(0.);
      par.setFlagUsingBlank(true);
    }
    else{
      status = 0;
      fits_read_key(fptr, TFLOAT, "BZERO", &bzero, comment, &status);
      status = 0;
      fits_read_key(fptr, TFLOAT, "BSCALE", &bscale, NULL, &status);
      this->setBlankKeyword(blank);
      this->setBscaleKeyword(bscale);
      this->setBzeroKeyword(bzero);
    }
  
    status = 0;
    fits_close_file(fptr, &status);
    if (status){
      duchampWarning("getCube","Error closing file: ");
      fits_report_error(stderr, status);
    }
  
    delete [] comment;

  }

  return returnStatus;

}

//////////////////////////////////////////////////

int FitsHeader::getBeamInfo(string fname)
{
  /**
   *  FitsHeader::getBeamInfo(string fname)
   *   Reading in the beam parameters from the header.
   *   Use these, plus the basic WCS parameters to calculate the size of
   *    the beam in pixels.
   */
  char *comment = new char[80];
  float bmaj,bmin,cdelt1,cdelt2;
  int returnStatus = 0, status = 0;
  fitsfile *fptr;         
  status = 0;
  if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
    fits_report_error(stderr, status);
    return FAILURE;
  }
  if( !fits_read_key(fptr, TFLOAT, "BMAJ", &bmaj, comment, &returnStatus) ){
    fits_read_key(fptr, TFLOAT, "BMIN", &bmin, comment, &status);
    fits_read_key(fptr, TFLOAT, "CDELT1", &cdelt1, comment, &status);
    fits_read_key(fptr, TFLOAT, "CDELT2", &cdelt2, comment, &status);
    this->setBeamSize( M_PI * (bmaj/2.) * (bmin/2.) / fabs(cdelt1*cdelt2) );
    this->setBmajKeyword(bmaj);
    this->setBminKeyword(bmin);
  }
  if (returnStatus){
    duchampWarning("getBeamInfo",
       "No beam information in header. Setting size to nominal 10 pixels.\n");
    this->setBeamSize(10.);
  }
  
  status = 0;
  fits_close_file(fptr, &status);
  if (status){
    duchampWarning("getCube","Error closing file: ");
    fits_report_error(stderr, status);
  }

  delete [] comment;

  return returnStatus;
}
