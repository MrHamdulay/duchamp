// -----------------------------------------------------------------------
// VOField.cc: Specification of a field for output to a VOTable
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
#include <sstream>
#include <iomanip>
#include <string>
#include <duchamp/Utils/VOField.hh>
#include <duchamp/Outputs/columns.hh>

namespace duchamp
{


  std::string fixUnitsVOT(std::string oldstring)
  {
    ///  @details
    /// Fix a string containing units to make it acceptable for a VOTable.
    /// 
    /// This function makes the provided units string acceptable
    /// according to the standard found at
    /// http://vizier.u-strasbg.fr/doc/catstd-3.2.htx 
    /// This should then be able to convert the units used in the text
    /// table to units suitable for putting in a VOTable.
    /// 
    /// Specifically, it removes any square brackets [] from the
    /// start/end of the string, and replaces blank spaces (representing
    /// multiplication) with a '.' (full stop).
    /// 
    /// \param oldstring Input unit string to be fixed
    /// \return String with fixed units

    std::string newstring;
    for(size_t i=0;i<oldstring.size();i++){
      if((oldstring[i]!='[')&&(oldstring[i]!=']')){
	if(oldstring[i]==' ') newstring += '.';
	else newstring += oldstring[i];
      }
    }
    return newstring;  
  }

  VOField::VOField()
  {
  }

  VOField::VOField(const VOField& other)
  {
    operator=(other);
  }

  VOField& VOField::operator= (const VOField& other)
  {
    if(this==&other) return *this;
    this->itsID = other.itsID;
    this->itsName = other.itsName;
    this->itsUCD = other.itsUCD;
    this->itsUnits = other.itsUnits;
    this->itsDatatype=other.itsDatatype;
    this->itsRef = other.itsRef;
    this->itsWidth = other.itsWidth;
    this->itsPrecision = other.itsPrecision;
    return *this;
  }


  void VOField::define(std::string id, std::string name, std::string ucd, std::string units, std::string datatype, std::string ref, int width, int prec)
  {
    /// @details
    /// A basic definition function, defining each parameter individually. 
    /// \param i The ID parameter
    /// \param n The name parameter
    /// \param U The UCD
    /// \param u The units (fixed by fixUnits())
    /// \param d The datatype
    /// \param r The ref
    /// \param w The width
    /// \param p The precision

    this->itsID = id;
    this->itsName = name;
    this->itsUCD = ucd;
    this->itsUnits = fixUnitsVOT(units);
    this->itsDatatype = datatype;
    this->itsRef = ref;
    this->itsWidth = width;
    this->itsPrecision = prec;
  }

  void VOField::define(Catalogues::Column column, std::string id, std::string ucd, std::string datatype, std::string ref)
  {
    /// @details
    /// Another definition function, using the information in a Catalogues::Column object for some parameters.
    /// \param column A Catalogues::Column object, used for name, units, width & precision
    /// \param i The ID parameter
    /// \param U The UCD
    /// \param d The datatype
    /// \param r The ref

    this->itsID = id;
    this->itsName = column.getName();
    this->itsUCD = ucd;
    this->itsUnits = fixUnitsVOT(column.getUnits());
    this->itsDatatype = datatype;
    this->itsRef = ref;
    this->itsWidth = column.getWidth();
    this->itsPrecision = column.getPrecision();
  }

  void VOField::define(Catalogues::Column column)
  {
    /// @details
    /// Another definition function, using the information in a Catalogues::Column object for some parameters.
    /// \param column A Catalogues::Column object

    this->itsID = column.getColID();
    this->itsName = column.getName();
    this->itsUCD = column.getUCD();
    this->itsUnits = fixUnitsVOT(column.getUnits());
    this->itsDatatype = column.getDatatype();
    this->itsRef = column.getExtraInfo();
    this->itsWidth = column.getWidth();
    this->itsPrecision = column.getPrecision();
  }

  VOField::VOField(Catalogues::Column column)
  {
    /// @details
    /// A more useful definition function, using the Column::COLNAME
    /// key to specify particular values for each of the parameters for
    /// different columns.
    /// \param column A Catalogues::Column object of a particular type. The
    /// column.getType() function is used to decide which call to
    /// VOField::define(Catalogues::Column column, std::string i, std::string
    /// U, std::string d, std::string r) to use

    this->define(column);

    // Adjust some of the names for clarity
    if(column.type()=="FINT") this->itsName = "Integrated_Flux";
    else if(column.type()=="FINTERR") this->itsName = "Integrated_Flux_Error";
    else if(column.type()=="FTOT") this->itsName = "Total_Flux";
    else if(column.type()=="FINT") this->itsName = "Total_Flux_Error";
    else if(column.type()=="FPEAK") this->itsName = "Peak_Flux";
    else if(column.type()=="XCENT") this->itsName = "X_Centroid";
    else if(column.type()=="YCENT") this->itsName = "Y_Centroid";
    else if(column.type()=="ZCENT") this->itsName = "Z_Centroid";

  }

  void VOField::printField(std::ostream &stream)
  {
    /// @details
    /// Print the Field entry with appropriate formatting.
    /// \param stream The output stream to send the text to.

    stream << "<FIELD name=\"" <<this->itsName
	   << "\" ID=\"" << this->itsID
	   << "\" ucd=\"" << this->itsUCD;
    if(this->itsRef!="") stream << "\" ref=\"" << this->itsRef;
    stream << "\" datatype=\"" << this->itsDatatype;
    stream << "\" unit=\"" << this->itsUnits;
    if(this->itsDatatype=="char")
      stream << "\" arraysize=\"" << this->itsWidth;
    else{
      stream << "\" width=\"" << this->itsWidth;
      if(this->itsDatatype=="float" || this->itsDatatype=="double")
	stream << "\" precision=\"" << this->itsPrecision;
    }
    stream << "\"/>\n";

  }




}
