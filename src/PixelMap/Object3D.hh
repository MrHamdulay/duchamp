// -----------------------------------------------------------------------
// Object3D.hh: Definition of the Object3D class that holds
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
#include <duchamp/Utils/Section.hh>
#include <vector>
#include <algorithm>
#include <map>
#include <iostream>
#include <fstream>

namespace PixelInfo
{

  /// @brief A set of pixels in 3D.  
  /// 
  /// @details This stores the pixels in a STL map, connecting a
  /// channel number (z-value) to a unique Object2D. Also recorded
  /// are the average x-, y- and z-values (via their sums), as well as
  /// their extrema.

  class Object3D
  {
  public:
    Object3D();
    Object3D(const Object3D& o);
    Object3D& operator= (const Object3D& o);  
    virtual ~Object3D(){};

    /// @brief Is a 3-D voxel in the Object? 
    bool isInObject(long x, long y, long z);
    /// @brief Is a 3-D voxel in the Object? 
    bool isInObject(Voxel v){return isInObject(v.getX(),v.getY(),v.getZ());};
 
    /// @brief Add a single 3-D voxel to the Object. 
    virtual void addPixel(long x, long y, long z);
    /// @brief Add a single 3-D voxel to the Object. 
    virtual void addPixel(Voxel v){addPixel(v.getX(),v.getY(),v.getZ());};
    /// @brief Add a scan to the object 
    void addScan(Scan s, long z);
    /// @brief Add a full channel map to the Object. 
    void addChannel(const long &z, Object2D &obj);
    /// @brief Add a full channel map to the object.
    // void addChannel(const std::pair<long, Object2D> &chan){addChannel(chan.first,chan.second);};

    /// @brief Calculate the averages and extrema of the three coordinates. 
    void calcParams();
    /// @brief Return the average x-value.
    float getXaverage();
    /// @brief Return the average y-value.
    float getYaverage();
    /// @brief Return the average z-value.
    float getZaverage();
    /// @brief Return the minimum x-value.
    long getXmin(){return xmin;}; 
    /// @brief Return the minimum y-value.
    long getYmin(){return ymin;};
    /// @brief Return the minimum z-value.
    long getZmin(){return zmin;};
    /// @brief Return the maximum x-value.
    long getXmax(){return xmax;};
    /// @brief Return the maximum y-value.
    long getYmax(){return ymax;};
    /// @brief Return the maximum z-value.
    long getZmax(){return zmax;};

    /// @brief Return the number of distinct voxels in the Object. 
    unsigned int getSize(){return numVox;};

    /// @brief Return the number of distinct channels in the Object. 
    size_t getNumDistinctZ(){return chanlist.size();};

    /// @brief Return the number of spatial pixels -- ie. the number of distinct (x,y) sets in the Object. 
    unsigned long getSpatialSize(){return spatialMap.getSize();};

    /// @brief Return a vector set of all voxels in the Object. 
    std::vector<Voxel> getPixelSet();
    /// @brief Return a vector set of all voxels in the Object, with flux values from the given array. 
    std::vector<Voxel> getPixelSet(float *array, size_t *dim);

    /// @brief Return a vector list of the channel numbers in the Object
    std::vector<long> getChannelList();

    /// @brief Return the largest number of adjacent channels in the Object.
    unsigned int getMaxAdjacentChannels();

    /// @brief Get the location of the spatial borders. 
    std::vector<std::vector<Voxel> > getVertexSet();

    /// @brief Get the channel map for channel z. 
    Object2D getChanMap(long z);

    /// @brief Return an Object2D showing the spatial (x,y) distribution of voxels in the Object 
    Object2D getSpatialMap(){return spatialMap;};

    /// @brief Add constant offsets to each of the dimensions, changing the parameters at the same time. 
    virtual void addOffsets(long xoff, long yoff, long zoff);

    /// @brief Return the bounding box for the object, as a duchamp::Section object
    duchamp::Section getBoundingSection(int boundary=1);

    /// @brief Writing to a binary file
    void write(std::string filename);
    /// @brief Reading from a binary file
    std::streampos read(std::string filename, std::streampos loc=0);

    /// @brief Class function to print to a stream
    void print(std::ostream& theStream);
    
    /// @brief Output operator for the Object3D. 
    friend std::ostream& operator<< ( std::ostream& theStream, Object3D& obj);

    /// @brief Add two Object3Ds. Overlapping channels are combined using addChannel(). 
    friend Object3D operator+ (Object3D lhs, Object3D rhs);

  protected:
    std::map<long,Object2D> chanlist;  ///< The list of 2D channel maps
    unsigned long           numVox;    ///< How many voxels in the Object?
    Object2D                spatialMap;///< The 2D spatial map of the Object
    float                   xSum;      ///< Sum of the x-values
    float                   ySum;      ///< Sum of the y-values
    float                   zSum;      ///< Sum of the z-values
    long                    xmin,xmax; ///< min and max x-values of object
    long                    ymin,ymax; ///< min and max y-values of object
    long                    zmin,zmax; ///< min and max z-values of object
  }; 

}

#endif //OBJECT3D_H
