// -----------------------------------------------------------------------
// ChanMap.hh: Definition of ChanMap, a class to combine a two-dimensional
//             object with a channel number.
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
#ifndef CHANMAP_H
#define CHANMAP_H

#include <duchamp/PixelMap/Voxel.hh>
#include <duchamp/PixelMap/Scan.hh>
#include <duchamp/PixelMap/Object2D.hh>
#include <vector>
#include <algorithm>
#include <iostream>

namespace PixelInfo
{

  /// @brief A class to store a channel+Object2D map.
  /// @details This represents a 2-dimensional set of pixels that has an
  /// associated channel number. Sets of these will form a three
  /// dimensional object.

  class ChanMap
  {
  public:
    ChanMap();
    ChanMap(long z){itsZ=z;};
    ChanMap(long z, Object2D obj){itsZ=z; itsObject=obj;};
    ChanMap(const ChanMap& m);
    ChanMap& operator= (const ChanMap& m);
    virtual ~ChanMap(){};

    /// @brief Define the ChanMap using a channel number and Object2D. 
    void     define(long z, Object2D obj){itsZ=z; itsObject=obj;};

    /// @brief Return the value of the channel number. 
    long     getZ(){return itsZ;};

    /// @brief Set the value of the channel number. 
    void     setZ(long l){itsZ=l;};

    /// @brief Return the Object2D set of scans. 
    Object2D getObject(){return itsObject;};

    /// @brief Return the i-th scan of the Object2D .
    Scan     getScan(int i){return itsObject.scanlist[i];};

    /// @brief The number of scans in the Object2D set. 
    long     getNumScan(){return itsObject.scanlist.size();};

    /// @brief Add constant offsets to each of the coordinates.
    void     addOffsets(long xoff, long yoff, long zoff);

    /// @brief The less-than operator
    friend bool operator< (ChanMap lhs, ChanMap rhs);

    /// @brief Add two ChanMaps together.
    friend ChanMap operator+ (ChanMap lhs, ChanMap rhs);

    friend class Object3D;

  private:
    long     itsZ;      ///< The channel number.
    Object2D itsObject; ///< The set of scans of object pixels.

  };

}

#endif // define CHANMAP_H
