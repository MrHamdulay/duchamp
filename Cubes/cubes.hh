#ifndef CUBES_H
#define CUBES_H

#include <iostream>
#include <string>
#include <vector>
#include <wcs.h>

#ifndef PARAM_H
#include <param.hh>
#endif
#ifndef DETECTION_H
#include <Detection/detection.hh>
#endif

using std::string;
using std::vector;

/****************************************************************/
/////////////////////////////////////////////////////////////
//// Definition of an n-dimensional data array:
////     array of pixel values, size & dimensions
////     array of Detection objects
/////////////////////////////////////////////////////////////


class DataArray
{
public:
  DataArray(){numDim=0; numPixels=0;};
  DataArray(short int nDim){numDim=nDim; numPixels=0;};
  DataArray(short int nDim, long size);
  DataArray(short int nDim, long *dimensions);
  virtual ~DataArray(){};
  // Size and Dimension related
  long                 getDimX(){if(numDim>0) return axisDim[0]; else return 0;};
  long                 getDimY(){if(numDim>1) return axisDim[1]; else return 1;};
  long                 getDimZ(){if(numDim>2) return axisDim[2]; else return 1;};
  void                 getDim(long &x, long &y, long &z);
  long                 getSize(){return numPixels;};
  short int            getNumDim(){return numDim;};
  // Related to the various arrays
  void                 getDimArray(long *output);
  void                 getArray(float *output);
  virtual void         saveArray(float *input, long size);
  float                getPixValue(long pos){return array[pos];};
  void                 setPixValue(long pos, float f){array[pos] = f;};
  // Related to the object lists
  Detection            getObject(long number){return objectList[number];};
  void                 addObject(Detection object); 
                       // adds a single detection to the object list
  vector <Detection>   getObjectList(){return objectList;};
  void                 addObjectList(vector <Detection> newlist); 
                       // adds all objects in a detection list to the object list
  long                 getNumObj(){return objectList.size();};
  void                 clearDetectionList(){this->objectList.clear();};
  // Parameter list related.
  void                 readParam(string &paramfile){par.readParams(paramfile);};
  void                 showParam(std::ostream &stream){stream << par;};
  Param                getParam(){return par;};
  void                 saveParam(Param newpar){par = newpar;};
  Param&               pars(){Param &rpar = par; return rpar;};
  bool                 isBlank(int vox){return par.isBlank(array[vox]);};

  friend std::ostream& operator<< ( std::ostream& theStream, DataArray &array);


protected:
  short int            numDim;     // number of dimensions.
  long                *axisDim;    // array of dimensions of cube 
                                   //     (ie. how large in each direction).
  long                 numPixels;  // total number of pixels in cube
  float               *array;      // array of data
  vector <Detection>   objectList; // the list of detected objects in the image
  Param                par;        // a parameter list.
};


/****************************************************************/
/////////////////////////////////////////////////////////////
//// Definition of an image object (2D):
////     a DataArray object
////     arrays for: probability values (for FDR)
////                 mask image to indicate location of objects
////                 detected objects
////     statistics information
/////////////////////////////////////////////////////////////

class Image : public DataArray
{
public:
  Image(){
    numPixels=0;
    numDim=2;};
  Image(long nPix);
  Image(long *dimensions);
  virtual ~Image(){};

  // Defining the array
  void      saveArray(float *input, long size);
  void      extractSpectrum(float *Array, long *dim, long pixel);
  void      extractImage(float *Array, long *dim, long channel);
    // Accessing the data.
  float     getPixValue(long x, long y){return array[y*axisDim[0] + x];};
  float     getPixValue(long pos){return array[pos];};
  float     getPValue(long pos){return pValue[pos];};
  float     getPValue(long x, long y){return pValue[y*axisDim[0] + x];};
  short int getMaskValue(long pos){return mask[pos];};
  short int getMaskValue(long x, long y){return mask[y*axisDim[0] + x];};
  // the next few should have checks against array overflow...
  void      setPixValue(long x, long y, float f){array[y*axisDim[0] + x] = f;};
  void      setPixValue(long pos, float f){array[pos] = f;};
  void      setPValue(long pos, float p){pValue[pos] = p;};
  void      setPValue(long x, long y, float p){pValue[y*axisDim[0] + x] = p;};
  void      setMaskValue(long pos, short int m){mask[pos] = m;};
  void      setMaskValue(long x, long y, short int m){mask[y*axisDim[0] + x] = m;};
  // Stats-related
  void      setStats(float m, float s, float c){mean=m; sigma=s; cutLevel=c;};
  void      findStats(int code);
  float     getMean(){return mean;};
  void      setMean(float m){mean=m;};
  float     getSigma(){return sigma;};
  void      setSigma(float s){sigma=s;};
  float     getCut(){return cutLevel;};
  void      setCut(float c){cutLevel=c;};
  float     getPCut(){return pCutLevel;};
  void      setPCut(float p){pCutLevel = p;};
  float     getAlpha(){return alpha;};
  void      setAlpha(float a){alpha = a;};
  int       getMinSize(){return minSize;};
  void      setMinSize(int s){minSize = s;};

  void      maskObject(Detection &object);

  // Detection-related
  void      lutz_detect();                  // in Detection/lutz_detect.cc
  void      spectrumDetect();               // in Detection/spectrumDetect.cc
  int       setupFDR();                     // in Detection/thresholding_functions.cc
  bool      isDetection(float value);       // in Detection/thresholding_functions.cc
  bool      isDetection(long x, long y);    // in Detection/thresholding_functions.cc
  bool      isDetectionFDR(float pvalue);   // in Detection/thresholding_functions.cc

  
private: 
  float     *pValue;     // the array of p-values for each pixel 
                         //                  --> used by FDR method
  short int *mask;       // a mask image indicating where objects are
	   		  
  float      mean;       // the mean background level of the image
  float      sigma;      // the standard deviation of the background in the image
  float      cutLevel;   // the limiting value (in sigmas above the mean) for
                         //     a pixel to be called a detection.
  float      alpha;      // used by FDR routine -- significance level
  float      pCutLevel;  // the limiting P-value for the FDR analysis
  int        minSize;    // the minimum number of pixels for a detection to be accepted.
};

/****************************************************************/
/////////////////////////////////////////////////////////////
//// Definition of an data-cube object (3D):
////     a DataArray object limited to dim=3
/////////////////////////////////////////////////////////////

class Cube : public DataArray
{
public:
  Cube(){numPixels=0; numDim=3; flagWCS=false;};
  Cube(long nPix);
  Cube(long *dimensions);
  virtual ~Cube(){};            // destructor

  // additional accessor functions -- in Cubes/cubes.cc unless otherwise specified.

  int     getCube(string fname);
  void    initialiseCube(long *dimensions);
  void    saveReconstructedCube();
  int     readReconCube();

  float   getPixValue(long pos){return array[pos];};
  float   getPixValue(long x, long y, long z){
    return array[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x];};
  short   getDetectMapValue(long pos){return detectMap[pos];};
  short   getDetectMapValue(long x, long y){return detectMap[y*axisDim[0] + x];};
  bool    isRecon(){return reconExists;};
  float   getReconValue(long pos){return recon[pos];};
  float   getReconValue(long x, long y, long z){
    return recon[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x];};
  float   getBaselineValue(long pos){return baseline[pos];};
  float   getBaselineValue(long x, long y, long z){
    return baseline[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x];};
  // these should have checks against array overflow...
  void    setPixValue(long pos, float f){array[pos] = f;};
  void    setPixValue(long x, long y, long z, float f){
    array[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x] = f;};
  void    setDetectMapValue(long pos, short f){detectMap[pos] = f;};
  void    setDetectMapValue(long x, long y, short f){
    detectMap[y*axisDim[0] + x] = f;};
  void    setReconValue(long pos, float f){recon[pos] = f;};
  void    setReconValue(long x, long y, long z, float f){
    recon[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x] = f;};
  void    setReconFlag(bool f){reconExists = f;};
  void    saveArray(float *input, long size);
  void    saveRecon(float *input, long size);
  void    getRecon(float *output);

  // Statistics for cube
  float   getSpecMean(int pixel){return specMean[pixel];};
  float   getSpecSigma(int pixel){return specSigma[pixel];};
  float   getChanMean(int channel){return chanMean[channel];};
  float   getChanSigma(int channel){return chanSigma[channel];};
  void    setCubeStats();    // in Cubes/cubes.cc

  // Functions that act on the cube
  void    removeMW();        // in Cubes/cubes.cc
  void    trimCube();        // in Cubes/trimImage.cc
  void    unTrimCube();      // in Cubes/trimImage.cc
  void    removeBaseline();  // in ATrous/baselineSubtract.cc
  void    replaceBaseline(); // in ATrous/baselineSubtract.cc
  void    invert();          // in Cubes/invertCube.cc
  void    reInvert();        // in Cubes/invertCube.cc

  // Reconstruction and Searching functions
  void    ReconSearch1D();   // in ATrous/ReconSearch.cc
  void    ReconSearch2D();   // in ATrous/ReconSearch.cc
  void    ReconSearch3D();   // in ATrous/ReconSearch.cc
  void    SimpleSearch3D();  // in Cubes/CubicSearch.cc

  // Dealing with the WCS
  bool    isWCS(){return flagWCS;};
  void    setWCS(wcsprm *w);
  wcsprm *getWCS();
  void    setNWCS(int n){nwcs = n;};
  int     getNWCS(){return nwcs;};
  void    setBUnit(char *s){bunit = s;};
  string  getBUnit(){return bunit;};
	 
  // Dealing with the detections 
  void    ObjectMerger();          // in Cubes/Merger.cc
  void    calcObjectWCSparams();
  void    sortDetections();
  void    updateDetectMap();
  void    updateDetectMap(Detection obj);

  // Text outputting of detected objects.
  void    outputDetectionsKarma(std::ostream &stream);     // in Cubes/detectionIO.cc
  void    outputDetectionsVOTable(std::ostream &stream);   // in Cubes/detectionIO.cc
  void    outputDetectionList();                           // in Cubes/detectionIO.cc
  void    logDetectionList();                              // in Cubes/detectionIO.cc
  void    logDetection(Detection obj, int counter);        // in Cubes/detectionIO.cc

  // Graphical plotting of detections.
  void    plotDetectionMap(string pgDestination);     // in Cubes/plotting.cc
  void    plotMomentMap(string pgDestination);        // in Cubes/plotting.cc
  void    plotWCSaxes();                              // in Cubes/plotting.cc
  void    outputSpectra();                            // in Cubes/outputSpectra.cc
  void    drawScale(float xstart, float ystart, float channel, float scaleLength);
                                                      // in Cubes/drawMomentCutout.cc


private: 
  float  *recon;           // reconstructed array -- used when doing a trous reconstruction.
  bool    reconExists;     // flag saying whether there is a reconstruction
  short  *detectMap;       // "moment map" -- x,y locations of detected pixels
  float  *baseline;        // array of spectral baseline values.
		       
  float  *specMean;        // array of means  for each spectrum in cube
  float  *specSigma;       // array of sigmas for each spectrum in cube
  float  *chanMean;        // array of means  for each channel map in cube
  float  *chanSigma;       // array of sigmas for each channel map in cube
		           
  bool    flagWCS;         // a flag indicating whether there is a valid WCS present.
  wcsprm *wcs;             // the WCS parameters for the cube -- a struct from wcslib
  int     nwcs;            // number of WCS parameters
  string  bunit;           // The header keyword BUNIT -- the units of brightness in the FITS file.
};

/****************************************************************/
//////////////////////////////////////////////////////
// Prototypes for functions that use above classes
//////////////////////////////////////////////////////

DataArray getImage(string fname, short int maxdim);
Image getImage(string fname);

void findSources(Image &image);
void findSources(Image &image, float mean, float sigma); 

vector <Detection> reconSearch(long *dim,float *originalArray,float *reconArray, Param &par);
vector <Detection> cubicSearch(long *dim, float *Array, Param &par);
vector <Detection> cubicSearchNMerge(long *dim, float *Array, Param &par);

void growObject(Detection &object, Image &image);
void growObject(Detection &object, Cube &cube);

void drawMomentCutout(Cube &cube, Detection &object);

#endif
