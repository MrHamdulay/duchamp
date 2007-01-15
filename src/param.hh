#ifndef PARAM_H
#define PARAM_H

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <wcs.h>
#include <wcshdr.h>

using std::string;
using std::vector;

class FitsHeader; // foreshadow this so that Param knows it exists

/**
 * Class to store general parameters.
 *
 * This is a general repository for parameters that are used by all
 * functions. This is how the user interacts with the program, as
 * parameters are read in from disk files through functions in this
 * class.
 */

class Param
{
public:
  Param();
  virtual ~Param(){if(sizeOffsets>0) delete [] offsets;};
  Param(const Param& p);
  Param& operator= (const Param& p);
  //-----------------
  // Functions in param.cc
  //
  /** Read in parameters from a disk file. */
  int    readParams(string paramfile);

  /** Copy certain necessary FITS header parameters from a FitsHeader object */
  void   copyHeaderInfo(FitsHeader &head);

  /** Determine filename in which to save the Hanning-smoothed array. */
  string outputSmoothFile();

  /** Determine filename in which to save the reconstructed array. */
  string outputReconFile(); 

  /** Determine filename in which to save the residual array from the atrous reconstruction. */
  string outputResidFile(); 

  /** Print the parameter set in a readable fashion. */
  friend std::ostream& operator<< ( std::ostream& theStream, Param& par);
  friend class Image;
  //------------------
  // Functions in FitsIO/subsection.cc
  //
  /** Make sure the subsection string is OK, and read the axis subsections. */
  int    verifySubsection();

  /** Set the correct offset values for each axis */
  void   setOffsets(struct wcsprm *wcs);
  //--------------------
  // These are inline functions.
  //
  /** Is a pixel value a BLANK?
   *  Tests whether the value passed as the argument is BLANK or not.
   *   If flagBlankPix is false, return false.
   *   Otherwise, compare to the relevant FITS keywords, using integer comparison.
   */
  bool   isBlank(float &value){
    return this->flagBlankPix &&
      (this->blankKeyword == int((value-this->bzeroKeyword)/this->bscaleKeyword));
  };

  /** Is a given channel flagged as being in the Milky Way?*/           
  bool   isInMW(int z){return ( flagMW && (z>=minMW) && (z<=maxMW) );};

  //--------------------
  // Basic inline accessor functions
  //
  string getImageFile(){return imageFile;};
  void   setImageFile(string fname){imageFile = fname;};
  string getFullImageFile(){
    if(flagSubsection) return imageFile+subsection;
    else return imageFile;
  };
  bool   getFlagSubsection(){return flagSubsection;};
  void   setFlagSubsection(bool flag){flagSubsection=flag;};
  string getSubsection(){return subsection;};
  void   setSubsection(string range){subsection = range;};
  bool   getFlagReconExists(){return flagReconExists;};
  void   setFlagReconExists(bool flag){flagReconExists=flag;};
  string getReconFile(){return reconFile;};
  void   setReconFile(string file){reconFile = file;};
  bool   getFlagSmoothExists(){return flagSmoothExists;};
  void   setFlagSmoothExists(bool flag){flagSmoothExists=flag;};
  string getSmoothFile(){return smoothFile;};
  void   setSmoothFile(string file){smoothFile = file;};
  //
  bool   getFlagLog(){return flagLog;};
  void   setFlagLog(bool flag){flagLog=flag;};
  string getLogFile(){return logFile;};
  void   setLogFile(string fname){logFile = fname;};
  string getOutFile(){return outFile;};
  void   setOutFile(string fname){outFile = fname;};
  string getSpectraFile(){return spectraFile;};
  void   setSpectraFile(string fname){spectraFile = fname;};
  bool   getFlagOutputSmooth(){return flagOutputSmooth;};
  void   setFlagOutputSmooth(bool flag){flagOutputSmooth=flag;};
  bool   getFlagOutputRecon(){return flagOutputRecon;};
  void   setFlagOutputRecon(bool flag){flagOutputRecon=flag;};
  bool   getFlagOutputResid(){return flagOutputResid;};
  void   setFlagOutputResid(bool flag){flagOutputResid=flag;};
  bool   getFlagVOT(){return flagVOT;};
  void   setFlagVOT(bool flag){flagVOT=flag;};
  string getVOTFile(){return votFile;};
  void   setVOTFile(string fname){votFile = fname;};
  bool   getFlagKarma(){return flagKarma;};
  void   setFlagKarma(bool flag){flagKarma=flag;};
  string getKarmaFile(){return karmaFile;};
  void   setKarmaFile(string fname){karmaFile = fname;};
  bool   getFlagMaps(){return flagMaps;};
  void   setFlagMaps(bool flag){flagMaps=flag;};
  string getDetectionMap(){return detectionMap;};
  void   setDetectionMap(string fname){detectionMap = fname;};
  string getMomentMap(){return momentMap;};
  void   setMomentMap(string fname){momentMap = fname;};
  bool   getFlagXOutput(){return flagXOutput;};
  void   setFlagXOutput(bool b){flagXOutput=b;};
  //
  bool   getFlagNegative(){return flagNegative;};
  void   setFlagNegative(bool flag){flagNegative=flag;};
  bool   getFlagBlankPix(){return flagBlankPix;};
  void   setFlagBlankPix(bool flag){flagBlankPix=flag;};
  float  getBlankPixVal(){return blankPixValue;};
  void   setBlankPixVal(float v){blankPixValue=v;};
  int    getBlankKeyword(){return blankKeyword;};
  void   setBlankKeyword(int v){blankKeyword=v;};
  float  getBscaleKeyword(){return bscaleKeyword;};
  void   setBscaleKeyword(float v){bscaleKeyword=v;};
  float  getBzeroKeyword(){return bzeroKeyword;};
  void   setBzeroKeyword(float v){bzeroKeyword=v;};
  bool   getFlagUsingBlank(){return flagUsingBlank;};
  void   setFlagUsingBlank(bool b){flagUsingBlank=b;};
  bool   getFlagMW(){return flagMW;};
  bool   setFlagMW(bool flag){flagMW=flag;};
  int    getMaxMW(){return maxMW;};
  void   setMaxMW(int m){maxMW=m;};
  int    getMinMW(){return minMW;};
  void   setMinMW(int m){minMW=m;};
  void   setBeamSize(float s){numPixBeam = s;};
  float  getBeamSize(){return numPixBeam;};
  bool   getFlagUsingBeam(){return flagUsingBeam;};
  void   setFlagUsingBeam(bool b){flagUsingBeam=b;};
  //
  bool   getFlagCubeTrimmed(){return flagTrimmed;};
  void   setFlagCubeTrimmed(bool flag){flagTrimmed = flag;};
  long   getBorderLeft(){return borderLeft;};
  void   setBorderLeft(long b){borderLeft = b;};
  long   getBorderRight(){return borderRight;};
  void   setBorderRight(long b){borderRight = b;};
  long   getBorderBottom(){return borderBottom;};
  void   setBorderBottom(long b){borderBottom = b;};
  long   getBorderTop(){return borderTop;};
  void   setBorderTop(long b){borderTop = b;};
  //
  long   getXOffset(){return xSubOffset;};
  void   setXOffset(long o){xSubOffset = o;};
  long   getYOffset(){return ySubOffset;};
  void   setYOffset(long o){ySubOffset = o;};
  long   getZOffset(){return zSubOffset;};
  void   setZOffset(long o){zSubOffset = o;};
  //
  int    getMinPix(){return minPix;};
  void   setMinPix(int m){minPix=m;};
  //	 
  bool   getFlagGrowth(){return flagGrowth;};
  void   setFlagGrowth(bool flag){flagGrowth=flag;};
  float  getGrowthCut(){return growthCut;};
  void   setGrowthCut(float c){growthCut=c;};
  //	 
  bool   getFlagFDR(){return flagFDR;};
  void   setFlagFDR(bool flag){flagFDR=flag;};
  float  getAlpha(){return alphaFDR;};
  void   setAlpha(float a){alphaFDR=a;};
  //
  bool   getFlagBaseline(){return flagBaseline;};
  void   setFlagBaseline(bool flag){flagBaseline = flag;};
  //
  float  getCut(){return snrCut;};
  void   setCut(float c){snrCut=c;};
  float  getThreshold(){return threshold;};
  void   setThreshold(float f){threshold=f;};
  bool   getFlagUserThreshold(){return flagUserThreshold;};
  void   setFlagUserThreshold(bool b){flagUserThreshold=b;};
  //	 
  bool   getFlagSmooth(){return flagSmooth;};
  void   setFlagSmooth(bool b){flagSmooth=b;};
  int    getHanningWidth(){return hanningWidth;};
  void   setHanningWidth(int f){hanningWidth=f;};
  //	 
  bool   getFlagATrous(){return flagATrous;};
  void   setFlagATrous(bool flag){flagATrous=flag;};
  int    getReconDim(){return reconDim;};
  void   setReconDim(int i){reconDim=i;};
  int    getMinScale(){return scaleMin;};
  void   setMinScale(int s){scaleMin=s;};
  float  getAtrousCut(){return snrRecon;};
  void   setAtrousCut(float c){snrRecon=c;};
  int    getFilterCode(){return filterCode;};
  void   setFilterCode(int c){filterCode=c;};
  string getFilterName(){return filterName;};
  void   setFilterName(string s){filterName=s;};
  //	 
  bool   getFlagAdjacent(){return flagAdjacent;};
  void   setFlagAdjacent(bool flag){flagAdjacent=flag;};
  float  getThreshS(){return threshSpatial;};
  void   setThreshS(float t){threshSpatial=t;};
  float  getThreshV(){return threshVelocity;};
  void   setThreshV(float t){threshVelocity=t;};
  int    getMinChannels(){return minChannels;};
  void   setMinChannels(int n){minChannels=n;};
  //
  string getSpectralMethod(){return spectralMethod;};
  void   setSpectralMethod(string s){spectralMethod=s;};
  string getSpectralUnits(){return spectralUnits;};
  void   setSpectralUnits(string s){spectralUnits=s;};
  bool   drawBorders(){return borders;};
  void   setDrawBorders(bool f){borders=f;};
  bool   drawBlankEdge(){return blankEdge;};
  void   setDrawBlankEdge(bool f){blankEdge=f;};

  /** Are we in verbose mode? */
  bool   isVerbose(){return verbose;};
  void   setVerbosity(bool f){verbose=f;};
  
private:
  // Input files
  string imageFile;       ///< The image to be analysed.
  bool   flagSubsection;  ///< Whether we just want a subsection of the image
  string subsection;      ///< The subsection requested, taking the form [x1:x2,y1:y2,z1:z2]. If you want the full range of one index, use *
  bool   flagReconExists; ///< The reconstructed array is in a FITS file on disk.
  string reconFile;       ///< The FITS file containing the reconstructed array.
  bool   flagSmoothExists;///< The Hanning-smoothed array is in a FITS file.
  string smoothFile;      ///< The FITS file containing the smoothed array.

  // Output files
  bool   flagLog;         ///< Should we do the intermediate logging?
  string logFile;         ///< Where the intermediate logging goes.
  string outFile;         ///< Where the final results get put.
  string spectraFile;     ///< Where the spectra are displayed
  bool   flagOutputSmooth;///< Should the Hanning-smoothed cube be written?
  bool   flagOutputRecon; ///< Should the reconstructed cube be written?
  bool   flagOutputResid; ///< Should the reconstructed cube be written?
  bool   flagVOT;         ///< Should we save results in VOTable format?
  string votFile;         ///< Where the VOTable goes.
  bool   flagKarma;       ///< Should we save results in Karma annotation format?
  string karmaFile;       ///< Where the Karma annotation file goes.
  bool   flagMaps;        ///< Should we produce detection and moment maps in postscript form?
  string detectionMap;    ///< The name of the detection map (ps file).
  string momentMap;       ///< The name of the 0th moment map (ps file).
  bool   flagXOutput;     ///< Should there be an xwindows output of the detection map?

  // Cube related parameters 
  bool   flagNegative;    ///< Are we going to search for negative features? 
  bool   flagBlankPix;    ///< A flag that indicates whether there are pixels defined as BLANK and whether we need to remove & ignore them in processing.
  float  blankPixValue;   ///< Pixel value that is considered BLANK.
  int    blankKeyword;    ///< The FITS header keyword BLANK.
  float  bscaleKeyword;   ///< The FITS header keyword BSCALE.
  float  bzeroKeyword;    ///< The FITS header keyword BZERO.
  bool   flagUsingBlank;  ///< If true, we are using the blankPixValue keyword, 
                          ///< otherwise we use the value in the FITS header.
  bool   flagMW;          ///< A flag that indicates whether to ignore the Milky Way channels.
  int    maxMW;           ///< Last  Galactic velocity plane for HIPASS cubes
  int    minMW;           ///< First Galactic velocity plane for HIPASS cubes
  float  numPixBeam;      ///< Size (area) of the beam in pixels.
  bool   flagUsingBeam;   ///< If true, we are using the numPixBeam parameter, 
                          ///< otherwise we use the value in the FITS header.
  // Trim-related
  bool   flagTrimmed;     ///< Has the cube been trimmed of excess BLANKs around the edge?
  long   borderLeft;      ///< The number of BLANK pixels trimmed from the left of the cube;
  long   borderRight;     ///< The number trimmed from the Right of the cube;
  long   borderBottom;    ///< The number trimmed from the Bottom of the cube;
  long   borderTop;       ///< The number trimmed from the Top of the cube;
  // Subsection offsets
  long  *offsets;         ///< The array of offsets for each FITS axis.
  long   sizeOffsets;     ///< The size of the offsets array.
  long   xSubOffset;      ///< The offset in the x-direction from the subsection
  long   ySubOffset;      ///< The offset in the y-direction from the subsection
  long   zSubOffset;      ///< The offset in the z-direction from the subsection
  // Baseline related
  bool   flagBaseline;    ///< Whether to do baseline subtraction before reconstruction and/or searching.
  // Detection-related
  int    minPix;          ///< Minimum number of pixels for a detected object to be counted
  // Object growth
  bool   flagGrowth;      ///< Are we growing objects once they are found?
  float  growthCut;       ///< The SNR that we are growing objects down to.
  // FDR analysis
  bool   flagFDR;         ///< Should the FDR method be used? 
  float  alphaFDR;        ///< Alpha value for FDR detection algorithm
  // Basic detection
  float  snrCut;          ///< How many sigma above mean is a detection when sigma-clipping
  float  threshold;       ///< What the threshold is (when sigma-clipping).
  bool   flagUserThreshold;///< Whether the user has defined a threshold of their own.
  // Smoothing of the cube
  bool   flagSmooth;      ///< Should the cube be smoothed before searching?
  int    hanningWidth;    ///< Width for hanning smoothing.
  // A trous reconstruction parameters
  bool   flagATrous;      ///< Are we using the a trous reconstruction?
  int    reconDim;        ///< How many dimensions to use for the reconstruction?
  int    scaleMin;        ///< Min scale used in a trous reconstruction
  float  snrRecon;        ///< SNR cutoff used in a trous reconstruction (only wavelet coefficients that survive this threshold are kept)
  int    filterCode;      ///< The code number for the filter to be used (saves having to parse names)
  string filterName;      ///< The code number converted into a name, for outputting purposes.

  // Volume-merging parameters
  bool   flagAdjacent;    ///< Whether to use the adjacent criterion for judging if objects are to be merged.
  float  threshSpatial;   ///< Maximum spatial separation between objects
  float  threshVelocity;  ///< Maximum channels separation between objects
  int    minChannels;     ///< Minimum no. of channels to make an object 
  // Input-Output related
  string spectralMethod;  ///< A string indicating choice of spectral plotting method: choices are "peak" (default) or "sum" 
  string spectralUnits;   ///< A string indicating what units the spectral axis should be quoted in.
  bool   borders;         ///< Whether to draw a border around the individual pixels of a detection in the spectral display
  bool   blankEdge;       ///< Whether to draw a border around the BLANK pixel region in the moment maps and cutout images
  bool   verbose;         ///< Whether to use maximum verbosity -- use progress indicators in the reconstruction & merging steps.

};

//===========================================================================

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
  virtual ~FitsHeader(){wcsvfree(&nwcs,&wcs);};
  FitsHeader(const FitsHeader& h);
  FitsHeader& operator= (const FitsHeader& h);

  //--------------------
  // Functions in param.cc
  //
  /** Assign correct WCS parameters.  */
  void    setWCS(struct wcsprm *w);

  /** Return the WCS parameters in a WCSLIB wcsprm struct. */
  struct wcsprm *getWCS();

  // front ends to WCS functions
  /** Convert pixel coords to world coords for a single point. */
  int     wcsToPix(const double *world, double *pix){  
    return wcsToPixSingle(this->wcs, world, pix);
  }

  /** Convert pixel coords to world coords for many points. */
  int     wcsToPix(const double *world, double *pix, const int npts){  
    return wcsToPixMulti(this->wcs, world, pix, npts);
  }

  /** Convert world coords to pixel coords for a single point. */
  int     pixToWCS(const double *pix, double *world){  
    return pixToWCSSingle(this->wcs, pix, world);
  }

  /** Convert world coords to pixel coords for many points. */
  int     pixToWCS(const double *pix, double *world, const int npts){  
    return pixToWCSMulti(this->wcs, pix,world, npts);
  }

  /** Convert a (x,y,z) position to a velocity. */
  double  pixToVel(double &x, double &y, double &z);

  /** Convert a set of  (x,y,z) positions to a set of velocities. */
  double *pixToVel(double &x, double &y, double *zarray, int size);

  /** Convert a spectral coordinate to a velocity coordinate.*/
  double  specToVel(const double &z);

  /** Convert a velocity coordinate to a spectral coordinate.*/
  double  velToSpec(const float &vel);

  /** Get an IAU-style name for an equatorial or galactic coordinates. */
  string  getIAUName(double ra, double dec);

  /** Correct the units for the spectral axis */
  void    fixUnits(Param &par);
 
  //--------------------
  // Functions in FitsIO/headerIO.cc
  //
  /** Read all header info. */
  int     readHeaderInfo(string fname, Param &par);

  /** Read BUNIT keyword */
  int     readBUNIT(string fname);

  /** Read BLANK & related keywords */
  int     readBLANKinfo(string fname, Param &par);

  /** Read beam-related keywords */
  int     readBeamInfo(string fname, Param &par);
 
  //--------------------
  // Function in FitsIO/wcsIO.cc
  //
  /** Read the WCS information from a file. */
  int     defineWCS(string fname, Param &par);

  //--------------------
  // Basic inline accessor functions
  //
  bool    isWCS(){return wcsIsGood;};
  int     getNWCS(){return nwcs;};
  void    setNWCS(int i){nwcs=i;};
  string  getSpectralUnits(){return spectralUnits;};
  void    setSpectralUnits(string s){spectralUnits=s;};
  string  getSpectralDescription(){return spectralDescription;};
  void    setSpectralDescription(string s){spectralDescription=s;};
  string  getFluxUnits(){return fluxUnits;};
  void    setFluxUnits(string s){fluxUnits=s;};
  string  getIntFluxUnits(){return intFluxUnits;};
  void    setIntFluxUnits(string s){intFluxUnits=s;};
  float   getBeamSize(){return beamSize;};
  void    setBeamSize(float f){beamSize=f;};
  float   getBmajKeyword(){return bmajKeyword;};
  void    setBmajKeyword(float f){bmajKeyword=f;};
  float   getBminKeyword(){return bminKeyword;};
  void    setBminKeyword(float f){bminKeyword=f;};
  int     getBlankKeyword(){return blankKeyword;};
  void    setBlankKeyword(int f){blankKeyword=f;};
  float   getBzeroKeyword(){return bzeroKeyword;};
  void    setBzeroKeyword(float f){bzeroKeyword=f;};
  float   getBscaleKeyword(){return bscaleKeyword;};
  void    setBscaleKeyword(float f){bscaleKeyword=f;};
  float   getAvPixScale(){
    return sqrt( fabs ( (wcs->pc[0]*wcs->cdelt[0])*
			(wcs->pc[wcs->naxis+1]*wcs->cdelt[1])));
  };


private:
  struct wcsprm *wcs;           ///< The WCS parameters for the cube in a struct from the wcslib library.
  int     nwcs;                 ///< The number of WCS parameters
  bool    wcsIsGood;            ///< A flag indicating whether there is a valid WCS present.
  string  spectralUnits;        ///< The units of the spectral dimension
  string  spectralDescription;  ///< The description of the spectral dimension (Frequency, Velocity, ...)
  string  fluxUnits;            ///< The units of pixel flux (from header)
  string  intFluxUnits;         ///< The units of pixel flux (from header)
  float   beamSize;             ///< The calculated beam size in pixels.
  float   bmajKeyword;          ///< The FITS header keyword BMAJ.
  float   bminKeyword;          ///< The FITS header keyword BMIN.
  int     blankKeyword;         ///< The FITS header keyword BLANK.
  float   bzeroKeyword;         ///< The FITS header keyword BZERO.
  float   bscaleKeyword;        ///< The FITS header keyword BSCALE.
  double  scale;                ///< scale param for converting spectral coords
  double  offset;               ///< offset param for converting spectral coords
  double  power;                ///< power param for converting spectral coords
};

string makelower( string s );

#endif
