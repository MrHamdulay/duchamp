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

class Detection;
class FitsHeader;

/**
 * A namespace controlling the formatting of columns of output for Duchamp.
 */

namespace Column
{
  // First, define some basic quantities.
  // The default values for the columns, and default values for
  //  the precision of different types of columns.

  /** Total number of columns being considered.*/
  const int numColumns=32;    
  /** Enumerated column titles */
  enum COLNAME {NUM=0, NAME, X, Y, Z,
		RA, DEC, VEL, 
		WRA, WDEC, WVEL,
		FINT, FTOT, FPEAK, SNRPEAK,
		X1, X2, Y1, Y2, Z1, Z2, NPIX, FLAG,
		XAV, YAV, ZAV, XCENT, YCENT, ZCENT, 
		XPEAK, YPEAK, ZPEAK}; 

  /** Total number of columns used in logfile (no WCS ones). */
  const int numColumnsLog=14; 
  /** Enumerated column titles for logfile*/
  enum LOGNAME {lNUM=0, lX, lY, lZ,
		lFTOT, lFPEAK, lSNRPEAK,
		lX1, lX2, lY1, lY2, lZ1, lZ2, lNPIX}; 

  /** Number of types of precision. */
  const int numPrec=6;        
  /** Enumerated precision categories */
  enum PrecType {prFLUX=0, prVEL, prXYZ, prPOS, prWPOS, prSNR}; 

  /** Precision values in same order as enum list.*/
  const int prec[numPrec]={3, 3, 1, 6, 2, 2}; 

  /** Default widths of all columns.*/
  const int defaultWidth[numColumns]=
    {5, 8, 6, 6, 6,
     13, 13, 7, 9, 9, 7,
     10, 10, 9, 7,
     4, 4, 4, 4, 4, 4, 6, 5,
     6, 6, 6, 7, 7, 7, 7, 7, 7};

  /** Default precisions for all columns.*/
  const int defaultPrec[numColumns]=
    {0, 0, prec[prXYZ], prec[prXYZ], prec[prXYZ],
     0, 0, prec[prVEL], 
     prec[prWPOS], prec[prWPOS], prec[prVEL],
     prec[prFLUX], prec[prFLUX], prec[prFLUX], 
     prec[prSNR], 0, 0, 0, 0, 0, 0, 0, 0,
     prec[prXYZ], prec[prXYZ], prec[prXYZ], 
     prec[prXYZ], prec[prXYZ], prec[prXYZ], 
     prec[prXYZ], prec[prXYZ], prec[prXYZ]}; 

  /** Default Titles of all columns. */
  const std::string defaultName[numColumns]=
    {"Obj#","Name","X","Y","Z",
     "RA","DEC","VEL",
     "w_RA","w_DEC","w_VEL",
     "F_int","F_tot","F_peak", "S/Nmax",
     "X1","X2","Y1","Y2","Z1","Z2","Npix","Flag",
     "X_av", "Y_av", "Z_av", "X_cent", "Y_cent", "Z_cent",
     "X_peak", "Y_peak", "Z_peak"};

  /** Default units of all columns. */
  const std::string defaultUnits[numColumns]=
    {"","","","","",
     "","","",
     "[arcmin]","[arcmin]","",
     "","","", "",
     "","","","","","","[pix]","",
     "","","","","","","","",""}; 


  /** Class to store information about a single column.
   * This contains information about a given column -- its width, the 
   *  precision associated with it, its title and the units.
   * Plus the usual array of accessor functions and so on.
   */
  class Col{
  public:
    Col();          ///< Basic constructor
    Col(int num);   ///< Specific constructor
    Col(const Col& c);///< Assignment constructor
    Col& operator=(const Col& c); ///< Copy constructor
    virtual ~Col(); ///< Default destructor;
    //--------------
    // basic accessor functions
    //
    int    getWidth(){return width;};         
    void   setWidth(int i){width=i;};    
    int    getPrecision(){return precision;};     
    void   setPrecision(int i){precision=i;};
    std::string getName(){return name;};          
    void   setName(std::string s){name=s;};  
    std::string getUnits(){return units;};         
    void   setUnits(std::string s){units=s;}; 

    //--------------
    // other functions
    //
    /** Make the column one space wider. */
    void   widen(){width++;};

    /** Increase the precision by one, widening the column if necessary. */
    void   upPrec(){precision++; width++;};

    //--------------
    // Outputting functions -- all in columns.cc
    //
    /** write the title of the column to the stream */
    void   printTitle(std::ostream &stream);

    /** write the units of the column to the stream */
    void   printUnits(std::ostream &stream);

    /** write dashes the full width of the column to the stream */
    void   printDash (std::ostream &stream);

    /** write blanks the full width of the column to the stream */
    void   printBlank(std::ostream &stream);

    /** Print a given value in a column with correct width & precision. */
    template <class T> void printEntry(std::ostream &stream, T value);

  private:
    int width;          ///< How wide is the column (in ascii spaces)
    int precision;      ///< What precision should be used to print
			///   the values? (If 0, the setprecision command
			///   is not used.)
    std::string name;   ///< The title of the column
    std::string units;  ///< The units that the values in the column
			///   are expressed in.
  };

  
}

/** Returns a vector of Col for results file output.*/
std::vector<Column::Col> getFullColSet(std::vector<Detection> &objectList, 
				       FitsHeader &head);

/** Returns a vector of Col for logfile output.*/
std::vector<Column::Col> getLogColSet(std::vector<Detection> &objectList, 
				      FitsHeader &head);


#endif

