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
#include <wcslib/wcsunits.h>
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


  void duchampFITSerror(int status, std::string subroutine, std::string error)
  {
    if(status){
      DUCHAMPWARN(subroutine,error);
      fits_report_error(stderr, status);
    }
  }


  OUTCOME Cube::saveMomentMapImage()
  {
    /// @details
    ///  A function to save the moment-0 map to a FITS file.
   
    int newbitpix = FLOAT_IMG;
    long *fpixel = new long[this->head.WCS().naxis];
    for(int i=0;i<this->header().WCS().naxis;i++) fpixel[i]=1;
    int status = 0;  /* MUST initialize status */
    fitsfile *fptr;         

    std::string fileout = "!" + this->par.outputMomentMapFile(); 
    // the ! is there so that it writes over an existing file.

    status = 0;
    if(fits_create_file(&fptr,fileout.c_str(),&status)){
      duchampFITSerror(status,"saveMomentMapImage","Error creating file:");
      return FAILURE;
    }
    else {

      if(this->writeBasicHeader(fptr, newbitpix,true)==FAILURE){
	DUCHAMPWARN("write Recon Cube", "Failure writing to header");
	return FAILURE;
      }

      writeMomentMapHeaderInfo(fptr, this->par);
	
      long size = this->axisDim[0] * this->axisDim[1];
      float *momentMap = new float[size];
      std::vector<bool> detectionMap = this->getMomentMap(momentMap);
      status=0;
      if(fits_write_pix(fptr, TFLOAT, fpixel, size, momentMap, &status)){
	duchampFITSerror(status,"saveMomentMapImage","Error writing data");
      }
      status = 0;
      if(fits_close_file(fptr, &status)){
	duchampFITSerror(status,"saveMomentMapImage","Error closing file");
      }

      delete [] momentMap;

    }

    delete [] fpixel;

    return SUCCESS;

  }

  OUTCOME Cube::saveMaskCube()
  {
    /// @details
    ///  A function to save a mask to a FITS file, indicating where the
    ///  detections where made. The value of the detected pixels is
    ///  determined by the flagMaskWithObjectNum parameter: if true,
    ///  the value of the pixels is given by the corresponding object
    ///  ID number; if false, they take the value 1 for all
    ///  objects. Pixels not in a detected object have the value 0.

    int newbitpix = SHORT_IMG;
    long *fpixel = new long[this->head.WCS().naxis];
    for(int i=0;i<this->header().WCS().naxis;i++) fpixel[i]=1;
    int status = 0;  /* MUST initialize status */
    fitsfile *fptrNew;         

    std::string fileout = "!" + this->par.outputMaskFile(); 
    // the ! is there so that it writes over an existing file.

    status = 0;
    if(fits_create_file(&fptrNew,fileout.c_str(),&status)){
      duchampFITSerror(status,"saveMask","Error creating file:");
      return FAILURE;
    }
    else {
      if(this->writeBasicHeader(fptrNew, newbitpix)==FAILURE){
	DUCHAMPWARN("write Mask Cube", "Failure writing to header");
	return FAILURE;
      }

      writeMaskHeaderInfo(fptrNew, this->par);

      char *comment = new char[FLEN_COMMENT];
      strcpy(comment,"");
      char *keyword = new char[FLEN_KEYWORD];
      std::string newunits;
      if(this->par.getFlagMaskWithObjectNum())
	newunits = "Object ID";
      else
	newunits = "Detection flag";
      strcpy(keyword,"BUNIT");
      if(fits_update_key(fptrNew, TSTRING, keyword, (char *)newunits.c_str(), comment, &status)){
	duchampFITSerror(status,"saveMask","Error writing BUNIT header:");
      }
      delete [] comment;
      delete [] keyword;
	
      short *mask = new short[this->numPixels];
      for(size_t i=0;i<this->numPixels;i++) mask[i]=0;
      std::vector<Detection>::iterator obj;
      for(obj=this->objectList->begin();obj<this->objectList->end();obj++){
        std::vector<PixelInfo::Voxel> voxlist = obj->getPixelSet();
        std::vector<PixelInfo::Voxel>::iterator vox;
        for(vox=voxlist.begin();vox<voxlist.end();vox++){
          size_t pixelpos = vox->getX() + this->axisDim[0]*vox->getY() + 
            this->axisDim[0]*this->axisDim[1]*vox->getZ();
          if(this->par.getFlagMaskWithObjectNum()) mask[pixelpos] = obj->getID();
	  else mask[pixelpos] = 1;
        }
      }
      status=0;
      if(fits_write_pix(fptrNew, TSHORT, fpixel, this->numPixels, mask, &status)){
	duchampFITSerror(status,"saveMask","Error writing mask array:");
      }
      status = 0;
      if(fits_close_file(fptrNew, &status)){
	duchampFITSerror(status,"saveMask","Error closing file:");
      }

      delete [] mask;

    }

    delete [] fpixel;

    return SUCCESS;
  }
  
  //---------------------------------------------------------------------------

  OUTCOME Cube::saveSmoothedCube()
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
    fitsfile *fptrNew;         

    if(this->par.getFlagOutputSmooth()){
      std::string fileout = "!" + this->par.outputSmoothFile(); 
      // the ! is there so that it writes over an existing file.

      status = 0;
      if(fits_create_file(&fptrNew,fileout.c_str(),&status)){
	duchampFITSerror(status,"saveSmoothedCube","Error creating smoothed FITS file:");
	return FAILURE;
      }
      else {

	  if(this->writeBasicHeader(fptrNew, -32)==FAILURE){
	    DUCHAMPWARN("write Smoothed Cube", "Failure writing to header");
	    return FAILURE;
	  }

	writeSmoothHeaderInfo(fptrNew, this->par);

	if(this->par.getFlagBlankPix())
	  fits_write_imgnull(fptrNew, TFLOAT, 1, this->numPixels, this->recon, &blankval, &status);
	else 
	  fits_write_img(fptrNew, TFLOAT, 1, this->numPixels, this->recon, &status);
	if(status){
	  duchampFITSerror(status,"saveSmothedCube","Error writing smoothed array:");
	  return FAILURE;
	}

	status = 0;
	if(fits_close_file(fptrNew, &status)){
	  duchampFITSerror(status,"saveSmoothedCube","Error closing file:");
	}
      }
    }

    return SUCCESS;

  }

  //---------------------------------------------------------------------------

  OUTCOME Cube::saveReconstructedCube()
  {
    /// @details
    ///  A function to save the reconstructed and/or residual arrays.
    ///   A number of header keywords are written as well, indicating the 
    ///    nature of the reconstruction that has been done.
    ///   The file is always written -- if the filename (as calculated 
    ///    based on the recon parameters) exists, then it is overwritten.
  
    float blankval = this->par.getBlankPixVal();

    if(!this->reconAllocated){
      DUCHAMPERROR("saveReconCube","Have not allocated reconstructed array, so cannot save");
      return FAILURE;
    }

    int status = 0;  /* MUST initialize status */
    fitsfile *fptrNew;         
  
    if(this->par.getFlagOutputRecon()){
      std::string fileout = "!" + this->par.outputReconFile(); 
      // the ! is there so that it writes over an existing file.

      status = 0;
      if(fits_create_file(&fptrNew,fileout.c_str(),&status)){
	duchampFITSerror(status,"saveReconCube","Error creating file:");
	return FAILURE;
      }
      else
	{

	  if(this->writeBasicHeader(fptrNew, -32)==FAILURE){
	    DUCHAMPWARN("write Recon Cube", "Failure writing to header");
	    return FAILURE;
	  }
	  
	  writeReconHeaderInfo(fptrNew, this->par, "recon");

	  status=0;
	  long *fpixel = new long[this->header().WCS().naxis];
	  for(int i=0;i<this->numDim;i++) fpixel[i]=1;
	  long group=0;
	  if(this->par.getFlagBlankPix())
	    fits_write_imgnull_flt(fptrNew, group, 1, this->numPixels, this->recon, blankval, &status);
	  else  
	    fits_write_img_flt(fptrNew, group, 1, this->numPixels, this->recon, &status);
	  if(status){
	    duchampFITSerror(status,"saveReconCube","Error writing reconstructed array:");
	    return FAILURE;
	  }
	  delete [] fpixel;

	  status = 0;
	  if(fits_close_file(fptrNew, &status)){
	    duchampFITSerror(status,"saveReconCube","Error closing file:");
	  }
	}
    }


    if(this->par.getFlagOutputResid()){
      float *resid = new float[this->numPixels];
      for(size_t i=0;i<this->numPixels;i++) 
	resid[i] = this->array[i] - this->recon[i];

      std::string fileout = "!" + this->par.outputResidFile(); 
      // the ! is there so that it writes over an existing file.
      status = 0;
      if(fits_create_file(&fptrNew,fileout.c_str(),&status)){
	duchampFITSerror(status,"saveResidualCube","Error creating new file:");
	return FAILURE;
      }
      else
	{

	  if(this->writeBasicHeader(fptrNew, -32)==FAILURE){
	    DUCHAMPWARN("write Recon Cube", "Failure writing to header");
	    return FAILURE;
	  }

	  writeReconHeaderInfo(fptrNew, this->par, "resid");

	  if(this->par.getFlagBlankPix())
	    fits_write_imgnull(fptrNew, TFLOAT, 1, this->numPixels, resid, &blankval, &status);
	  else  
	    fits_write_img(fptrNew, TFLOAT, 1, this->numPixels, resid, &status);
	  if(status){
	    duchampFITSerror(status,"saveResidualCube","Error writing reconstructed array:");
	    return FAILURE;
	  }

	  status=0;
	  if(fits_close_file(fptrNew, &status)){
	    duchampFITSerror(status,"saveResidualCube","Error closing file:");
	  }
	}
      delete [] resid;
    }

    return SUCCESS;
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
    else DUCHAMPWARN("write_header_info","explanation not present");
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


  //---------------------------------------------------------------------------

  OUTCOME Cube::writeBasicHeader(fitsfile *fptr, int bitpix, bool is2D)
  {
    char *header, *hptr, keyname[9];
    int  i, nkeyrec, status = 0;
    
    const size_t naxis=this->numDim;
    long* naxes = new long[this->numDim];
    for(size_t i=0;i<naxis;i++) naxes[i]=this->axisDim[i];
    if(is2D) naxes[this->head.WCS().spec]=1;
    // write the required header keywords 
    fits_write_imghdr(fptr, bitpix, naxis, naxes,  &status);

    // Write beam information
    this->head.beam().writeToFITS(fptr);

    // Write bunit information
    status = 0;
    strcpy(keyname,"BUNIT");
    if (fits_update_key(fptr, TSTRING, keyname, (char *)this->head.getFluxUnits().c_str(), NULL, &status)){
      DUCHAMPWARN("saveImage","Error writing bunit info:");
      fits_report_error(stderr, status);
      return FAILURE;
    }

    // convert the wcsprm struct to a set of 80-char keys
    if ((status = wcshdo(WCSHDO_all, this->head.getWCS(), &nkeyrec, &header))) {
      DUCHAMPWARN("saveImage","Could not convert WCS information to FITS header. WCS Error Code = "<<status<<": "<<wcs_errmsg[status]);
      return FAILURE;
    }

    hptr = header;
    strncpy(keyname,hptr,8);
    for (i = 0; i < nkeyrec; i++, hptr += 80) {
      status=0;
      if(fits_update_card(fptr,keyname,hptr,&status)){
	DUCHAMPWARN("saveImage","Error writing header card");
	fits_report_error(stderr,status);
	return FAILURE;
      }
    }
    
    if(bitpix>0){
      if(this->par.getFlagBlankPix()){
	strcpy(keyname,"BSCALE");
	float bscale=this->head.getBscaleKeyword();
	if(fits_update_key(fptr, TFLOAT, keyname, &bscale, NULL, &status)){
	  duchampFITSerror(status,"saveImage","Error writing BSCALE header:");
	}
	strcpy(keyname,"BZERO");
	float bzero=this->head.getBzeroKeyword();
	if(fits_update_key(fptr, TFLOAT, keyname, &bzero, NULL, &status)){
	  duchampFITSerror(status,"saveImage","Error writing BZERO header:");
	}
	strcpy(keyname,"BLANK");
	int blank=this->head.getBlankKeyword();
	if(fits_update_key(fptr, TINT, keyname, &blank, NULL, &status)){
	  duchampFITSerror(status,"saveImage","Error writing BLANK header:");
	}
	if(fits_set_imgnull(fptr, blank, &status)){
	  duchampFITSerror(status, "saveImage", "Error setting null value:");
	}
	if(fits_set_bscale(fptr, bscale, bzero, &status)){
	  duchampFITSerror(status,"saveImage","Error setting scale:");
	}
      }
    }

    delete [] naxes;

    return SUCCESS;

  }

}
