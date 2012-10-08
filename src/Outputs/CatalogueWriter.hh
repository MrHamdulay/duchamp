// -----------------------------------------------------------------------
// CatalogueWriter.hh: Base class for writing results to output files
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
#ifndef DUCHAMP_CATALOGUE_WRITER_H_
#define DUCHAMP_CATALOGUE_WRITER_H_

#include <ios>
#include <string>
#include <vector>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Outputs/columns.hh>
#include <duchamp/Utils/Statistics.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp {

  /// @brief Base class for writing out catalogues.  
  /// @details This class forms the basis for derived classes that
  /// handle writing of the catalogue to different types of files. The
  /// class makes no assumption about the type of output (eg. file or
  /// stream etc), but it does hold pointers to all the metadata from
  /// the cube, as well as to the object list. Implementations of the
  /// individual writing methods is left up to the base class.
  class CatalogueWriter
  {
  public:
    CatalogueWriter();
    CatalogueWriter(std::string name);
    CatalogueWriter(const CatalogueWriter& other);
    CatalogueWriter& operator= (const CatalogueWriter& other);
    virtual ~CatalogueWriter(){};
    
    /// @brief open the catalogue for writing
    virtual bool openCatalogue(std::ios_base::openmode mode = std::ios_base::out )=0;

    /// @brief Write header information - not including parameters
    virtual void writeHeader() = 0;

    /// @brief Write the input parameters
    virtual void writeParameters() = 0;

    /// @brief Write out the entire catalogue
    virtual void writeEntries() = 0;

    /// @brief Write a single catalogue entry based on a detected object
    virtual void writeEntry(Detection *object) = 0;
    
    /// @brief Write footer information -- stuff to come after the catalogue table
    virtual void writeFooter() = 0;

    /// @brief Close the catalogue (file/stream/whatever)
    virtual bool closeCatalogue() = 0;

    void setName(std::string s){itsName=s;};
    std::string name(){return itsName;};
    bool isOpen(){return itsOpenFlag;};
    Catalogues::CatalogueSpecification columnSpec(){return *itsColumnSpecification;};
    void setColumnSpec(Catalogues::CatalogueSpecification *cols){itsColumnSpecification = cols;};
    void setParam(Param *par){itsParam=par;};
    void setStats(Statistics::StatsContainer<float> *stats){itsStats=stats;};
    void setHead(FitsHeader *head){itsHead=head;};
    void setObjectList(std::vector<Detection> *objlist){itsObjectList=objlist;};

    /// @brief Set up the pointers to point to the cube's metadata
    void setup(Cube *cube);

  protected:
    /// @brief The name of the output file, if necessary
    std::string itsName;
    /// @brief Is the output open and suitable for writing
    bool        itsOpenFlag;
    /// @brief A pointer to the list of column specifications
    Catalogues::CatalogueSpecification *itsColumnSpecification;
    /// @brief A pointer to the set of input parameters
    Param *itsParam;
    /// @brief A pointer to the statistics used by the cube
    Statistics::StatsContainer<float> *itsStats;
    /// @brief A pointer to the fits header description
    FitsHeader *itsHead;
    /// @brief A pointer to the list of detected objects
    std::vector<Detection> *itsObjectList;
    /// @brief The cube dimensions
    size_t *itsCubeDim;

  };

}


#endif
