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
  /** 
   * FitsHeader::readHeaderInfo
   *  A simple front-end function to the three header access
   *   functions defined below.
   *
   */

  int returnValue = SUCCESS;

  if(this->readBUNIT(fname)==FAILURE) returnValue=FAILURE;
  
  if(this->readBLANKinfo(fname, par)==FAILURE) returnValue=FAILURE;
  
  if(this->readBeamInfo(fname, par)==FAILURE) returnValue=FAILURE;

  return returnValue;
}


//////////////////////////////////////////////////

int FitsHeader::readBUNIT(string fname)
{
  /**
   *  FitsHeader::readBUNIT(string fname)
   *   Read the BUNIT header keyword, to store the units
   *    of brightness (flux).
   */
  fitsfile *fptr;         
  char *comment = new char[FLEN_COMMENT];
  char *unit = new char[FLEN_VALUE];
  int returnStatus = 0, status = 0;

  // Open the FITS file
  status = 0;
  if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
    fits_report_error(stderr, status);
    return FAILURE;
  }

  // Read the BUNIT keyword, and translate to standard unit format if needs be
  fits_read_key(fptr, TSTRING, "BUNIT", unit, comment, &returnStatus);
  if (returnStatus){
    duchampWarning("readBUNIT","Error reading BUNIT keyword: ");
    fits_report_error(stderr, returnStatus);
  }
  else{
    wcsutrn(0,unit);
    this->fluxUnits = unit;
  }

  // Close the FITS file
  status = 0;
  fits_close_file(fptr, &status);
  if (status){
    duchampWarning("readBUNIT","Error closing file: ");
    fits_report_error(stderr, status);
  }

  delete [] comment, unit;

  return returnStatus;
}

//////////////////////////////////////////////////

int FitsHeader::readBLANKinfo(string fname, Param &par)
{
  /**
   *   FitsHeader::readBLANKinfo(string fname, Param &par)
   *    Reading in the Blank pixel value keywords.
   *    If the BLANK keyword is in the header, use that and store the relevant 
   *     values. Also copy them into the parameter set.
   *    If not, use the default value (either the default from param.cc or 
   *     from the param file) and assume simple values for the keywords 
   *        --> the scale keyword is the same as the blank value, 
   *            the blank keyword (which is an int) is 1 and 
   *            the bzero (offset) is 0.
   */
  int returnStatus = 0, status = 0;
  if(par.getFlagBlankPix()){  // Only do this if we want the blank pix value

    fitsfile *fptr;         
    char *comment = new char[FLEN_COMMENT];
    int blank;
    float bscale, bzero;
    
    // Open the FITS file.
    if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
      fits_report_error(stderr, status);
      return FAILURE;
    }

    // Read the BLANK keyword. 
    //  If this isn't present, then report the error, and set to 
    //     the nominal values given in Param
    //  If it is, read the other two necessary keywords, and then set
    //     the values accordingly.
    if(fits_read_key(fptr, TINT, "BLANK", &blank, comment, &returnStatus)){
      duchampWarning("readBLANKinfo","Error reading BLANK keyword: ");
      fits_report_error(stderr, returnStatus);
      std::stringstream errmsg;
      errmsg << "Using default BLANK value (" 
	     << par.getBlankPixVal() << ").\n";
      duchampWarning("readBLANKinfo", errmsg.str());
      this->blankKeyword  = 1;
      this->bscaleKeyword = par.getBlankPixVal();
      this->bzeroKeyword  = 0;
      par.setBlankKeyword(1);
      par.setBzeroKeyword(0);
      par.setFlagUsingBlank(true);
    }
    else{
      status = 0;
      fits_read_key(fptr, TFLOAT, "BZERO", &bzero, comment, &status);
      status = 0;
      fits_read_key(fptr, TFLOAT, "BSCALE", &bscale, NULL, &status);
      this->blankKeyword  = blank;
      this->bscaleKeyword = bscale;
      this->bzeroKeyword  = bzero;
      par.setBlankKeyword(blank);
      par.setBscaleKeyword(bscale);
      par.setBzeroKeyword(bzero);
      par.setBlankPixVal( blank*bscale + bzero );
    }
  
    // Close the FITS file.
    status = 0;
    fits_close_file(fptr, &status);
    if (status){
      duchampWarning("readBLANKinfo","Error closing file: ");
      fits_report_error(stderr, status);
    }
  
    delete [] comment;

  }

  return returnStatus;

}

//////////////////////////////////////////////////

int FitsHeader::readBeamInfo(string fname, Param &par)
{
  /**
   *  FitsHeader::readBeamInfo(string fname, Param &par)
   *   Reading in the beam parameters from the header.
   *   Use these, plus the basic WCS parameters to calculate the size of
   *    the beam in pixels. Copy the beam size into the parameter set.
   *   If information not present in FITS header, use the parameter
   *    set to define the beam size.
   */
  char *comment = new char[80];
  string keyword[4]={"BMAJ","BMIN","CDELT1","CDELT2"};
  float bmaj,bmin,cdelt1,cdelt2;
  int status[6];
  fitsfile *fptr;         

  for(int i=0;i<6;i++) status[i] = 0;

  // Open the FITS file
  if( fits_open_file(&fptr,fname.c_str(),READONLY,&status[0]) ){
    fits_report_error(stderr, status[0]);
    return FAILURE;
  }

  // Read the Keywords -- first look for BMAJ. If it is present, read the
  //   others, and calculate the beam size.
  // If it is not, give warning and set beam size to nominal value.
  fits_read_key(fptr, TFLOAT, (char *)keyword[0].c_str(), &bmaj, 
		comment, &status[1]);
  fits_read_key(fptr, TFLOAT, (char *)keyword[1].c_str(), &bmin, 
		comment, &status[2]);
  fits_read_key(fptr, TFLOAT, (char *)keyword[2].c_str(), &cdelt1, 
		comment, &status[3]);
  fits_read_key(fptr, TFLOAT, (char *)keyword[3].c_str(), &cdelt2, 
		comment, &status[4]);

  if(status[1]||status[2]||status[3]||status[4]){ // error
    stringstream errmsg;
    errmsg << "Header keywords not present: ";
    for(int i=0;i<4;i++) if(status[i+1]) errmsg<<keyword[i]<<" ";
    errmsg << "\nUsing parameter beamSize to determine size of beam.\n";
    duchampWarning("readBeamInfo",errmsg.str());
    this->setBeamSize(par.getBeamSize());
    par.setFlagUsingBeam(true);
  }
  else{ // all keywords present
    this->setBeamSize( M_PI * (bmaj/2.) * (bmin/2.) / fabs(cdelt1*cdelt2) );
    this->setBmajKeyword(bmaj);
    this->setBminKeyword(bmin);
    par.setBeamSize(this->beamSize);
  }
   
  // Close the FITS file.
  fits_close_file(fptr, &status[5]);
  if (status[5]){
    duchampWarning("readBeamInfo","Error closing file: ");
    fits_report_error(stderr, status[5]);
  }

  delete [] comment;

  int returnStatus = status[0];
  for(int i=1;i<6;i++) if(status[i]>returnStatus) returnStatus=status[i];

  return returnStatus;
}
