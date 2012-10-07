// -----------------------------------------------------------------------
// CatalogueSpecification.cc: Define the specification of a catalogue,
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
#include <duchamp/Outputs/CatalogueSpecification.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/Outputs/columns.hh>
#include <vector>
#include <map>
#include <string>

namespace duchamp {

  namespace Catalogues {

    CatalogueSpecification::CatalogueSpecification(const CatalogueSpecification& other)
    {
      operator=(other);
    }

    CatalogueSpecification& CatalogueSpecification::operator= (const CatalogueSpecification& other)
    {
      if(this == &other) return *this;
      this->itsColumnList = other.itsColumnList;
      this->itsTypeMap = other.itsTypeMap;
      return *this;
    }

    void CatalogueSpecification::addColumn(Column col)
    {
      if( this->itsTypeMap.find(col.type()) != this->itsTypeMap.end() ) // already in list
	this->itsColumnList.erase( this->itsColumnList.begin()+this->itsTypeMap[col.type()] );

      this->itsColumnList.push_back(col);
      this->itsTypeMap[col.type()] = this->itsColumnList.size() - 1;

    }

  }

}
