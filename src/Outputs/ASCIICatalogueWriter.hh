// -----------------------------------------------------------------------
// ASCIICatalogueWriter.hh: Writing output catalogues to basic ASCII-format
//                          files (or the screen).
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
#ifndef DUCHAMP_ASCII_CATALOGUE_WRITER_H_
#define DUCHAMP_ASCII_CATALOGUE_WRITER_H_

#include <duchamp/Outputs/CatalogueWriter.hh>
#include <duchamp/Outputs/FileCatalogueWriter.hh>
#include <duchamp/Outputs/columns.hh>
#include <duchamp/Detection/detection.hh>
#include <ios>
#include <iostream>
#include <fstream>

namespace duchamp {

  class ASCIICatalogueWriter : public FileCatalogueWriter
  {
  public:
    ASCIICatalogueWriter();
    ASCIICatalogueWriter(std::string name);
    ASCIICatalogueWriter(Catalogues::DESTINATION dest);
    ASCIICatalogueWriter(std::string name, Catalogues::DESTINATION dest);
    ASCIICatalogueWriter(const ASCIICatalogueWriter& other);
    ASCIICatalogueWriter& operator= (const ASCIICatalogueWriter& other);
    virtual ~ASCIICatalogueWriter(){};
    
    /// @brief open the catalogue for writing
    bool openCatalogue(std::ios_base::openmode mode = std::ios_base::out );

    /// @brief Write header information - not including parameters
    void writeHeader();
    void writeCommandLineEntry(int argc, char *argv[]);

    void writeParameters();
    void writeStats();
    void writeTableHeader();
    void writeEntry(Detection *object);
    void writeCubeSummary();
    void writeFooter(){};

    bool closeCatalogue();


  protected:
    std::ostream *itsStream;
    Catalogues::DESTINATION itsDestination;
     

  };


}


#endif
