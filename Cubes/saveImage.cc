#include <iostream>
#include <sstream>
#include <string>
#include <wcs.h>
#include <wcshdr.h>
#include <fitsio.h>
#include <duchamp.hh>
#include <Cubes/cubes.hh>

void write_header_info(fitsfile *fptr, Param &par, string nature);
void Cube::saveReconstructedCube()
{
  
  int newbitpix = FLOAT_IMG;

  float *resid = new float[this->numPixels];
  for(int i=0;i<this->numPixels;i++) resid[i] = this->array[i] - this->recon[i];
  float blankval = this->par.getBlankPixVal();

  long *fpixel = new long[this->numDim];
  for(int i=0;i<this->numDim;i++) fpixel[i]=1;
  int status = 0;  /* MUST initialize status */
  fitsfile *fptrOld, *fptrNew;         
  fits_open_file(&fptrOld,this->par.getImageFile().c_str(),READONLY,&status);
  if (status) fits_report_error(stderr, status);
  
  if(this->par.getFlagOutputRecon()){
    string fileout = "!" + this->par.getImageFile(); // ! so that it writes over an existing file.
    fileout = fileout.substr(0,fileout.size()-5); // remove the ".fits" on the end.
    std::stringstream ss;
    ss << ".RECON"<<this->par.getAtrousCut()<<".fits";
    fileout += ss.str();

    status = 0;
    fits_create_file(&fptrNew,fileout.c_str(),&status);
    if (status) fits_report_error(stderr, status);
    status = 0;
    fits_movabs_hdu(fptrOld, 1, NULL, &status);
    if (status) fits_report_error(stderr, status);
    status = 0;
    fits_copy_header(fptrOld, fptrNew, &status);
    if (status) fits_report_error(stderr, status);
    char *comment = new char[80];
    long dud;
    status = 0;
    fits_update_key(fptrNew, TINT, "BITPIX", &newbitpix, "number of bits per data pixel", &status);
    if (status) fits_report_error(stderr, status);
    status = 0;
    float bscale=1., bzero=0.;
    fits_update_key(fptrNew, TFLOAT, "BSCALE", &bscale, "PHYSICAL = PIXEL*BSCALE + BZERO", &status);
    fits_update_key(fptrNew, TFLOAT, "BZERO", &bzero, "", &status);
    fits_set_bscale(fptrNew, 1., 0., &status);
    if (status) fits_report_error(stderr, status);
    // Need to correct the dimensions, if we have subsectioned the image...    
    if(this->par.getFlagSubsection()){
      fits_read_key(fptrOld, TLONG, "NAXIS1", &dud, comment, &status);
      fits_update_key(fptrNew, TLONG, "NAXIS1", &(this->axisDim[0]), comment, &status);
      fits_read_key(fptrOld, TLONG, "NAXIS2", &dud, comment, &status);
      fits_update_key(fptrNew, TLONG, "NAXIS2", &(this->axisDim[1]), comment, &status);
      fits_read_key(fptrOld, TLONG, "NAXIS3", &dud, comment, &status);
      fits_update_key(fptrNew, TLONG, "NAXIS3", &(this->axisDim[2]), comment, &status);
    }

    write_header_info(fptrNew, this->par, "recon");

    if(this->par.getFlagBlankPix())
      fits_write_pixnull(fptrNew, TFLOAT, fpixel, this->numPixels, this->recon, &blankval, &status);
    else  fits_write_pix(fptrNew, TFLOAT, fpixel, this->numPixels, this->recon, &status);

    status = 0;
    fits_close_file(fptrNew, &status);
    if (status) fits_report_error(stderr, status);
  }

  if(this->par.getFlagOutputResid()){
    string fileout = "!" + this->par.getImageFile(); // ! so that it writes over an existing file.
    fileout = fileout.substr(0,fileout.size()-5);
    std::stringstream ss;
    ss << ".RESID"<<this->par.getAtrousCut()<<".fits";
    fileout += ss.str();

    status = 0;
    fits_create_file(&fptrNew,fileout.c_str(),&status);
    if (status) fits_report_error(stderr, status);
    status = 0;
    fits_movabs_hdu(fptrOld, 1, NULL, &status);
    if (status) fits_report_error(stderr, status);
    status = 0;
    fits_copy_header(fptrOld, fptrNew, &status);
    if (status) fits_report_error(stderr, status);

    status = 0;
    fits_update_key(fptrNew, TINT, "BITPIX", &newbitpix, "number of bits per data pixel", &status);
    if (status) fits_report_error(stderr, status);
    status = 0;
    float bscale=1., bzero=0.;
    fits_update_key(fptrNew, TFLOAT, "BSCALE", &bscale, "PHYSICAL = PIXEL*BSCALE + BZERO", &status);
    fits_update_key(fptrNew, TFLOAT, "BZERO", &bzero, "", &status);
    fits_set_bscale(fptrNew, 1., 0., &status);
    if (status) fits_report_error(stderr, status);

    // Need to correct the dimensions, if we have subsectioned the image...
    char *comment = new char[80];
    long dud;
    if(this->pars().getFlagSubsection()){
      fits_read_key(fptrOld, TLONG, "NAXIS1", &dud, comment, &status);
      fits_update_key(fptrNew, TLONG, "NAXIS1", &(this->axisDim[0]), comment, &status);
      fits_read_key(fptrOld, TLONG, "NAXIS2", &dud, comment, &status);
      fits_update_key(fptrNew, TLONG, "NAXIS2", &(this->axisDim[1]), comment, &status);
      fits_read_key(fptrOld, TLONG, "NAXIS3", &dud, comment, &status);
      fits_update_key(fptrNew, TLONG, "NAXIS3", &(this->axisDim[2]), comment, &status);
    }

    write_header_info(fptrNew, this->par, "resid");

    if(this->par.getFlagBlankPix())
      fits_write_pixnull(fptrNew, TFLOAT, fpixel, this->numPixels, resid, &blankval, &status);
    else  fits_write_pix(fptrNew, TFLOAT, fpixel, this->numPixels, resid, &status);

    fits_close_file(fptrNew, &status);
  }

  delete [] recon;
  delete [] resid;
  delete [] fpixel;

}


void write_header_info(fitsfile *fptr, Param &par, string nature)
{
  int status = 0;
  char *comment, *keyword;
  string explanation = "",ReconResid="";

  fits_write_history(fptr, (char *)header_history1.c_str(), &status);
				   
  fits_write_history(fptr, (char *)header_history2.c_str(), &status);

  fits_write_comment(fptr, (char *)header_comment.c_str(), &status);

  float valf = par.getAtrousCut();
  fits_write_key(fptr, TFLOAT, (char *)keyword_snrRecon.c_str(), &valf, 
		 (char *)comment_snrRecon.c_str(), &status);

  int vali = par.getMinScale();
  fits_write_key(fptr, TINT, (char *)keyword_scaleMin.c_str(), &vali, 
		 (char *)comment_scaleMin.c_str(), &status);

  vali = par.getFilterCode();
  fits_write_key(fptr, TINT, (char *)keyword_filterCode.c_str(), &vali, 
		 (char *)comment_filterCode.c_str(), &status);

  if(nature == "recon"){
    explanation = "Duchamp: This is the RECONSTRUCTED cube";
    ReconResid = "RECON";
  }
  else if(nature == "resid"){
    explanation = "Duchamp: This is the RESIDUAL cube";
    ReconResid = "RESID";
  }
  else std::cerr << "WARNING : write_header_info : explanation not present!\n";
  fits_write_comment(fptr, (char *)explanation.c_str(), &status);
  fits_write_key(fptr, TSTRING, (char *)keyword_ReconResid.c_str(), 
		 (char *)ReconResid.c_str(), (char *)comment_ReconResid.c_str(), &status);

}
