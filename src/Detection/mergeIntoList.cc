// -----------------------------------------------------------------------
// mergeIntoList.cc: Add a Detection to a list, merging with existing
//                   members if necessary.
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
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Utils/utils.hh>

namespace duchamp
{

  void mergeIntoList(Detection &object, std::vector <Detection> &objList, 
		     Param &par)
  {
    /// @details
    /// A function to add a detection to a list of detections, checking
    /// first to see if it can be combined with existing members of the
    /// list.
    /// 
    /// The areClose testing and combining is now done with the
    /// parameters as given by the Param set, not just assuming adjacency
    /// (as previously done).
    /// 
    /// \param object The Detection to be merged into the list.
    /// \param objList The vector list of Detections.
    /// \param par The Param set, used for testing if merging needs to be done.

    bool haveMerged = false;

    std::vector<Detection>::iterator iter;
    for(uint ctr=0; (!(haveMerged) && (ctr<objList.size())); ctr++){
    
      if(areClose(object, objList[ctr], par)){
	Detection newobj = objList[ctr] + object;
	iter = objList.begin() + ctr;
	objList.erase( iter );
	objList.push_back( newobj );
	haveMerged = true;
      }
    
    }
  
    if(!haveMerged) objList.push_back(object);

  }

}
