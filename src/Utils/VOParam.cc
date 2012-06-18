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
  
  template <class T> VOParam::VOParam(std::string n, std::string U, std::string d, T v, int w, std::string u):
    name(n),UCD(U),datatype(d),width(w),units(u)
  {
    /// @details
    /// A basic definition function, defining each parameter
    /// individually. The value (v) is written to a stringstream, and
    /// from there stored as a string.
    /// \param n The name
    /// \param U The UCD
    /// \param d The datatype
    /// \param v The value
    /// \param w The width
    /// \param u The units

    // this->name = n;
    // this->UCD = U;
    // this->datatype = d;
    // this->width = w;
    std::stringstream ss;
    ss << v;
    this->value = ss.str();
    // this->units = u;
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
    this->name = other.name;
    this->UCD = other.UCD;
    this->datatype=other.datatype;
    this->width = other.width;
    this->value = other.value;
    this->units = other.units;
    return *this;
  }
  

  void VOParam::printParam(std::ostream &stream)
  {
    /// @details
    /// Print the Param entry with appropriate formatting.
    /// \param stream The output stream to send the text to.

    stream << "<PARAM name=\"" <<this->name
	   << "\" ucd=\"" << this->UCD
	   << "\" datatype=\"" << this->datatype;
    if(this->units!="")
      stream << "\" units=\"" << this->units;
    if(this->width!=0){
      if(datatype=="char")
	stream << "\" arraysize=\"" << this->width;
      else
	stream << "\" width=\"" << this->width;
    }
    stream << "\" value=\"" << this->value
	   << "\"/>\n";
  }

  //------------------------------------------------



}
