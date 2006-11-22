#include <iostream>
#include <sstream>
#include <string>
#include <wcs.h>
#include <wcshdr.h>
#define WCSLIB_GETWCSTAB 
// define this so that we don't try and redefine wtbarr 
// (this is a problem when using cfitsio v.3 and g++ v.4)

#include <fitsio.h>
#include <duchamp.hh>
#include <Cubes/cubes.hh>

void writeReconHeaderInfo(fitsfile *fptr, Param &par, string nature);
void writeSmoothHeaderInfo(fitsfile *fptr, Param &par);

void Cube::saveSmoothedCube()
{
  /**
   *  Cube::saveSmoothedCube()
   *
   *   A function to save the Hanning-smoothed arrays to a FITS file.
   *   Additional header keywords are written as well, indicating the 
   *    width of the Hanning filter.
   *   The file is always written -- if the filename (as calculated 
   *    based on the parameters) exists, then it is overwritten.
   */
  
  int newbitpix = FLOAT_IMG;

  float blankval = this->par.getBlankPixVal();

  long *fpixel = new long[this->numDim];
  for(int i=0;i<this->numDim;i++) fpixel[i]=1;
  int status = 0;  /* MUST initialize status */
  fitsfile *fptrOld, *fptrNew;         
  fits_open_file(&fptrOld,this->par.getImageFile().c_str(),READONLY,&status);
  if (status) fits_report_error(stderr, status);
  
  if(this->par.getFlagOutputSmooth()){
    string fileout = "!" + this->par.outputSmoothFile(); 
    // the ! is there so that it writes over an existing file.

    status = 0;
    fits_create_file(&fptrNew,fileout.c_str(),&status);
    if (status)
      fits_report_error(stderr, status);
    else {
      status = 0;
      fits_movabs_hdu(fptrOld, 1, NULL, &status);
      if (status) fits_report_error(stderr, status);
      status = 0;
      fits_copy_header(fptrOld, fptrNew, &status);
      if (status) fits_report_error(stderr, status);
      char *comment = new char[80];
      long dud;
      status = 0;
      fits_update_key(fptrNew, TINT, "BITPIX", &newbitpix, 
		      "number of bits per data pixel", &status);
      if (status) fits_report_error(stderr, status);
      status = 0;
      float bscale=1., bzero=0.;
      fits_update_key(fptrNew, TFLOAT, "BSCALE", &bscale, 
		      "PHYSICAL = PIXEL*BSCALE + BZERO", &status);
      fits_update_key(fptrNew, TFLOAT, "BZERO", &bzero, "", &status);
      fits_set_bscale(fptrNew, 1., 0., &status);
      if (status) fits_report_error(stderr, status);
      // Need to correct the dimensions, if we have subsectioned the image
      if(this->par.getFlagSubsection()){
	fits_read_key(fptrOld, TLONG, "NAXIS1", &dud, comment, &status);
	fits_update_key(fptrNew, TLONG, "NAXIS1", 
			&(this->axisDim[0]), comment, &status);
	fits_read_key(fptrOld, TLONG, "NAXIS2", &dud, comment, &status);
	fits_update_key(fptrNew, TLONG, "NAXIS2", 
			&(this->axisDim[1]), comment, &status);
	fits_read_key(fptrOld, TLONG, "NAXIS3", &dud, comment, &status);
	fits_update_key(fptrNew, TLONG, "NAXIS3", 
			&(this->axisDim[2]), comment, &status);
      }

      writeSmoothHeaderInfo(fptrNew, this->par);

      if(this->par.getFlagBlankPix())
	fits_write_pixnull(fptrNew, TFLOAT, fpixel, this->numPixels, 
			   this->recon, &blankval, &status);
      else fits_write_pix(fptrNew, TFLOAT, fpixel, this->numPixels, 
			  this->recon, &status);

      status = 0;
      fits_close_file(fptrNew, &status);
      if (status) fits_report_error(stderr, status);
    }
  }

}


void Cube::saveReconstructedCube()
{
  /**
   *  Cube::saveReconstructedCube()
   *
   *   A function to save the reconstructed and/or residual arrays.
   *   A number of header keywords are written as well, indicating the 
   *    nature of the reconstruction that has been done.
   *   The file is always written -- if the filename (as calculated 
   *    based on the recon parameters) exists, then it is overwritten.
   */
  
  int newbitpix = FLOAT_IMG;

  float *resid = new float[this->numPixels];
  for(int i=0;i<this->numPixels;i++) 
    resid[i] = this->array[i] - this->recon[i];
  float blankval = this->par.getBlankPixVal();

  long *fpixel = new long[this->numDim];
  for(int i=0;i<this->numDim;i++) fpixel[i]=1;
  int status = 0;  /* MUST initialize status */
  fitsfile *fptrOld, *fptrNew;         
  fits_open_file(&fptrOld,this->par.getImageFile().c_str(),READONLY,&status);
  if (status) fits_report_error(stderr, status);
  
  if(this->par.getFlagOutputRecon()){
    string fileout = "!" + this->par.outputReconFile(); 
    // the ! is there so that it writes over an existing file.

    status = 0;
    fits_create_file(&fptrNew,fileout.c_str(),&status);
    if (status)
      fits_report_error(stderr, status);
    else
      {
	status = 0;
	fits_movabs_hdu(fptrOld, 1, NULL, &status);
	if (status) fits_report_error(stderr, status);
	status = 0;
	fits_copy_header(fptrOld, fptrNew, &status);
	if (status) fits_report_error(stderr, status);
	char *comment = new char[80];
	long dud;
	status = 0;
	fits_update_key(fptrNew, TINT, "BITPIX", &newbitpix, 
			"number of bits per data pixel", &status);
	if (status) fits_report_error(stderr, status);
	status = 0;
	float bscale=1., bzero=0.;
	fits_update_key(fptrNew, TFLOAT, "BSCALE", &bscale, 
			"PHYSICAL = PIXEL*BSCALE + BZERO", &status);
	fits_update_key(fptrNew, TFLOAT, "BZERO", &bzero, "", &status);
	fits_set_bscale(fptrNew, 1., 0., &status);
	if (status) fits_report_error(stderr, status);
	// Need to correct the dimensions, if we have subsectioned the image
	if(this->par.getFlagSubsection()){
	  fits_read_key(fptrOld, TLONG, "NAXIS1", &dud, comment, &status);
	  fits_update_key(fptrNew, TLONG, "NAXIS1", 
			  &(this->axisDim[0]), comment, &status);
	  fits_read_key(fptrOld, TLONG, "NAXIS2", &dud, comment, &status);
	  fits_update_key(fptrNew, TLONG, "NAXIS2", 
			  &(this->axisDim[1]), comment, &status);
	  fits_read_key(fptrOld, TLONG, "NAXIS3", &dud, comment, &status);
	  fits_update_key(fptrNew, TLONG, "NAXIS3", 
			  &(this->axisDim[2]), comment, &status);
	}

	writeReconHeaderInfo(fptrNew, this->par, "recon");

	if(this->par.getFlagBlankPix())
	  fits_write_pixnull(fptrNew, TFLOAT, fpixel, this->numPixels, 
			     this->recon, &blankval, &status);
	else  fits_write_pix(fptrNew, TFLOAT, fpixel, this->numPixels, 
			     this->recon, &status);

	status = 0;
	fits_close_file(fptrNew, &status);
	if (status) fits_report_error(stderr, status);
      }
  }


  if(this->par.getFlagOutputResid()){
    string fileout = "!" + this->par.outputResidFile(); 
    // the ! is there so that it writes over an existing file.
    status = 0;
    fits_create_file(&fptrNew,fileout.c_str(),&status);
    if (status)
      fits_report_error(stderr, status);
    else
      {
	status = 0;
	fits_movabs_hdu(fptrOld, 1, NULL, &status);
	if (status) fits_report_error(stderr, status);
	status = 0;
	fits_copy_header(fptrOld, fptrNew, &status);
	if (status) fits_report_error(stderr, status);

	status = 0;
	fits_update_key(fptrNew, TINT, "BITPIX", &newbitpix, 
			"number of bits per data pixel", &status);
	if (status) fits_report_error(stderr, status);
	status = 0;
	float bscale=1., bzero=0.;
	fits_update_key(fptrNew, TFLOAT, "BSCALE", &bscale, 
			"PHYSICAL = PIXEL*BSCALE + BZERO", &status);
	fits_update_key(fptrNew, TFLOAT, "BZERO", &bzero, "", &status);
	fits_set_bscale(fptrNew, 1., 0., &status);
	if (status) fits_report_error(stderr, status);

	// Need to correct the dimensions, if we have subsectioned the image...
	char *comment = new char[80];
	long dud;
	if(this->pars().getFlagSubsection()){
	  fits_read_key(fptrOld, TLONG, "NAXIS1", &dud, comment, &status);
	  fits_update_key(fptrNew, TLONG, "NAXIS1", 
			  &(this->axisDim[0]), comment, &status);
	  fits_read_key(fptrOld, TLONG, "NAXIS2", &dud, comment, &status);
	  fits_update_key(fptrNew, TLONG, "NAXIS2", 
			  &(this->axisDim[1]), comment, &status);
	  fits_read_key(fptrOld, TLONG, "NAXIS3", &dud, comment, &status);
	  fits_update_key(fptrNew, TLONG, "NAXIS3", 
			  &(this->axisDim[2]), comment, &status);
	}

	writeReconHeaderInfo(fptrNew, this->par, "resid");

	if(this->par.getFlagBlankPix())
	  fits_write_pixnull(fptrNew, TFLOAT, fpixel, this->numPixels, 
			     resid, &blankval, &status);
	else  fits_write_pix(fptrNew, TFLOAT, fpixel, this->numPixels, 
			     resid, &status);

	fits_close_file(fptrNew, &status);
      }
  }

  delete [] resid;
  delete [] fpixel;

}


void writeReconHeaderInfo(fitsfile *fptr, Param &par, string nature)
{
  /**
   *  writeReconHeaderInfo(fptr, par, nature)
   *
   *   A simple function that writes all the necessary keywords and comments
   *    to the FITS header pointed to by fptr.
   *   The keyword names and comments are taken from duchamp.hh
   *   The parameter "nature" indicates what type of file is being written:
   *    should be either "recon" or "resid".
   */


  int status = 0;
  char *comment, *keyword;
  string explanation = "",ReconResid="";

  fits_write_history(fptr, (char *)header_reconHistory1.c_str(), &status);
				   
  fits_write_history(fptr, (char *)header_reconHistory2.c_str(), &status);

  fits_write_history(fptr, (char *)header_reconHistory_input.c_str(), &status);

  fits_write_history(fptr, (char *)par.getImageFile().c_str(), &status);

  if(par.getFlagSubsection()){
    fits_write_comment(fptr,(char *)header_reconSubsection_comment.c_str(),
		       &status);
    fits_write_key(fptr, TSTRING, (char *)keyword_subsection.c_str(), 
		   (char *)par.getSubsection().c_str(),
		   (char *)comment_subsection.c_str(), &status);
  }
    
  fits_write_comment(fptr, (char *)header_atrous_comment.c_str(), &status);

  float valf = par.getAtrousCut();
  fits_write_key(fptr, TFLOAT, (char *)keyword_snrRecon.c_str(), &valf, 
		 (char *)comment_snrRecon.c_str(), &status);

  int vali = par.getReconDim();
  fits_write_key(fptr, TINT, (char *)keyword_reconDim.c_str(), &vali, 
		 (char *)comment_reconDim.c_str(), &status);

  vali = par.getMinScale();
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
  else duchampWarning("write_header_info","explanation not present!\n");
  fits_write_comment(fptr, (char *)explanation.c_str(), &status);
  fits_write_key(fptr, TSTRING, (char *)keyword_ReconResid.c_str(), 
		 (char *)ReconResid.c_str(), 
		 (char *)comment_ReconResid.c_str(), &status);

}

void writeSmoothHeaderInfo(fitsfile *fptr, Param &par)
{
  /**
   *  writeSmoothHeaderInfo(fptr, par)
   *
   *   A simple function that writes all the necessary keywords and comments
   *    to the FITS header pointed to by fptr.
   *   The keyword names and comments are taken from duchamp.hh
   */


  int status = 0;
  char *comment, *keyword;

  fits_write_history(fptr, (char *)header_smoothHistory.c_str(), &status);
  status = 0;
  fits_write_history(fptr, (char *)header_smoothHistory_input.c_str(),&status);
  status = 0;
  fits_write_history(fptr, (char *)par.getImageFile().c_str(), &status);

  if(par.getFlagSubsection()){
    status = 0;
    fits_write_comment(fptr,(char *)header_smoothSubsection_comment.c_str(),
		       &status);
    status = 0;
    fits_write_key(fptr, TSTRING, (char *)keyword_subsection.c_str(), 
		   (char *)par.getSubsection().c_str(),
		   (char *)comment_subsection.c_str(), &status);
  }
    
  int val = par.getHanningWidth();
  fits_write_key(fptr, TINT, (char *)keyword_hanningwidth.c_str(), &val, 
		 (char *)comment_hanningwidth.c_str(), &status);

}
