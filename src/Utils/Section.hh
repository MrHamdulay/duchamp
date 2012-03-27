// -----------------------------------------------------------------------
// Section.hh: Definition of the Section class, used to store
//             definitions of subsections of a FITS file.
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
#ifndef SECTION_H
#define SECTION_H

#include <string>
#include <vector>
#include <duchamp/duchamp.hh>

namespace duchamp
{

  /// @brief A class to store information on subsections of a cube. 
  ///
  /// @details This keeps the subsection itself as a string, plus the
  /// parsed information in the format of vectors of starting
  /// positions and lengths of each dimension.

  class Section
  {
  public:
    Section();
    Section(std::string &s){subsection=s; numSections=0; flagParsed=false;};
    Section(const Section& s);
    Section& operator= (const Section& s);
    virtual ~Section(){};

    /// @brief Has the section been defined by a parse() call?
    bool isDefined(){return numSections>0;};

    /// @brief Are all axis sections of length >0?
    bool isValid();

    /// @brief Convert the subsection string into the lists of numerical values. 
    OUTCOME parse(std::vector<size_t> dimAxes);
    OUTCOME parse(std::vector<long> dimAxes);
    OUTCOME parse(std::vector<int> dimAxes);
    OUTCOME parse(size_t *dim, int size);
    OUTCOME parse(long *dim, int size);

    bool isParsed(){return flagParsed;};

    /// @brief Test whether a given voxel (x,y,z) lies within the subsection 
    bool isInside(int x, int y, int z){
      return (  ( (x>=starts[0])&&(x<starts[0]+dims[0])  ) &&
		( (y>=starts[1])&&(y<starts[1]+dims[1])  ) &&
		( (z>=starts[2])&&(z<starts[2]+dims[2])  )   );
    }

    /// @brief Save the subsection string 
    void setSection(std::string s){subsection=s;};
    /// @brief Return the subsection string 
    std::string getSection(){return subsection;};

    /// @brief Save a single dimension's subsection string 
    void setSection(unsigned int i, std::string s){sections[i] = s;};
    /// @brief Return a particular dimension's subsection string 
    std::string getSection(unsigned int i){if(i>=numSections) return "*"; else return sections[i];};

    /// @brief Return a particular starting value 
    int getStart(unsigned int i){if(i>=numSections) return 0; else return starts[i];};
    /// @brief Set a particular starting value 
    void setStart(unsigned int i, unsigned int val){starts[i]=val;};

    /// @brief Return a particular dimension length 
    int getDim(unsigned int i){if(i>=numSections) return 1; else return dims[i];};
    /// @brief Set a particular dimension length 
    void setDim(unsigned int i, unsigned int val){dims[i]=val;};

    /// @brief Return a particular ending value 
    int getEnd(unsigned int i){if(i>=numSections) return 0; else return starts[i]+dims[i]-1;};
    /// @brief Set a particular ending value, using the current starting value 
    void setEnd(unsigned int i, unsigned int val){dims[i]=val-starts[i]+1;};

    /// @brief Return the full list of start values 
    std::vector<int> getStartList(){return starts;};
    /// @brief Return the full list of dimension lengths 
    std::vector<int> getDimList(){return dims;};

    /// @brief Return a section that is the intersection of this with another
    Section intersect(Section &other);

    /// @brief Return a section that is the intersection of this with the full section given by a set of axis dimensions
    Section intersect(std::vector<long> dimAxes);
    Section intersect(std::vector<int> dimAxes);
    Section intersect(long *dim, int size);

    /// @brief Return a section that is the result of applying some subseection of the current one, where the "other" subsection acts in the coordinates of this.
    friend Section operator* (Section &lhs, Section &rhs);

  private:
    bool flagParsed;            ///< Has the Section been parsed yet?
    std::string subsection;   ///< The string indicating the subsection, of the format [a:b,c:d,e:f] etc.
    std::vector<std::string> sections; // The individual section strings for each dimension
    size_t numSections;         ///< The number of sections in the string.
    std::vector<int> starts;  ///< The list of starting values (a,c,e)
    std::vector<int> dims;    ///< The list of lengths of each dimension (b-a+1,d-c+1,f-e+1)
  };

  std::string nullSection(int ndim);

}

#endif //SECTION_H
