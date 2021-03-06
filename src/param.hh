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
  void recordParameters(std::ostream& theStream, Param &par, std::string paramName, std::string paramDesc, std::string paramValue);

  /// @brief A macro to handle streamed output to recordParameters
#define recordParam(outstream,par,string1,string2,instream) \
  do{                                                       \
    std::ostringstream oss;                                 \
    oss<<instream;                                          \
    recordParameters(outstream,par,string1,string2,oss.str());	\
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

    /// @brief Return the input name without a .fits or .fits.gz extension
    std::string inputWithoutExtension();

    /// @brief Determine filename in which to save the moment map. 
    std::string outputMomentMapFile();

    /// @brief Determine filename in which to save the moment-0 mask.
    std::string outputMomentMaskFile();

    /// @brief Determine filename in which to save the baseline array. 
    std::string outputBaselineFile();

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

    void writeToBinaryFile(std::string &filename);
    std::streampos readFromBinaryFile(std::string &filename, std::streampos loc=0);
    void writeToBinaryFile(){writeToBinaryFile(binaryCatalogue);};
    std::streampos readFromBinaryFile(std::streampos loc=0){return readFromBinaryFile(binaryCatalogue,loc);};

    //------------------
    // Functions in FitsIO/subsection.cc
    //
    /// @brief Make sure the subsection strings are OK, with FITS access. 
    OUTCOME verifySubsection();
    /// @brief Parse the subsection strings with a dimension set
    OUTCOME parseSubsections(std::vector<size_t> &dim);
    OUTCOME parseSubsections(std::vector<long> &dim);
    OUTCOME parseSubsections(std::vector<int> &dim);
    OUTCOME parseSubsections(size_t *dim, int size);

    /// @brief Set the correct offset values for each axis 
    void   setOffsets(struct wcsprm *wcs);

    /// @brief Set the correct offset values for each axis 
    void   setOffsets(struct wcsprm &wcs);

    //------------------
    // These are in param.cc
    /// @brief Is a pixel value a BLANK? 
    bool   isBlank(float &value);

    /// @brief Has a given channel been flagged by the user?
      bool isFlaggedChannel(int z);
      std::vector<bool> getChannelFlags(int numChannels);

    /// @brief Is a given pixel position OK for use with stats calculations? 
    bool   isStatOK(int x, int y, int z);

    /// @brief Make a mask array -- an array saying whether each pixel is BLANK or not
    std::vector<bool> makeBlankMask(float *array, size_t size);

    /// @brief Make a mask array -- an array saying whether each pixel is able to be used for statistics calculations
    std::vector<bool> makeStatMask(float *array, size_t *dim);

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
    std::vector<int> getObjectListVector(){return objectListVector;};
    void   setObjectListVector(std::vector<int> v){objectListVector = v;};
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
    bool   getFlagWriteBinaryCatalogue(){return flagWriteBinaryCatalogue;};
    void   setFlagWriteBinaryCatalogue(bool b){flagWriteBinaryCatalogue=b;};
    std::string getBinaryCatalogue(){return binaryCatalogue;};
    void   setBinaryCatalogue(std::string s){binaryCatalogue=s;};
    bool   getFlagPlotSpectra(){return flagPlotSpectra;};
    void   setFlagPlotSpectra(bool b){flagPlotSpectra=b;};
    std::string getSpectraFile(){return spectraFile;};
    void   setSpectraFile(std::string fname){spectraFile = fname;};
    bool   getFlagPlotIndividualSpectra(){return flagPlotIndividualSpectra;};
    void   setFlagPlotIndividualSpectra(bool b){flagPlotIndividualSpectra=b;};
    bool   getFlagTextSpectra(){return flagTextSpectra;};
    void   setFlagTextSpectra(bool b){flagTextSpectra = b;};
    std::string getSpectraTextFile(){return spectraTextFile;};
    void   setSpectraTextFile(std::string fname){spectraTextFile = fname;};
    bool   getFlagOutputBaseline(){return flagOutputBaseline;};
    void   setFlagOutputBaseline(bool flag){flagOutputBaseline=flag;};
    std::string getFileOutputBaseline(){return fileOutputBaseline;};
    void   setFileOutputBaseline(std::string s){fileOutputBaseline=s;};
    bool   getFlagOutputMomentMap(){return flagOutputMomentMap;};
    void   setFlagOutputMomentMap(bool flag){flagOutputMomentMap=flag;};
    std::string getFileOutputMomentMap(){return fileOutputMomentMap;};
    void   setFileOutputMomentMap(std::string s){fileOutputMomentMap=s;};
    bool   getFlagOutputMomentMask(){return flagOutputMomentMask;};
    void   setFlagOutputMomentMask(bool flag){flagOutputMomentMask=flag;};
    std::string getFileOutputMomentMask(){return fileOutputMomentMask;};
    void   setFileOutputMomentMask(std::string s){fileOutputMomentMask=s;};
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
    std::string getAnnotationType(){return annotationType;};
    void   setAnnotationType(std::string s){annotationType=s;};
    bool   getFlagKarma(){return flagKarma;};
    void   setFlagKarma(bool flag){flagKarma=flag;};
    std::string getKarmaFile(){return karmaFile;};
    void   setKarmaFile(std::string fname){karmaFile = fname;};
    bool   getFlagDS9(){return flagDS9;};
    void   setFlagDS9(bool flag){flagDS9=flag;};
    std::string getDS9File(){return ds9File;};
    void   setDS9File(std::string fname){ds9File = fname;};
    bool   getFlagCasa(){return flagCasa;};
    void   setFlagCasa(bool flag){flagCasa=flag;};
    std::string getCasaFile(){return casaFile;};
    void   setCasaFile(std::string fname){casaFile = fname;};
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
    std::vector<int> getFlaggedChannels(){return flaggedChannels;};
    void   setFlaggedChannels(std::vector<int> v){flaggedChannels=v;};
    std::string getFlaggedChannelList(){return flaggedChannelList;};
    void   setFlaggedChannelList(std::string s){flaggedChannelList=s;};
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
      std::string getBaselineType(){return baselineType;};
      void setBaselineType(std::string s){baselineType = s;};
      unsigned int getBaselineBoxWidth(){return baselineBoxWidth;};
      void setBaselineBoxWidth(unsigned int i){baselineBoxWidth = i;};
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
    std::string getSmoothEdgeMethod(){return smoothEdgeMethod;};
    void   setSmoothEdgeMethod(std::string s){smoothEdgeMethod=s;};  
    float getSpatialSmoothCutoff(){return spatialSmoothCutoff;};
    void setSpatialSmoothCutoff(float f){spatialSmoothCutoff=f;};  
    //	 
    bool   getFlagATrous(){return flagATrous;};
    void   setFlagATrous(bool flag){flagATrous=flag;};
    int    getReconDim(){return reconDim;};
    void   setReconDim(int i){reconDim=i;};
    unsigned int getMinScale(){return scaleMin;};
    void   setMinScale(unsigned int s){scaleMin=s;};
    unsigned int getMaxScale(){return scaleMax;};
    void   setMaxScale(unsigned int s){scaleMax=s;};
    float  getAtrousCut(){return snrRecon;};
    void   setAtrousCut(float c){snrRecon=c;};
    float  getReconConvergence(){return reconConvergence;};
    void   setReconConvergence(float f){reconConvergence = f;};
    int    getFilterCode(){return filterCode;};
    void   setFilterCode(int c){filterCode=c;};
    std::string getFilterName(){return reconFilter->getName();};
    Filter& filter(){ Filter &rfilter = *reconFilter; return rfilter; }; 
    //	 
    bool   getFlagAdjacent(){return flagAdjacent;};
    void   setFlagAdjacent(bool flag){flagAdjacent=flag;};
    float  getThreshS(){return threshSpatial;};
    void   setThreshS(float t){threshSpatial=t;};
    float  getThreshV(){return threshVelocity;};
    void   setThreshV(float t){threshVelocity=t;};
    unsigned int   getMinPix(){return minPix;};
    void   setMinPix(unsigned int m){minPix=m;};
    unsigned int getMinChannels(){return minChannels;};
    void   setMinChannels(unsigned int n){minChannels=n;};
    unsigned int getMinVoxels(){return minVoxels;};
    void   setMinVoxels(unsigned int n){minVoxels=n;};
    int   getMaxPix(){return maxPix;};
    void   setMaxPix(int m){maxPix=m;};
    int getMaxChannels(){return maxChannels;};
    void   setMaxChannels(int n){maxChannels=n;};
    int getMaxVoxels(){return maxVoxels;};
    void   setMaxVoxels(int n){maxVoxels=n;};
    bool   getFlagRejectBeforeMerge(){return flagRejectBeforeMerge;};
    void   setFlagRejectBeforeMerge(bool flag){flagRejectBeforeMerge=flag;};
    bool   getFlagTwoStageMerging(){return flagTwoStageMerging;};
    void   setFlagTwoStageMerging(bool flag){flagTwoStageMerging=flag;};
    //
    std::string getSpectralMethod(){return spectralMethod;};
    void   setSpectralMethod(std::string s){spectralMethod=s;};
    std::string getSpectralType(){return spectralType;};
    void   setSpectralType(std::string s){spectralType=s;};
    float  getRestFrequency(){return restFrequency;};
    void   setRestFrequency(float f){restFrequency=f;};
    bool    getFlagRestFrequencyUsed(){return restFrequencyUsed;};
    void    setFlagRestFrequencyUsed(bool b){restFrequencyUsed=b;};
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
  
    /// @brief Set the comment characters
    void setCommentString(std::string comment){commentStr = comment;};
    std::string commentString(){return commentStr;};

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
    std::vector<int> objectListVector; ///<< Vector list of the objects requested

    // Output files
    bool        flagLog;         ///< Should we do the intermediate logging?
    std::string logFile;         ///< Where the intermediate logging goes.
    std::string outFile;         ///< Where the final results get put.
    bool        flagSeparateHeader;///< Should the header information(parameters & statistics) be written to a separate file to the table ofresults?
    std::string headerFile;      ///< Where the header information to go with the results table should be written.
    bool        flagWriteBinaryCatalogue; ///< Whether to write the catalogue to a binary file
    std::string binaryCatalogue; ///< The binary file holding the catalogue of detected pixels.
    bool        flagPlotSpectra; ///< Should we plot the spectrum of each detection?
    std::string spectraFile;     ///< Where the spectra are displayed
    bool        flagPlotIndividualSpectra; ///< Should the sources be plotted with spectra in individual files?
    bool        flagTextSpectra; ///< Should a text file with all spectra be written?
    std::string spectraTextFile; ///< Where the text spectra are written.
    bool        flagOutputBaseline;  ///< Should the cube of baseline values be written to a FITS file?
    std::string fileOutputBaseline;  ///< The name of the baseline FITS file
    bool        flagOutputMomentMap;  ///< Should the moment map image be written to a FITS file?
    std::string fileOutputMomentMap;  ///< The name of the moment map FITS file
    bool        flagOutputMomentMask;  ///< Should the moment-0 mask be written to a FITS file?
    std::string fileOutputMomentMask;  ///< The name of the moment-0 mask FITS file
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
    bool        flagCasa;        ///< Should we save results in CASA region format?
    std::string casaFile;        ///< Where the CASA region file goes.
    bool        flagDS9;         ///< Should we save results in DS9 annotation format?
    std::string ds9File;         ///< Where the DS9 annotation file goes.
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
    std::string flaggedChannelList; ///< List of channels to be flagged (ignored).
    std::vector <int> flaggedChannels; ///< The flagged channels listed individually 
    std::vector<bool> flaggedChannelMask; ///< Whether a given channel is flagged, up to the maximum listed flagged channel

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
      std::string baselineType;  ///< Which baselining method to use
      unsigned int baselineBoxWidth; ///< For the median baselining method, the full width of the sliding box.

    // Detection-related
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
    std::string smoothEdgeMethod; ///< Method for dealing with the edges when 2D smoothing: 'equal','truncate','scale'
    float       spatialSmoothCutoff; ///< Cutoff value for determining kernel size

    // A trous reconstruction parameters
    bool   flagATrous;      ///< Are we using the a trous reconstruction?
    int    reconDim;        ///< How many dimensions to use for the reconstruction?
    unsigned int    scaleMin;        ///< Min scale used in a trous reconstruction
    unsigned int    scaleMax;        ///< Max scale used in a trous reconstruction
    float  snrRecon;        ///< SNR cutoff used in a trous reconstruction (only wavelet coefficients that survive this threshold are kept)
    float  reconConvergence;///< Convergence criterion for reconstruction - maximum fractional change in residual standard deviation
    Filter *reconFilter;     ///< The filter used for reconstructions.
    int    filterCode;      ///< The code number for the filter to be used (saves having to parse names)

    // Volume-merging parameters
    bool   flagAdjacent;    ///< Whether to use the adjacent criterion for judging if objects are to be merged.
    float  threshSpatial;   ///< Maximum spatial separation between objects
    float  threshVelocity;  ///< Maximum channels separation between objects
    unsigned int minPix;         ///< Minimum number of pixels for a detected object to be counted
    unsigned int minChannels;     ///< Minimum no. of channels to make an object 
    unsigned int minVoxels;       ///< Minimum no. of voxels required in an object
    int maxPix;
    int maxChannels;
    int maxVoxels;
    bool   flagRejectBeforeMerge; ///< Whether to reject sources before merging
    bool   flagTwoStageMerging;  ///< Whether to do a partial merge when the objects are found, via the mergeIntoList function.

    // Input-Output related
    std::string spectralMethod; ///< A string indicating choice of spectral plotting method: choices are "peak" (default) or "sum"
    std::string spectralType;   ///< The requested CTYPE that the spectral axis be converted to
    float  restFrequency;       ///< The rest frequency that should be used when converting - overrides that in the FITS file
    bool   restFrequencyUsed;   ///< A flag indicating whether the spectral type has been converted from that in the FITS file
    std::string spectralUnits;  ///< A string indicating what units the spectral axis should be quoted in.
    std::string pixelCentre;    ///< A string indicating which type of centre to give the results in: "average", "centroid", or "peak"(flux).
    std::string sortingParam;   ///< A string indicating the parameter to sort the detection list on.
    bool   borders;             ///< Whether to draw a border around the individual pixels of a detection in the spectral display
    bool   blankEdge;           ///< Whether to draw a border around the BLANK pixel region in the moment maps and cutout images
    bool   verbose;             ///< Whether to use maximum verbosity -- use progress indicators in the reconstruction & merging steps.

    std::string commentStr; ///< Any comment characters etc that need to be prepended to any output via the << operator.

  };

  //===========================================================================


}
#endif
