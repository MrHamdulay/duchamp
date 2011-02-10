#include <iostream>
#include <math.h>
#include <FitsIO/Beam.hh>
#include <duchamp.hh>

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
