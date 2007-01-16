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

/**
 * A namespace controlling the formatting of columns of output for Duchamp.
 */

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
    //--------------
    // basic accessor functions
    //
    int    getWidth(){return width;};         
    void   setWidth(int i){width=i;};    
    int    getPrecision(){return precision;};     
    void   setPrecision(int i){precision=i;};
    string getName(){return name;};          
    void   setName(string s){name=s;};  
    string getUnits(){return units;};         
    void   setUnits(string s){units=s;}; 

    //--------------
    // other functions
    //
    /** Make the column one space wider. */
    int    widen(){width++;};

    /** Increase the precision by one, widening the column if necessary. */
    int    upPrec(){precision++; if(width<precision+3) width++;};

    //--------------
    // Outputting functions -- all in columns.cc
    //
    /** write the title of the column to the stream */
    void   printTitle(std::ostream &stream){
      stream << std::setw(this->width) << std::setfill(' ') << this->name;
    };

    /** write the units of the column to the stream */
    void   printUnits(std::ostream &stream){
      stream << std::setw(this->width) << std::setfill(' ') << this->units;
    };

    /** write dashes the full width of the column to the stream */
    void   printDash (std::ostream &stream){
      stream << std::setw(this->width) << std::setfill('-')
	     << "" << std::setfill(' ');
    };

    /** write blanks the full width of the column to the stream */
    void   printBlank(std::ostream &stream){
      stream << std::setw(this->width) << std::setfill(' ') << "";
    };

    /** Print a given value in a column with correct width & precision. */
    template <class T> void printEntry(std::ostream &stream, T value);

  private:
    int width;          ///< How wide is the column (in ascii spaces)
    int precision;      ///< What precision should be used to print the values? (If 0, the setprecision command is not used.)
    string name;        ///< The title of the column
    string units;       ///< The units that the values in the column are expressed in.
  };

  
}
/** Returns a vector of Col for results file output.*/
vector<Column::Col> getFullColSet(vector<Detection> &objectList, 
				  FitsHeader &head);

/** Returns a vector of Col for logfile output.*/
vector<Column::Col> getLogColSet(vector<Detection> &objectList, 
				 FitsHeader &head);


#endif

