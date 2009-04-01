// -----------------------------------------------------------------------
// ChanMap.cc: Member functions for ChanMap class.
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
#include <duchamp/PixelMap/Voxel.hh>
#include <duchamp/PixelMap/Scan.hh>
#include <duchamp/PixelMap/Object2D.hh>
#include <duchamp/PixelMap/Object3D.hh>
#include <duchamp/PixelMap/ChanMap.hh>
#include <vector>

namespace PixelInfo
{

  ChanMap::ChanMap()
  {
    this->itsZ = -1;
    this->itsObject = Object2D();
  }

  ChanMap::ChanMap(const ChanMap& m){
    operator=(m);
  }

  ChanMap& ChanMap::operator= (const ChanMap& m)
  {
    if(this == &m) return *this;
    this->itsZ=m.itsZ; 
    this->itsObject=m.itsObject;
    return *this;
  }

  void ChanMap::addOffsets(long xoff, long yoff, long zoff)
  {
    this->itsZ += zoff;
    this->itsObject.addOffsets(xoff,yoff);
  }
 
  bool operator< (ChanMap lhs, ChanMap rhs)
  {
    /// @details This only acts on the channel number. 
    return (lhs.itsZ<rhs.itsZ);
  }

  ChanMap operator+ (ChanMap lhs, ChanMap rhs)
  {
    ///  @details If they are the same channel, add the Objects,
    ///  otherwise return a blank ChanMap.
    
    ChanMap newmap;
    if(lhs.itsZ==rhs.itsZ){
      newmap.itsZ = lhs.itsZ;
      newmap.itsObject = lhs.itsObject + rhs.itsObject;
    }
    return newmap;
  }
  

}
