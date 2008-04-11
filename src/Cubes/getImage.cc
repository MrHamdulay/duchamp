// -----------------------------------------------------------------------
// getImage.cc: Read in a FITS file to a Cube object.
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
#include <wcslib/wcs.h>
#include <wcslib/wcshdr.h>
#include <wcslib/fitshdr.h>
#include <wcslib/wcsfix.h>
#include <wcslib/wcsunits.h>
#define WCSLIB_GETWCSTAB // define this so that we don't try and redefine 
                         //  wtbarr (this is a problem when using gcc v.4+)
#include <fitsio.h>
#include <math.h>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp
{

  std::string imageType[4] = {"point", "spectrum", "image", "cube"};

  int Cube::getMetadata(){  
    /** 
     * A front-end to the Cube::getMetadata() function, that does 
     *  subsection checks.
     * Assumes the Param is set up properly.
     */
    std::string fname = par.getImageFile();
    if(par.getFlagSubsection()) fname+=par.getSubsection();
    return getMetadata(fname);
  }
  //--------------------------------------------------------------------

  int Cube::getMetadata(std::string fname)
  {
    /**
     * Read in the metadata associated with the cube from the file
     *  fname (assumed to be in FITS format).  Function is a front end
     *  to the I/O functions in the FitsIO/ directory.  This function
     *  will check that the file exists, report the dimensions and
     *  then get other functions to read the WCS and necessary header
     *  keywords. This function does not read in the data array --
     *  that is done by Cube::getCube().
     *  \param fname A std::string with name of FITS file.
     *  \return SUCCESS or FAILURE.
     */

    int numAxes, status = 0;  /* MUST initialize status */
    fitsfile *fptr;         

    int exists;
    fits_file_exists(fname.c_str(),&exists,&status);
    if(exists<=0){
      fits_report_error(stderr, status);
      std::stringstream errmsg;
      errmsg << "Requested image (" << fname << ") does not exist!\n";
      duchampError("Cube Reader", errmsg.str());
      return FAILURE;
    }

    // Open the FITS file -- make sure it exists
    status = 0;
    if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
      fits_report_error(stderr, status);
      if(((status==URL_PARSE_ERROR)||(status==BAD_NAXIS))
	 &&(this->pars().getFlagSubsection()))
	duchampError("Cube Reader",
		     "It may be that the subsection you've entered is invalid.\n\
Either it has the wrong number of axes, or one axis has too large a range.\n");
      return FAILURE;
    }

    // Read the size information -- number and lengths of all axes.
    // Just use this for reporting purposes.
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

    // Report the size of the FITS file
    if(this->par.isVerbose()){
      std::cout << "Dimensions of FITS file: ";
      int dim = 0;
      std::cout << dimAxes[dim];
      while(dim+1<numAxes) std::cout << "x" << dimAxes[++dim];
      std::cout << std::endl;
    }

    delete [] dimAxes;

    // Get the WCS information
    this->head.defineWCS(fname, this->par);

    if(!this->head.isWCS()) 
      duchampWarning("Cube Reader","WCS is not good enough to be used.\n");

    // Read the necessary header information, and copy some of it into the Param.
    this->head.readHeaderInfo(fname, this->par);

    return SUCCESS;

  }
  //--------------------------------------------------------------------


  int Cube::getCube(std::string fname)
  {
    /**
     * Read in a cube from the file fname (assumed to be in FITS
     *  format).  Function is a front end to the data I/O function in
     *  the FitsIO/ directory.  This function calls the getMetadata()
     *  function, then reads in the actual data array.
     *  \param fname A std::string with name of FITS file.
     *  \return SUCCESS or FAILURE.
     */

    this->getMetadata(fname);

    // Get the data array from the FITS file.
    // Report the dimensions of the data array that was read (this can be
    //   different to the full FITS array).
    if(this->par.isVerbose()) std::cout << "Reading data ... "<<std::flush;
    if(this->getFITSdata(fname)) return FAILURE;
    if(this->par.isVerbose()){
      std::cout << "Done. Data array has dimensions: ";
      std::cout << this->axisDim[0];
      if(this->axisDim[1]>1) std::cout  <<"x"<< this->axisDim[1];
      if(this->axisDim[2]>1) std::cout  <<"x"<< this->axisDim[2];
      std::cout << "\n";
    }   

    if(this->axisDim[2] == 1){
      this->par.setMinChannels(0);
    }

    // Convert the flux Units if the user has so requested
    this->convertFluxUnits();

    return SUCCESS;    

  }
  //--------------------------------------------------------------------


  void Cube::convertFluxUnits()
  {
    /**
     *  If the user has requested new flux units (via the input
     *  parameter newFluxUnits), this function converts the units
     *  given by BUNIT to those given by newFluxUnits. If no simple
     *  conversion can be found (by using wcs***) then nothing is done
     *  and the user is informed, otherwise the FitsHeader::fluxUnits
     *  parameter is updated and the pixel array is converted
     *  accordingly.
     *
     */


    if(this->par.getNewFluxUnits()!=""){

      std::string oldUnit = this->head.getFluxUnits();
      std::string newUnit = this->par.getNewFluxUnits();

      if(this->par.isVerbose()){
	std::cout << "Converting flux units from " << oldUnit << " to " << newUnit << "... ";
	std::cout << std::flush;
      }

      double scale,offset,power;
      int status = wcsunits(oldUnit.c_str(), newUnit.c_str(), &scale, &offset, &power);

      if(status==0){
	
	this->head.setFluxUnits( newUnit );
	this->head.setIntFluxUnits();

	for(int i=0;i<this->numPixels;i++)
	  if(!this->isBlank(i)) this->array[i] = pow(scale * array[i] + offset, power);
	if(this->par.isVerbose()) {
	  std::cout << " Done.\n";
	}
      }
      else{
	std::stringstream ss;
	ss << "Could not convert units from " << oldUnit << " to " << newUnit
	   << ".\nLeaving BUNIT as " << oldUnit << "\n";
	if(this->par.isVerbose()) std::cout << "\n";
	duchampWarning("convertFluxUnits", ss.str());
      }
    }

  }


}
