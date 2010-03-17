// -----------------------------------------------------------------------
// saveImage.cc: Write a wavelet-reconstructed or smoothed array to a
//               FITS file.
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
#include <string.h>
#include <wcslib/wcs.h>
#include <wcslib/wcshdr.h>
#define WCSLIB_GETWCSTAB 
// define this so that we don't try and redefine wtbarr 
// (this is a problem when using cfitsio v.3 and g++ v.4)

#include <fitsio.h>
#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/PixelMap/Voxel.hh>
#include <duchamp/PixelMap/Object3D.hh>

namespace duchamp
{

  /// @brief Write FITS headers in correct format for reconstructed array output 
  void writeReconHeaderInfo(fitsfile *fptr, Param &par, std::string nature);

  /// @brief Write FITS headers in correct format for smoothed array output 
  void writeSmoothHeaderInfo(fitsfile *fptr, Param &par);

  /// @brief Write FITS headers in correct format for mask array output 
  void writeMaskHeaderInfo(fitsfile *fptr, Param &par);

  /// @brief Write FITS headers in correct format for moment-0 array output 
  void writeMomentMapHeaderInfo(fitsfile *fptr, Param &par);

  //---------------------------------------------------------------------------

  char *numerateKeyword(std::string key, int num)
  {
    /// @details A utility function to combine a keyword and a
    /// value, to produce a relevant FITS keyword for a given
    /// axis. For example numerateKeyword(CRPIX,1) returns CRPIX1.
    std::stringstream ss;
    ss << key << num;
    return (char *)ss.str().c_str();
  }

  void Cube::saveMomentMapImage()
  {
    /// @details
    ///  A function to save the moment-0 map to a FITS file.
   
    int newbitpix = FLOAT_IMG;
    char *comment = new char[FLEN_COMMENT];
    strcpy(comment,"");
    long *fpixel = new long[2];
    for(int i=0;i<2;i++) fpixel[i]=1;
    int status = 0;  /* MUST initialize status */
    fitsfile *fptr;         

    std::string fileout = "!" + this->par.outputMomentMapFile(); 
    // the ! is there so that it writes over an existing file.
    char *keyword = new char[FLEN_KEYWORD];

    status = 0;
    fits_create_file(&fptr,fileout.c_str(),&status);
    if (status){
      duchampError("saveMomentMapImage","Error creating file:");
      fits_report_error(stderr, status);
    }
    else {
      status = 0;
      long *dim = new long[2];
      for (uint i = 0; i < 2; i++) dim[i] = this->axisDim[i];
      if (fits_create_img(fptr, newbitpix, 2, dim, &status)) {
	duchampError("saveMomentMapImage","Error creating image:");
	fits_report_error(stderr, status);
      }

      if(!this->par.getFlagUsingBeam()){   // if the cube has beam info, copy it
	float bmaj=this->head.getBmajKeyword(), bmin=this->head.getBminKeyword(), bpa=this->head.getBpaKeyword();
	status = 0;
	strcpy(keyword,"BMAJ");
	if (fits_update_key(fptr, TFLOAT, keyword, &bmaj, NULL, &status)){
	  duchampError("saveMomentMapImage","Error saving beam info:");
	  fits_report_error(stderr, status);
	}
	status = 0;
	strcpy(keyword,"BMIN");
	if (fits_update_key(fptr, TFLOAT, keyword, &bmin, NULL, &status)){
	  duchampError("saveMomentMapImage","Error saving beam info:");
	  fits_report_error(stderr, status);
	}
	status = 0;
	strcpy(keyword,"BPA");
	if (fits_update_key(fptr, TFLOAT, keyword, &bpa, NULL, &status)){
	  duchampError("saveMomentMapImage","Error saving beam info:");
	  fits_report_error(stderr, status);
	}
      }			
      status = 0;
      strcpy(keyword,"EQUINOX");
      if (fits_update_key(fptr, TFLOAT, keyword, &this->head.WCS().equinox, NULL, &status)){
	duchampError("saveMomentMapImage","Error saving equinox:");
	fits_report_error(stderr, status);
      }
      status = 0;
      char unit[this->head.getIntFluxUnits().size()+1];
      strcpy(unit, this->head.getIntFluxUnits().c_str());
      strcpy(keyword,"BUNIT");
      if (fits_update_key(fptr, TSTRING, keyword, unit,  NULL, &status)){
	duchampError("saveMomentMapImage","Error saving BUNIT:");
	fits_report_error(stderr, status);
      }
      float val;
      int axisNumbers[2];
      axisNumbers[0] = this->head.WCS().lng;
      axisNumbers[1] = this->head.WCS().lat;
      for (uint d = 0; d < 2; d++) {
	int i = axisNumbers[d];
	status = 0;
	if (fits_update_key(fptr, TSTRING, numerateKeyword("CTYPE", d + 1), this->head.WCS().ctype[i],  NULL, &status)){
	  duchampError("saveMomentMapImage","Error saving CTYPE:");
	  fits_report_error(stderr, status);
	}
	status = 0;
	if (fits_update_key(fptr, TSTRING, numerateKeyword("CUNIT", d + 1), this->head.WCS().cunit[i],  NULL, &status)){
	  duchampError("saveMomentMapImage","Error saving CUNIT:");
	  fits_report_error(stderr, status);
	}
	status = 0;
	val = this->head.WCS().crval[i];
	if (fits_update_key(fptr, TFLOAT, numerateKeyword("CRVAL", d + 1), &val, NULL, &status)){
	  duchampError("saveMomentMapImage","Error saving CRVAL:");
	  fits_report_error(stderr, status);
	}
	val = this->head.WCS().cdelt[i];
	status = 0;
	if (fits_update_key(fptr, TFLOAT, numerateKeyword("CDELT", d + 1), &val, NULL, &status)){
	  duchampError("saveMomentMapImage","Error saving CDELT:");
	  fits_report_error(stderr, status);
	}
	val = this->head.WCS().crpix[i];
	status = 0;
	if (fits_update_key(fptr, TFLOAT, numerateKeyword("CRPIX", d + 1), &val, NULL, &status)){
	  duchampError("saveMomentMapImage","Error saving CRPIX:");
	  fits_report_error(stderr, status);
	}
	val = this->head.WCS().crota[i];
	status = 0;
	if (fits_update_key(fptr, TFLOAT, numerateKeyword("CROTA", d + 1), &val, NULL, &status)){
	  duchampError("saveMomentMapImage","Error saving CROTA:");
	  fits_report_error(stderr, status);
	}
      }

      delete [] comment;
      delete [] keyword;

      writeMomentMapHeaderInfo(fptr, this->par);
	
      long size = this->axisDim[0] * this->axisDim[1];
      float *momentMap = new float[size];
      std::vector<bool> detectionMap;
      this->getMomentMap(momentMap,detectionMap);
      status=0;
      if(fits_write_pix(fptr, TFLOAT, fpixel, size, momentMap, &status)){
	duchampError("saveMomentMapImage","Error writing data");
	fits_report_error(stderr,status);
      }
      status = 0;
      if(fits_close_file(fptr, &status)){
	duchampError("saveMomentMapImage","Error closing file");
	fits_report_error(stderr, status);
      }

      delete [] momentMap;

    }

    delete [] fpixel;


  }

  void Cube::saveMaskCube()
  {
    /// @details
    ///  A function to save a mask to a FITS file, indicating where the
    ///  detections where made. The value of the detected pixels is
    ///  determined by the flagMaskWithObjectNum parameter: if true,
    ///  the value of the pixels is given by the corresponding object
    ///  ID number; if false, they take the value 1 for all
    ///  objects. Pixels not in a detected object have the value 0.

    int newbitpix = SHORT_IMG;
    char *comment = new char[FLEN_COMMENT];
    strcpy(comment,"");
    long *fpixel = new long[this->header().WCS().naxis];
    for(int i=0;i<this->header().WCS().naxis;i++) fpixel[i]=1;
    int status = 0;  /* MUST initialize status */
    fitsfile *fptrOld, *fptrNew;         
    fits_open_file(&fptrOld,this->par.getImageFile().c_str(),READONLY,&status);
    if (status){
      duchampError("saveMask","Fits Error 1:");
      fits_report_error(stderr, status);
    }

    std::string fileout = "!" + this->par.outputMaskFile(); 
    // the ! is there so that it writes over an existing file.

    status = 0;
    fits_create_file(&fptrNew,fileout.c_str(),&status);
    if (status){
      duchampError("saveMask","Fits Error 2:");
      fits_report_error(stderr, status);
    }
    else {
      status = 0;
      fits_movabs_hdu(fptrOld, 1, NULL, &status);
      if (status){
	duchampError("saveMask","Fits Error 3:");
	fits_report_error(stderr, status);
      }
      status = 0;
      fits_copy_header(fptrOld, fptrNew, &status);
      if (status){
	duchampError("saveMask","Fits Error 4:");
	fits_report_error(stderr, status);
      }
      status = 0;
      char *keyword = new char[FLEN_KEYWORD];
      strcpy(keyword,"BITPIX");
      strcpy(comment,"number of bits per data pixel");
      fits_update_key(fptrNew, TINT, keyword, &newbitpix, comment, &status);
      if (status){
	duchampError("saveMask","Fits Error 5:");
	fits_report_error(stderr, status);
      }
      float bscale=1., bzero=0.;
      strcpy(comment,"");
      strcpy(keyword,"BSCALE");
      fits_update_key(fptrNew, TFLOAT, keyword, &bscale, comment, &status);
      if (status){
	duchampError("saveMask","Fits Error 6:");
	fits_report_error(stderr, status);
      }
      strcpy(keyword,"BZERO");
      fits_update_key(fptrNew, TFLOAT, keyword, &bzero, comment, &status);
      if (status){
	duchampError("saveMask","Fits Error 7:");
	fits_report_error(stderr, status);
      }
      fits_set_bscale(fptrNew, 1, 0, &status);
      if (status){
	duchampError("saveMask","Fits Error 8:");
	fits_report_error(stderr, status);
      }
      std::string newunits;
      if(this->par.getFlagMaskWithObjectNum())
	newunits = "Object ID";
      else
	newunits = "Detection flag";
      strcpy(keyword,"BUNIT");
      fits_update_key(fptrNew, TSTRING, keyword, (char *)newunits.c_str(), comment, &status);
      if (status){
	duchampError("saveMask","Fits Error 9:");
	fits_report_error(stderr, status);
      }
      long dud;
      // Need to correct the dimensions, if we have subsectioned the image
      if(this->par.getFlagSubsection()){
	std::stringstream naxis;
	fits_read_key(fptrOld, TLONG, (char *)naxis.str().c_str(), &dud, comment, &status);
	fits_update_key(fptrNew, TLONG, (char *)naxis.str().c_str(),
			&(this->axisDim[0]), comment, &status);
	naxis.str("");
	naxis << "NAXIS" << this->head.WCS().lat;
	fits_read_key(fptrOld, TLONG, (char *)naxis.str().c_str(), &dud, comment, &status);
	fits_update_key(fptrNew, TLONG, (char *)naxis.str().c_str(), 
			&(this->axisDim[1]), comment, &status);
	naxis.str("");
	naxis << "NAXIS" << this->head.WCS().spec;
	fits_read_key(fptrOld, TLONG, (char *)naxis.str().c_str(), &dud, comment, &status);
	fits_update_key(fptrNew, TLONG, (char *)naxis.str().c_str(), 
			&(this->axisDim[2]), comment, &status);
      }

      delete [] comment;
      delete [] keyword;

      writeMaskHeaderInfo(fptrNew, this->par);
	
      short *mask = new short[this->numPixels];
      for(int i=0;i<this->numPixels;i++) mask[i]=0;
      std::vector<Detection>::iterator obj;
      for(obj=this->objectList->begin();obj<this->objectList->end();obj++){
        std::vector<PixelInfo::Voxel> voxlist = obj->getPixelSet();
        std::vector<PixelInfo::Voxel>::iterator vox;
        for(vox=voxlist.begin();vox<voxlist.end();vox++){
          int pixelpos = vox->getX() + this->axisDim[0]*vox->getY() + 
            this->axisDim[0]*this->axisDim[1]*vox->getZ();
          if(this->par.getFlagMaskWithObjectNum()) mask[pixelpos] = obj->getID();
	  else mask[pixelpos] = 1;
        }
      }
      status=0;
      fits_write_pix(fptrNew, TSHORT, fpixel, this->numPixels, mask, &status);
      if(status){
	duchampError("saveMask","Fits Error 10:");
	fits_report_error(stderr,status);
      }
      status = 0;
      fits_close_file(fptrNew, &status);
      if (status){
	duchampError("saveMask","Fits Error 11:");
	fits_report_error(stderr, status);
      }

      delete [] mask;

    }

    delete [] fpixel;

  }
  
  //---------------------------------------------------------------------------

  void Cube::saveSmoothedCube()
  {
    /// @brief
    ///   A function to save the smoothed arrays to a FITS file.
    ///   Additional header keywords are written as well, indicating the
    ///   width of the Hanning filter or the dimensions of the Gaussian
    ///   kernel.
    ///   The file is always written -- if the filename (as calculated 
    ///    based on the parameters) exists, then it is overwritten.
  
    float blankval = this->par.getBlankPixVal();

    int status = 0;  /* MUST initialize status */
    fitsfile *fptrOld, *fptrNew;         
    fits_open_file(&fptrOld,this->par.getFullImageFile().c_str(),READONLY,&status);
    if (status) fits_report_error(stderr, status);
  
    if(this->par.getFlagOutputSmooth()){
      std::string fileout = "!" + this->par.outputSmoothFile(); 
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

	writeSmoothHeaderInfo(fptrNew, this->par);

	if(this->par.getFlagBlankPix())
	  fits_write_imgnull(fptrNew, TFLOAT, 1, this->numPixels, this->recon, &blankval, &status);
	else 
	  fits_write_img(fptrNew, TFLOAT, 1, this->numPixels, this->recon, &status);

	status = 0;
	fits_close_file(fptrNew, &status);
	if (status) fits_report_error(stderr, status);

      }
    }

  }

  //---------------------------------------------------------------------------

  void Cube::saveReconstructedCube()
  {
    /// @details
    ///  A function to save the reconstructed and/or residual arrays.
    ///   A number of header keywords are written as well, indicating the 
    ///    nature of the reconstruction that has been done.
    ///   The file is always written -- if the filename (as calculated 
    ///    based on the recon parameters) exists, then it is overwritten.
  
    float blankval = this->par.getBlankPixVal();

    int status = 0;  /* MUST initialize status */
    fitsfile *fptrOld, *fptrNew;         
    fits_open_file(&fptrOld,this->par.getFullImageFile().c_str(),READONLY,&status);
    if (status) fits_report_error(stderr, status);
  
    if(this->par.getFlagOutputRecon()){
      std::string fileout = "!" + this->par.outputReconFile(); 
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

	  writeReconHeaderInfo(fptrNew, this->par, "recon");

	  if(this->par.getFlagBlankPix())
	    fits_write_imgnull(fptrNew, TFLOAT, 1, this->numPixels, this->recon, &blankval, &status);
	  else  
	    fits_write_img(fptrNew, TFLOAT, 1, this->numPixels, this->recon, &status);

	  status = 0;
	  fits_close_file(fptrNew, &status);
	  if (status) fits_report_error(stderr, status);
	}
    }


    if(this->par.getFlagOutputResid()){
      float *resid = new float[this->numPixels];
      for(int i=0;i<this->numPixels;i++) 
	resid[i] = this->array[i] - this->recon[i];

      std::string fileout = "!" + this->par.outputResidFile(); 
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

	  writeReconHeaderInfo(fptrNew, this->par, "resid");

	  if(this->par.getFlagBlankPix())
	    fits_write_imgnull(fptrNew, TFLOAT, 1, this->numPixels, resid, &blankval, &status);
	  else  
	    fits_write_img(fptrNew, TFLOAT, 1, this->numPixels, resid, &status);

	  fits_close_file(fptrNew, &status);
	}
      delete [] resid;
    }


  }

  //---------------------------------------------------------------------------

  void writeReconHeaderInfo(fitsfile *fptr, Param &par, std::string nature)
  {
    /// @details
    ///   A simple function that writes all the necessary keywords and comments
    ///    to the FITS header pointed to by fptr.
    ///   The keyword names and comments are taken from duchamp.hh
    ///   The parameter "nature" indicates what type of file is being written:
    ///    should be either "recon" or "resid".

    int status = 0;
    std::string explanation = "",ReconResid="";

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

  //---------------------------------------------------------------------------

  void writeSmoothHeaderInfo(fitsfile *fptr, Param &par)
  {
    /// @details
    ///   A simple function that writes all the necessary keywords and comments
    ///    to the FITS header pointed to by fptr.
    ///   The keyword names and comments are taken from duchamp.hh

    int status = 0;

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
    
    if(par.getSmoothType()=="spatial"){
      // if kernMin is negative (not defined), make it equal to kernMaj
      if(par.getKernMin() < 0) par.setKernMin(par.getKernMaj());

      fits_write_key(fptr, TSTRING, (char *)keyword_smoothtype.c_str(),
		     (char *)header_smoothSpatial.c_str(),
		     (char *)comment_smoothtype.c_str(), &status);
      float valf = par.getKernMaj();
      fits_write_key(fptr, TFLOAT, (char *)keyword_kernmaj.c_str(), &valf,
		     (char *)comment_kernmaj.c_str(), &status);
      valf = par.getKernMin();
      fits_write_key(fptr, TFLOAT, (char *)keyword_kernmin.c_str(), &valf,
		     (char *)comment_kernmin.c_str(), &status);
      valf = par.getKernPA();
      fits_write_key(fptr, TFLOAT, (char *)keyword_kernpa.c_str(), &valf,
		     (char *)comment_kernpa.c_str(), &status);
    }
    else if(par.getSmoothType()=="spectral"){
      fits_write_key(fptr, TSTRING, (char *)keyword_smoothtype.c_str(),
		     (char *)header_smoothSpectral.c_str(),
		     (char *)comment_smoothtype.c_str(), &status);
      int vali = par.getHanningWidth();
      fits_write_key(fptr, TINT, (char *)keyword_hanningwidth.c_str(), &vali,
		     (char *)comment_hanningwidth.c_str(), &status);
    }
  }

  //---------------------------------------------------------------------------

  void writeMaskHeaderInfo(fitsfile *fptr, Param &par)
  {
    /// @details
    ///   A simple function that writes all the necessary keywords and comments
    ///    to the FITS header pointed to by fptr.
    ///   The keyword names and comments are taken from duchamp.hh

    int status = 0;

    fits_write_history(fptr, (char *)header_maskHistory.c_str(), &status);
    status = 0;
    fits_write_history(fptr, (char *)header_maskHistory_input.c_str(),&status);
    status = 0;
    fits_write_history(fptr, (char *)par.getImageFile().c_str(), &status);

    if(par.getFlagSubsection()){
      status = 0;
      fits_write_comment(fptr,(char *)header_maskSubsection_comment.c_str(),
			 &status);
      status = 0;
      fits_write_key(fptr, TSTRING, (char *)keyword_subsection.c_str(), 
		     (char *)par.getSubsection().c_str(),
		     (char *)comment_subsection.c_str(), &status);
    }
  }

  //---------------------------------------------------------------------------

  void writeMomentMapHeaderInfo(fitsfile *fptr, Param &par)
  {
    /// @details
    ///   A simple function that writes all the necessary keywords and comments
    ///    to the FITS header pointed to by fptr.
    ///   The keyword names and comments are taken from duchamp.hh

    int status = 0;

    fits_write_history(fptr, (char *)header_moment0History.c_str(), &status);
    status = 0;
    fits_write_history(fptr, (char *)header_moment0History_input.c_str(),&status);
    status = 0;
    fits_write_history(fptr, (char *)par.getImageFile().c_str(), &status);

    if(par.getFlagSubsection()){
      status = 0;
      fits_write_comment(fptr,(char *)header_moment0Subsection_comment.c_str(),
			 &status);
      status = 0;
      fits_write_key(fptr, TSTRING, (char *)keyword_subsection.c_str(), 
		     (char *)par.getSubsection().c_str(),
		     (char *)comment_subsection.c_str(), &status);
    }
  }

}
