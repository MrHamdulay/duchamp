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

string imageType[4] = {"point", "spectrum", "image", "cube"};

int Cube::getCube(string fname)
{
  /**
   * Cube::getCube(string )
   *  Read in a cube from the file fname (assumed to be in FITS format).
   *  Function reads in the following:
   *      - axis dimensions
   *      - pixel array
   *      - WCS information (in form of WCSLIB wcsprm structure)
   *      - Header keywords: BUNIT (brightness unit), 
   *                         BLANK, BZERO, BSCALE (for blank pixel value)
   *                         BMAJ, BMIN, CDELT1, CDELT2 (for beam size)
   */


  short int maxdim=3;
  long *dimAxes = new long[maxdim];
  for(int i=0;i<maxdim;i++) dimAxes[i]=1;
  long nelements;
  int bitpix,numAxes;
  int status = 0,  nkeys;  /* MUST initialize status */
  fitsfile *fptr;         

  if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
    fits_report_error(stderr, status);
    return FAILURE;
  }

  status = 0;
  fits_get_img_param(fptr, maxdim, &bitpix, &numAxes, dimAxes, &status);
  if(status){
    fits_report_error(stderr, status);
    return FAILURE;
  }

  long *fpixel = new long[numAxes];
  for(int i=0;i<numAxes;i++) fpixel[i]=1;

  if(numAxes<=3)  std::cout << "Dimensions of " << imageType[numAxes];
  else std::cout << "Dimensions of " << imageType[3];
  std::cout << ": " << dimAxes[0];
  if(numAxes>1) std::cout << "x" << dimAxes[1];
  if(numAxes>2) std::cout << "x" << dimAxes[2];
  std::cout << std::endl;

  int npix = dimAxes[0]*dimAxes[1]*dimAxes[2];
  float *array = new float[npix];
  int anynul;
  char *nullarray = new char[npix];
  for(int i=0;i<npix;i++) nullarray[i] = 0;

  //-------------------------------------------------------------
  // Reading in the pixel array.
  //  The location of any blank pixels (as determined by the header keywords) 
  //   is stored in nullarray (set to 1). Also anynul will be set to 1 to 
  //   indicate the presence of blank pixels. 
  //   If anynul==1 then all pixels are non-blank.

  status = 0;
  fits_read_pixnull(fptr, TFLOAT, fpixel, npix, array, 
		    nullarray, &anynul, &status);
//   fits_read_pix(fptr, TFLOAT, fpixel, npix, NULL, array, &anynul, &status);
  if(status){
    duchampError("getCube","There was an error reading in the data array:");
    fits_report_error(stderr, status);
    return FAILURE;
  }

  if(anynul==0){    
    // no blank pixels, so don't bother with any trimming or checking...
    if(this->par.getFlagBlankPix()) {  
      // if user requested fixing, inform them of change.
      duchampWarning("getCube",
		     "No blank pixels, so setting flagBlankPix to false.\n");
    }
    this->par.setFlagBlankPix(false); 
  }

  //-------------------------------------------------------------
  // Read the header in preparation for WCS and BUNIT/BLANK/etc extraction.
  // And define a FitsHeader object that will be saved into the Cube.
  FitsHeader newHead;

  char *hdr = new char;
  int noComments = 1; //so that fits_hdr2str will ignore COMMENT, HISTORY etc
  int nExc = 0;
  status = 0;
  fits_hdr2str(fptr, noComments, NULL, nExc, &hdr, &nkeys, &status);
  if( status ){
    duchampWarning("getCube","Error reading in header to string: ");
    fits_report_error(stderr, status);
  }

  //-------------------------------------------------------------
  // Read the BUNIT and CUNIT3 header keywords, to store the units
  //  of brightness (flux) and the spectral dimension.
  char *comment = new char[80];
  char *unit = new char[80];
  status = 0;
  fits_read_key(fptr, TSTRING, "BUNIT", unit, comment, &status);
  if (status){
    duchampWarning("getCube","Error reading BUNIT keyword: ");
    fits_report_error(stderr, status);
  }
  else{
    wcsutrn(0,unit);
    newHead.setFluxUnits(unit);
  }


  //-------------------------------------------------------------
  // Reading in the Blank pixel value keywords.
  //  If the BLANK keyword is in the header, use that and store the relevant 
  //   values.
  //  If not, use the default value (either the default from param.cc or 
  //   from the param file) and assume simple values for the keywords 
  //        --> the scale keyword is the same as the blank value, 
  //            the blank keyword (which is an int) is 1 and 
  //            the bzero (offset) is 0.
  int blank;
  float bscale, bzero;
  status = 0;
  if( !fits_read_key(fptr, TINT32BIT, "BLANK", &blank, comment, &status) ){
    fits_read_key(fptr, TFLOAT, "BZERO", &bzero, comment, &status);
    fits_read_key(fptr, TFLOAT, "BSCALE", &bscale, comment, &status);
    newHead.setBlankKeyword(blank);
    newHead.setBscaleKeyword(bscale);
    newHead.setBzeroKeyword(bzero);
  }
  if(this->par.getFlagBlankPix() && status){
    duchampWarning("getCube","Error reading BLANK keyword: ");
    fits_report_error(stderr, status);
    std::stringstream errmsg;
    errmsg << "Using default BLANK value (" << this->par.getBlankPixVal() << ").\n";
    duchampWarning("getCube", errmsg.str());
    for(int i=0;i<npix;i++) if(isnan(array[i])) array[i] = this->par.getBlankPixVal();
    newHead.setBlankKeyword(1);
    newHead.setBscaleKeyword(this->par.getBlankPixVal());
    newHead.setBzeroKeyword(0.);
    this->par.setFlagUsingBlank(true);
  }
  

  //-------------------------------------------------------------
  // Reading in the beam parameters from the header.
  // Use these, plus the basic WCS parameters to calculate the size of
  //  the beam in pixels.
  float bmaj,bmin,cdelt1,cdelt2;
  status = 0;
  if( !fits_read_key(fptr, TFLOAT, "BMAJ", &bmaj, comment, &status) ){
    fits_read_key(fptr, TFLOAT, "BMIN", &bmin, comment, &status);
    fits_read_key(fptr, TFLOAT, "CDELT1", &cdelt1, comment, &status);
    fits_read_key(fptr, TFLOAT, "CDELT2", &cdelt2, comment, &status);
    newHead.setBeamSize( M_PI * (bmaj/2.) * (bmin/2.) / fabs(cdelt1*cdelt2) );
    newHead.setBmajKeyword(bmaj);
    newHead.setBminKeyword(bmin);
  }

  delete [] comment;

  if (status){
    duchampWarning("getCube",
		   "No beam information in header. Setting size to nominal 10 pixels.\n");
    newHead.setBeamSize(10.);
  }
  
  status = 0;
  fits_close_file(fptr, &status);
  if (status){
    duchampWarning("getCube","Error closing file: ");
    fits_report_error(stderr, status);
  }

  this->initialiseCube(dimAxes);
  this->saveArray(array,npix);
  this->par.copyHeaderInfo(newHead);
  //-------------------------------------------------------------
  // Once the array is saved, change the value of the blank pixels from
  // 0 (as they are set by fits_read_pixnull) to the correct blank value
  // as determined by the above code.
  for(int i=0; i<npix;i++){
    if(nullarray[i]==1) this->array[i] = blank*bscale + bzero;  
  }

  //-------------------------------------------------------------
  // Now convert the FITS header to a WCS structure, using WCSLIB functions.
  int relax=1, ctrl=2, nwcs, nreject, flag;
  wcsprm *wcs = new wcsprm;
  if(flag = wcsini(true,numAxes,wcs)){
    std::stringstream errmsg;
    errmsg << "wcsini failed! Code="<<flag<<": "<<wcs_errmsg[flag]<<std::endl;
    duchampError("getCube",errmsg.str());
    return FAILURE;
  }
  wcs->flag=-1;
  if(flag = wcspih(hdr, nkeys, relax, ctrl, &nreject, &nwcs, &wcs)) {
    std::stringstream errmsg;
    errmsg << "wcspih failed! Code="<<flag<<": "<<wcs_errmsg[flag]<<std::endl;
    duchampWarning("getCube",errmsg.str());
  }
  else{  
    int stat[6],axes[3]={dimAxes[0],dimAxes[1],dimAxes[2]};
    if(flag=wcsfix(1,axes,wcs,stat)) {
      std::stringstream errmsg;
      errmsg << "wcsfix failed:\n";
      for(int i=0; i<NWCSFIX; i++)
	if (stat[i] > 0) 
	  errmsg <<" flag="<<flag<<": "<< wcsfix_errmsg[stat[i]]<<std::endl;
      duchampWarning("getCube", errmsg.str() );
    }
    if(flag=wcsset(wcs)){
      std::stringstream errmsg;
      errmsg<<"wcsset failed! Code="<<flag <<": "<<wcs_errmsg[flag]<<std::endl;
      duchampWarning("getCube",errmsg.str());
    }

    if(numAxes>2){

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
	duchampWarning("getCube",
		       "No rest frequency defined. Using frequency units in spectral axis.\n");
	desiredType = duchampFrequencyType;
	this->par.setSpectralUnits("MHz");
	if(strcmp(wcs->cunit[2],"")==0){
	  duchampWarning("getCube",
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
	  duchampWarning("getCube",errmsg.str());
	}	
      }
    
    } // end of if(numAxes>2)
    
    newHead.setWCS(wcs);
    newHead.setNWCS(nwcs);

    wcsfree(wcs);
  }

  newHead.fixUnits(this->par);  
  this->setHead(newHead);

  if(!newHead.isWCS()) 
    duchampWarning("getCube","WCS is not good enough to be used.\n");

  delete hdr;
  delete [] array;
  delete [] fpixel;
  delete [] dimAxes;

  return SUCCESS;

}

