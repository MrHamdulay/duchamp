// -----------------------------------------------------------------------
// VOParam.hh: Specification of a parameter for output to a VOTable
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

#ifndef VOPARAM_HH
#define VOPARAM_HH

#include <iostream>
#include <string>

namespace duchamp
{

  ///  @brief A class that holds information for a <PARAM > entry in a
  ///  VOTable. @details It also provides useful functions to store
  ///  and print that information.
  class VOParam
  {
  public:
    VOParam();
    template <class T> VOParam(std::string name, std::string UCD, std::string datatype, T value, int width, std::string units);
    VOParam(const VOParam& other);
    VOParam& operator= (const VOParam& other);
    virtual ~VOParam(){};
 
    /// @brief Print the PARAM entry 
    void printParam(std::ostream &stream);
    
  private:
    std::string name;
    std::string UCD;
    std::string datatype;
    std::string value;
    int width;
    std::string units;
    
  };

}

#endif //VOPARAM_HH
