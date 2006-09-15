#include <iostream>
#include <string>
#include <string.h>
#include <wcs.h>
#include <wcshdr.h>
#include <fitshdr.h>
#include <wcsfix.h>
#include <wcsunits.h>
#define WCSLIB_GETWCSTAB // define this so that we don't try and redefine 
                         //  wtbarr (this is a problem when using gcc v.4+
#include <fitsio.h>
#include <math.h>
#include <duchamp.hh>
#include <Cubes/cubes.hh>

int FitsHeader::readHeaderInfo(string fname, Param &par)
{

  this->getBUNIT(fname);
  
  this->getBLANKinfo(fname, par);
  
  this->getBeamInfo(fname);

  this->defineWCS(fname, par);
  
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
  char *comment = new char[80];
  char *unit = new char[80];
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
    std::cerr << "---------->"<<unit<<std::endl;
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
  fitsfile *fptr;         
  char *comment = new char[80];
  int blank;
  float bscale, bzero;
  int returnStatus = 0, status = 0;
  status = 0;
  if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
    fits_report_error(stderr, status);
    return FAILURE;
  }
  if(!fits_read_key(fptr, TINT32BIT, "BLANK", &blank, comment, &returnStatus)){
    fits_read_key(fptr, TFLOAT, "BZERO", &bzero, comment, &status);
    fits_read_key(fptr, TFLOAT, "BSCALE", &bscale, comment, &status);
    this->setBlankKeyword(blank);
    this->setBscaleKeyword(bscale);
    this->setBzeroKeyword(bzero);
  }
  if(par.getFlagBlankPix() && returnStatus){
    duchampWarning("getBLANKinfo","Error reading BLANK keyword: ");
    fits_report_error(stderr, returnStatus);
    std::stringstream errmsg;
    errmsg << "Using default BLANK value (" << par.getBlankPixVal() << ").\n";
    duchampWarning("getBLANKinfo", errmsg.str());
    this->setBlankKeyword(1);
    this->setBscaleKeyword(par.getBlankPixVal());
    this->setBzeroKeyword(0.);
    par.setFlagUsingBlank(true);
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
    duchampWarning("getCube",
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

//////////////////////////////////////////////////

int FitsHeader::defineWCS(string fname, Param &par)
{
  /**
   *  FitsHeader::defineWCS(wcsprm *wcs, char *hdr, Param &par) 
   *   A function that reads the WCS header information from the 
   *    FITS file given by fptr.
   *   It will also sort out the spectral axis, and covert to the correct 
   *    velocity type, or frequency type if need be.
   */

  fitsfile *fptr;
  int bitpix,numAxes;
  int noComments = 1; //so that fits_hdr2str will ignore COMMENT, HISTORY etc
  int nExc = 0,nkeys;
  char *hdr;
  int status = 0;
  if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
    fits_report_error(stderr, status);
    return FAILURE;
  }
  status = 0;
  if(fits_get_img_dim(fptr, &numAxes, &status)){
    fits_report_error(stderr, status);
    return FAILURE;
  }
  long *dimAxes = new long[numAxes];
  for(int i=0;i<numAxes;i++) dimAxes[i]=1;
  status = 0;
  if(fits_get_img_size(fptr, numAxes, dimAxes, &status)){
    fits_report_error(stderr, status);
    return FAILURE;
  }
  status = 0;
  fits_hdr2str(fptr, noComments, NULL, nExc, &hdr, &nkeys, &status);
  if( status ){
    duchampWarning("defineWCS","Error whilst reading FITS header to string: ");
    fits_report_error(stderr, status);
    return FAILURE;
  }
  status = 0;
  fits_close_file(fptr, &status);
  if (status){
    duchampWarning("getCube","Error closing file: ");
    fits_report_error(stderr, status);
  }
  
  int relax=1, ctrl=2, nwcs, nreject, flag;
  wcsprm *wcs = new wcsprm;
  if(flag = wcsini(true,numAxes,wcs)){
    std::stringstream errmsg;
    errmsg << "wcsini failed! Code=" << flag
	   << ": " << wcs_errmsg[flag] << std::endl;
    duchampError("defineWCS",errmsg.str());
    return FAILURE;
  }
  wcs->flag=-1;
  if(flag = wcspih(hdr, nkeys, relax, ctrl, &nreject, &nwcs, &wcs)) {
    std::stringstream errmsg;
    errmsg << "wcspih failed! Code="<<flag<<": "<<wcs_errmsg[flag]<<std::endl;
    duchampWarning("defineWCS",errmsg.str());
  }
  else{  
    int stat[6];
    int axes[3]={dimAxes[wcs->lng],dimAxes[wcs->lat],dimAxes[wcs->spec]};
    if(flag=wcsfix(1,axes,wcs,stat)) {
      std::stringstream errmsg;
      errmsg << "wcsfix failed:\n";
      for(int i=0; i<NWCSFIX; i++)
	if (stat[i] > 0) 
	  errmsg <<" flag="<<flag<<": "<< wcsfix_errmsg[stat[i]]<<std::endl;
      duchampWarning("defineWCS", errmsg.str() );
    }
    if(flag=wcsset(wcs)){
      std::stringstream errmsg;
      errmsg<<"wcsset failed! Code="<<flag <<": "<<wcs_errmsg[flag]<<std::endl;
      duchampWarning("defineWCS",errmsg.str());
    }

    if(wcs->naxis>2){

      string desiredType,specType = wcs->ctype[2];
      int index = wcs->spec;
      if(wcs->restfrq != 0){
	// Set the spectral axis to a standard specification: VELO-F2V
	desiredType = duchampVelocityType;
	if(wcs->restwav == 0) wcs->restwav = 299792458.0 / wcs->restfrq;
      }
      else{
	// No rest frequency defined, so put spectral dimension in frequency. 
	// Set the spectral axis to a standard specification: FREQ
	duchampWarning("defineWCS",
       "No rest frequency defined. Using frequency units in spectral axis.\n");
	desiredType = duchampFrequencyType;
 	par.setSpectralUnits("MHz");
	if(strcmp(wcs->cunit[2],"")==0){
	  duchampWarning("defineWCS",
	  "No frequency unit given. Assuming frequency axis is in Hz.\n");
	  strcpy(wcs->cunit[2],"Hz");
	}
      }
    
      if(strncmp(specType.c_str(),desiredType.c_str(),4)!=0){
	index = -1;
	if( flag = wcssptr(wcs, &index, (char *)desiredType.c_str())){
	  std::stringstream errmsg;
	  errmsg<<"wcssptr failed! Code="<<flag <<": "
		<<wcs_errmsg[flag]<<std::endl;
	  duchampWarning("defineWCS",errmsg.str());
	}	
      }
    
    } // end of if(numAxes>2)
    
    this->setWCS(wcs);
    this->setNWCS(nwcs);

    wcsfree(wcs);

  }

  this->fixUnits(par);

  return SUCCESS;

}
