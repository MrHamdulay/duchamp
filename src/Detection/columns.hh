#ifndef COLUMNS_H
#define COLUMNS_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using std::string;
using std::vector;

class Detection;
class FitsHeader;

namespace Column
{
  // First, define some basic quantities.
  // The default values for the columns, and default values for
  //  the precision of different types of columns.

  const int numColumns=23;    ///< Total number of columns being considered.
  /** Enumerated column titles */
  enum COLNAME {NUM=0, NAME, X, Y, Z,
		RA, DEC, VEL, 
		WRA, WDEC, WVEL,
		FINT, FTOT, FPEAK, SNRPEAK,
		X1, X2, Y1, Y2, Z1, Z2, NPIX, FLAG}; 

  const int numColumnsLog=14; ///< Total number of columns used in logfile (no WCS ones).
  /** Enumerated column titles for logfile*/
  enum LOGNAME {lNUM, lX, lY, lZ,
		lFTOT, lFPEAK, lSNRPEAK,
		lX1, lX2, lY1, lY2, lZ1, lZ2, lNPIX}; 


  const int numPrec=6;        ///< Number of types of precision
  enum PrecType {prFLUX, prVEL, prXYZ, prPOS, prWPOS, prSNR}; ///< Enumerated precision categories
  const int prec[numPrec]={3, 3, 1, 6, 2, 2}; ///< Precision values in same order as enum list.

  const int defaultWidth[numColumns]=
    {5, 8, 6, 6, 6,
     13, 13, 7, 9, 9, 7,
     10, 10, 9, 7,
     4, 4, 4, 4, 4, 4, 6, 5};          ///< Default widths of all columns.
  const int defaultPrec[numColumns]=
    {0, 0, prec[prXYZ], prec[prXYZ], prec[prXYZ],
     0, 0, prec[prVEL], 
     prec[prWPOS], prec[prWPOS], prec[prVEL],
     prec[prFLUX], prec[prFLUX], prec[prFLUX], 
     prec[prSNR], 0, 0, 0, 0, 0, 0, 0, 0}; ///< Default precisions for all columns.
  const string defaultName[numColumns]=
    {"Obj#","Name","X","Y","Z",
     "RA","DEC","VEL",
     "w_RA","w_DEC","w_VEL",
     "F_int","F_tot","F_peak", "S/Nmax",
     "X1","X2","Y1","Y2","Z1","Z2","Npix","Flag"}; ///< Default Titles of all columns
  const string defaultUnits[numColumns]=
    {"","","","","",
     "","","",
     "[arcmin]","[arcmin]","",
     "","","", "",
     "","","","","","","[pix]",""}; ///< Default units of all columns.


  /** Class to store information about a single column.
   * This contains information about a given column -- its width, the 
   *  precision associated with it, its title and the units.
   * Plus the usual array of accessor functions and so on.
   */
  class Col{
  public:
    Col();          ///< Basic constructor
    Col(int num);   ///< Specific constructor
    virtual ~Col(); ///< Default destructor;
    // basic accessor functions
    int    getWidth();          ///< return column width
    void   setWidth(int i);     ///< set the column width
    int    getPrecision();      ///< return the column precision
    void   setPrecision(int i); ///< set the column precision
    string getName();           ///< return the title of the column
    void   setName(string s);   ///< set the column's title
    string getUnits();          ///< return the units of the column
    void   setUnits(string s);  ///< set the units of the column
    // other functions
    int    widen();             ///< Make the column one space wider.
    int    upPrec();            ///< Increase the precision by one, widening the column if necessary.
    // outputting functions -- all in columns.cc
    void   printTitle(std::ostream &stream); ///< write the title of the column to the stream
    void   printUnits(std::ostream &stream); ///< write the units of the column to the stream
    void   printDash (std::ostream &stream); ///< write dashes the full width of the column to the stream
    void   printBlank(std::ostream &stream); ///< write blanks the full width of the column to the stream
    template <class T> void printEntry(std::ostream &stream, T value); ///< Print a given value in a column with correct width & precision.

  private:
    int width;          ///< How wide is the column (in ascii spaces)
    int precision;      ///< What precision should be used to print the values? (If 0, the setprecision command is not used.)
    string name;        ///< The title of the column
    string units;       ///< The units that the values in the column are expressed in.
  };

  // inline Col functions
  inline int    Col::getWidth(){return width;}
  inline void   Col::setWidth(int i){width=i;}
  inline int    Col::getPrecision(){return precision;}
  inline void   Col::setPrecision(int i){precision=i;}
  inline string Col::getName(){return name;}
  inline void   Col::setName(string s){name=s;}
  inline string Col::getUnits(){return units;}
  inline void   Col::setUnits(string s){units=s;}
  inline int    Col::widen(){width++;}
  inline int    Col::upPrec(){precision++; if(width<precision+3) width++;}
  inline void   Col::printTitle(std::ostream &stream){
    stream << std::setw(this->width) << std::setfill(' ') << this->name;
  }
  inline void   Col::printUnits(std::ostream &stream){
    stream << std::setw(this->width) << std::setfill(' ') << this->units;
  }
  inline void   Col::printDash (std::ostream &stream){
    stream << std::setw(this->width) << std::setfill('-')
	   << "" << std::setfill(' ');
  }
  inline void   Col::printBlank(std::ostream &stream){
    stream << std::setw(this->width) << std::setfill(' ') << "";
  }

  
}
vector<Column::Col> getFullColSet(vector<Detection> &objectList, 
				  FitsHeader &head);
vector<Column::Col> getLogColSet(vector<Detection> &objectList, 
				 FitsHeader &head);


#endif

