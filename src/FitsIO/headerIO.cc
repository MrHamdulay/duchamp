// -----------------------------------------------------------------------
// headerIO.cc: Read in various PHU keywords from a FITS file.
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
#include <string>
#include <string.h>
#include <wcslib/wcsunits.h>
#define WCSLIB_GETWCSTAB // define this so that we don't try and redefine 
                         //  wtbarr (this is a problem when using gcc v.4+
#include <fitsio.h>
#include <longnam.h>
#include <math.h>
#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp
{

  OUTCOME FitsHeader::readHeaderInfo(std::string fname, Param &par)
  {
    ///  A simple front-end function to the three header access
    ///   functions defined below.

    OUTCOME returnValue = SUCCESS;

    // Get the brightness unit, so that we can set the units for the 
    //  integrated flux when we go to fixUnits.
    if(this->readBUNIT(fname) == FAILURE) return FAILURE;
  
    if(this->readBLANKinfo(fname, par)==FAILURE) returnValue=FAILURE;
  
    if(this->readBeamInfo(fname, par)==FAILURE) returnValue=FAILURE;

    if(this->wcs->spec>=0) this->fixUnits(par);

    return returnValue;
  }


  //////////////////////////////////////////////////

  OUTCOME FitsHeader::readBUNIT(std::string fname)
  {
    ///   Read the BUNIT header keyword, to store the units of brightness (flux).
    ///  \param fname The name of the FITS file.

    fitsfile *fptr;         
    char *comment = new char[FLEN_COMMENT];
    char *unit = new char[FLEN_VALUE];
    OUTCOME returnStatus=SUCCESS;
    int status = 0;

    // Open the FITS file
    status = 0;
    if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
      fits_report_error(stderr, status);
      return FAILURE;
    }

    // Read the BUNIT keyword, and translate to standard unit format if needs be
    std::string header("BUNIT");
    fits_read_key(fptr, TSTRING, (char *)header.c_str(), unit, comment, &status);
    if (status){
      duchampWarning("Cube Reader","Error reading BUNIT keyword: ");
      fits_report_error(stderr, status);
      return FAILURE;
    }
    else{
      wcsutrn(0,unit);
      this->fluxUnits = unit;
      this->originalFluxUnits = unit;
    }

    // Close the FITS file
    status = 0;
    fits_close_file(fptr, &status);
    if (status){
      duchampWarning("Cube Reader","Error closing file: ");
      fits_report_error(stderr, status);
    }

    delete [] comment;
    delete [] unit;

    return returnStatus;
  }

  //////////////////////////////////////////////////

  OUTCOME FitsHeader::readBLANKinfo(std::string fname, Param &par)
  {
    ///    Reading in the Blank pixel value keywords, which is only done
    ///    if requested via the flagBlankPix parameter.
    /// 
    ///    If the BLANK keyword is in the header, use that and store the relevant 
    ///     values. Also copy them into the parameter set.
    ///    If not, use the default value (either the default from param.cc or 
    ///     from the param file) and assume simple values for the keywords:
    ///     <ul><li> The scale keyword is the same as the blank value, 
    ///         <li> The blank keyword (which is an int) is 1 and 
    ///         <li> The bzero (offset) is 0.
    ///    </ul>
    /// \param fname The name of the FITS file.
    /// \param par The Param set: to know the flagBlankPix value and to
    /// store the keywords.

    OUTCOME returnStatus = SUCCESS;
    int status = 0;

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
    //  If this isn't present, make sure flagTrim is false (if it is
    //  currently true, let the user know you're doing this) and set
    //  flagBlankPix to false so that the isBlank functions all return
    //  false
    //  If it is, read the other two necessary keywords, and then set
    //     the values accordingly.

    std::string header("BLANK");
    if(fits_read_key(fptr, TINT, (char *)header.c_str(), &blank, comment, &status)){

      par.setFlagBlankPix(false);

      if(par.getFlagTrim()){
	par.setFlagTrim(false);
	std::stringstream errmsg;
	if(returnStatus == KEY_NO_EXIST)
	  duchampWarning("Cube Reader", 
			 "There is no BLANK keyword present. Not doing any trimming.\n");
	else{
	  duchampWarning("Cube Reader", 
			 "Error reading BLANK keyword, so not doing any trimming.");
	  fits_report_error(stderr, status);
	  return FAILURE;
	}
      }
    }
    else{
      status = 0;
      header="BZERO";
      fits_read_key(fptr, TFLOAT, (char *)header.c_str(), &bzero, comment, &status);
      status = 0;
      header="BSCALE";
      fits_read_key(fptr, TFLOAT, (char *)header.c_str(), &bscale, NULL, &status);
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
      duchampWarning("Cube Reader","Error closing file: ");
      fits_report_error(stderr, status);
    }
  
    delete [] comment;

    return returnStatus;

  }

  //////////////////////////////////////////////////

  OUTCOME FitsHeader::readBeamInfo(std::string fname, Param &par)
  {
    ///   Reading in the beam parameters from the header.
    ///   Use these, plus the basic WCS parameters to calculate the size of
    ///    the beam in pixels. Copy the beam size into the parameter set.
    ///   If information not present in FITS header, use the parameter
    ///    set to define the beam size.
    /// \param fname The name of the FITS file.
    /// \param par The Param set.

    int status=0;
    fitsfile *fptr;         

    // Open the FITS file
    if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
      fits_report_error(stderr, status);
      return FAILURE;
    }
    
    this->itsBeam.readFromFITS(fptr, par, this->getAvPixScale());
   
    // Close the FITS file.
    status=0;
    fits_close_file(fptr, &status);
    if (status){
      duchampWarning("Cube Reader","Error closing file: ");
      fits_report_error(stderr, status);
    }

    return SUCCESS;
  }

}
