// -----------------------------------------------------------------------
// VOParam.cc: Output of the detected objects to a VOTable
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
#include <duchamp/Utils/VOParam.hh>

namespace duchamp {

  VOParam::VOParam()
  {
  }
  
  template <class T> VOParam::VOParam(std::string name, std::string UCD, std::string datatype, T value, int width, std::string units):
    itsName(name),itsUCD(UCD),itsDatatype(datatype),itsWidth(width),itsUnits(units)
  {
    /// @details
    /// A basic definition function, defining each parameter
    /// individually. The value (v) is written to a stringstream, and
    /// from there stored as a string.
    /// \param name The name
    /// \param UCD The UCD
    /// \param datatype The datatype
    /// \param value The value
    /// \param width The width
    /// \param units The units

    std::stringstream ss;
    ss << value;
    this->itsValue = ss.str();
  }
  template VOParam::VOParam(std::string n, std::string U, std::string d, bool v, int w, std::string u);
  template VOParam::VOParam(std::string n, std::string U, std::string d, int v, int w, std::string u);
  template VOParam::VOParam(std::string n, std::string U, std::string d, unsigned int v, int w, std::string u);
  template VOParam::VOParam(std::string n, std::string U, std::string d, long v, int w, std::string u);
  template VOParam::VOParam(std::string n, std::string U, std::string d, float v, int w, std::string u);
  template VOParam::VOParam(std::string n, std::string U, std::string d, double v, int w, std::string u);
  template VOParam::VOParam(std::string n, std::string U, std::string d, std::string v, int w, std::string u);

  VOParam::VOParam(const VOParam& other)
  {
    operator=(other);
  }

  VOParam& VOParam::operator= (const VOParam& other)
  {
    if(this==&other) return *this;
    this->itsName = other.itsName;
    this->itsUCD = other.itsUCD;
    this->itsDatatype=other.itsDatatype;
    this->itsWidth = other.itsWidth;
    this->itsValue = other.itsValue;
    this->itsUnits = other.itsUnits;
    return *this;
  }
  

  void VOParam::printParam(std::ostream &stream)
  {
    /// @details
    /// Print the Param entry with appropriate formatting.
    /// \param stream The output stream to send the text to.

    stream << "<PARAM name=\"" <<this->itsName
	   << "\" ucd=\"" << this->itsUCD
	   << "\" datatype=\"" << this->itsDatatype;
    if(this->itsUnits!="")
      stream << "\" units=\"" << this->itsUnits;
    if(this->itsWidth!=0){
      if(this->itsDatatype=="char")
	stream << "\" arraysize=\"" << this->itsWidth;
      else
	stream << "\" width=\"" << this->itsWidth;
    }
    stream << "\" value=\"" << this->itsValue
	   << "\"/>\n";
  }

  //------------------------------------------------



}
