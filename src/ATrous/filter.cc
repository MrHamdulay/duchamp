// -----------------------------------------------------------------------
// filter.cc: Defining a filter function for wavelet reconstruction.
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
#include <duchamp/duchamp.hh>
#include <duchamp/ATrous/filter.hh>
#include <math.h>
#include <vector>

namespace duchamp
{

    Filter::Filter():
	name("")
  {
      this->sigmaFactors = std::vector< std::vector<double>* >(3);
      this->maxNumScales = std::vector<unsigned int>(3,0);
  }

  Filter::Filter(const Filter& f)
  {
    operator=(f);
  }

  Filter& Filter::operator=(const Filter& f)
  {
    if(this==&f) return *this;
    this->name = f.name;
    this->filter1D = f.filter1D;
    this->maxNumScales = f.maxNumScales;
    this->sigmaFactors = f.sigmaFactors;
    return *this;
  }

  //-----------------------------------------------------------------------

  Filter::~Filter()
  {
    this->filter1D.clear();
    this->maxNumScales.clear();
    this->sigmaFactors.clear();
  }
  //-----------------------------------------------------------------------


  unsigned int Filter::getNumScales(size_t length)
  {
    unsigned int num;
    if(length==0) return 0;
    switch(this->filter1D.size()){
    case 5: 
      num = int(log(double(length-1))/M_LN2) - 1;
      break;
    case 3:
      num = int(log(double(length-1))/M_LN2);
      break;
    default:
      num = 1 + int(log(double(length-1)/double(this->filter1D.size()-1))/M_LN2);
      break;
    }
    return num;
  }
  //-----------------------------------------------------------------------

  unsigned int Filter::getMaxSize(int scale)
  {
    switch(this->filter1D.size()){
    case 5:
      return int(pow(2,scale+1)) + 1;
      break;
    case 3:
      return int(pow(2,scale)) + 1;
      break;
    default:
      return int(pow(2,scale-1))*(this->filter1D.size()-1) + 1;
      break;
    }
  }
 
}
