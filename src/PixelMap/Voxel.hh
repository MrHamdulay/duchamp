// -----------------------------------------------------------------------
// Voxel.hh: Definition of the Voxel class, storing a single 3D voxel
//           plus an associated flux.
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
#ifndef VOXEL_H
#define VOXEL_H

#include <iostream>

namespace PixelInfo
{
  //==========================================================================

  /// Voxel class.
  ///  A 3-dimensional pixel, with x,y,z position + flux

  class Voxel
  {
  public:
    /// @brief Default constructor. 
    Voxel(){};
    /// @brief Specific constructor, defining an (x,y,z) location and flux f. 
    Voxel(long x, long y, long z, float f);
    /// @brief Specific constructor, defining an (x,y,z) location, setting f=0. 
    Voxel(long x, long y, long z);
    /// @brief Copy constructor. 
    Voxel(const Voxel& v);
    /// @brief Assignment operator. 
    Voxel& operator= (const Voxel& v);
    virtual ~Voxel(){};

    // accessor functions
    void   setX(long x){itsX = x;};
    void   setY(long y){itsY = y;};
    void   setZ(long z){itsZ = z;};
    void   setF(float f){itsF = f;};
    /// @brief Define an (x,y) coordinate 
    void   setXY(long x, long y){itsX = x; itsY = y;};
    /// @brief Define an (x,y,z) coordinate 
    void   setXYZ(long x, long y, long z){itsX = x; itsY = y; itsZ = z;};
    /// @brief Define an (x,y) coordinate with a flux f 
    void   setXYF(long x, long y, float f){itsX = x; itsY = y; itsF = f;};
    /// @brief Define an (x,y,z) coordinate with a flux f 
    void   setXYZF(long x, long y, long z, float f){itsX = x; itsY = y; itsZ = z; itsF = f;};
    long   getX(){return itsX;};
    long   getY(){return itsY;};
    long   getZ(){return itsZ;};
    float  getF(){return itsF;};
    
    /// @brief Return an index value for an array 
    size_t arrayIndex(const size_t *dim);

    /// @brief Operator to print information of voxel. 
    friend std::ostream& operator<< ( std::ostream& theStream, Voxel& vox);

    /// @brief Operator to test for equality.
    friend bool operator== (Voxel lhs, Voxel rhs);

    /// @brief Less-than operator to allow ordering
    friend bool operator< (Voxel lhs, Voxel rhs);
    
    /// @brief Function to test for equality of positions only.
    bool match(Voxel other);
    

  protected:
    long  itsX;         ///< x-position of pixel
    long  itsY;         ///< y-position of pixel
    long  itsZ;         ///< z-position of pixel
    float itsF;         ///< flux of pixel
  };

  //==========================================================================

  /// Pixel class.
  ///  A 2-dimensional type of voxel, with just x & y position + flux

  class Pixel : public Voxel
  {
  public:
    Pixel(){itsZ=0;};
    Pixel(long x, long y, float f);
    Pixel(const Pixel& p);
    Pixel& operator= (const Pixel& p);
    virtual ~Pixel(){};
    // accessor functions
    void  setXY(long x, long y){itsX = x; itsY = y;};
    void  setXYF(long x, long y, float f){itsX = x; itsY = y; itsF = f;};

  };


}

#endif // VOXEL_H
