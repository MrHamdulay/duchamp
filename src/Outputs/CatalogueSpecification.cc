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
#include <duchamp/Outputs/CatalogueSpecification.hh>
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
      this->itsCommentString = other.itsCommentString;
      return *this;
    }

    void CatalogueSpecification::addColumn(Column col)
    {
      this->removeColumn(col.type());
      this->itsColumnList.push_back(col);
      // this->itsTypeMap[col.type()] = this->itsColumnList.size() - 1;
      this->setMap();
    }

      void CatalogueSpecification::setMap()
      {
	  for(size_t i=0;i<this->itsColumnList.size();i++)
	      this->itsTypeMap[this->itsColumnList[i].type()] = i;
      }

    void CatalogueSpecification::removeColumn(std::string type)
    {
	if( this->itsTypeMap.find(type) != this->itsTypeMap.end() ) // already in list
	    this->itsColumnList.erase( this->itsColumnList.begin()+this->itsTypeMap[type] );
	this->setMap();
    }

      bool CatalogueSpecification::hasColumn(std::string type)
      {
	  return this->itsTypeMap.find(type) != this->itsTypeMap.end();
      }

    void CatalogueSpecification::outputTableHeader(std::ostream &stream, Catalogues::DESTINATION tableType, bool flagWCS)
    {
      /// @details
      ///  Prints the header row for a table of detections. The columns
      ///  that are included depend on the value of tableType, according
      ///  to the Column::doCol() function. The format is a row of
      ///  dashes, a row with column names, a row with column units, and
      ///  another row of dashes.
      /// \param stream Where the output is written
      /// \param columns The vector list of Column objects
      /// \param tableType A string saying what format to use: one of
      /// "file", "log", "screen" or "votable" (although the latter
      /// shouldn't be used with this function).
      /// \param flagWCS A flag for use with Column::doCol(), specifying
      /// whether to use FINT or FTOT.

      stream << this->itsCommentString;
      for(size_t i=0;i<this->itsColumnList.size();i++)
	if(this->itsColumnList[i].doCol(tableType,flagWCS)) this->itsColumnList[i].printDash(stream);
      stream << "\n"<<this->itsCommentString;
      for(size_t i=0;i<this->itsColumnList.size();i++)
	if(this->itsColumnList[i].doCol(tableType,flagWCS)) this->itsColumnList[i].printTitle(stream);
      stream << "\n"<<this->itsCommentString;
      for(size_t i=0;i<this->itsColumnList.size();i++)
	if(this->itsColumnList[i].doCol(tableType,flagWCS)) this->itsColumnList[i].printUnits(stream);
      stream << "\n"<<this->itsCommentString;
      for(size_t i=0;i<this->itsColumnList.size();i++)
	if(this->itsColumnList[i].doCol(tableType,flagWCS)) this->itsColumnList[i].printDash(stream);
      stream << "\n";

      for(size_t i=0;i<this->itsCommentString.size();i++) this->itsColumnList[0].widen();
    }


  }

}
