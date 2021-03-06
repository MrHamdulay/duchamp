// -----------------------------------------------------------------------
// fitsHeader.hh: Information about the FITS file's header.
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
#ifndef FITSHEADER_H
#define FITSHEADER_H

#include <string>
#include <wcslib/wcs.h>
#include <math.h>
#include <duchamp/duchamp.hh>
#include <duchamp/FitsIO/DuchampBeam.hh>

namespace duchamp
{

  class Param;

  /// 
  /// @brief Class to store FITS header information.
  /// 
  /// @details  Stores information from a FITS header, including WCS information
  ///    in the form of a wcsprm struct, as well as various keywords.
  /// 
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
    /// @brief Assign correct WCS parameters.  
    void    setWCS(struct wcsprm *w);

    /// @brief Return a pointer to the WCS parameters 
    struct wcsprm *getWCS(){return wcs;};

    /// @brief Return the WCS parameters in a WCSLIB wcsprm struct. 
    struct wcsprm *getWCScopy();

    /// @brief Provides a reference to the WCS parameters
    struct wcsprm& WCS(){ struct wcsprm &rwcs = *wcs; return rwcs; }; 

    // front ends to WCS functions
    /// @brief Convert pixel coords to world coords for a single point. 
    int     wcsToPix(double xWorld, double yWorld, double zWorld, double &xPix, double &yPix, double &zPix);

    /// @brief Convert pixel coords to world coords for a single point. 
    int     wcsToPix(const double *world, double *pix);

    /// @brief Convert pixel coords to world coords for many points. 
    int     wcsToPix(const double *world, double *pix, const int npts);

    /// @brief Convert world coords to pixel coords for a single point. 
    int     pixToWCS(double xPix, double yPix, double zPix, double &xWorld, double &yWorld, double &zWorld);

    /// @brief Convert world coords to pixel coords for a single point. 
    int     pixToWCS(const double *pix, double *world);

    /// @brief Convert world coords to pixel coords for many points. 
    int     pixToWCS(const double *pix, double *world, const int npts);

    /// @brief Convert a (x,y,z) position to a velocity. 
    double  pixToVel(double &x, double &y, double &z);

    /// @brief Convert a set of  (x,y,z) positions to a set of velocities. 
    double *pixToVel(double &x, double &y, double *zarray, int size);

    /// @brief Convert a spectral coordinate to a velocity coordinate.
    double  specToVel(const double &z);

    /// @brief Convert a velocity coordinate to a spectral coordinate.
    double  velToSpec(const float &vel);

    /// @brief Get an IAU-style name for an equatorial or galactic coordinates. 
    std::string  getIAUName(double ra, double dec);

    /// @brief Correct the units for the spectral axis 
    void    fixSpectralUnits(std::string units);
    
    /// @brief Define the units for integrated flux 
    void    setIntFluxUnits();

    //--------------------
    // Functions in FitsIO/headerIO.cc
    //
    /// @brief Read all header info. 
    OUTCOME     readHeaderInfo(std::string fname, Param &par);
    OUTCOME     readHeaderInfo(Param &par);
    OUTCOME     readHeaderInfo(fitsfile *fptr, Param &par);

    /// @brief Read BUNIT keyword 
    // OUTCOME     readBUNIT(std::string fname);
    OUTCOME     readBUNIT(fitsfile *fptr);

    /// @brief Read BLANK & related keywords 
    // OUTCOME     readBLANKinfo(std::string fname, Param &par);
    OUTCOME     readBLANKinfo(fitsfile *fptr, Param &par);

    /// @brief Read beam-related keywords 
    // OUTCOME     readBeamInfo(std::string fname, Param &par);
    OUTCOME     readBeamInfo(fitsfile *fptr, Param &par);
 
    //--------------------
    // Function in FitsIO/wcsIO.cc
    //
    /// @brief Read the WCS information from a file. 
    OUTCOME     defineWCS(std::string fname, Param &par);
    OUTCOME     defineWCS(Param &par);
    OUTCOME     defineWCS(fitsfile *fptr, Param &par);
    OUTCOME     defineWCS(wcsprm *theWCS, int nWCS, size_t *dimAxes, Param &par);

    //--------------------
    // Basic inline accessor functions
    //
    fitsfile *FPTR(){return fptr;};
    int     openFITS(std::string name);
    int     closeFITS();
    /// @brief Is the WCS good enough to be used? 
    bool    isWCS(){return wcsIsGood;};
    /// @brief Is the spectral axis OK to be used? 
    bool    isSpecOK(){return (wcs->spec >= 0);};
    bool    canUseThirdAxis(){return ((wcs->spec >= 0)||(wcs->naxis>2));};
    void    set2D(bool b){flag2D = b;};
    bool    is2D(){return flag2D;};
    int     getNWCS(){return nwcs;};
    void    setNWCS(int i){nwcs=i;};
    int     getNumAxes(){if(wcs->flag==-1) return 0; else return wcs->naxis;};
    void    setNumAxes(int i){wcs->naxis=i;};
    std::string  getSpectralUnits(){return spectralUnits;};
    void    setSpectralUnits(std::string s){spectralUnits=s;};
    std::string  getSpectralType(){return spectralType;};
    void    setSpectralType(std::string s){spectralType=s;};
    std::string  getSpectralDescription(){return spectralDescription;};
    void    setSpectralDescription(std::string s){spectralDescription=s;};
    std::string  getOrigFluxUnits(){return originalFluxUnits;};
    void    setOrigFluxUnits(std::string s){originalFluxUnits=s;};
    std::string  getFluxUnits(){return fluxUnits;};
    void    setFluxUnits(std::string s){fluxUnits=s;};
    std::string  getIntFluxUnits(){return intFluxUnits;};
    void    setIntFluxUnits(std::string s){intFluxUnits=s;};
    DuchampBeam getBeam(){return itsBeam;};
    DuchampBeam& beam(){DuchampBeam& rbeam=itsBeam; return rbeam;};
    void    setBeam(DuchampBeam &b){itsBeam=b;};
    int     getBlankKeyword(){return blankKeyword;};
    void    setBlankKeyword(int f){blankKeyword=f;};
    float   getBzeroKeyword(){return bzeroKeyword;};
    void    setBzeroKeyword(float f){bzeroKeyword=f;};
    float   getBscaleKeyword(){return bscaleKeyword;};
    void    setBscaleKeyword(float f){bscaleKeyword=f;};
    std::string lngtype();
    std::string lattype();

    /// @brief Return the average pixel scale (eg deg/pix) of the two spatial axes. 
    float   getAvPixScale(){
      return sqrt( fabs ( (wcs->pc[0]*wcs->cdelt[0])*
			  (wcs->pc[wcs->naxis+1]*wcs->cdelt[1])));
    };

    float getShapeScale();
    std::string getShapeUnits();

    bool    needBeamSize();

  private:
    fitsfile      *fptr;                ///< A pointer to the FITS file that can be used by the cftisio library.
    struct wcsprm *wcs;                 ///< The WCS parameters for the cube in a struct from the wcslib library.
    int            nwcs;                ///< The number of WCS parameters
    bool           wcsIsGood;           ///< A flag indicating whether there is a valid WCS present.
    bool           flag2D;              ///< Is the image only a 2D one (leaving out redundant dimensions of size 1)?
    std::string    spectralUnits;       ///< The units of the spectral dimension
    std::string    spectralType;        ///< The spectral type - first 4 letters of the WCS ctype code. Used for the results output.
    std::string    spectralDescription; ///< The description of the spectral dimension (Frequency, Velocity, ...)
    std::string    originalFluxUnits;   ///< The units of pixel flux in the original image
    std::string    fluxUnits;           ///< The units of pixel flux as used (either from image or from parameter set)
    std::string    intFluxUnits;        ///< The units of integrated flux (from header)
    DuchampBeam    itsBeam;             ///< The beam information
    int            blankKeyword;        ///< The FITS header keyword BLANK.
    float          bzeroKeyword;        ///< The FITS header keyword BZERO.
    float          bscaleKeyword;       ///< The FITS header keyword BSCALE.
    double         scale;               ///< scale parameter for converting spectral coords
    double         offset;              ///< offset parameter for converting spectral coords
    double         power;               ///< power parameter for converting spectral coords
  };

}

#endif // FITSHEADER_H
