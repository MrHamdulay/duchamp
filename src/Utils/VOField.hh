// -----------------------------------------------------------------------
// VOField.hh: Specification of a field for output to a VOTable
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

#ifndef VOFIELD_HH
#define VOFIELD_HH

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <time.h>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Cubes/cubes.hh> 
#include <duchamp/Detection/detection.hh>
#include <duchamp/Outputs/columns.hh>
 

namespace duchamp
{

  ///  @brief A class that holds information for a <FIELD > entry in a
  ///  VOTable. 
  ///  @details It also provides useful functions to store and print
  ///  that information.

  class VOField
  {
  public:
    VOField();
    /// @brief Define a FIELD for a particular Catalogues::Column object  
    VOField(Catalogues::Column);
    VOField(const VOField& other);
    VOField& operator= (const VOField& other);
    virtual ~VOField(){};

    /// @brief Define a FIELD by individual parameters 
    void define(std::string i, std::string n, std::string U, std::string u, std::string d, std::string r, int w, int p);
    /// @brief Define a FIELD by individual parameters, using a Catalogues::Column object 
    void define(Catalogues::Column column, std::string i, std::string U, std::string d, std::string r);
    /// @brief Define a FIELD by individual parameters, using only a Catalogues::Column object 
    void define(Catalogues::Column column);

    void setID(std::string s){itsID=s;};
    void setName(std::string s){itsName=s;};
    void setUCD(std::string s){itsUCD=s;};
    void setUnits(std::string s){itsUnits=s;};
    void setDatatype(std::string s){itsDatatype=s;};
    void setRef(std::string s){itsRef=s;};
    void setWidth(int i){itsWidth=i;};
    void setPrecision(int i){itsPrecision=i;};

    std::string ID(){return itsID;};
    std::string name(){return itsName;};
    std::string UCD(){return itsUCD;};
    std::string units(){return itsUnits;};
    std::string datatype(){return itsDatatype;};
    std::string ref(){return itsRef;};
    int width(){return itsWidth;};
    int precision(){return itsPrecision;};

    /// @brief Print the FIELD entry 
    void printField(std::ostream &stream);
    
  private:
    std::string itsID;
    std::string itsName;
    std::string itsUCD;
    std::string itsUnits;
    std::string itsDatatype;
    std::string itsRef;
    int itsWidth;
    int itsPrecision;

  };

}

#endif //VOFIELD_HH
