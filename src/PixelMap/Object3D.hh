// -----------------------------------------------------------------------
// Object3D.hh: Definition of Object3D and ChanMap, classes that hold
//              pixel information for a three-dimensional object.
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
#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <duchamp/PixelMap/Voxel.hh>
#include <duchamp/PixelMap/Scan.hh>
#include <duchamp/PixelMap/Object2D.hh>
#include <duchamp/PixelMap/ChanMap.hh>
#include <vector>
#include <algorithm>
#include <iostream>

namespace PixelInfo
{

  /// @brief A set of pixels in 3D.  
  /// 
  /// @details This stores the pixels in a list of ChanMap objects --
  /// ie. a set of Object2Ds, each with a different channel number.
  /// Also recorded are the average x-, y- and z-values (via their
  /// sums), as well as their extrema.

  class Object3D
  {
  public:
    Object3D();
    Object3D(const Object3D& o);
    Object3D& operator= (const Object3D& o);  
    virtual ~Object3D(){};

    /// @brief Is a 3-D voxel in the Object? 
    bool     isInObject(long x, long y, long z);
    /// @brief Is a 3-D voxel in the Object? 
    bool     isInObject(Voxel v){return this->isInObject(v.getX(),v.getY(),v.getZ());};
 
    /// @brief Add a single 3-D voxel to the Object. 
    void     addPixel(long x, long y, long z);
    /// @brief Add a single 3-D voxel to the Object. 
    void     addPixel(Voxel v){this->addPixel(v.getX(),v.getY(),v.getZ());};
    /// @brief Add a scan to the object 
    void     addScan(Scan s, long z);
    /// @brief Add a full channel map to the Object. 
    void     addChannel(ChanMap channel);
    /// @brief Add a full channel map to the Object. 
    void     addChannel(long z, Object2D obj){
      ChanMap channel(z,obj);
      this->addChannel(channel);
    }

    /// @brief Sort the list of channel maps by their channel number. 
    void     order();

    /// @brief Calculate the averages and extrema of the three coordinates. 
    void     calcParams();
    /// @brief Return the average x-value.
    float    getXcentre(){return xSum/float(numVox);};
    /// @brief Return the average y-value.
    float    getYcentre(){return ySum/float(numVox);};
    /// @brief Return the average z-value.
    float    getZcentre(){return zSum/float(numVox);};
    /// @brief Return the minimum x-value.
    long     getXmin(){return xmin;}; 
    /// @brief Return the minimum y-value.
    long     getYmin(){return ymin;};
    /// @brief Return the minimum z-value.
    long     getZmin(){return zmin;};
    /// @brief Return the maximum x-value.
    long     getXmax(){return xmax;};
    /// @brief Return the maximum y-value.
    long     getYmax(){return ymax;};
    /// @brief Return the maximum z-value.
    long     getZmax(){return zmax;};

    /// @brief Return the number of distinct voxels in the Object. 
    long     getSize(){return numVox;};

    /// @brief Return the number of distinct channels in the Object. 
    long     getNumDistinctZ();

    /// @brief Return the number of channels in the Object. 
    long     getNumChanMap(){return this->maplist.size();};

    /// @brief Return the number of spatial pixels -- ie. the number of distinct (x,y) sets in the Object. 
    long     getSpatialSize();
 
    /// @brief Get the pixNum-th voxel 
    Voxel    getPixel(int pixNum);

    /// @brief Return a vector set of all voxels in the Object. 
    std::vector<Voxel> getPixelSet();

    /// @brief Get the i-th channel map. 
    ChanMap  getChanMap(int i){return this->maplist[i];};

    /// @brief Get the channel number of the i-th channel map. 
    long     getZ(int i){return this->maplist[i].itsZ;};

    /// @brief Return an Object2D showing the spatial (x,y) distribution of voxels in the Object 
    Object2D getSpatialMap();

    /// @brief Add constant offsets to each of the dimensions, changing the parameters at the same time. 
    void     addOffsets(long xoff, long yoff, long zoff);

    /// @brief Output operator for the Object3D. 
    friend std::ostream& operator<< ( std::ostream& theStream, Object3D& obj);

    /// @brief Add two Object3Ds. Overlapping channels are combined using addChannel(). 
    friend Object3D operator+ (Object3D lhs, Object3D rhs){
      Object3D output = lhs;
      for(unsigned int m=0;m<rhs.maplist.size();m++) output.addChannel(rhs.maplist[m]);
      return output;
    }

  private:
    std::vector<ChanMap> maplist;   ///< The list of channel maps
    long                 numVox;    ///< How many voxels in the Object?
    float                xSum;      ///< Sum of the x-values
    float                ySum;      ///< Sum of the y-values
    float                zSum;      ///< Sum of the z-values
    long                 xmin,xmax; ///< min and max x-values of object
    long                 ymin,ymax; ///< min and max y-values of object
    long                 zmin,zmax; ///< min and max z-values of object
  }; 

}

#endif //OBJECT3D_H
