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
#include <PixelMap/Scan.hh>
#include <PixelMap/Voxel.hh>

namespace PixelInfo
{

  /**
   * A class to store the pixel information for an object defined in a
   * 2-dimensional plane.
   *
   * The object consists of a set of Scans (stored as a std::vector),
   * together with basic information on the maximum, minimum, and
   * average values (actually stored as xSum, ySum) for the x and y
   * pixel values.
   */

  class Object2D
  {
  public:
    Object2D(){numPix=0;};
    Object2D(const Object2D& o);
    Object2D& operator= (const Object2D& o);  
    virtual ~Object2D(){};
  
    /** Clear the Object and set the number of pixels to zero.*/
    void  clear(){scanlist.clear(); numPix=0;};

    /** Make sure there are no overlapping or adjacent scans in the
	Scan list. */
    void  cleanup();

    /** Add a pixel to the Object, making sure no scans overlap
	afterwards. */
    void  addPixel(long x, long y);
    /** Add the (x,y) part of a Voxel to the Object, using
	addPixel(long,long)*/
    void  addPixel(Voxel v){this->addPixel(v.getX(),v.getY());};
    /** Add a Pixel to the Object, using addPixel(long,long)*/
    void  addPixel(Pixel p){this->addPixel(p.getX(),p.getY());};

    /** Add a full Scan to the Object, making sure there are no
	overlapping scans afterwards. */
    void  addScan(Scan scan);

    /** Test whether a pixel (x,y) is in the Object. */
    bool  isInObject(long x, long y);
    /** Test whether the (x,y) part of a Voxel is in the Object.*/
    bool  isInObject(Voxel v){return this->isInObject(v.getX(),v.getY());};
    /** Test whether a Pixel is in the Object.*/
    bool  isInObject(Pixel p){return this->isInObject(p.getX(),p.getY());};

    /** Test whether a given Scan overlaps with any pixels in the Object. */
    bool  scanOverlaps(Scan scan);

    /** Return the number of pixels in the Object. */
    long  getSize(){return numPix;};

    /** Return the number of Scans in the Object. */
    long  getNumScan(){return scanlist.size();};

    /** Get the i-th Scan in the Scan list. */
    Scan  getScan(int i){return scanlist[i];};

    /** Order the Scans in the list, using the < operator for Scans. */
    void  order(){std::stable_sort(scanlist.begin(),scanlist.end());};

    /** Add values to the x- and y-axes, making sure to add the
	offsets to the sums and min/max values. */
    void  addOffsets(long xoff, long yoff){
      for(int i=0;i<scanlist.size();i++) scanlist[i].addOffsets(xoff,yoff);
      xSum += xoff*numPix;
      xmin += xoff; xmax += xoff;
      ySum += yoff*numPix;
      ymin += yoff; ymax += yoff;
    };

    /** Calculate the sums, mins, maxs for x&y -- should not be
	necessary as these are done when pixels & Scans are added. */
    void  calcParams();

    /** Return the average x-value. */
    float getXcentre(){return xSum/float(numPix);};

    /** Return the average y-value. */
    float getYcentre(){return ySum/float(numPix);};

    long  getXmin(){return xmin;};
    long  getYmin(){return ymin;};
    long  getXmax(){return xmax;};
    long  getYmax(){return ymax;};

    /** Return the number of distinct y-values.*/
    long  getNumDistinctY();
    /** Return the number of distinct x-values.*/
    long  getNumDistinctX();
  
    /** A stream output operator. */
    friend std::ostream& operator<< ( std::ostream& theStream, Object2D& obj);

    /** Adding two Objects together. */
    friend Object2D operator+ (Object2D lhs, Object2D rhs){
      Object2D output = lhs;
      for(int s=0;s<rhs.scanlist.size();s++) output.addScan(rhs.scanlist[s]);
      return output;
    }

    friend class Object3D; 
    friend class ChanMap;

  private:
    std::vector<Scan> scanlist;       ///< The list of Scans
    long              numPix;         ///< Number of pixels in the Object
    float             xSum;           ///< Sum of x values
    float             ySum;           ///< Sum of y values
    long              xmin,xmax;      ///< min and max x-values of object
    long              ymin,ymax;      ///< min and max y-values of object
  };

}

#endif //OBJECT2D_H
