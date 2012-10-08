// -----------------------------------------------------------------------
// FileCatalogueWriter.hh: Writing output catalogues to disk files
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
#ifndef DUCHAMP_FILE_CATALOGUE_WRITER_H_
#define DUCHAMP_FILE_CATALOGUE_WRITER_H_

#include <duchamp/Outputs/FileCatalogueWriter.hh>
#include <duchamp/Outputs/CatalogueWriter.hh>
#include <ios>
#include <iostream>
#include <fstream>

namespace duchamp {

  class FileCatalogueWriter : public CatalogueWriter
  {
  public:
    FileCatalogueWriter();
    FileCatalogueWriter(std::string name);
    FileCatalogueWriter(const FileCatalogueWriter& other);
    FileCatalogueWriter& operator= (const FileCatalogueWriter& other);
    virtual ~FileCatalogueWriter();
    
    /// @brief open the catalogue file for writing
    bool openCatalogue(std::ios_base::openmode mode = std::ios_base::out );

    /// @brief close the catalogue file
    bool closeCatalogue();

  protected:
    std::ofstream itsFileStream;

  };

}

#endif

