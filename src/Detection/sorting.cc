// -----------------------------------------------------------------------
// sorting.cc: Sort the list of Detections by channel or velocity.
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
#include <vector>
#include <algorithm>
#include <duchamp/Detection/detection.hh>

using std::vector;

/**
 * A class to match things pair-wise (useful for sorting).
 *
 * This class is deigned to match two quantities to each other.  It
 * was devised to find a way of taking a pair of lists that are
 * matched, and sorting one list while keeping the second matched
 * pair-wise.
 *
 * The elements are currently just assumed to be floats. This could be
 * extended by templating, but at this stage we don't need to...
 */
class Pair
{
public:
  Pair(){};
  virtual ~Pair(){};
  friend bool operator< (const Pair& lhs, const Pair& rhs){
    /**
     *  A comparison operator for pairs.  Compare the primary elements
     *  of the two pairs, using the basic < operator.
     */
    return (lhs.primary < rhs.primary);
  };
  void define(float p, float m){
    /** Basic assignment function. */
    primary=p; matching=m;
  };
  float get1(){return primary;};
  float get2(){return matching;};
private:
  float primary;  ///< The main element -- this will be the one that
		  ///can be compared.
  float matching; ///< The secondary element -- this cannot be
		  ///compared with other objects, it just tracks the
		  ///primary.
};

//======================================================================

namespace duchamp
{

  void SortByZ(vector <Detection> &inputList)
  {
    /**
     * A Function that takes a list of Detections and sorts them in
     * order of increasing z-pixel value.  Upon return, the inputList
     * is sorted.
     *
     * We use the std::stable_sort function, so that the order of
     * objects with the same z-value is preserved.
     * \param inputList List of Detections to be sorted.
     * \return The inputList is returned with the elements sorted.
     */

    long size = inputList.size();
    Pair *info = new Pair[size];
  
    for(int i=0;i<size;i++) info[i].define(inputList[i].getZcentre(), float(i));

    std::stable_sort(info,info+size);
  
    vector <Detection> sorted;
    for(int i=0;i<size;i++) sorted.push_back( inputList[int(info[i].get2())] );

    delete [] info;

    inputList.clear();
    for(int i=0;i<size;i++) inputList.push_back( sorted[i] );
    sorted.clear();
  
  }

  //======================================================================

  void SortByVel(vector <Detection> &inputList)
  {
    /**
     * A Function that takes a list of Detections and sorts them in 
     *  order of increasing velocity.
     * Every member of the vector needs to have WCS defined, (and if so,
     *   then vel is assumed to be defined for all), otherwise no sorting
     *   is done.
     *
     * We use the std::stable_sort function, so that the order of
     * objects with the same z-value is preserved.
     *
     * \param inputList List of Detections to be sorted.
     * \return The inputList is returned with the elements sorted,
     * unless the WCS is not good for at least one element, in which
     * case it is returned unaltered.
     */

    bool isGood = true;
    for(int i=0;i<inputList.size();i++) isGood = isGood && inputList[i].isWCS();

    if(isGood){

      long size = inputList.size();
      Pair *info = new Pair[size];
    
      for(int i=0;i<size;i++) info[i].define(inputList[i].getVel(), float(i));

      std::stable_sort(info, info+size);
  
      vector <Detection> sorted;
      for(int i=0;i<size;i++) sorted.push_back( inputList[int(info[i].get2())] );

      delete [] info;

      inputList.clear();
      for(int i=0;i<size;i++) inputList.push_back( sorted[i] );
      sorted.clear();
  
    }

  }  

}
