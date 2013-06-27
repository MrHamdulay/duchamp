// -----------------------------------------------------------------------
// Object2D.hh: Definition of Object2D, a class to hold pixel
//              information for a 2-dimensional object.
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
#ifndef OBJECT2D_H
#define OBJECT2D_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <duchamp/PixelMap/Scan.hh>
#include <duchamp/PixelMap/Voxel.hh>

namespace PixelInfo
{

  /// @brief A class to store the pixel information for an object
  /// defined in a 2-dimensional plane.
  /// 
  /// @details The object consists of a set of Scans (stored as a
  /// std::vector), together with basic information on the maximum,
  /// minimum, and average values (actually stored as xSum, ySum) for
  /// the x and y pixel values.

  class Object2D
  {
  public:
    Object2D();
    Object2D(const Object2D& o);
    Object2D& operator= (const Object2D& o);  
    virtual ~Object2D(){};
  
    /// @brief Clear the Object and set the number of pixels to zero.
    void  clear(){scanlist.clear(); numPix=0;};

    /// @brief Make sure there are no overlapping or adjacent scans in the Scan list. 
    void  cleanup();

    /// @brief Add a pixel to the Object, making sure no scans overlap afterwards. 
    void  addPixel(long x, long y);
    // /// @brief Add the (x,y) part of a Voxel to the Object, using addPixel(long,long)
    // void  addPixel(Voxel &v){this->addPixel(v.getX(),v.getY());};
    // /// @brief Add a Pixel to the Object, using addPixel(long,long)
    // void  addPixel(Pixel &p){this->addPixel(p.getX(),p.getY());};

    /// @brief Add a full Scan to the Object, making sure there are no overlapping scans afterwards. 
    void  addScan(Scan &scan);

    /// @brief Test whether a pixel (x,y) is in the Object. 
    bool  isInObject(long x, long y);
    /// @brief Test whether the (x,y) part of a Voxel is in the Object.
    bool  isInObject(Voxel v){return this->isInObject(v.getX(),v.getY());};
    /// @brief Test whether a Pixel is in the Object.
    bool  isInObject(Pixel p){return this->isInObject(p.getX(),p.getY());};

    /// @brief Test whether a given Scan overlaps with any pixels in the Object. 
    bool  scanOverlaps(Scan &scan);

    bool canMerge(Object2D &other, float threshS, bool flagAdj);
    bool isNear(Object2D &other, long gap);
    bool isClose(Object2D &other, float threshS, bool flagAdj);   

    /// @brief Return the number of pixels in the Object. 
    unsigned long  getSize(){return numPix;};

    /// @brief Return the number of Scans in the Object. 
    long  getNumScan(){return scanlist.size();};

    /// @brief Return the scan list.
    std::vector<Scan> getScanlist(){return scanlist;};

    /// @brief Get the i-th Scan in the Scan list. 
    Scan  getScan(int i){return scanlist[i];};

    /// @brief Order the Scans in the list, using the < operator for Scans. 
     void  order(){std::stable_sort(scanlist.begin(),scanlist.end());};
    //void order(){scanlist.sort();};

    /// @brief Add values to the x- and y-axes, making sure to add the offsets to the sums and min/max values. 
    void  addOffsets(long xoff, long yoff);

    /// @brief Calculate the sums, mins, maxs for x&y -- should not be necessary as these are done when pixels & Scans are added. 
    void  calcParams();

    /// @brief Return the average x-value. 
    float getXaverage(){return xSum/float(numPix);};

    /// @brief Return the average y-value. 
    float getYaverage(){return ySum/float(numPix);};

    long  getXmin(){return xmin;};
    long  getYmin(){return ymin;};
    long  getXmax(){return xmax;};
    long  getYmax(){return ymax;};

    /// @brief Return the number of distinct y-values.
    long  getNumDistinctY();
    /// @brief Return the number of distinct x-values.
    long  getNumDistinctX();
  
    /// @brief Return a measurement of the primary position angle
    double getPositionAngle();
    /// @brief Return the lengths of the principal axes.
    std::pair<double,double> getPrincipalAxes();
    /// @brief Find the best fitting ellipse weighted by some flux array
    bool findEllipse(bool weightByFlux, float *array, size_t xdim, size_t ydim, int xzero, int yzero, float x0, float y0);

    double major(){return majorAxis;};
    double minor(){return minorAxis;};
    double posAng(){return posAngle;};

    /// @brief A stream output operator. 
    friend std::ostream& operator<< ( std::ostream& theStream, Object2D& obj);

    /// @brief Adding two Objects together. 
    friend Object2D operator+ (Object2D lhs, Object2D rhs);

    friend class ChanMap;
    friend class Object3D; 
    friend class Detection;

  protected:
    std::vector<Scan>   scanlist;       ///< The list of Scans
    unsigned long     numPix;         ///< Number of pixels in the Object
    float             xSum;           ///< Sum of x values
    float             ySum;           ///< Sum of y values
    long              xmin,xmax;      ///< min and max x-values of object
    long              ymin,ymax;      ///< min and max y-values of object
    double            majorAxis;      ///< fitted major principal axis
    double            minorAxis;      ///< fitted minor principal axis
    double            posAngle;       ///< fitted position angle of principal axis
  };

}

#endif //OBJECT2D_H
