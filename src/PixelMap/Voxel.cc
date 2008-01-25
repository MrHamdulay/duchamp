// -----------------------------------------------------------------------
// Voxel.cc: Member functions for the Voxel class.
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
#include <iomanip>
#include <duchamp/PixelMap/Voxel.hh>

namespace PixelInfo
{

  Voxel::Voxel(long x, long y, long z, float f)
  { 
    this->itsX=x; 
    this->itsY=y; 
    this->itsZ=z; 
    this->itsF=f;
  }
  //--------------------------------------------------------------------

  Voxel::Voxel(const Voxel& v)
  {
    operator=(v);
  }
  //--------------------------------------------------------------------

  Voxel& Voxel::operator= (const Voxel& v)
  {
    if(this == &v) return *this;
    this->itsX=v.itsX; 
    this->itsY=v.itsY; 
    this->itsZ=v.itsZ; 
    this->itsF=v.itsF;
    return *this;
  }
  //--------------------------------------------------------------------

  std::ostream& operator<< ( std::ostream& theStream, Voxel& vox)
  {
    /**
     * A convenient way of printing the coordinate and flux values of
     * a voxel.  They are all printed to a single line (with no
     * carriage-return), with the flux to precision of 4.
     */  

    theStream << std::setw(4) << vox.itsX ;
    theStream << " " << std::setw(4) << vox.itsY;
    theStream << " " << std::setw(4) << vox.itsZ;
    theStream << std::setprecision(4);
    theStream << "  " << vox.itsF;
    return theStream;

  }
  //------------------------------------------------------

  bool operator== (Voxel lhs, Voxel rhs)
  {
    /**
     * For two voxels to be equal, all four parameters must be equal.
     */ 
    return (lhs.itsX == rhs.itsX) &&
      (lhs.itsY == rhs.itsY) &&
      (lhs.itsZ == rhs.itsZ) &&
      (lhs.itsF == rhs.itsF);
  }
  //------------------------------------------------------

  bool Voxel::match(Voxel other)
  {
    /**
     * This function just tests for equality of position. The flux is ignored.
     */ 
    return (this->itsX == other.itsX) &&
      (this->itsY == other.itsY) &&
      (this->itsZ == other.itsZ);
  }
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------

  Pixel::Pixel(long x, long y, float f)
  { 
    this->itsX=x; 
    this->itsY=y; 
    this->itsF=f;
  }
  //--------------------------------------------------------------------

  Pixel::Pixel(const Pixel& p)
  {
    operator=(p);
  }
  //--------------------------------------------------------------------

  Pixel& Pixel::operator= (const Pixel& p)
  {
    if(this == &p) return *this;
    this->itsX=p.itsX; 
    this->itsY=p.itsY; 
    this->itsF=p.itsF;
    return *this;
  }
  //--------------------------------------------------------------------

}
