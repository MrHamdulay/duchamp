// -----------------------------------------------------------------------
// subsection.cc: Make sure subsections are valid for a particular
//                FITS file.
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
#include <vector>
#include <math.h>
#include <wcslib/wcs.h>
#define WCSLIB_GETWCSTAB // define this so that we don't try and redefine 
                         //  wtbarr (this is a problem when using gcc v.4+
#include <fitsio.h>
#include <duchamp/param.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/Utils/Section.hh>

namespace duchamp
{

  void Param::setOffsets(struct wcsprm *wcs)
  {
    /// If there is a subsection being used, set the offset values
    /// according to the correct dimensions given by the WCS struct.  
    /// If not, set the offsets to zero.
    /// \param wcs The WCSLIB wcsprm struct that defines which axis is which.

    if(this->flagSubsection){ // if so, then the offsets array is defined.
      int specAxis = wcs->spec;
      if(specAxis<0) specAxis=2;
      if(specAxis>=wcs->naxis) specAxis = wcs->naxis-1;
      this->xSubOffset = this->pixelSec.getStart(wcs->lng);
      this->ySubOffset = this->pixelSec.getStart(wcs->lat);
      this->zSubOffset = this->pixelSec.getStart(specAxis);
    }
    else{// else they should be 0
      this->xSubOffset = this->ySubOffset = this->zSubOffset = 0;
    }
  }

  void Param::setOffsets(struct wcsprm &wcs)
  {
    /// If there is a subsection being used, set the offset values
    /// according to the correct dimensions given by the WCS struct.  
    /// If not, set the offsets to zero.
    /// \param wcs The WCSLIB wcsprm struct that defines which axis is which.
    
    if(this->flagSubsection){ // if so, then the offsets array is defined.
      int specAxis = wcs.spec;
      if(specAxis<0) specAxis=2;
      if(specAxis>=wcs.naxis) specAxis = wcs.naxis-1;
      this->xSubOffset = this->pixelSec.getStart(wcs.lng);
      this->ySubOffset = this->pixelSec.getStart(wcs.lat);
      this->zSubOffset = this->pixelSec.getStart(specAxis);
    }
    else{// else they should be 0
      this->xSubOffset = this->ySubOffset = this->zSubOffset = 0;
    }
  }

  OUTCOME Param::verifySubsection()
  {
    /// Checks that the subsection strings (the pixel and stats
    /// subsections) are in the appropriate format, with the correct
    /// number of entries (one for each axis).
    /// 
    /// This reads the dimensional information from the FITS file, and
    /// uses this with the Section::parse() function to make sure each
    /// section is OK.
    /// 
    /// \return SUCCESS/FAILURE depending on outcome of the
    /// Section::parse() calls. Also FAILURE if something goes wrong with
    /// the FITS access.

    if(!this->flagSubsection && !this->flagStatSec){
      // if we get here, we are using neither subsection
      return SUCCESS;
    }
    else{
      // otherwise, at least one of the subsections is being used, and
      // so we need to check them

      // First open the requested FITS file and check its existence and 
      //  number of axes.
      int numAxes,status = 0;  /* MUST initialize status */
      fitsfile *fptr;         

      // Make sure the FITS file exists
      int exists;
      fits_file_exists(this->imageFile.c_str(),&exists,&status);
      if(exists<=0){
	fits_report_error(stderr, status);
	duchampWarning("Cube Reader", "Requested image does not exist!\n");
	return FAILURE;
      }
      // Open the FITS file
      if( fits_open_file(&fptr,this->imageFile.c_str(),READONLY,&status) ){
	fits_report_error(stderr, status);
	return FAILURE;
      }
      // Read the size information -- number of axes and their sizes
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
      // Close the FITS file.
      status = 0;
      fits_close_file(fptr, &status);
      if (status){
	duchampWarning("Cube Reader","Error closing file: ");
	fits_report_error(stderr, status);
      }

      ///////////////////
      // Now parse the subsection and make sure all that works.
  
      std::vector<long> dim(numAxes);
      for(int i=0;i<numAxes;i++) dim[i] = dimAxes[i];
      delete [] dimAxes;
  
      if(this->flagSubsection)
	if(this->pixelSec.parse(dim)==FAILURE) return FAILURE;

      if(this->flagStatSec)
	if(this->statSec.parse(dim)==FAILURE)  return FAILURE;
  
      return SUCCESS;
    }

  }

}
