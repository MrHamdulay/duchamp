// -----------------------------------------------------------------------
// CatalogueWriter.cc: Implementation of the base class for writing
//                     results to output files
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

#include <duchamp/Outputs/CatalogueWriter.hh>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Outputs/columns.hh>
#include <duchamp/Utils/Statistics.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp {

  CatalogueWriter::CatalogueWriter()
  {
    itsName="";
    itsOpenFlag=false;
    this->itsParam=0;
    this->itsStats=0;
    this->itsHead=0;
    this->itsObjectList=0;
    this->itsCubeDim=0;
  }

  CatalogueWriter::CatalogueWriter(std::string name):
    itsName(name)
  {
    itsOpenFlag=false;
    this->itsParam=0;
    this->itsStats=0;
    this->itsHead=0;
    this->itsObjectList=0;
    this->itsCubeDim=0;
  }
  
  CatalogueWriter::CatalogueWriter(const CatalogueWriter& other)
  {
    this->operator=(other);
  }

  CatalogueWriter& CatalogueWriter::operator= (const CatalogueWriter& other)
  {
    if(this == &other) return *this;
    this->itsName = other.itsName;
    this->itsOpenFlag = other.itsOpenFlag;
    this->itsColumnSpecification = other.itsColumnSpecification;
    this->itsParam=other.itsParam;
    this->itsStats=other.itsStats;
    this->itsHead=other.itsHead;
    this->itsObjectList=other.itsObjectList;
    this->itsCubeDim=other.itsCubeDim;
    return *this;
  }

  void CatalogueWriter::setup(Cube *cube)
  {
    /// @details Defines the various pointer members to point to the
    /// relevant information from the cube in question.
    this->itsParam = &(cube->pars());
    this->itsStats = &(cube->stats());
    this->itsHead = &(cube->header());
    this->itsObjectList = cube->pObjectList();
    this->itsColumnSpecification = cube->pFullCols();
    this->itsCubeDim = cube->getDimArray();
  }

  void CatalogueWriter::writeEntries()
  {
    if(this->itsOpenFlag){
      for(std::vector<Detection>::iterator obj=this->itsObjectList->begin();obj<this->itsObjectList->end();obj++)
	this->writeEntry(&*obj);
    }
  }


}
