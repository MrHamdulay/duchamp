#ifndef CUBES_H
#define CUBES_H

#include <iostream>
#include <string>
#include <vector>

#ifndef PARAM_H
#include <param.hh>
#endif
#ifndef DETECTION_H
#include <Detection/detection.hh>
#endif
#ifndef COLUMNS_H
#include <Detection/columns.hh>
#endif
#ifndef PLOTS_H
#include <Cubes/plots.hh>
#endif
#ifndef STATS_H
#include <Utils/Statistics.hh>
#endif

using std::string;
using std::vector;
using namespace Column;
using namespace Statistics;

/****************************************************************/
/** 
 * Base class for the image container.
 *
 * Definition of an n-dimensional data array:
 *     array of pixel values, size & dimensions
 *     array of Detection objects
 */

class DataArray
{
public:
  DataArray();  ///< Basic DataArray constructor
  DataArray(short int nDim); ///< Basic nDim-dimensional DataArray constructor
  DataArray(short int nDim, long size);///< Basic nDim-dimensional DataArray constructor, specifying size.
  DataArray(short int nDim, long *dimensions); ///< Basic nDim-dimensional DataArray constructor, specifying size of dimensions.
  virtual ~DataArray(); ///< Basic DataArray constructor

  // Size and Dimension related
  long               getDimX();   ///< Return X dimension, defaulting to 0.
  long               getDimY();   ///< Return Y dimension, defaulting to 1.
  long               getDimZ();   ///< Return Z dimension, defaulting to 1.
  long               getSize();   ///< Return number of voxels.
  short int          getNumDim(); ///< Return number of dimensions.
  void               getDim(long &x, long &y, long &z); ///< Return first three dimensional axes.

  // Related to the various arrays
  void               getDimArray(long *output); ///< Return array of dimensional sizes.
  void               getArray(float *output); ///< Return pixel value array.
  virtual void       saveArray(float *input, long size); ///< Save pixel value array.
  float              getPixValue(long pos); ///< Returns the flux value at voxel number pos.
  void               setPixValue(long pos, float f); ///< Sets the value of pixel pos to f.

  // Related to the object lists
  Detection          getObject(long number); ///< Returns the Detection at position 'number' in the list.
  void               addObject(Detection object); ///< Adds a single detection to the object list.
  vector <Detection> getObjectList(); ///< Returns the full list of Detections.
  void               addObjectList(vector <Detection> newlist);  ///< Adds all objects in a detection list to the object list.
  void               addObjectOffsets(); ///< Add pixel offsets to object coordinates.
  long               getNumObj(); ///< Returns the number of detected objects.
  void               clearDetectionList(); ///< Delete all Detections from the object list. 

  // Parameter list related.
  int                readParam(string paramfile); ///< Read parameters.
  void               showParam(std::ostream &stream); ///< Output the Param set.
  Param              getParam(); ///< Return the Param set.
  void               saveParam(Param newpar); ///< Save a Param set to the Cube.
  Param&             pars(); ///< Provides a reference to the Param set.
	        
  bool               isBlank(int vox); ///< Is the voxel number given by vox a BLANK value?

  // Statistics
  StatsContainer<float> getStats(); ///< Returns the StatsContainer.
  StatsContainer<float>& stats(); ///< Provides a reference to the StatsContainer.
  void saveStats(StatsContainer<float> newStats); ///< Save a StatsContainer to the Cube.

  bool isDetection(float value); ///< A detection test for value. 
  bool isDetection(long voxel); ///< A detection test for pixel. 

  friend std::ostream& operator<< (std::ostream& theStream, DataArray &array);
  ///< Output operator for DataArray.

protected:
  short int               numDim;     ///< Number of dimensions.
  long                   *axisDim;    ///< Array of dimensions of cube (ie. how large in each direction).
  long                    numPixels;  ///< Total number of pixels in cube.
  float                  *array;      ///< Array of data.
  vector <Detection>      objectList; ///< The list of detected objects.
  Param                   par;        ///< A parameter list.
  StatsContainer<float>   Stats;      ///< The statistics for the DataArray.
};

////////////
//// DataArray inline function definitions
////////////

inline long      DataArray::getDimX(){ if(numDim>0) return this->axisDim[0];else return 0;};
inline long      DataArray::getDimY(){ if(numDim>1) return this->axisDim[1];else return 1;};
inline long      DataArray::getDimZ(){ if(numDim>2) return this->axisDim[2];else return 1;};
inline long      DataArray::getSize(){ return this->numPixels; };
inline short int DataArray::getNumDim(){ return this->numDim; };

inline float     DataArray::getPixValue(long pos){ return array[pos]; };
inline void      DataArray::setPixValue(long pos, float f){array[pos] = f;};
inline Detection DataArray::getObject(long number){ return objectList[number]; };
inline vector <Detection> DataArray::getObjectList(){ return objectList; };
inline long      DataArray::getNumObj(){ return objectList.size(); };
inline void      DataArray::clearDetectionList(){ this->objectList.clear(); };
inline int       DataArray::readParam(string paramfile){
    /** 
     *  Uses Param::readParams() to read parameters from a file.
     *  \param paramfile The file to be read.
     */
  return par.readParams(paramfile); };
inline void      DataArray::showParam(std::ostream &stream){ stream << this->par; };
inline Param     DataArray::getParam(){ return this->par; };
inline void      DataArray::saveParam(Param newpar){this->par = newpar;};
inline Param&    DataArray::pars(){ Param &rpar = this->par; return rpar; };
inline bool      DataArray::isBlank(int vox){ return this->par.isBlank(this->array[vox]); };
inline StatsContainer<float> DataArray::getStats(){ return this->Stats; };
inline StatsContainer<float>& DataArray::stats(){
  StatsContainer<float> &rstats = this->Stats; 
  return rstats;
}; 
inline void      DataArray::saveStats(StatsContainer<float> newStats){ this->Stats = newStats;};




/****************************************************************/
/**
 * Definition of an data-cube object (3D):
 *     a DataArray object limited to dim=3
 */

class Cube : public DataArray
{
public:
  Cube();                 ///< Basic Cube constructor.
  Cube(long nPix);        ///< Alternative Cube constructor.
  Cube(long *dimensions); ///< Alternative Cube constructor.
  virtual ~Cube();        ///< Basic Cube destructor.

  // INLINE functions -- definitions included after class declaration.
  bool        isBlank(int vox);                               ///< Is the voxel number given by vox a BLANK value?
  bool        isBlank(long x, long y, long z);                ///< Is the voxel at (x,y,z) a BLANK value?
  float       getPixValue(long pos);                          ///< Returns the flux value at voxel number pos.
  float       getPixValue(long x, long y, long z);            ///< Returns the flux value at voxel (x,y,z).
  short       getDetectMapValue(long pos);                    ///< Returns the value of the detection map at spatial pixel pos.
  short       getDetectMapValue(long x, long y);              ///< Returns the value of the detection map at spatial pixel (x,y).
  bool        isRecon();                                      ///< Does the Cube::recon array exist? 
  float       getReconValue(long pos);                        ///< Returns the reconstructed flux value at voxel number pos. 
  float       getReconValue(long x, long y, long z);          ///< Returns the reconstructed flux value at voxel (x,y,z).
  float       getBaselineValue(long pos);                     ///< Returns the baseline flux value at voxel number pos.
  float       getBaselineValue(long x, long y, long z);       ///< Returns the baseline flux value at voxel (x,y,z).
  void        setPixValue(long pos, float f);                 ///< Sets the value of voxel pos.
  void        setPixValue(long x, long y, long z, float f);   ///< Sets the value of voxel (x,y,z).
  void        setDetectMapValue(long pos, short f);           ///< Sets the value of the detection map at spatial pixel pos.
  void        setDetectMapValue(long x, long y, short f);     ///< Sets the value of the detection map at spatial pixel (x,y).
  void        setReconValue(long pos, float f);               ///< Sets the reconstructed flux value of voxel pos.
  void        setReconValue(long x, long y, long z, float f); ///< Sets the reconstructed flux value of voxel (x,y,z).
  void        setReconFlag(bool f);                           ///< Sets the value of the reconExists flag.
  vector<Col> getLogCols();                                   ///< Return the vector of log file columns
  void        setLogCols(vector<Col> C);                      ///< Set the vector of log file columns
  vector<Col> getFullCols();                                  ///< Return the vector of the full output column set.
  void        setFullCols(vector<Col> C);                     ///< Set the vector of the full output column set.

  // additional functions -- in Cubes/cubes.cc
  void        initialiseCube(long *dimensions);   ///< Allocate memory correctly, with WCS defining the correct axes.
  int         getCube();                          ///< Read in a FITS file, with subsection correction.
  int         getopts(int argc, char ** argv);    ///< Read in command-line options.
  void        readSavedArrays();                  ///< Read in reconstructed & smoothed arrays from FITS files on disk.
  void        saveArray(float *input, long size); ///< Save an external array to the Cube's pixel array
  void        saveRecon(float *input, long size); ///< Save an external array to the Cube's reconstructed array.
  void        getRecon(float *output);            ///< Save reconstructed array to an external array.
  void        removeMW();                         ///< Set Milky Way channels to zero.
  // Statistics for cube
  void        setCubeStatsOld();                  ///< Calculate the statistics for the Cube (older version).
  void        setCubeStats();                     ///< Calculate the statistics for the Cube.
  int         setupFDR();                         ///< Set up thresholds for the False Discovery Rate routine.
  bool        isDetection(long x, long y, long z);///< A detection test for a given voxel.
  // Dealing with the detections 
  void        calcObjectWCSparams();              ///< Calculate the WCS parameters for each Cube Detection.
  void        sortDetections();                   ///< Sort the list of detections.
  void        updateDetectMap();                  ///< Update the map of detected pixels.
  void        updateDetectMap(Detection obj);     ///< Update the map of detected pixels for a given Detection.
  float       enclosedFlux(Detection obj);        ///< Find the flux enclosed by a Detection.
  void        setupColumns();                     ///< Set up the output column definitions for the Cube and its Detection list.
  bool        objAtSpatialEdge(Detection obj);    ///< Is the object at the edge of the image?
  bool        objAtSpectralEdge(Detection obj);   ///< Is the object at an end of the spectrum?
  void        setObjectFlags();                   ///< Set warning flags for the detections.
  // Graphical plotting of the cube and the detections.
  void        plotBlankEdges();                   ///< Draw blank edges of cube.
  // Dealing with the WCS
  FitsHeader  getHead();                          ///< Return the FitsHeader.
  void        setHead(FitsHeader F);              ///< Set the FitsHeader. 
  FitsHeader& header();                           ///< Provides a reference to the FitsHeader.
  int         wcsToPix(const double *world, double *pix); ///< Convert a point from WCS to Pixel coords.
  int         wcsToPix(const double *world, double *pix, const int npts); ///< Convert a set of points from WCS to Pixel coords.
  int         pixToWCS(const double *pix, double *world); ///< Convert a point from Pixel to WCS coords.
  int         pixToWCS(const double *pix, double *world, const int npts); ///< Convert a set of points from Pixel to WCS coords.

  // FITS-I/O related functions -- not in cubes.cc
  // in Cubes/getImage.cc
  int         getCube(string fname);     ///< Function to read in FITS file.
  // in FitsIO/dataIO.cc
  int         getFITSdata(string fname); ///< Function to retrieve FITS data array
  // in Cubes/saveImage.cc
  void        saveSmoothedCube();        ///< Save Hanning-smoothed array to disk.
  void        saveReconstructedCube();   ///< Save Reconstructed array to disk.
  // in Cubes/readRecon.cc
  int         readReconCube();           ///< Read in reconstructed array from FITS file.
  // in Cubes/readSmooth.cc  
  int         readSmoothCube();          ///< Read in Hanning-smoothed array from FITS file.

  // Functions that act on the cube
  // in Cubes/trimImage.cc
  void        trimCube();        ///< Remove excess BLANK pixels from spatial edge of cube.
  void        unTrimCube();      ///< Replace BLANK pixels to spatial edge of cube.
  // in Cubes/baseline.cc
  void        removeBaseline();  ///< Removes the baselines from the spectra, and stores in Cube::baseline
  void        replaceBaseline(); ///< Replace the baselines stored in Cube::baseline
  // in Cubes/invertCube.cc
  void        invert();          ///< Multiply all pixel values by -1.
  void        reInvert();        ///< Undo the inversion, and invert fluxes of all detected objects.

  // Reconstruction, Searching and Merging functions
// in ATrous/ReconSearch.cc
  void        ReconSearch();     ///< Front-end to reconstruction & searching functions.
  void        ReconCube();       ///< Switcher to reconstruction functions
  void        ReconCube1D();     ///< Performs 1-dimensional a trous reconstruction on the Cube.
  void        ReconCube2D();     ///< Performs 2-dimensional a trous reconstruction on the Cube.
  void        ReconCube3D();     ///< Performs 3-dimensional a trous reconstruction on the Cube.
  // in Cubes/CubicSearch.cc
  void        CubicSearch();     ///< Front-end to the cubic searching routine.
  // in Cubes/smoothCube.cc 
  void        SmoothSearch();    ///< Front-end to the smoothing and searching functions.
  void        SmoothCube();      ///< A function to Hanning-smooth the cube.
  // in Cubes/Merger.cc
  void        ObjectMerger();    ///< Merge all objects in the detection list so that only distinct ones are left.
  
  // Text outputting of detected objects.
  //   in Cubes/detectionIO.cc
  void        outputDetectionsKarma(std::ostream &stream);   ///< Output detections to a Karma annotation file.
  void        outputDetectionsVOTable(std::ostream &stream); ///< Output detections to a VOTable. 
  void        outputDetectionList();     ///< Output detections to the output file and standard output.
  void        logDetectionList();        ///< Output detections to the log file.
  void        logDetection(Detection obj, int counter);   ///< Output a single detection to the log file      

  //  in Cubes/plotting.cc
  void        plotDetectionMap(string pgDestination);     ///< Plot a spatial map of detections based on number of detected channels. 
  void        plotMomentMap(string pgDestination);        ///< Plot a spatial map of detections based on 0th moment map of each object.     
  void        plotMomentMap(vector<string> pgDestination);///< Plot a spatial map of detections based on 0th moment map of each object to a number of PGPLOT devices.
  void        plotWCSaxes();                              ///< Draw WCS axes over a PGPLOT map.

  //  in Cubes/outputSpectra.cc
  void        outputSpectra();    ///< Print spectra of each detected object.
  void        plotSpectrum(Detection obj,Plot::SpectralPlot &plot);  ///< Print spectrum of a single object
  //  in Cubes/drawMomentCutout.cc
  void        drawMomentCutout(Detection &object); ///< Draw the 0th moment map for a single object.
  void        drawScale(float xstart,float ystart,float channel); ///< Draw a scale bar indicating angular size of the cutout.
  void        drawFieldEdge(); ///< Draw a yellow line around the edge of the spatial extent of pixels.

private: 
  float      *recon;            ///< reconstructed array - used when doing a trous reconstruction.
  bool        reconExists;      ///< flag saying whether there is a reconstruction
  short      *detectMap;        ///< "moment map" - x,y locations of detected pixels
  float      *baseline;         ///< array of spectral baseline values.
			     
  bool        reconAllocated;   ///< have we allocated memory for the recon array?
  bool        baselineAllocated;///< have we allocated memory for the baseline array?
			     
  FitsHeader  head;             ///< the WCS and other header information.
  vector<Col> fullCols;         ///< the list of all columns as printed in the results file
  vector<Col> logCols;          ///< the list of columns as printed in the log file

};

////////////
//// Cube inline function definitions
////////////

inline bool  Cube::isBlank(int vox){ return par.isBlank(array[vox]); };
inline bool  Cube::isBlank(long x, long y, long z){ return par.isBlank(array[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x]); };
inline float Cube::getPixValue(long pos){ return array[pos]; };
inline float Cube::getPixValue(long x, long y, long z){ return array[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x]; };
inline short Cube::getDetectMapValue(long pos){ return detectMap[pos]; };
inline short Cube::getDetectMapValue(long x, long y){ return detectMap[y*axisDim[0]+x]; };
inline bool  Cube::isRecon(){ return reconExists; };
inline float Cube::getReconValue(long pos){ return recon[pos]; };
inline float Cube::getReconValue(long x, long y, long z){ return recon[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x];};
inline float Cube::getBaselineValue(long pos){ return baseline[pos]; };
inline float Cube::getBaselineValue(long x, long y, long z){ return baseline[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x]; };
  // these next ones should have checks against array overflow...
inline void  Cube::setPixValue(long pos, float f){array[pos] = f;};
inline void  Cube::setPixValue(long x, long y, long z, float f){ array[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x] = f;};
inline void  Cube::setDetectMapValue(long pos, short f){ detectMap[pos] = f;};
inline void  Cube::setDetectMapValue(long x, long y, short f){ detectMap[y*axisDim[0] + x] = f;};
inline void  Cube::setReconValue(long pos, float f){recon[pos] = f;};
inline void  Cube::setReconValue(long x, long y, long z, float f){ recon[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x] = f; };
inline void  Cube::setReconFlag(bool f){reconExists = f;};
inline vector<Col> Cube::getLogCols(){return logCols;};
inline void        Cube::setLogCols(vector<Col> C){logCols=C;};
inline vector<Col> Cube::getFullCols(){return fullCols;};
inline void        Cube::setFullCols(vector<Col> C){fullCols=C;};
inline FitsHeader  Cube::getHead(){ return head; }; 
inline void        Cube::setHead(FitsHeader F){}; 
inline FitsHeader& Cube::header(){ FitsHeader &h = head; return h; }; 
inline int Cube::wcsToPix(const double *world, double *pix)
{
  /** 
   *  Use the WCS in the FitsHeader to convert from WCS to pixel coords for
   *   a single point.
   *  \param world The world coordinates.
   *  \param pix The returned pixel coordinates.
   */
  return this->head.wcsToPix(world,pix);
} 
inline int Cube::wcsToPix(const double *world, double *pix, const int npts)
{
  /** 
   *  Use the WCS in the FitsHeader to convert from WCS to pixel coords for
   *   a set of points.
   *  \param world The world coordinates.
   *  \param pix The returned pixel coordinates.
   *  \param npts The number of points being converted.
   */
  return this->head.wcsToPix(world,pix,npts);
}
inline int Cube::pixToWCS(const double *pix, double *world)
{
  /** 
   *  Use the WCS in the FitsHeader to convert from pixel to WCS coords for
   *   a single point.
   *  \param pix The pixel coordinates.
   *  \param world The returned world coordinates.
   */
  return this->head.pixToWCS(pix,world);
}
inline int Cube::pixToWCS(const double *pix, double *world, const int npts)
{
  /** 
   *  Use the WCS in the FitsHeader to convert from pixel to WCS coords for
   *   a set of points.
   *  \param pix The pixel coordinates.
   *  \param world The returned world coordinates.
   *  \param npts The number of points being converted.
   */
  return this->head.pixToWCS(pix,world,npts);
}


//------------------------------------------------------------------------

/****************************************************************/
/**
 *  A DataArray object limited to two dimensions, and with some additional
 *   special functions. 
 *
 *  It is used primarily for searching a 1- or 2-D array with 
 *   lutz_detect() and spectrumDetect().
 */

class Image : public DataArray
{
public:
  Image(){numPixels=0; numDim=2;};
  Image(long nPix);
  Image(long *dimensions);
  virtual ~Image(){};

  // Defining the array
  void      saveArray(float *input, long size);
  void      extractSpectrum(float *Array, long *dim, long pixel);
  void      extractImage(float *Array, long *dim, long channel);
  void      extractSpectrum(Cube &cube, long pixel);
  void      extractImage(Cube &cube, long channel);
  // Accessing the data.
  float     getPixValue(long x, long y){return array[y*axisDim[0] + x];};
  float     getPixValue(long pos){return array[pos];};
  // the next few should have checks against array overflow...
  void      setPixValue(long x, long y, float f){array[y*axisDim[0] + x] = f;};
  void      setPixValue(long pos, float f){array[pos] = f;};
  bool      isBlank(int vox){return par.isBlank(array[vox]);};
  bool      isBlank(long x,long y){return par.isBlank(array[y*axisDim[0]+x]);};

  // Detection-related
  void      lutz_detect();            // in Detection/lutz_detect.cc
  void      spectrumDetect();         // in Detection/spectrumDetect.cc
  int       getMinSize(){return minSize;};
  void      setMinSize(int i){minSize=i;};
  // the rest are in Detection/thresholding_functions.cc
  bool      isDetection(long x, long y){
    long voxel = y*axisDim[0] + x;
    if(isBlank(x,y)) return false;
    else return Stats.isDetection(array[voxel]);
  };  

  void      removeMW();
  
private: 
  int        minSize;    ///< the minimum number of pixels for a detection to be accepted.
};


/****************************************************************/
//////////////////////////////////////////////////////
// Prototypes for functions that use above classes
//////////////////////////////////////////////////////

void findSources(Image &image);
void findSources(Image &image, float mean, float sigma); 

vector <Detection> searchReconArray(long *dim, float *originalArray,
				    float *reconArray, Param &par,
				    StatsContainer<float> &stats);
vector <Detection> search3DArray(long *dim, float *Array, Param &par,
				 StatsContainer<float> &stats);

void growObject(Detection &object, Cube &cube);

#endif
