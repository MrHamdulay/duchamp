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
