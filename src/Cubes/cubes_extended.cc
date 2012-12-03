// -----------------------------------------------------------------------
// cubes_extended.cc: Extra member functions for the Cube class, that
//                    depend on extra classes.
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
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <string>
#include <math.h>

#include <wcslib/wcs.h>

#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/FitsIO/ReadExistingRecon.hh>
#include <duchamp/FitsIO/ReadExistingSmooth.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Outputs/columns.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/mycpgplot.hh>
#include <duchamp/Utils/Statistics.hh>
using namespace mycpgplot;
using namespace Statistics;

namespace duchamp
{

  void Cube::sortDetections()
  {
    ///  @details
    ///  A front end to the sort-by functions.
    ///  If there is a good WCS, the detection list is sorted by the requested parameter.
    ///  Otherwise, it is sorted by increasing z-pixel value.
    ///  The ID numbers are then re-calculated.
  
//     if(this->head.isWCS()) SortByVel(*this->objectList);
//     else SortByZ(*this->objectList);
    if(!this->head.isWCS()){
      if(this->par.getSortingParam()=="ra"){
	DUCHAMPWARN("sortDetections","No good WCS, so sorting by xvalue");
	SortDetections(*this->objectList, "xvalue");
      }
      else if(this->par.getSortingParam()=="dec"){
	DUCHAMPWARN("sortDetections","No good WCS, so sorting by yvalue");
	SortDetections(*this->objectList, "yvalue");
      }
      else if(this->par.getSortingParam()=="vel" || this->par.getSortingParam()=="w50"){
	DUCHAMPWARN("sortDetections","No good WCS, so sorting by zvalue");
	SortDetections(*this->objectList, "zvalue");
      }
      else {
	SortDetections(*this->objectList, this->par.getSortingParam());
      }
    }
    else SortDetections(*this->objectList, this->par.getSortingParam());
    for(size_t i=0; i<this->objectList->size();i++) 
      this->objectList->at(i).setID(i+1);

  }
  //--------------------------------------------------------------------

  void Cube::readSavedArrays()
  {
    /// @details
    ///  This function reads in reconstructed and/or smoothed arrays that have 
    ///   been saved on disk in FITS files. 
    ///  To do this it calls the functions Cube::readReconCube() and 
    ///   Cube::readSmoothCube().
    ///  The Param set is consulted to determine which of these arrays are needed.

    // If the reconstructed array is to be read in from disk
    if( this->par.getFlagReconExists() && this->par.getFlagATrous() ){
      std::cout << "Reading reconstructed array: "<<std::endl;
      // if( this->readReconCube() == FAILURE){
      ReadExistingRecon reconReader(this);
      if( reconReader.read() == FAILURE){
	DUCHAMPWARN("Duchamp", "Could not read in existing reconstructed array. Will perform reconstruction using assigned parameters.");
	this->par.setFlagReconExists(false);
      }
      else std::cout << "Reconstructed array available.\n";
    }

    if( this->par.getFlagSmoothExists() && this->par.getFlagSmooth() ){
      std::cout << "Reading smoothed array: "<<std::endl;
      ReadExistingSmooth smoothReader(this);
      if( smoothReader.read() == FAILURE){
	//      if( this->readSmoothCube() == FAILURE){
	DUCHAMPWARN("Duchamp", "Could not read in existing smoothed array. Will smooth the cube using assigned parameters.");
	this->par.setFlagSmoothExists(false);
      }
      else std::cout << "Smoothed array available.\n";
    }
    
  }

}
