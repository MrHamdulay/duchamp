#include <iostream>
#include <sstream>
#include <string>
#include <fitsio.h>
#include <wcs.h>
#include <wcshdr.h>
#include <Cubes/cubes.hh>

void Cube::saveReconstructedCube()
{
  
  float *resid = new float[this->numPixels];
  for(int i=0;i<this->numPixels;i++) resid[i] = this->array[i] - this->recon[i];
  float blankval = this->par.getBlankPixVal();

  short int maxdim=3;
  long *fpixel = new long[maxdim];
  for(int i=0;i<maxdim;i++) fpixel[i]=1;
  long *dimAxes = new long[maxdim];
  for(int i=0;i<maxdim;i++) dimAxes[i]=1;
  long nelements;
  int bitpix,numAxes,anynul;
  int status = 0,  nkeys, ct=1;  /* MUST initialize status */
  fitsfile *fptrOld, *fptrNew;         
  fits_open_file(&fptrOld,this->par.getImageFile().c_str(),READONLY,&status);
  if (status) fits_report_error(stderr, status);
  
  if(this->par.getFlagOutputRecon()){
    string fileout1 = "!" + this->par.getImageFile(); // ! so that it writes over an existing file.
    fileout1 = fileout1.substr(0,fileout1.size()-5); // remove the ".fits" on the end.
    std::stringstream ss1;
    ss1 << ".RECON"<<this->par.getAtrousCut()<<".fits";
    fileout1 += ss1.str();
    string syscall = "rm -f " + fileout1;
    system(syscall.c_str());

    status = 0;
    fits_create_file(&fptrNew,fileout1.c_str(),&status);
    if (status) fits_report_error(stderr, status);
    status = 0;
    fits_movabs_hdu(fptrOld, 1, NULL, &status);
    if (status) fits_report_error(stderr, status);
    status = 0;
    fits_copy_hdu(fptrOld, fptrNew, 0, &status);
    if (status) fits_report_error(stderr, status);
    // Need to correct the dimensions, if we have subsectioned the image...
    if(this->pars().getFlagSubsection()){
      char *comment = new char[80];
      long dud;
      fits_read_key(fptrOld, TLONG, "NAXIS1", &dud, comment, &status);
      fits_update_key(fptrNew, TLONG, "NAXIS1", &(this->axisDim[0]), comment, &status);
      fits_read_key(fptrOld, TLONG, "NAXIS2", &dud, comment, &status);
      fits_update_key(fptrNew, TLONG, "NAXIS2", &(this->axisDim[2]), comment, &status);
      fits_read_key(fptrOld, TLONG, "NAXIS3", &dud, comment, &status);
      fits_update_key(fptrNew, TLONG, "NAXIS3", &(this->axisDim[1]), comment, &status);
    }

    //////////////////////////////
    ////// NEED TO ADD NEW HEADER KEYS 
    ////// INDICATING WHAT THE FILE IS...
    //////////////////////////////

    if(this->par.getFlagBlankPix())
      fits_write_pixnull(fptrNew, TFLOAT, fpixel, this->numPixels, recon, &blankval, &status);
    else  fits_write_pix(fptrNew, TFLOAT, fpixel, this->numPixels, recon, &status);

    fits_close_file(fptrNew, &status);
  }

  if(this->par.getFlagOutputResid()){
    string fileout2 = "!" + this->par.getImageFile(); // ! so that it writes over an existing file.
    fileout2 = fileout2.substr(0,fileout2.size()-5);
    std::stringstream ss2;
    ss2 << ".RESID"<<this->par.getAtrousCut()<<".fits";
    fileout2 += ss2.str();
    string syscall = "rm -f " + fileout2;
    system(syscall.c_str());  

    status = 0;
    fits_create_file(&fptrNew,fileout2.c_str(),&status);
    if (status) fits_report_error(stderr, status);
    status = 0;
    fits_movabs_hdu(fptrOld, 1, NULL, &status);
    if (status) fits_report_error(stderr, status);
    status = 0;
    fits_copy_hdu(fptrOld, fptrNew, 0, &status);
    if (status) fits_report_error(stderr, status);

    // Need to correct the dimensions, if we have subsectioned the image...
    if(this->pars().getFlagSubsection()){
      char *comment = new char[80];
      long dud;
      fits_read_key(fptrOld, TLONG, "NAXIS1", &dud, comment, &status);
      fits_update_key(fptrNew, TLONG, "NAXIS1", &(this->axisDim[0]), comment, &status);
      fits_read_key(fptrOld, TLONG, "NAXIS2", &dud, comment, &status);
      fits_update_key(fptrNew, TLONG, "NAXIS2", &(this->axisDim[2]), comment, &status);
      fits_read_key(fptrOld, TLONG, "NAXIS3", &dud, comment, &status);
      fits_update_key(fptrNew, TLONG, "NAXIS3", &(this->axisDim[1]), comment, &status);
   }
    //////////////////////////////
    ////// NEED TO ADD NEW HEADER KEYS 
    ////// INDICATING WHAT THE FILE IS...
    //////////////////////////////

    if(this->par.getFlagBlankPix())
      fits_write_pixnull(fptrNew, TFLOAT, fpixel, this->numPixels, resid, &blankval, &status);
    else  fits_write_pix(fptrNew, TFLOAT, fpixel, this->numPixels, resid, &status);

    fits_close_file(fptrNew, &status);
  }

  delete [] recon;
  delete [] resid;
  delete [] fpixel;
  delete [] dimAxes;

}
