// -----------------------------------------------------------------------
// columns.hh: Define the column class that formats Duchamp output.
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
#ifndef COLUMNS_H
#define COLUMNS_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <math.h>

namespace duchamp
{

  class Detection;
  class FitsHeader;

  /// A namespace controlling the formatting of columns of output for Duchamp.
  namespace Catalogues
  {

    /// @brief An enumeration designed to cover the different output types
    enum DESTINATION {FILE, SCREEN, LOG, VOTABLE};

    /// @brief Enumerated precision categories 
    enum PrecType {prFLUX=3, prVEL=3, prXYZ=1, prPOS=6, prWPOS=2, prSNR=2}; 

    /// @brief Base coordinate system reference for VOTables
    const std::string coordRef="J2000";

    ///  @brief Class to store information about a single column.
    ///
    ///  @details This contains information about a given column --
    ///  its width, the precision associated with it, its title and
    ///  the units.  Plus the usual array of accessor functions and so
    ///  on.

    class Column{
    public:
      Column();          ///< Basic constructor
      Column(std::string type); //< calls the default constructor for the given type
      Column(std::string type, std::string name, std::string units, int width, int prec, std::string ucd="", std::string datatype="", std::string colID="", std::string extraInfo=""); ///< Generic constructor
      Column(const Column& c);///< Assignment constructor
      Column& operator=(const Column& c); ///< Copy constructor
      virtual ~Column(){}; ///< Default destructor;
      //--------------
      // basic accessor functions
      //
      int    getWidth(){return itsWidth;};         
      void   setWidth(int i){itsWidth=i;};    
      int    getPrecision(){return itsPrecision;};     
      void   setPrecision(int i){itsPrecision=i;};
      std::string getName(){return itsName;};          
      void   setName(std::string s){itsName=s;};  
      std::string getUnits(){return itsUnits;};         
      void   setUnits(std::string s){itsUnits=s;}; 
      //? COLNAME getType(){return type;};
      std::string type(){return itsType;};
      void   setType(std::string type){itsType=type;};
      std::string getUCD(){return itsUCD;};
      void        setUCD(std::string s){itsUCD=s;};
      std::string getDatatype(){return itsDatatype;};
      void        setDatatype(std::string s){itsDatatype=s;};
      std::string getColID(){return itsColID;};
      void        setColID(std::string s){itsColID=s;};
      std::string getExtraInfo(){return itsExtraInfo;};
      void        setExtraInfo(std::string s){itsExtraInfo=s;};

      //--------------
      // other functions
      //
      /// @brief Make the column one space wider. 
      void   widen(){itsWidth++;};

      /// @brief Increase the precision by one, widening the column if necessary. 
      void   upPrec(){itsPrecision++; itsWidth++;};

      void   changePrec(int p);

	//-----------
	// managing the width,precision,etc based on a value
	void checkPrec(double d);
	void checkWidth(int w);
	void check(int i)          {int negVal=(i<0)?1:0; checkWidth(int(log10(fabs(double(i)))+1)+negVal);};
	void check(long i)         {int negVal=(i<0)?1:0; checkWidth(int(log10(fabs(double(i)))+1)+negVal);};
	void check(unsigned int i) {checkWidth(int(log10(double(i))+1));};
	void check(unsigned long i){checkWidth(int(log10(double(i))+1));};
	void check(std::string s){checkWidth(int(s.size()));};
	void check(float f, bool doPrec=true) {if(doPrec) checkPrec(double(f)); int negVal=(f<0)?1:0; checkWidth(int(log10(fabs(f))+1)+1+itsPrecision+negVal); };
	void check(double d, bool doPrec=true){if(doPrec) checkPrec(d);         int negVal=(d<0)?1:0; checkWidth(int(log10(fabs(d))+1)+1+itsPrecision+negVal); };

      //--------------
      // Outputting functions -- all in columns.cc
      //
      /// @brief write the title of the column to the stream 
      void   printTitle(std::ostream &stream);

      /// @brief write the units of the column to the stream 
      void   printUnits(std::ostream &stream);

      /// @brief write dashes the full width of the column to the stream 
      void   printDash (std::ostream &stream);

      /// @brief write blanks the full width of the column to the stream 
      void   printBlank(std::ostream &stream);

      /// @brief Print a given value in a column with correct width & precision. 
      template <class T> void printEntry(std::ostream &stream, T value);

      /// @brief Decides whether the column is used for a given table type 
      bool   doCol(DESTINATION dest, bool flagFint=true);


    private:
      std::string itsType;   ///< The type of the column - for reference purposes
      std::string itsName;   ///< The title of the column
      std::string itsUnits;  ///< The units that the values in the column are expressed in.
      int itsWidth;          ///< How wide is the column (in ascii spaces)
      int itsPrecision;      ///< What precision should be used to print the values? (If 0, the setprecision command is not used.)
      std::string itsUCD;    ///< The UCD associated with the value
      std::string itsDatatype; ///< What datatype do the values take?
      std::string itsColID;  ///< The column ID
      std::string itsExtraInfo; ///< Any other info? This can be the 'ref' entry for a VOTable field.
    };

    Column defaultColumn(std::string type);


  }

}

#endif

