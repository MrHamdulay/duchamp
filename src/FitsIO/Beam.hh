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
