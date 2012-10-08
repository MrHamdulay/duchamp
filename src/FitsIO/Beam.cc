// -----------------------------------------------------------------------
// Beam.cc: Basic beam-handling functionality
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
#include <math.h>
#include <duchamp/FitsIO/Beam.hh>
#include <duchamp/duchamp.hh>

namespace duchamp
{

  Beam::Beam():
    itsMaj(0), itsMin(0), itsPA(0), itsArea(0)
  {
  }

  Beam::Beam(float maj, float min, float pa):
    itsMaj(maj),itsMin(min),itsPA(pa)
  {
    this->calculateArea();
  }

  Beam::Beam(const Beam &b)
  {
    operator=(b);
  }

  Beam& Beam::operator=(const Beam &b)
  {
    if(this == &b ) return *this;
    this->itsMaj = b.itsMaj;
    this->itsMin = b.itsMin;
    this->itsPA = b.itsPA;
    this->itsArea = b.itsArea;
    return *this;
  }

  void Beam::calculateArea()
  {
    this->itsArea =  M_PI * (this->itsMaj/2.) * (this->itsMin/2.) / M_LN2;
  }

  void Beam::areaToFWHM()
  {
    float fwhm = 2.*sqrt( this->itsArea * M_LN2 / M_PI );
    this->itsMaj = fwhm;
    this->itsMin = fwhm;
    this->itsPA = 0.;
  }

  void Beam::define(float maj, float min, float pa)
  {
    this->itsMaj = maj;
    this->itsMin = min;
    this->itsPA  = pa;
    this->calculateArea();
  }

  void Beam::setFWHM(float fwhm)
  {
    this->itsMaj = fwhm;
    this->itsMin = fwhm;
    this->itsPA = 0.;
    this->calculateArea();
  }

  void Beam::setArea(float area)
  {
    this->itsArea = area;
    this->areaToFWHM();
  }


}
