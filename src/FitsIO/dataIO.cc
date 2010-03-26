// -----------------------------------------------------------------------
// dataIO.cc: Read the data array from a FITS file into a Cube object.
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
#define WCSLIB_GETWCSTAB // define this so that we don't try and redefine 
                         //  wtbarr (this is a problem when using gcc v.4+
#include <fitsio.h>
#include <math.h>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp
{

  OUTCOME Cube::getFITSdata(std::string fname)
  {
    /// This function retrieves the data array from the FITS file at the 
    ///   location given by the string argument.
    ///  Only the two spatial axes and the one spectral axis are stored in the
    ///   data array. These axes are given by the wcsprm variables wcs->lng, 
    ///   wcs->lat and wcs->spec. 
    ///  All other axes are just read by their first pixel, using the 
    ///   fits_read_subsetnull_flt function

    int numAxes, status = 0;  /* MUST initialize status */
    fitsfile *fptr;  

    // Open the FITS file
    status = 0;
    if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
      fits_report_error(stderr, status);
      return FAILURE;
    }

    // Read the size of the FITS file -- number and sizes of the axes
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

    // Identify which axes are the "interesting" ones 
    int lng,lat,spc;
    if(this->head.isWCS() && (this->head.WCS().spec>=0)){
      lng = this->head.WCS().lng;
      lat = this->head.WCS().lat;
      spc = this->head.WCS().spec;
    }
    else{
      lng = 0;
      lat = 1;
      spc = 2;
    }
    
    int anynul;
    int npix = dimAxes[lng];
    if(numAxes>1) npix *= dimAxes[lat];
    if((spc>=0)&&(numAxes>spc)) npix *= dimAxes[spc];

    float *pixarray = new float[npix];// the array of pixel values
    long *inc = new long[numAxes];    // the data-sampling increment

    // define the first and last pixels for each axis.
    // set both to 1 for the axes we don't want, and to the full
    //  range for the ones we do.
    long *fpixel = new long[numAxes];
    long *lpixel = new long[numAxes];
    for(int i=0;i<numAxes;i++){
      inc[i] = fpixel[i] = lpixel[i] = 1;
    }
    lpixel[lng] = dimAxes[lng];
    if(numAxes>1) lpixel[lat] = dimAxes[lat];
    if((spc>=0)&&(numAxes>spc)) lpixel[spc] = dimAxes[spc];

    int colnum = 0;  // want the first dataset in the FITS file

    // read the relevant subset, defined by the first & last pixel ranges
    status = 0;
    if(fits_read_subset_flt(fptr, colnum, numAxes, dimAxes,
			    fpixel, lpixel, inc, 
			    this->par.getBlankPixVal(), pixarray, &anynul, &status)){
      duchampError("Cube Reader",
		   "There was an error reading in the data array:");
      fits_report_error(stderr, status);
      return FAILURE;
    }

    delete [] fpixel;
    delete [] lpixel;
    delete [] inc;

    if(anynul==0){    
      // no blank pixels, so don't bother with any trimming or checking...
      if(this->par.getFlagTrim()) {  
	// if user requested fixing, inform them of change.
	duchampWarning("Cube Reader",
		       "No blank pixels, so setting flagTrim to false.\n");
      }
      this->par.setFlagBlankPix(false); 
      this->par.setFlagTrim(false);
    }

    if(this->initialiseCube(dimAxes) == FAILURE) return FAILURE;
    this->saveArray(pixarray,npix);
    delete [] pixarray;
    delete [] dimAxes;

    // Close the FITS file -- not needed any more in this function.
    status = 0;
    fits_close_file(fptr, &status);
    if (status){
      duchampWarning("Cube Reader","Error closing file: ");
      fits_report_error(stderr, status);
    }

    return SUCCESS;

  }

}
