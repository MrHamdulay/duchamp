// -----------------------------------------------------------------------
// Beam.hh: Basic beam-handling functionality
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
#ifndef DUCHAMP_BEAM_HH
#define DUCHAMP_BEAM_HH

#include <iostream>

namespace duchamp
{

  class Beam 
  {
  public:
    Beam();
    Beam(float maj, float min, float pa);
    virtual ~Beam(){};
    Beam(const Beam &b);
    Beam& operator=(const Beam &b);

    virtual void define(float maj, float min, float pa);
    void setFWHM(float fwhm);
    void setArea(float area);

    float maj(){return itsMaj;};
    float min(){return itsMin;};
    float pa(){return itsPA;};
    float area(){return itsArea;};

  protected:
    float itsMaj;
    float itsMin;
    float itsPA;
    float itsArea;

    void calculateArea();
    void areaToFWHM();

  };





}

#endif
