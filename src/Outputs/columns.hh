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

namespace duchamp
{

  class Detection;
  class FitsHeader;

  /// A namespace controlling the formatting of columns of output for Duchamp.
  namespace Catalogues
  {

    /// @brief An enumeration designed to cover the different output types
    enum DESTINATION {FILE, SCREEN, LOG, VOTABLE};

    // First, define some basic quantities.
    // The default values for the columns, and default values for
    //  the precision of different types of columns.

    /// @brief Total number of columns being considered.
    const int numColumns=38;    
    /// @brief Enumerated column titles 
    enum COLNAME {NUM=0, NAME, X, Y, Z,
		  RA, DEC, RAJD, DECJD, VEL, 
		  WRA, WDEC, W50, W20, WVEL,
		  FINT, FTOT, FPEAK, SNRPEAK,
		  X1, X2, Y1, Y2, Z1, Z2, NPIX, FLAG,
		  XAV, YAV, ZAV, XCENT, YCENT, ZCENT, 
		  XPEAK, YPEAK, ZPEAK,
		  NUMCH, SPATSIZE,
		  UNKNOWN};  // the last is for unclassified columns

    /// @brief Guide for which columns are used for the results file 
    const bool isFile[numColumns+1]=
      {true,true,true,true,true,
       true,true,false,false,true,
       true,true,true,true,true,
       true,true,true,true,
       true,true,true,true,true,true,true,true,
       true,true,true,true,true,true,
       true,true,true,
       false,false,
       false};

    /// @brief Guide for which columns are used for the results printed to screen 
    const bool isScreen[numColumns+1]=
      {true,true,true,true,true,
       true,true,false,false,true,
       true,true,true,false,false,
       true,true,true,true,
       true,true,true,true,true,true,true,true,
       false,false,false,false,false,false,
       false,false,false,
       false,false,
       false};

    /// @brief Guide for which columns are used for the log file 
    const bool isLog[numColumns+1]=
      {true,false,true,true,true,
       false,false,false,false,false,
       false,false,false,false,false,
       false,true,true,true,
       true,true,true,true,true,true,true,false,
       false,false,false,false,false,false,
       false,false,false,
       true,true,
       false};

     /// @brief Guide for which columns are used for the VOTable file 
    const bool isVOTable[numColumns+1]=
      {true,true,false,false,false,
       false,false,true,true,true,
       true,true,true,true,true,
       true,true,true,true,
       false,false,false,false,false,false,false,true,
       true,true,true,true,true,true,
       true,true,true,
       false,false,
       false};

    /// @brief Total number of columns used in logfile (no WCS ones). 
    const int numColumnsLog=16; 
    /// @brief Enumerated column titles for logfile
    enum LOGNAME {lNUM=0, lX, lY, lZ,
		  lFTOT, lFPEAK, lSNRPEAK,
		  lX1, lX2, lY1, lY2, lZ1, lZ2, lNPIX,
		  lNUMCH, lSPATSIZE}; 

    /// @brief Number of types of precision. 
    const int numPrec=6;        
    /// @brief Enumerated precision categories 
    enum PrecType {prFLUX=3, prVEL=3, prXYZ=1, prPOS=6, prWPOS=2, prSNR=2}; 

    /// @brief Default widths of all columns.
    const int defaultWidth[numColumns+1]=
      {5, 8, 6, 6, 6,
       11, 10, 11, 11, 7, 
       9, 9, 7, 7, 7,
       10, 10, 9, 7,
       4, 4, 4, 4, 4, 4, 6, 5,
       6, 6, 6, 7, 7, 7, 
       7, 7, 7,
       6, 8,
       1};

    /// @brief Default precisions for all columns.
    const int defaultPrec[numColumns+1]=
      {0, 0, prXYZ, prXYZ, prXYZ,
       0, 0, prPOS, prPOS, prVEL, 
       prWPOS, prWPOS, prVEL, prVEL, prVEL,
       prFLUX, prFLUX, prFLUX, 
       prSNR, 0, 0, 0, 0, 0, 0, 0, 0,
       prXYZ, prXYZ, prXYZ, prXYZ, prXYZ, prXYZ, 
       prXYZ, prXYZ, prXYZ,
       0, 0,
       0}; 

    /// @brief Default Titles of all columns. 
    const std::string defaultName[numColumns+1]=
      {"Obj#","Name","X","Y","Z",
       "RA","DEC","RAJD","DECJD","VEL",
       "w_RA","w_DEC","w_50","w_20","w_VEL",
       "F_int","F_tot","F_peak", "S/Nmax",
       "X1","X2","Y1","Y2","Z1","Z2","Npix","Flag",
       "X_av", "Y_av", "Z_av", "X_cent", "Y_cent", "Z_cent",
       "X_peak", "Y_peak", "Z_peak",
       "Nch", "Sp_size",
       " "};

    /// @brief The column ID (for use in a VOTable)
    const std::string defaultColID[numColumns+1]=
      {"col_objnum","col_name","col_x","col_y","col_z",
       "col_ra","col_dec","col_rajd","col_decjd","col_vel",
       "col_wra","col_wdec","col_w50","col_w20","col_wvel",
       "col_fint","col_ftot","col_fpeak", "col_snrmax",
       "col_x1","col_x2","col_y1","col_y2","col_z1","col_z2","col_npix","col_flag",
       "col_xav", "col_yav", "col_zav", "col_xcent", "col_ycent", "col_zcent",
       "col_xpeak", "col_ypeak", "col_zpeak",
       "col_nch", "col_spsize",
       ""};

    /// @brief Default units of all columns. 
    const std::string defaultUnits[numColumns+1]=
      {"","","","","",
       "","","[deg]","[deg]","",
       "[arcmin]","[arcmin]","","","",
       "","","", "",
       "","","","","","","[pix]","",
       "","","","","","","","","",
       "","",
       ""}; 

    /// @brief Default UCDs for all columns
    const std::string defaultUCDs[numColumns+1] = 
      {"meta.id", "meta.id;meta.main", "pos.cartesian.x", "pos.cartesian.y", "pos.cartesian.z",
       "", "", "", "", "", 
       "", "", "", "", "", 
       "phot.flux;spect.line.intensity", "phot.flux;spect.line.intensity", "phot.flux;spect.line.intensity", "phot.flux;stat.snr",
       "pos.cartesian.x;stat.min", "pos.cartesian.y;stat.min", "pos.cartesian.z;stat.min", "pos.cartesian.x;stat.max", "pos.cartesian.y;stat.max", "pos.cartesian.z;stat.max", "", "meta.code.qual",
       "pos.cartesian.x", "pos.cartesian.y", "pos.cartesian.z", "pos.cartesian.x", "pos.cartesian.y", "pos.cartesian.z",
       "pos.cartesian.x", "pos.cartesian.y", "pos.cartesian.z",
       "",""
       ""};

    ///@brief The datatype of each column
    const std::string defaultDatatype[numColumns+1] = 
      {"int", "char", "float", "float", "float",
       "char", "char", "float", "float", "float",
       "float", "float", "float", "float", "float",
       "float", "float", "float", "float",
       "int", "int", "int", "int", "int", "int", "int", "char",
       "float", "float", "float", "float", "float", "float",
       "int", "int", "int", 
       "int", "int", 
       ""};

    const std::string coordRef="J2000";
    /// @brief Default values for the VOTable reference string
    const std::string defaultExtraInfo[numColumns+1] =
      {"", "", "", "", "",
       coordRef, coordRef, coordRef, coordRef, "",
       coordRef, coordRef, "", "", "",
       "", "", "", "", 
       "", "", "", "", "", "", "", "", 
       "", "", "", "", "", "", 
       "", "", "", 
       "", "", 
       ""};

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
      //?      Col(int num, int prec=-1);   ///< Specific constructor
      //?      Col(std::string name, std::string units, int width, int prec, std::string ucd="", std::string datatype="", std::string extraInfo=""); ///< Generic constructor
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
      int itsWidth;          ///< How wide is the column (in ascii spaces)
      int itsPrecision;      ///< What precision should be used to print the values? (If 0, the setprecision command is not used.)
      std::string itsName;   ///< The title of the column
      std::string itsUnits;  ///< The units that the values in the column are expressed in.
      std::string itsUCD;    ///< The UCD associated with the value
      std::string itsDatatype; ///< What datatype do the values take?
      std::string itsExtraInfo; ///< Any other info? This can be the 'ref' entry for a VOTable field.
      std::string itsColID;  ///< The column ID
      //? COLNAME type;       ///< The type of the column
      std::string itsType;   ///< The type of the column - for reference purposes
    };

    Column defaultColumn(std::string type);


  }

}

#endif

