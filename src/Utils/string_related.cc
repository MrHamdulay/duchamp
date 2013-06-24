// -----------------------------------------------------------------------
// string_related.cc: General utility functions for manipulating strings 
//                    and input flags/parameters
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
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <duchamp/Utils/utils.hh>


std::string makelower( std::string s )
{
  // "borrowed" from Matt Howlett's 'fred'
  std::string out = "";
  for( size_t i=0; i<s.size(); ++i ) {
    out += tolower(s[i]);
  }
  return out;
}

std::string stringize(bool b)
{
  /// Convert a bool variable to the textual equivalent. 
  /// \return A std::string with the english equivalent of the bool.

  std::string output;
  if(b) output="true";
  else output="false";
  return output;
}

bool boolify( std::string s )
{
  ///  Convert a std::string to a bool variable: 
  ///  "1" and "true" get converted to true;
  ///  "0" and "false" (and anything else) get converted to false.
  /// \return The bool equivalent of the string.

  if((s=="1") || (makelower(s)=="true")) return true;
  else if((s=="0") || (makelower(s)=="false")) return false;
  else return false;
}

std::string readSval(std::stringstream& ss)
{
  std::string val; 
  ss >> val; 
  return val;
}

std::string readFilename(std::stringstream& ss)
{
  std::string val;
  getline(ss,val);
  return removeLeadingBlanks(val);
}

bool readFlag(std::stringstream& ss)
{
  std::string val; 
  ss >> val; 
  return boolify(val);
}

float readFval(std::stringstream& ss)
{
  float val; 
  ss >> val; 
  return val;
}

int readIval(std::stringstream& ss)
{
  int val; 
  ss >> val; 
  return val;
}

std::string removeLeadingBlanks(std::string s)
{
  /**
   * All blank spaces from the start of the string to the first
   * non-blank-space character are deleted.
   */ 
  int i=0;
  while(isspace(s[i])){
    i++;
  }
  std::string newstring="";
  for(unsigned int j=i;j<s.size();j++) newstring += s[j];
  return newstring;
}
//------------------------------------------------------------------------

std::string deblank(std::string s)
{
  /**
   * All blank spaces from the start of the string to the first
   * non-blank-space character, and from the last non-blank-space
   * character to the end are deleted.
   */ 
  int beg=0;
  while(isspace(s[beg])){
    beg++;
  }
  int end=s.size()-1;
  while(isspace(s[end])){
    end--;
  }
  std::string newstring;
  for(int j=beg;j<=end;j++) newstring += s[j];
  return newstring;
}

void writeStringToBinaryFile(std::ofstream &outfile, std::string str)
{
  size_t size=str.size();
  outfile.write(reinterpret_cast<const char*>(&size), sizeof size);
  outfile.write(str.c_str(), sizeof(char) * size);
}

std::string readStringFromBinaryFile(std::ifstream &infile)
{
  size_t size;
  infile.read(reinterpret_cast<char*>(&size), sizeof size);
  char *cstr = new char[size];
  infile.read(cstr, sizeof(char) * size);
  std::string str(cstr);
  str = str.substr(0,size); // don't know why this is necessary - if left out, sometimes get a string returned that is too long...
  delete cstr; 
  return str;
}

std::vector<int> selectionToIntVec(std::string &str)
{
    ///  Converts a comma-separated list of items & ranges to a vector
    /// list of integers. For instance, the string 1-4,6,9-11 will be
    /// converted to the vector [1,2,3,4,6,9,10,11] 
    /// \return An STL integer vector

    std::stringstream ss1;
    std::string tmp;
    std::vector<int> tmplist;
    ss1.str(str);
    while(!ss1.eof()){
      getline(ss1,tmp,',');
      for(size_t i=0;i<tmp.size();i++) if(tmp[i]=='-') tmp[i]=' ';
      int a,b;
      std::stringstream ss2;
      ss2.str(tmp);
      ss2 >>a;
      if(!ss2.eof()) ss2 >> b;
      else b=a;
      for(int n=a;n<=b;n++){
	tmplist.push_back(n);
      }      
    }
    return tmplist;

}
