// -----------------------------------------------------------------------
// CatalogueSpecification.hh: Define the specification of a catalogue,
//                            being a set of Columns
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
#ifndef DUCHAMP_CATALOGUE_SPEC_H
#define DUCHAMP_CATALOGUE_SPEC_H

#include <duchamp/duchamp.hh>
#include <duchamp/Outputs/columns.hh>
#include <vector>
#include <map>

namespace duchamp {

  namespace Catalogues {

    class CatalogueSpecification 
    {
    public:
      CatalogueSpecification(){};
      CatalogueSpecification(const CatalogueSpecification& other);
      CatalogueSpecification& operator= (const CatalogueSpecification& other);
      virtual ~CatalogueSpecification(){};

      void addColumn(Column col);
      Column column(std::string type);
      size_t size(){return itsColumnList.size();};
      

    protected:
      std::vector<Column> itsColumnList;
      std::map<std::string, int> itsTypeMap;
    };


  }

}
#endif
