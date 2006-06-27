#ifndef COLUMNS_H
#define COLUMNS_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace Column
{

  const int fluxPrec=3, velPrec=3, xyzPrec=1, posPrec=6, wposPrec=2;

  class Col{
  public:
    Col(){width=0;name="";unit="";};
    Col(int w, string n, string u){width=w; name=n; unit=u;};
    ~Col(){};
    int widen(){width++;};
    int getWidth(){return width;};
    void setWidth(int i){width=i;};
    string getName(){return name;};
    void setName(string s){name=s;};
    string getUnit(){return unit;};
    void setUnit(string s){unit=s;};
    void title(std::ostream &stream){
      stream << std::setw(this->width) << this->name;};
    void units(std::ostream &stream){
      stream << std::setw(this->width) << this->unit;};
    void dash (std::ostream &stream){
      stream << std::setfill('-') << std::setw(this->width) 
	     << ""<< std::setfill(' ');};
    void blank(std::ostream &stream){
      stream << std::setfill(' ') << std::setw(this->width) << "";};
    
    template <class type> void entry(std::ostream &stream, type e){
      stream << std::setw(this->width) << e;};

    int width;
    string name;
    string unit;
  };

  class ColSet{
  public:
    ColSet(){defined=false; inserted=false;}
    ~ColSet(){};
    vector<Col> vec;
    bool defined;
    bool inserted;
    void addFluxCol(Col fluxCol){
      if(!this->inserted) this->vec.insert(this->vec.begin()+11, fluxCol);
      this->inserted = true;
    }

  };

}


#endif

