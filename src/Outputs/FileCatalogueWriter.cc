// -----------------------------------------------------------------------
// FileCatalogueWriter.cc: Writing output catalogues to disk files
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
#include <duchamp/Outputs/FileCatalogueWriter.hh>
#include <duchamp/Outputs/CatalogueWriter.hh>
#include <ios>
#include <iostream>
#include <fstream>
namespace duchamp {

  FileCatalogueWriter::FileCatalogueWriter():
    CatalogueWriter()
  {
  }

  FileCatalogueWriter::FileCatalogueWriter(std::string name):
    CatalogueWriter(name)
  {
  }

  FileCatalogueWriter::FileCatalogueWriter(const FileCatalogueWriter& other)
  {
    this->operator=(other);
  }

  FileCatalogueWriter& FileCatalogueWriter::operator= (const FileCatalogueWriter& other)
  {
    if(this==&other) return *this;
    ((CatalogueWriter &) *this) = other;
    this->itsOpenFlag=false;
    return *this;
  }
  
  FileCatalogueWriter::~FileCatalogueWriter()
  {
    if(this->itsOpenFlag) this->closeCatalogue();
  }

  bool FileCatalogueWriter::openCatalogue(std::ios_base::openmode mode)
  {
    if(this->itsName == ""){
      DUCHAMPERROR("FileCatalogueWriter","No catalogue name provided");
      this->itsOpenFlag = false;
    }
    else {
      this->itsFileStream.open(this->itsName.c_str(),mode);
      this->itsOpenFlag = !this->itsFileStream.fail();
    }
    if(!this->itsOpenFlag) 
      DUCHAMPERROR("FileCatalogueWriter","Could not open file \""<<this->itsName<<"\"");
    return this->itsOpenFlag;

  }



  bool FileCatalogueWriter::closeCatalogue()
  {
    this->itsFileStream.close();
    if(!this->itsFileStream.fail()) this->itsOpenFlag = false;
    return !this->itsFileStream.fail();
  }


}
