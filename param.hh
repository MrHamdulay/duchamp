#ifndef PARAM_H
#define PARAM_H

#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;

/**
 * Param class.
 *   Used for storing parameters used by all functions.
 */
class Param
{
public:
  Param();
  virtual ~Param(){};
  void parseSubsection();                           // in param.cc
  void readParams(string &paramfile);               // in param.cc
  bool isBlank(float &value);                       // in param.cc
  friend std::ostream& operator<< ( std::ostream& theStream, Param& par);
  friend class Image;
 
  //
  string getImageFile(){return imageFile;};
  void   setImageFile(string fname){imageFile = fname;};
  bool   getFlagSubsection(){return flagSubsection;};
  void   setFlagSubsection(bool flag){flagSubsection=flag;};
  string getSubsection(){return subsection;};
  void   setSubsection(string range){subsection = range;};
  bool   getFlagLog(){return flagLog;};
  void   setFlagLog(bool flag){flagLog=flag;};
  string getLogFile(){return logFile;};
  void   setLogFile(string fname){logFile = fname;};
  string getOutFile(){return outFile;};
  void   setOutFile(string fname){outFile = fname;};
  string getSpectraFile(){return spectraFile;};
  void   setSpectraFile(string fname){spectraFile = fname;};
  bool   getFlagOutputRecon(){return flagOutputRecon;};
  void   setFlagOutputRecon(bool flag){flagOutputRecon=flag;};
  bool   getFlagOutputResid(){return flagOutputResid;};
  void   setFlagOutputResid(bool flag){flagOutputResid=flag;};
  bool   getFlagVOT(){return flagVOT;};
  void   setFlagVOT(bool flag){flagVOT=flag;};
  string getVOTFile(){return votFile;};
  void   setVOTFile(string fname){votFile = fname;};
  bool   getFlagMaps(){return flagMaps;};
  void   setFlagMaps(bool flag){flagMaps=flag;};
  string getDetectionMap(){return detectionMap;};
  void   setDetectionMap(string fname){detectionMap = fname;};
  string getMomentMap(){return momentMap;};
  void   setMomentMap(string fname){momentMap = fname;};
  //
  bool   getFlagBlankPix(){return flagBlankPix;};
  void   setFlagBlankPix(bool flag){flagBlankPix=flag;};
  bool   getNanFlag(){return nanAsBlank;};
  void   setNanFlag(bool flag){nanAsBlank = flag;};
  float  getBlankPixVal(){return blankPixValue;};
  void   setBlankPixVal(float v){blankPixValue=v;};
  int    getBlankKeyword(){return blankKeyword;};
  void   setBlankKeyword(int v){blankKeyword=v;};
  float  getBscaleKeyword(){return bscaleKeyword;};
  void   setBscaleKeyword(float v){bscaleKeyword=v;};
  float  getBzeroKeyword(){return bzeroKeyword;};
  void   setBzeroKeyword(float v){bzeroKeyword=v;};
  bool   getFlagMW(){return flagMW;};
  bool   setFlagMW(bool flag){flagMW=flag;};
  int    getMaxMW(){return maxMW;};
  void   setMaxMW(int m){maxMW=m;};
  int    getMinMW(){return minMW;};
  void   setMinMW(int m){minMW=m;};
  void   setBeamSize(float s){numPixBeam = s;};
  float  getBeamSize(){return numPixBeam;};
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
  //	 
  bool   getFlagATrous(){return flagATrous;};
  void   setFlagATrous(bool flag){flagATrous=flag;};
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
  bool   drawBorders(){return borders;};
  void   setDrawBorders(bool f){borders=f;};
  bool   isVerbose(){return verbose;};
  void   setVerbosity(bool f){verbose=f;};
  
private:
  // Input files
  string imageFile;       // The image to be analysed.
  bool   flagSubsection;  // Whether we just want a subsection of the image
  string subsection;      // The subsection requested, of the form [x1:x2,y1:y2,z1:z2]
                          //   If you want the full range of one index, use *
  // Output files
  bool   flagLog;         // Should we do the intermediate logging?
  string logFile;         // Where the intermediate logging goes.
  string outFile;         // Where the final results get put.
  string spectraFile;     // Where the spectra are displayed
  bool   flagOutputRecon; // Should the reconstructed cube be written?
  bool   flagOutputResid; // Should the reconstructed cube be written?
  bool   flagVOT;         // Should we save results in VOTable format?
  string votFile;         // Where the VOTable goes.
  bool   flagMaps;        // Should we produce detection and moment maps in postscript form?
  string detectionMap;    // The name of the detection map (ps file).
  string momentMap;       // The name of the 0th moment map (ps file).

  // Cube related parameters 
  bool   flagBlankPix;    // A flag that indicates whether there are pixels defined as BLANK,
                          //   with the value given by the next parameter.
  float  blankPixValue;   // Pixel value that is considered BLANK.
  int    blankKeyword;    // The FITS header keyword BLANK.
  float  bscaleKeyword;   // The FITS header keyword BSCALE.
  float  bzeroKeyword;    // The FITS header keyword BZERO.
  bool   nanAsBlank;      // Are the BLANK pixels defined by NaNs?
  bool   flagMW;          // A flag that indicates whether to excise the Milky Way.
  int    maxMW;           // Last  Galactic velocity plane for HIPASS cubes
  int    minMW;           // First Galactic velocity plane for HIPASS cubes
  float  numPixBeam;      // Size (area) of the beam in pixels.
  // Trim-related         
  bool   flagTrimmed;     // Has the cube been trimmed of excess BLANKs around the edge?
  long   borderLeft;      // The number of BLANK pixels trimmed from the Left of the cube;
  long   borderRight;     // The number of BLANK pixels trimmed from the Right of the cube;
  long   borderBottom;    // The number of BLANK pixels trimmed from the Bottom of the cube;
  long   borderTop;       // The number of BLANK pixels trimmed from the Top of the cube;
  // Subsection offsets
  long   xSubOffset;      // The offset in the x-direction from the subsection
  long   ySubOffset;      // The offset in the y-direction from the subsection
  long   zSubOffset;      // The offset in the z-direction from the subsection
  // Baseline related;
  bool   flagBaseline;    // Whether to do baseline subtraction before reconstruction and/or searching.
  // Detection-related    
  int    minPix;          // Minimum number of pixels for a detected object to be counted
  // Object growth        
  bool   flagGrowth;      // Are we growing objects once they are found?
  float  growthCut;       // The SNR that we are growing objects down to.
  // FDR analysis         
  bool   flagFDR;         // Should the FDR method be used? 
  float  alphaFDR;        // Alpha value for FDR detection algorithm
  // Other detection      
  float  snrCut;          // How many sigma above mean is a detection when sigma-clipping
  // A trous reconstruction parameters
  bool   flagATrous;      // Are we using the a trous reconstruction?
  int    scaleMin;        // Min scale used in a trous reconstruction
  float  snrRecon;        // SNR cutoff used in a trous reconstruction
                          //   (only wavelet coefficients that survive
                          //    this threshold are kept)
  int    filterCode;      // The code number for the filter to be used (saves having to parse names)
  string filterName;      // The code number converted into a name, for outputting purposes.

  // Volume-merging parameters
  bool   flagAdjacent;    // Whether to use the adjacent criterion for 
                          //    judging if objects are to be merged.
  float  threshSpatial;   // Maximum spatial separation between objects
  float  threshVelocity;  // Maximum channels separation between objects
  int    minChannels;     // Minimum no. of channels to make an object 
  // Input-Output related
  bool   borders;         // Whether to draw a border around the individual
                          //   pixels of a detection in the spectral display
  bool   verbose;         // Whether to use maximum verbosity -- progress indicators in the 
                          //   reconstruction & merging functions.

};


#endif
