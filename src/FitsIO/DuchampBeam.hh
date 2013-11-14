// -----------------------------------------------------------------------
// DuchampBeam.hh: Handling of a beam in the Duchamp context, with Param and 
//                 FITS I/O interfaces
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
#ifndef DUCHAMP_DUCHAMPBEAM_HH
#define DUCHAMP_DUCHAMPBEAM_HH
#include <duchamp/duchamp.hh>
#include <duchamp/FitsIO/Beam.hh>

#include <fitsio.h>

namespace duchamp
{
  
  enum BEAM_ORIGIN { EMPTY,HEADER, PARAM };
  class Param;

  class DuchampBeam : public Beam
  {
  public:
    DuchampBeam();
    DuchampBeam(float maj, float min, float pa);
    virtual ~DuchampBeam(){};
    DuchampBeam(const Beam &b);
    DuchampBeam(const DuchampBeam &b);
    DuchampBeam& operator=(const DuchampBeam &b);

    void empty();
      using Beam::define;
    void define(float maj, float min, float pa, BEAM_ORIGIN origin);
    void setFWHM(float fwhm, BEAM_ORIGIN origin);
    void setArea(float area, BEAM_ORIGIN origin);

    BEAM_ORIGIN origin(){return itsOrigin;};
    std::string originString();
    bool isDefined(){return (itsOrigin != EMPTY);};
    
    void define(Param &par,bool warn=true); // do we need this? How to define directly?

    void readFromFITS(fitsfile *fptr, Param &par, float pixelScale); // read from file a la headerIO.cc
    void writeToFITS(fitsfile *fptr); // write to file, but only if itsOrigin==HEADER. Use cfitsio commands directly.

  protected:
    BEAM_ORIGIN itsOrigin;
    float itsPixelScale;

  };

}

#endif
