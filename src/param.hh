// -----------------------------------------------------------------------
// param.hh: Definition of the parameter set.
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
#ifndef PARAM_H
#define PARAM_H

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <wcslib/wcs.h>
#include <wcslib/wcshdr.h>
#include <duchamp/Utils/Section.hh>
#include <duchamp/Utils/VOParam.hh>
#include <duchamp/ATrous/filter.hh>
#include <duchamp/FitsIO/DuchampBeam.hh>

namespace duchamp
{

  /// @brief Write out info on a parameter to e.g. the results file.
  void recordParameters(std::ostream& theStream, std::string paramName, std::string paramDesc, std::string paramValue);

  /// @brief A macro to handle streamed output to recordParameters
#define recordParam(outstream,string1,string2,instream)      \
  do{                                                       \
    std::ostringstream oss;                                 \
    oss<<instream;                                          \
    recordParameters(outstream,string1,string2,oss.str());   \
  }while(0)

  class FitsHeader; // foreshadow this so that Param knows it exists

  const int numSortingParamOptions = 10;
  const std::string sortingParamOptions[numSortingParamOptions]={"xvalue","yvalue","zvalue","ra","dec","vel","w50","iflux","pflux","snr"};


  /// @brief Class to store general parameters.
  /// 
  /// @details This is a general repository for parameters that are used by all
  /// functions. This is how the user interacts with the program, as
  /// parameters are read in from disk files through functions in this
  /// class.

  class Param
  {
  public:
    /// @brief Default constructor. 
    Param();

    /// @brief Copy constructor for Param. 
    Param(const Param& p);

    /// @brief Assignment operator for Param.
    Param& operator= (const Param& p);

    /// @brief Destructor function.  
    virtual ~Param();

    /// @brief Define the default values of each parameter.
    void  defaultValues();

    //-----------------
    // Functions in param.cc
    //
    /// @brief Parse the command line parameters correctly. 
    OUTCOME getopts(int argc, char ** argv, std::string progname="Duchamp");

    /// @brief Read in parameters from a disk file. 
    OUTCOME readParams(std::string paramfile);

    /// @brief Check the parameter list for inconsistencies 
    void   checkPars();

    OUTCOME checkImageExists();

    /// @brief Copy certain necessary FITS header parameters from a FitsHeader object 
    void   copyHeaderInfo(FitsHeader &head);

    /// @brief Determine filename in which to save the mask array. 
    std::string outputMomentMapFile();

    /// @brief Determine filename in which to save the mask array. 
    std::string outputMaskFile();

    /// @brief Determine filename in which to save the smoothed array. 
    std::string outputSmoothFile();

    /// @brief Determine filename in which to save the reconstructed array. 
    std::string outputReconFile(); 

    /// @brief Determine filename in which to save the residual array from the atrous reconstruction. 
    std::string outputResidFile(); 

    /// @brief Print the parameter set in a readable fashion. 
    friend std::ostream& operator<< ( std::ostream& theStream, Param& par);
    friend class Image;

    std::vector<VOParam> getVOParams();

    //------------------
    // Functions in FitsIO/subsection.cc
    //
    /// @brief Make sure the subsection strings are OK, with FITS access. 
    OUTCOME verifySubsection();
    /// @brief Parse the subsection strings with a dimension set
    OUTCOME parseSubsections(std::vector<long> &dim);
    OUTCOME parseSubsections(std::vector<int> &dim);
    OUTCOME parseSubsections(long *dim, int size);

    /// @brief Set the correct offset values for each axis 
    void   setOffsets(struct wcsprm *wcs);

    /// @brief Set the correct offset values for each axis 
    void   setOffsets(struct wcsprm &wcs);

    //------------------
    // These are in param.cc
    /// @brief Is a pixel value a BLANK? 
    bool   isBlank(float &value);

    /// @brief Is a given channel flagged as being in the Milky Way?           
    bool   isInMW(int z);

    /// @brief Is a given pixel position OK for use with stats calculations? 
    bool   isStatOK(int x, int y, int z);

    /// @brief Make a mask array -- an array saying whether each pixel is BLANK or not
    bool  *makeBlankMask(float *array, int size);

    /// @brief Make a mask array -- an array saying whether each pixel is able to be used for statistics calculations
    bool *makeStatMask(float *array, long *dim);

    //--------------------
    // Basic inline accessor functions
    //
    std::string getImageFile(){return imageFile;};
    void   setImageFile(std::string fname){imageFile = fname;};
    std::string getFullImageFile(){
      if(flagSubsection) return imageFile+pixelSec.getSection();
      else return imageFile;
    };
    bool   getFlagSubsection(){return flagSubsection;};
    void   setFlagSubsection(bool flag){flagSubsection=flag;};
    std::string getSubsection(){return pixelSec.getSection();};
    void   setSubsection(std::string range){pixelSec.setSection(range);};
    Section &section(){Section &rsection = pixelSec; return rsection;};
    bool   getFlagReconExists(){return flagReconExists;};
    void   setFlagReconExists(bool flag){flagReconExists=flag;};
    std::string getReconFile(){return reconFile;};
    void   setReconFile(std::string file){reconFile = file;};
    bool   getFlagSmoothExists(){return flagSmoothExists;};
    void   setFlagSmoothExists(bool flag){flagSmoothExists=flag;};
    std::string getSmoothFile(){return smoothFile;};
    void   setSmoothFile(std::string file){smoothFile = file;};
    //
    bool   getFlagUsePrevious(){return usePrevious;};
    void   setFlagUsePrevious(bool flag){usePrevious=flag;};
    std::string getObjectList(){return objectList;};
    void   setObjectList(std::string s){objectList = s;};
    std::vector<int> getObjectRequest();
    std::vector<bool> getObjectChoices();
    std::vector<bool> getObjectChoices(int numObjects);
    //
    bool   getFlagLog(){return flagLog;};
    void   setFlagLog(bool flag){flagLog=flag;};
    std::string getLogFile(){return logFile;};
    void   setLogFile(std::string fname){logFile = fname;};
    std::string getOutFile(){return outFile;};
    void   setOutFile(std::string fname){outFile = fname;};
    bool   getFlagSeparateHeader(){return flagSeparateHeader;};
    void   setFlagSeparateHeader(bool b){flagSeparateHeader=b;};
    std::string getHeaderFile(){return headerFile;};
    void   setHeaderFile(std::string s){headerFile=s;};
    bool   getFlagPlotSpectra(){return flagPlotSpectra;};
    void   setFlagPlotSpectra(bool b){flagPlotSpectra=b;};
    std::string getSpectraFile(){return spectraFile;};
    void   setSpectraFile(std::string fname){spectraFile = fname;};
    bool   getFlagTextSpectra(){return flagTextSpectra;};
    void   setFlagTextSpectra(bool b){flagTextSpectra = b;};
    std::string getSpectraTextFile(){return spectraTextFile;};
    void   setSpectraTextFile(std::string fname){spectraTextFile = fname;};
    bool   getFlagOutputMomentMap(){return flagOutputMomentMap;};
    void   setFlagOutputMomentMap(bool flag){flagOutputMomentMap=flag;};
    std::string getFileOutputMomentMap(){return fileOutputMomentMap;};
    void   setFileOutputMomentMap(std::string s){fileOutputMomentMap=s;};
    bool   getFlagOutputMask(){return flagOutputMask;};
    void   setFlagOutputMask(bool flag){flagOutputMask=flag;};
    std::string getFileOutputMask(){return fileOutputMask;};
    void   setFileOutputMask(std::string s){fileOutputMask=s;};
    bool   getFlagMaskWithObjectNum(){return flagMaskWithObjectNum;};
    void   setFlagMaskWithObjectNum(bool flag){flagMaskWithObjectNum=flag;};
    bool   getFlagOutputSmooth(){return flagOutputSmooth;};
    void   setFlagOutputSmooth(bool flag){flagOutputSmooth=flag;};
    std::string getFileOutputSmooth(){return fileOutputSmooth;};
    void   setFileOutputSmooth(std::string s){fileOutputSmooth=s;};
    bool   getFlagOutputRecon(){return flagOutputRecon;};
    void   setFlagOutputRecon(bool flag){flagOutputRecon=flag;};
    std::string getFileOutputRecon(){return fileOutputRecon;};
    void   setFileOutputRecon(std::string s){fileOutputRecon=s;};
    bool   getFlagOutputResid(){return flagOutputResid;};
    void   setFlagOutputResid(bool flag){flagOutputResid=flag;};
    std::string getFileOutputResid(){return fileOutputResid;};
    void   setFileOutputResid(std::string s){fileOutputResid=s;};
    bool   getFlagVOT(){return flagVOT;};
    void   setFlagVOT(bool flag){flagVOT=flag;};
    std::string getVOTFile(){return votFile;};
    void   setVOTFile(std::string fname){votFile = fname;};
    bool   getFlagKarma(){return flagKarma;};
    void   setFlagKarma(bool flag){flagKarma=flag;};
    std::string getAnnotationType(){return annotationType;};
    void   setAnnotationType(std::string s){annotationType=s;};
    std::string getKarmaFile(){return karmaFile;};
    void   setKarmaFile(std::string fname){karmaFile = fname;};
    bool   getFlagMaps(){return flagMaps;};
    void   setFlagMaps(bool flag){flagMaps=flag;};
    std::string getDetectionMap(){return detectionMap;};
    void   setDetectionMap(std::string fname){detectionMap = fname;};
    std::string getMomentMap(){return momentMap;};
    void   setMomentMap(std::string fname){momentMap = fname;};
    bool   getFlagXOutput(){return flagXOutput;};
    void   setFlagXOutput(bool b){flagXOutput=b;};
    int    getPrecFlux(){return precFlux;};
    void   setPrecFlux(int i){precFlux=i;};
    int    getPrecVel(){return precVel;};
    void   setPrecVel(int i){precVel=i;};
    int    getPrecSNR(){return precSNR;};
    void   setPrecSNR(int i){precSNR=i;};
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
    bool   getFlagMW(){return flagMW;};
    void   setFlagMW(bool flag){flagMW=flag;};
    int    getMaxMW(){return maxMW - zSubOffset;};
    void   setMaxMW(int m){maxMW=m;};
    int    getMinMW(){return minMW - zSubOffset;};
    void   setMinMW(int m){minMW=m;};
    void   setBeamSize(float s){areaBeam = s;};
    float  getBeamSize(){return areaBeam;};
    void   setBeamFWHM(float s){fwhmBeam = s;};
    float  getBeamFWHM(){return fwhmBeam;};
    void   setBeamAsUsed(DuchampBeam &b){beamAsUsed = b;}; // just have the set function as we only want this for the operator<< function. Set from FitsHeader::itsBeam
    std::string getNewFluxUnits(){return newFluxUnits;};
    void   setNewFluxUnits(std::string s){newFluxUnits=s;};
    std::string getSearchType(){return searchType;};
    void   setSearchType(std::string s){searchType=s;};
    //
    bool   getFlagTrim(){return flagTrim;};
    void   setFlagTrim(bool b){flagTrim=b;};
    bool   getFlagCubeTrimmed(){return hasBeenTrimmed;};
    void   setFlagCubeTrimmed(bool flag){hasBeenTrimmed = flag;};
    size_t getBorderLeft(){return borderLeft;};
    void   setBorderLeft(size_t b){borderLeft = b;};
    size_t getBorderRight(){return borderRight;};
    void   setBorderRight(size_t b){borderRight = b;};
    size_t getBorderBottom(){return borderBottom;};
    void   setBorderBottom(size_t b){borderBottom = b;};
    size_t getBorderTop(){return borderTop;};
    void   setBorderTop(size_t b){borderTop = b;};
    //
    long   getXOffset(){return xSubOffset;};
    void   setXOffset(long o){xSubOffset = o;};
    long   getYOffset(){return ySubOffset;};
    void   setYOffset(long o){ySubOffset = o;};
    long   getZOffset(){return zSubOffset;};
    void   setZOffset(long o){zSubOffset = o;};
    //
    unsigned int   getMinPix(){return minPix;};
    void   setMinPix(int m){minPix=m;};
    //	 
    bool   getFlagGrowth(){return flagGrowth;};
    void   setFlagGrowth(bool flag){flagGrowth=flag;};
    float  getGrowthCut(){return growthCut;};
    void   setGrowthCut(float c){growthCut=c;};
    float  getGrowthThreshold(){return growthThreshold;};
    void   setGrowthThreshold(float f){growthThreshold=f;};
    bool   getFlagUserGrowthThreshold(){return flagUserGrowthThreshold;};
    void   setFlagUserGrowthThreshold(bool b){flagUserGrowthThreshold=b;};
    //	 
    bool   getFlagFDR(){return flagFDR;};
    void   setFlagFDR(bool flag){flagFDR=flag;};
    float  getAlpha(){return alphaFDR;};
    void   setAlpha(float a){alphaFDR=a;};
    int    getFDRnumCorChan(){return FDRnumCorChan;};
    void   setFDRnumCorChan(int i){FDRnumCorChan=i;};
    //
    bool   getFlagBaseline(){return flagBaseline;};
    void   setFlagBaseline(bool flag){flagBaseline = flag;};
    //
    bool   getFlagStatSec(){return flagStatSec;};
    void   setFlagStatSec(bool flag){flagStatSec=flag;};
    std::string getStatSec(){return statSec.getSection();};
    void   setStatSec(std::string range){statSec.setSection(range);};
    Section &statsec(){Section &rsection = statSec; return rsection;};
    bool   getFlagRobustStats(){return flagRobustStats;};
    void   setFlagRobustStats(bool flag){flagRobustStats=flag;};
    float  getCut(){return snrCut;};
    void   setCut(float c){snrCut=c;};
    float  getThreshold(){return threshold;};
    void   setThreshold(float f){threshold=f;};
    bool   getFlagUserThreshold(){return flagUserThreshold;};
    void   setFlagUserThreshold(bool b){flagUserThreshold=b;};
    //	 
    bool   getFlagSmooth(){return flagSmooth;};
    void   setFlagSmooth(bool b){flagSmooth=b;};
    std::string getSmoothType(){return smoothType;};
    void   setSmoothType(std::string s){smoothType=s;};
    int    getHanningWidth(){return hanningWidth;};
    void   setHanningWidth(int f){hanningWidth=f;};
    void   setKernMaj(float f){kernMaj=f;};
    float  getKernMaj(){return kernMaj;};
    void   setKernMin(float f){kernMin=f;};
    float  getKernMin(){return kernMin;};
    void   setKernPA(float f){kernPA=f;};
    float  getKernPA(){return kernPA;};
    //	 
    bool   getFlagATrous(){return flagATrous;};
    void   setFlagATrous(bool flag){flagATrous=flag;};
    int    getReconDim(){return reconDim;};
    void   setReconDim(int i){reconDim=i;};
    unsigned int    getMinScale(){return scaleMin;};
    void   setMinScale(int s){scaleMin=s;};
    int    getMaxScale(){return scaleMax;};
    void   setMaxScale(int s){scaleMax=s;};
    float  getAtrousCut(){return snrRecon;};
    void   setAtrousCut(float c){snrRecon=c;};
    int    getFilterCode(){return filterCode;};
    void   setFilterCode(int c){filterCode=c;};
    std::string getFilterName(){return reconFilter.getName();};
    Filter& filter(){ Filter &rfilter = reconFilter; return rfilter; }; 
    //	 
    bool   getFlagAdjacent(){return flagAdjacent;};
    void   setFlagAdjacent(bool flag){flagAdjacent=flag;};
    float  getThreshS(){return threshSpatial;};
    void   setThreshS(float t){threshSpatial=t;};
    float  getThreshV(){return threshVelocity;};
    void   setThreshV(float t){threshVelocity=t;};
    int    getMinChannels(){return minChannels;};
    void   setMinChannels(int n){minChannels=n;};
    unsigned int getMinVoxels(){return minVoxels;};
    void   setMinVoxels(unsigned int n){minVoxels=n;};
    bool   getFlagRejectBeforeMerge(){return flagRejectBeforeMerge;};
    void   setFlagRejectBeforeMerge(bool flag){flagRejectBeforeMerge=flag;};
    bool   getFlagTwoStageMerging(){return flagTwoStageMerging;};
    void   setFlagTwoStageMerging(bool flag){flagTwoStageMerging=flag;};
    //
    std::string getSpectralMethod(){return spectralMethod;};
    void   setSpectralMethod(std::string s){spectralMethod=s;};
    std::string getSpectralUnits(){return spectralUnits;};
    void   setSpectralUnits(std::string s){spectralUnits=s;};
    std::string getPixelCentre(){return pixelCentre;};
    void   setPixelCentre(std::string s){pixelCentre=s;};
    std::string getSortingParam(){return sortingParam;};
    void   setSortingParam(std::string s){sortingParam=s;};
    bool   drawBorders(){return borders;};
    void   setDrawBorders(bool f){borders=f;};
    bool   drawBlankEdge(){return blankEdge;};
    void   setDrawBlankEdge(bool f){blankEdge=f;};

    /// @brief Are we in verbose mode? 
    bool   isVerbose(){return verbose;};
    void   setVerbosity(bool f){verbose=f;};
  
  private:
    // Input files
    std::string imageFile;       ///< The image to be analysed.
    bool        flagSubsection;  ///< Whether we just want a subsection of the image
    Section pixelSec;            ///< The Section object storing the pixel subsection information.
    bool        flagReconExists; ///< The reconstructed array is in a FITS file on disk.
    std::string reconFile;       ///< The FITS file containing the reconstructed array.
    bool        flagSmoothExists;///< The smoothed array is in a FITS file.
    std::string smoothFile;      ///< The FITS file containing the smoothed array.

    bool       usePrevious;      ///< Whether to read the detections from a previously-created log file
    std::string objectList;      ///< List of objects to re-plot

    // Output files
    bool        flagLog;         ///< Should we do the intermediate logging?
    std::string logFile;         ///< Where the intermediate logging goes.
    std::string outFile;         ///< Where the final results get put.
    bool        flagSeparateHeader;///< Should the header information(parameters & statistics) be written to a separate file to the table ofresults?
    std::string headerFile;      ///< Where the header information to go with the results table should be written.
    bool        flagPlotSpectra; ///< Should we plot the spectrum of each detection?
    std::string spectraFile;     ///< Where the spectra are displayed
    bool        flagTextSpectra; ///< Should a text file with all spectra be written?
    std::string spectraTextFile; ///< Where the text spectra are written.
    bool        flagOutputMomentMap;  ///< Should the moment map image be written to a FITS file?
    std::string fileOutputMomentMap;  ///< The name of the moment map FITS file
    bool        flagOutputMask;  ///< Should the mask image be written?
    std::string fileOutputMask;  ///< The name of the mask image.
    bool        flagMaskWithObjectNum;///< Should the mask values be labeled with the object ID (or just 1)?
    bool        flagOutputSmooth;///< Should the smoothed cube be written?
    std::string fileOutputSmooth;///< The name of the smoothed cube file
    bool        flagOutputRecon; ///< Should the reconstructed cube be written to a FITS file?
    std::string fileOutputRecon; ///< The name of the reconstructed cube file
    bool        flagOutputResid; ///< Should the residuals from the reconstruction be written to a FITS file?
    std::string fileOutputResid; ///< The name of the residual cube file
    bool        flagVOT;         ///< Should we save results in VOTable format?
    std::string votFile;         ///< Where the VOTable goes.
    bool        flagKarma;       ///< Should we save results in Karma annotation format?
    std::string karmaFile;       ///< Where the Karma annotation file goes.
    std::string annotationType;  ///< Should the annoations be circles or borders?
    bool        flagMaps;        ///< Should we produce detection and moment maps in postscript form?
    std::string detectionMap;    ///< The name of the detection map (postscript file).
    std::string momentMap;       ///< The name of the 0th moment map (ps file).
    bool        flagXOutput;     ///< Should there be an xwindows output of the detection map?
    int         precFlux;        ///< The desired precision for flux values.
    int         precVel;         ///< The desired precision for velocity/frequency values.
    int         precSNR;         ///< The desired precision for the SNR values.

    // Cube related parameters
    bool        flagNegative;    ///< Are we going to search for negative features?
    bool        flagBlankPix;    ///< A flag that indicates whether there are pixels defined as BLANK and whether we need to remove & ignore them in processing.
    float       blankPixValue;   ///< Pixel value that is considered BLANK.
    int         blankKeyword;    ///< The FITS header keyword BLANK.
    float       bscaleKeyword;   ///< The FITS header keyword BSCALE.
    float       bzeroKeyword;    ///< The FITS header keyword BZERO.
    std::string newFluxUnits;    ///< The user-requested flux units, to replace BUNIT.

    // Milky-Way parameters
    bool        flagMW;          ///< A flag that indicates whether to ignore the Milky Way channels.
    int         maxMW;           ///< Last  Milky Way channel
    int         minMW;           ///< First Milky Way channel

    // Trim-related
    bool        flagTrim;        ///< Does the user want the cube trimmed?
    bool        hasBeenTrimmed;  ///< Has the cube been trimmed of excess BLANKs around the edge?
    size_t      borderLeft;      ///< The number of BLANK pixels trimmed from the left of the cube;
    size_t      borderRight;     ///< The number trimmed from the Right of the cube;
    size_t      borderBottom;    ///< The number trimmed from the Bottom of the cube;
    size_t      borderTop;       ///< The number trimmed from the Top of the cube;

    // Subsection offsets
    long       *offsets;         ///< The array of offsets for each FITS axis.
    long        sizeOffsets;     ///< The size of the offsets array.
    long        xSubOffset;      ///< The subsection's x-axis offset
    long        ySubOffset;      ///< The subsection's y-axis offset
    long        zSubOffset;      ///< The subsection's z-axis offset

    // Baseline related
    bool        flagBaseline;    ///< Whether to do baseline subtraction before reconstruction and/or searching.

    // Detection-related
    int         minPix;          ///< Minimum number of pixels for a detected object to be counted
    float       areaBeam;        ///< Size (area) of the beam in pixels.
    float       fwhmBeam;        ///< FWHM of the beam in pixels.
    DuchampBeam beamAsUsed;      ///< A copy of the beam as used in FitsHeader - only used here for output
    std::string searchType;      ///< How to do the search: by channel map or by spectrum

    // Object growth
    bool        flagGrowth;      ///< Are we growing objects once they are found?
    float       growthCut;       ///< The SNR that we are growing objects down to.
    bool        flagUserGrowthThreshold; ///< Whether the user has manually defined a growth threshold
    float       growthThreshold; ///< The threshold for growing objects down to

    // FDR analysis
    bool        flagFDR;         ///< Should the FDR method be used? 
    float       alphaFDR;        ///< Alpha value for FDR detection algorithm
    int         FDRnumCorChan;   ///< Number of correlated channels, used in the FDR algorithm

    // Basic detection
    bool        flagStatSec;     ///< Whether we just want to use a subsection of the image to calculate the statistics.
    Section     statSec;         ///< The Section object storing the statistics subsection information.
    bool        flagRobustStats; ///< Whether to use robust statistics.
    float       snrCut;          ///< How many sigma above mean is a detection when sigma-clipping
    float       threshold;       ///< What the threshold is (when sigma-clipping).
    bool        flagUserThreshold;///< Whether the user has defined a threshold of their own.

    // Smoothing of the cube
    bool        flagSmooth;      ///< Should the cube be smoothed before searching?
    std::string smoothType;      ///< The type of smoothing to be done.
    int         hanningWidth;    ///< Width for hanning smoothing.
    float       kernMaj;         ///< Semi-Major axis of gaussian smoothing kernel
    float       kernMin;         ///< Semi-Minor axis of gaussian smoothing kernel
    float       kernPA;          ///< Position angle of gaussian smoothing kernel, in degrees east of north (i.e. anticlockwise).

    // A trous reconstruction parameters
    bool   flagATrous;      ///< Are we using the a trous reconstruction?
    int    reconDim;        ///< How many dimensions to use for the reconstruction?
    unsigned int    scaleMin;        ///< Min scale used in a trous reconstruction
    int    scaleMax;        ///< Max scale used in a trous reconstruction
    float  snrRecon;        ///< SNR cutoff used in a trous reconstruction (only wavelet coefficients that survive this threshold are kept)
    Filter reconFilter;     ///< The filter used for reconstructions.
    int    filterCode;      ///< The code number for the filter to be used (saves having to parse names)

    // Volume-merging parameters
    bool   flagAdjacent;    ///< Whether to use the adjacent criterion for judging if objects are to be merged.
    float  threshSpatial;   ///< Maximum spatial separation between objects
    float  threshVelocity;  ///< Maximum channels separation between objects
    int    minChannels;     ///< Minimum no. of channels to make an object 
    unsigned int    minVoxels;       ///< Minimum no. of voxels required in an object
    bool   flagRejectBeforeMerge; ///< Whether to reject sources before merging
    bool   flagTwoStageMerging;  ///< Whether to do a partial merge when the objects are found, via the mergeIntoList function.

    // Input-Output related
    std::string spectralMethod; ///< A string indicating choice of spectral plotting method: choices are "peak" (default) or "sum"
    std::string spectralUnits;   ///< A string indicating what units the spectral axis should be quoted in.
    std::string pixelCentre;///< A string indicating which type of centre to give the results in: "average", "centroid", or "peak"(flux).
    std::string sortingParam; ///< A string indicating the parameter to sort the detection list on.
    bool   borders;         ///< Whether to draw a border around the individual pixels of a detection in the spectral display
    bool   blankEdge;       ///< Whether to draw a border around the BLANK pixel region in the moment maps and cutout images
    bool   verbose;         ///< Whether to use maximum verbosity -- use progress indicators in the reconstruction & merging steps.

  };

  //===========================================================================


}
#endif
