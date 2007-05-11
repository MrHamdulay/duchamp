#ifndef FITSHEADER_H
#define FITSHEADER_H

#include <string>
#include <wcs.h>

class Param;

/**
 *  Class to store FITS header information.
 *
 *   Stores information from a FITS header, including WCS information
 *    in the form of a wcsprm struct, as well as various keywords.
 */
class FitsHeader
{

public:
  FitsHeader();
  virtual ~FitsHeader();
  FitsHeader(const FitsHeader& h);
  FitsHeader& operator= (const FitsHeader& h);

  //--------------------
  // Functions in param.cc
  //
  /** Assign correct WCS parameters.  */
  void    setWCS(struct wcsprm *w);

  /** Return the WCS parameters in a WCSLIB wcsprm struct. */
  struct wcsprm *getWCS();

  /** Provides a reference to the WCS parameters*/
  struct wcsprm& WCS(){ struct wcsprm &rwcs = *wcs; return rwcs; }; 

  // front ends to WCS functions
  /** Convert pixel coords to world coords for a single point. */
  int     wcsToPix(const double *world, double *pix);

  /** Convert pixel coords to world coords for many points. */
  int     wcsToPix(const double *world, double *pix, const int npts);

  /** Convert world coords to pixel coords for a single point. */
  int     pixToWCS(const double *pix, double *world);

  /** Convert world coords to pixel coords for many points. */
  int     pixToWCS(const double *pix, double *world, const int npts);

  /** Convert a (x,y,z) position to a velocity. */
  double  pixToVel(double &x, double &y, double &z);

  /** Convert a set of  (x,y,z) positions to a set of velocities. */
  double *pixToVel(double &x, double &y, double *zarray, int size);

  /** Convert a spectral coordinate to a velocity coordinate.*/
  double  specToVel(const double &z);

  /** Convert a velocity coordinate to a spectral coordinate.*/
  double  velToSpec(const float &vel);

  /** Get an IAU-style name for an equatorial or galactic coordinates. */
  std::string  getIAUName(double ra, double dec);

  /** Correct the units for the spectral axis */
  void    fixUnits(Param &par);
 
  //--------------------
  // Functions in FitsIO/headerIO.cc
  //
  /** Read all header info. */
  int     readHeaderInfo(std::string fname, Param &par);

  /** Read BUNIT keyword */
  int     readBUNIT(std::string fname);

  /** Read BLANK & related keywords */
  int     readBLANKinfo(std::string fname, Param &par);

  /** Read beam-related keywords */
  int     readBeamInfo(std::string fname, Param &par);
 
  //--------------------
  // Function in FitsIO/wcsIO.cc
  //
  /** Read the WCS information from a file. */
  int     defineWCS(std::string fname, Param &par);

  //--------------------
  // Basic inline accessor functions
  //
  /** Is the WCS good enough to be used? */
  bool    isWCS(){return wcsIsGood;};
  int     getNWCS(){return nwcs;};
  void    setNWCS(int i){nwcs=i;};
  int     getNumAxes(){return naxis;};
  void    setNumAxes(int i){naxis=i;};
  std::string  getSpectralUnits(){return spectralUnits;};
  void    setSpectralUnits(std::string s){spectralUnits=s;};
  std::string  getSpectralDescription(){return spectralDescription;};
  void    setSpectralDescription(std::string s){spectralDescription=s;};
  std::string  getFluxUnits(){return fluxUnits;};
  void    setFluxUnits(std::string s){fluxUnits=s;};
  std::string  getIntFluxUnits(){return intFluxUnits;};
  void    setIntFluxUnits(std::string s){intFluxUnits=s;};
  float   getBeamSize(){return beamSize;};
  void    setBeamSize(float f){beamSize=f;};
  float   getBmajKeyword(){return bmajKeyword;};
  void    setBmajKeyword(float f){bmajKeyword=f;};
  float   getBminKeyword(){return bminKeyword;};
  void    setBminKeyword(float f){bminKeyword=f;};
  float   getBpaKeyword(){return bpaKeyword;};
  void    setBpaKeyword(float f){bpaKeyword=f;};
  int     getBlankKeyword(){return blankKeyword;};
  void    setBlankKeyword(int f){blankKeyword=f;};
  float   getBzeroKeyword(){return bzeroKeyword;};
  void    setBzeroKeyword(float f){bzeroKeyword=f;};
  float   getBscaleKeyword(){return bscaleKeyword;};
  void    setBscaleKeyword(float f){bscaleKeyword=f;};

  /** Average the pixel scale (eg arcmin/pix) between the two
      spatial axes, and return. */
  float   getAvPixScale(){
    return sqrt( fabs ( (wcs->pc[0]*wcs->cdelt[0])*
			(wcs->pc[wcs->naxis+1]*wcs->cdelt[1])));
  };


private:
  struct wcsprm *wcs;           ///< The WCS parameters for the cube
				///   in a struct from the wcslib
				///   library.
  int     nwcs;                 ///< The number of WCS parameters
  bool    wcsIsGood;            ///< A flag indicating whether there
				///   is a valid WCS present.
  int     naxis;                ///< How many axes are in the header?
  std::string  spectralUnits;        ///< The units of the spectral dimension
  std::string  spectralDescription;  ///< The description of the
				     ///   spectral dimension (Frequency,
				     ///   Velocity, ...)
  std::string  fluxUnits;       ///< The units of pixel flux (from header)
  std::string  intFluxUnits;    ///< The units of integrated flux (from header)
  float   beamSize;             ///< The calculated beam size in pixels.
  float   bmajKeyword;          ///< The FITS header keyword BMAJ.
  float   bminKeyword;          ///< The FITS header keyword BMIN.
  float   bpaKeyword;           ///< The FITS header keyword BPA.
  int     blankKeyword;         ///< The FITS header keyword BLANK.
  float   bzeroKeyword;         ///< The FITS header keyword BZERO.
  float   bscaleKeyword;        ///< The FITS header keyword BSCALE.
  double  scale;                ///< scale parameter for converting
				///   spectral coords
  double  offset;               ///< offset parameter for converting
				///   spectral coords
  double  power;                ///< power parameter for converting
				///   spectral coords
};

#endif // FITSHEADER_H
