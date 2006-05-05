#include <iostream>
#include <string>
#include <wcs.h>
#include <wcshdr.h>
#include <fitshdr.h>
#include <wcsfix.h>
#include <fitsio.h>
#include <Cubes/cubes.hh>

#ifdef NAN 
float nanValue = NAN;
#endif

using std::endl;

enum OUTCOME {SUCCESS, FAILURE};

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
   *                         BLANK, BZERO, BSCALE (to determine blank pixel value)
   *                         BMAJ, BMIN, CDELT1, CDELT2 (to determine beam size)
   */


  short int maxdim=3;
  long *fpixel = new long[maxdim];
  for(int i=0;i<maxdim;i++) fpixel[i]=1;
  long *dimAxes = new long[maxdim];
  for(int i=0;i<maxdim;i++) dimAxes[i]=1;
  long nelements;
  int bitpix,numAxes,anynul;
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

  std::cout << "Cube dimensions: " << dimAxes[0];
  if(numAxes>1) std::cout << "x" << dimAxes[1];
  if(numAxes>2) std::cout << "x" << dimAxes[2];
  std::cout << endl;

  int npix = dimAxes[0]*dimAxes[1]*dimAxes[2];
  float *array = new float[npix];
  char *nullarray = new char[npix];
  for(int i=0;i<npix;i++) nullarray[i] = 0;
  status = 0;
  //-------------------------------------------------------------
  // Reading in the pixel array.
  //  The locationg of any blank pixels (as determined by the header keywords) is stored
  //  in nullarray (set to 1). Also anynul will be set to 1 to indicate the presence of
  //  blank pixels. If anynul==1 then all pixels are non-blank.

  fits_read_pixnull(fptr, TFLOAT, fpixel, npix, array, nullarray, &anynul, &status);
//   fits_read_pix(fptr, TFLOAT, fpixel, npix, NULL, array, &anynul, &status);
  if(status){
    fits_report_error(stderr, status);
    return FAILURE;
  }

//   std::cerr << "anynul = " << anynul << std::endl;
  if(anynul==0){    // no blank pixels, so don't bother with any trimming or checking...
    if(this->par.getFlagBlankPix())  // if user requested fixing, inform them of change.
      std::cerr << "No blank pixels, so setting flagBlankPix to false...\n";
    this->par.setFlagBlankPix(false); 
  }

  //-------------------------------------------------------------
  // Read the header in preparation for WCS and BUNIT/BLANK/etc extraction.
  char *hdr = new char;
  int noComments = 1; //so that fits_hdr2str will not write out COMMENT, HISTORY etc
  int nExc = 0;
  status = 0;
  fits_hdr2str(fptr, noComments, NULL, nExc, &hdr, &nkeys, &status);
  if( status ){
    std::cerr << "Error reading in header to string: ";
    fits_report_error(stderr, status);
  }

  //-------------------------------------------------------------
  // Read the BUNIT header keyword, to store the brightness unit.
  int blank;
  float bscale, bzero;
  char *comment = new char[80];
  char *bunit = new char[80];
  status = 0;
  fits_read_key(fptr, TSTRING, "BUNIT", bunit, comment, &status);
  if (status){
    std::cerr << "Error reading BUNIT keyword: ";
    fits_report_error(stderr, status);
  }
  else this->setBUnit(bunit);

  //-------------------------------------------------------------
  // Reading in the Blank pixel value keywords.
  //  If the BLANK keyword is in the header, use that and store the relevant values.
  //  If not, use the default value (either the default from param.cc or from the param file)
  //    and assume simple values for the keywords --> the scale keyword is the same as the 
  //    blank value, the blank keyword (which is an int) is 1 and the bzero (offset) is 0.
  status = 0;
  if(this->par.getFlagBlankPix()){
    if( !fits_read_key(fptr, TINT32BIT, "BLANK", &blank, comment, &status) ){
      fits_read_key(fptr, TFLOAT, "BZERO", &bzero, comment, &status);
      fits_read_key(fptr, TFLOAT, "BSCALE", &bscale, comment, &status);
      this->par.setBlankPixVal(blank*bscale+bzero);
      this->par.setBlankKeyword(blank);
      this->par.setBscaleKeyword(bscale);
      this->par.setBzeroKeyword(bzero);
    }
    if (status){
      std::cerr << "Error reading BLANK keyword: ";
      fits_report_error(stderr, status);
      std::cerr << "Using default BLANK (physical) value (" << this->par.getBlankPixVal() << ")." << endl;
      for(int i=0;i<npix;i++) if(isnan(array[i])) array[i] = this->par.getBlankPixVal();
      this->par.setBlankKeyword(1);
      this->par.setBscaleKeyword(this->par.getBlankPixVal());
      this->par.setBzeroKeyword(0.);
    }
  }

  //-------------------------------------------------------------
  // Reading in the beam parameters from the header.
  // Use these, plus the basic WCS parameters to calculate the size of the beam in pixels.
  float bmaj,bmin,cdelt1,cdelt2;
  status = 0;
  if( !fits_read_key(fptr, TFLOAT, "BMAJ", &bmaj, comment, &status) ){
    fits_read_key(fptr, TFLOAT, "BMIN", &bmin, comment, &status);
    fits_read_key(fptr, TFLOAT, "CDELT1", &cdelt1, comment, &status);
    fits_read_key(fptr, TFLOAT, "CDELT2", &cdelt2, comment, &status);
    this->par.setBeamSize( M_PI * (bmaj/2.) * (bmin/2.) / fabsf(cdelt1*cdelt2) );
  }
  if (status){
    std::cerr << "No beam information in header. Setting size to nominal 10 pixels." << endl;
    this->par.setBeamSize(10.);
  }

  status = 0;
  fits_close_file(fptr, &status);
  if (status){
    std::cerr << "Error closing file: ";
    fits_report_error(stderr, status);
  }

  this->initialiseCube(dimAxes);
  this->saveArray(array,npix);
  //-------------------------------------------------------------
  // Once the array is saved, change the value of the blank pixels from
  // 0 (as they are set by fits_read_pixnull) to the correct blank value
  // as determined by the above code.
  for(int i=0; i<npix;i++){
    //     this->blankarray[i] = bool(nullarray[i]);
    if(nullarray[i]==1) this->array[i] = blank*bscale + bzero;  
  }

  //-------------------------------------------------------------
  // Now convert the FITS header to a WCS structure, using the WCSLIB functions.
  int relax=1, ctrl=2, nwcs, nreject;
  wcsprm *wcs = new wcsprm;
  wcsini(true,numAxes,wcs);
  wcs->flag=-1;
  int flag;
  if(flag = wcspih(hdr, nkeys, relax, ctrl, &nreject, &nwcs, &wcs)) {
    std::cerr<<"getImage.cc:: WCSPIH failed! Code="<<flag<<": "<<wcs_errmsg[flag]<<endl;
//     return FAILURE;
  }
  else{  
    int stat[6],axes[3]={dimAxes[0],dimAxes[1],dimAxes[2]};
    if(flag=wcsfix(1,axes,wcs,stat)) {
      std::cerr<<"getImage.cc:: WCSFIX failed!"<<endl;
      for(int i=0; i<NWCSFIX; i++)
	if (stat[i] > 0) std::cerr<<"wcsfix ERROR "<<flag<<": "<< wcsfix_errmsg[stat[i]] <<endl;
      //     return FAILURE;
    }
    if(flag=wcsset(wcs)){
      std::cerr<<"getImage.cc:: WCSSET failed! Code="<<flag <<": "<<wcs_errmsg[flag]<<endl;
      //     return FAILURE;
    }

    this->setWCS(wcs);
    this->setNWCS(nwcs);
  }
  wcsfree(wcs);

  delete hdr;
  delete [] array;
  delete [] fpixel;
  delete [] dimAxes;

  return SUCCESS;

}

Cube getCube(string fname, Param par)
{
  short int maxdim=3;
  long *fpixel = new long[maxdim];
  for(int i=0;i<maxdim;i++) fpixel[i]=1;
  long *dimAxes = new long[maxdim];
  for(int i=0;i<maxdim;i++) dimAxes[i]=1;
  long nelements;
  int bitpix,numAxes,anynul;
  int status = 0,  nkeys;  /* MUST initialize status */
  fitsfile *fptr;         

  if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
    fits_report_error(stderr, status);
    return 1;
  }

  fits_get_img_param(fptr, maxdim, &bitpix, &numAxes, dimAxes, &status);
  int npix = dimAxes[0]*dimAxes[1]*dimAxes[2];
  float *array = new float[npix];
  fits_read_pix(fptr, TFLOAT, fpixel, npix, NULL, array, &anynul, &status);

  // Read the header
  char *hdr = new char;
  int noComments = 1; //fits_hdr2str will not write out COMMENT, HISTORY etc
  int nExc = 0;
  if( fits_hdr2str(fptr, noComments, NULL, nExc, &hdr, &nkeys, &status) ){
    fits_report_error(stderr, status);
    return 1;
  }

  float blank, bscale, bzero;
  char *comment = new char[80];
  //  if( fits_movabs_hdu(fptr, 1, IMAGE_HDU, &status)) fits_report_error(stderr, status);
  if( !fits_read_key(fptr, TFLOAT, "BLANK", &blank, comment, &status) ){
    fits_read_key(fptr, TFLOAT, "BZERO", &bzero, comment, &status);
    fits_read_key(fptr, TFLOAT, "BSCALE", &bscale, comment, &status);
    par.setBlankPixVal(blank*bscale+bzero);
  }
  if (status) fits_report_error(stderr, status);
  char *bunit = new char[80];
  fits_read_key(fptr, TSTRING, "BUNIT", bunit, comment, &status);
  if (status) fits_report_error(stderr, status);

  fits_close_file(fptr, &status);
  if (status) fits_report_error(stderr, status);

  int relax=1, ctrl=2, nwcs, nreject;
  wcsprm *wcs = new wcsprm;
  wcsini(true,numAxes,wcs);
  wcs->flag=-1;
  int flag;
  if(flag = wcspih(hdr, nkeys, relax, ctrl, &nreject, &nwcs, &wcs)) {
    std::cerr<<"getImage.cc:: WCSPIH failed! Code="<<flag<<": "<<wcs_errmsg[flag]<<endl;
    return 1;
  }
  
  int stat[6],axes[3]={dimAxes[0],dimAxes[1],dimAxes[2]};
  if(flag=wcsfix(1,axes,wcs,stat)) {
    std::cerr<<"getImage.cc:: WCSFIX failed!"<<endl;
    for(int i=0; i<NWCSFIX; i++)
      if (stat[i] > 0) std::cerr<<"wcsfix ERROR "<<flag<<": "<< wcsfix_errmsg[stat[i]] <<endl;
    return 1;
  }
  if(flag=wcsset(wcs)){
    std::cerr<<"getImage.cc:: WCSSET failed! Code="<<flag <<": "<<wcs_errmsg[flag]<<endl;
    return 1;
  }

  Cube *data = new Cube(dimAxes);
  data->saveArray(array,npix);
  data->setWCS(wcs);
  data->setNWCS(nwcs);
  data->setBUnit(bunit);

  wcsfree(wcs);
  delete hdr;
  delete [] array;
  delete [] fpixel;
  delete [] dimAxes;


  return *data;
}

Image getImage(string fname)
{
  int maxdim=2;
  long *fpixel = new long[maxdim];
  for(int i=0;i<maxdim;i++) fpixel[i]=1;
  long *dimAxes = new long[maxdim];
  for(int i=0;i<maxdim;i++) dimAxes[i]=1;
  long nelements;
  int bitpix,numAxes,anynul;
  int status = 0,  nkeys;  /* MUST initialize status */
  fitsfile *fptr;         

  std::cerr << fname.c_str() << endl;

  fits_open_file(&fptr,fname.c_str(),READONLY,&status);
  if (status) fits_report_error(stderr, status);
  fits_get_img_param(fptr, maxdim, &bitpix, &numAxes, dimAxes, &status);
  int npix = dimAxes[0]*dimAxes[1];
  float *cube = new float[npix];
  fits_read_pix(fptr, TFLOAT, fpixel, npix, NULL, cube, &anynul, &status);
  fits_close_file(fptr, &status);
  if (status) fits_report_error(stderr, status);

  Image data(dimAxes);
  data.saveArray(cube,npix);

  delete [] cube;
  delete [] fpixel;
  delete [] dimAxes;

  //  std::cerr << data.getSize()<<"  "<<data.getDimX()<<" "<<data.getDimY()<<endl;

  return data;
}

DataArray getImage(string fname, short int maxdim)
{
  long *fpixel = new long[maxdim];
  for(int i=0;i<maxdim;i++) fpixel[i]=1;
  long *dimAxes = new long[maxdim];
  for(int i=0;i<maxdim;i++) dimAxes[i]=1;
  long nelements;
  int bitpix,numAxes,anynul;
  int status = 0,  nkeys;  /* MUST initialize status */
  fitsfile *fptr;         

  std::cerr << fname.c_str() << endl;

  fits_open_file(&fptr,fname.c_str(),READONLY,&status);
  if (status) fits_report_error(stderr, status);
  fits_get_img_param(fptr, maxdim, &bitpix, &numAxes, dimAxes, &status);
  int npix = dimAxes[0];
  for(int i=1;i<numAxes;i++) npix *= dimAxes[i];
  float *cube = new float[npix];
  fits_read_pix(fptr, TFLOAT, fpixel, npix, NULL, cube, &anynul, &status);
  fits_close_file(fptr, &status);
  if (status) fits_report_error(stderr, status);

  DataArray data(numAxes,dimAxes);
  data.saveArray(cube,npix);

  delete [] cube;
  delete [] fpixel;
  delete [] dimAxes;

  return data;
}

