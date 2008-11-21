// -----------------------------------------------------------------------
// Scan.hh: Definition of the Scan class, used to store row
//          information as part of a 2D object.
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
#ifndef SCAN_H
#define SCAN_H

#include <iostream>
#include <vector>

/**
 * This namespace will store all the classes and functions necessary
 * to encode shapes and objects in 1-, 2- and 3-dimensions.
 */
namespace PixelInfo
{

  /**
   * A class to store the basic unit of pixel information, a scan
   * encoded by an (x,y) starting point, and a length (in the
   * x-direction).
   *
   * This class is used by other classes to store objects in 2- and
   * 3-dimensions.
   */

  class Scan
  {
  public:
    Scan();
    Scan(long y, long x, long xl){itsY=y; itsX=x; itsXLen=xl;};
    Scan(const Scan& s);
    Scan& operator= (const Scan& s);
    virtual ~Scan(){};

    /** Define a Scan by providing the three key parameters. */ 
    void define(long y, long x, long xl){itsY=y; itsX=x; itsXLen=xl;};

    /** Set the Scan to the null values, with the length=0. */
    void clear(){itsY=-1;itsX=-1;itsXLen=0;};

    // Accessor functions -- obvious.
    long getY(){return itsY;};
    void setY(long l){itsY=l;};
    long getX(){return itsX;};
    void setX(long l){itsX=l;};
    long getXlen(){return itsXLen;};
    void setXlen(long l){itsXLen=l;};

    /** An easy way to get the maximum x-value */
    long getXmax(){return itsX+itsXLen-1;};

    /** A way of setting the length by proxy, giving the maximum x-value. */
    void setXmax(long l){itsXLen = l-itsX+1;};

    /** Add a point to the left of the scan (ie.\ add the point itsX-1).*/
    void growLeft(){itsX--;itsXLen++;};

    /** Add a point to the right of the scan (ie.\ add the point xmax+1).*/
    void growRight(){itsXLen++;};

    /** Add values to the x- and y-axes. */
    void addOffsets(long xoff, long yoff){itsY+=yoff; itsX+=xoff;};

    /** Tests whether a given (x,y) point is in the scan.*/
    bool isInScan(long x, long y);

    /** Stream output operator for the Scan */
    friend std::ostream& operator<< ( std::ostream& theStream, Scan& scan);

    /** Less-than operator for Scans */
    friend bool operator< (Scan lhs, Scan rhs);

    /** Test whether one scan is equal to another. */
    friend bool operator== (Scan lhs, Scan rhs);

    friend class Object2D; ///< Enable Object2D to see the private members.

  private:
    long itsY;    ///< The y-value of each point in the scan.
    long itsX;    ///< The x-value of the start (left-hand end) of the scan.
    long itsXLen; ///< The length of the scan (number of pixels in the scan).

  };

  /** Combine two scans into one. */
  Scan unite(Scan &s1, Scan &s2);

  /** Keep only the pixels in both the two scans. */
  Scan intersect(Scan &s1, Scan &s2);

  /** Test whether two scans either overlap or are adjacent. */
  bool touching(Scan &s1, Scan &s2);

  /** Test whether two scans have pixels in common */
  bool overlap(Scan &s1, Scan &s2);

  /** Test whether two scans lie adjacent to each other (but not
      overlapping).*/
  bool adjacent(Scan &scan1, Scan &scan2);

  /** Return the null scan, y=-1, x=-1, xlen=0.*/
  Scan nullScan();

  /** Examine a vector list of Scans and merge any that are touching. */
  void mergeList(std::vector<Scan> scanlist);

  /** Get the minimum separation, in pixels, between two scans. */
  float minSep(Scan &s1, Scan &s2);

}

#endif //SCAN_H
