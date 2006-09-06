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

  enum PrecType {prFLUX, prVEL, prXYZ, prPOS, prWPOS};
  const int prec[5]={3, 3, 1, 6, 2};

  enum COLNAME {NUM=0, NAME, X, Y, Z,
		RA, DEC, VEL, 
		WRA, WDEC, WVEL,
		FINT, FTOT, FPEAK,
		X1, X2, Y1, Y2, Z1, Z2, NPIX, FLAG};

  enum LOGNAME {lNUM, lX, lY, lZ,
		lFTOT, lFPEAK,
		lX1, lX2, lY1, lY2, lZ1, lZ2, lNPIX};

  const int defaultWidth[22]={5, 8, 6, 6, 6,
			      13, 13, 7, 9, 9, 7,
			      10, 10, 9,
			      4, 4, 4, 4, 4, 4, 6, 5};
  const int defaultPrec[22]={0, 0, prec[prXYZ], prec[prXYZ], prec[prXYZ],
			     0, 0, prec[prVEL], 
			     prec[prWPOS], prec[prWPOS], prec[prVEL],
			     prec[prFLUX], prec[prFLUX], prec[prFLUX], 
			     0, 0, 0, 0, 0, 0, 0, 0};
  const string defaultName[22]={"Obj#","Name","X","Y","Z",
				"RA","DEC","VEL",
				"w_RA","w_DEC","w_VEL",
				"F_int","F_tot","F_peak",
				"X1","X2","Y1","Y2","Z1","Z2","Npix","Flag"};
  const string defaultUnits[22]={"","","","","",
				 "","","",
				 "[arcmin]","[arcmin]","",
				 "","","",
				 "","","","","","","[pix]",""};


  // Now define the Col class. 
  // This contains information about a given column -- its width, the 
  //  precision associated with it, its title and the units.
  // Plus the usual array of accessor functions and so on.

  class Col{
  public:
    Col(){width=1; precision=0; name=" "; units=" ";};
    Col(int num);  // in columns.cc
    virtual ~Col(){};
    // basic accessor functions
    int    getWidth(){return width;};
    void   setWidth(int i){width=i;};
    int    getPrecision(){return precision;};
    void   setPrecision(int i){precision=i;};
    string getName(){return name;};
    void   setName(string s){name=s;};
    string getUnits(){return units;};
    void   setUnits(string s){units=s;};
    // other functions
    int    widen(){width++;};
    int    upPrec(){precision++; if(width<precision+3) width++;};
    // outputting functions -- all in columns.cc
    void   printTitle(std::ostream &stream){
      stream << std::setw(this->width) << this->name;
    };
    void   printUnits(std::ostream &stream){
      stream << std::setw(this->width) << this->units;
    };
    void   printDash (std::ostream &stream){
      stream << std::setfill('-') << std::setw(this->width) 
	     << ""<< std::setfill(' ');
    };
    void   printBlank(std::ostream &stream){
      stream << std::setfill(' ') << std::setw(this->width) << "";
    };
    template <class T> void printEntry(std::ostream &stream, T value)// ;
    {
      stream << std::setprecision(precision);
      stream << std::setw(width) << value;
    }

  private:
    int width;          // how wide is the column (in ascii spaces)
    int precision;      // what precision should be used to print the values?
                        //   if 0, the setprecision command is not used.
    string name;        // the title of the column
    string units;       // what units the values in the column are expressed in
  };

  
}
vector<Column::Col> getFullColSet(vector<Detection> &objectList, FitsHeader &head);
vector<Column::Col> getLogColSet(vector<Detection> &objectList, FitsHeader &head);


#endif

