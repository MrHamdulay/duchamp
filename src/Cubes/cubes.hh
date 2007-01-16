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

  //-----------------------------------------
  // Obvious inline accessor functions.
  //
  long               getDimX(){if(numDim>0) return this->axisDim[0];else return 0;}; 
  long               getDimY(){if(numDim>1) return this->axisDim[1];else return 1;};
  long               getDimZ(){if(numDim>2) return this->axisDim[2];else return 1;};
  long               getSize(){ return this->numPixels; };
  short int          getNumDim(){ return this->numDim; };
  virtual float              getPixValue(long pos){ return array[pos]; };
  virtual void               setPixValue(long pos, float f){array[pos] = f;};;
  Detection          getObject(long number){ return objectList[number]; };
  vector <Detection> getObjectList(){ return objectList; };
  long               getNumObj(){ return objectList.size(); };

  /** Delete all objects from the list of detections. */
  void               clearDetectionList(){ this->objectList.clear(); };

  /** Read a parameter set from file. */
  int                readParam(string paramfile){
    /** 
     *  Uses Param::readParams() to read parameters from a file.
     *  \param paramfile The file to be read.
     */
  return par.readParams(paramfile); };

  //-----------------------------------------
  // Related to the various arrays
  // 
  /**  Return first three dimensional axes. */
  void               getDim(long &x, long &y, long &z);
  /** Return array of dimensional sizes.*/
  void               getDimArray(long *output);

  /** Return pixel value array. */
  void               getArray(float *output);

  /** Save pixel value array. */
  virtual void       saveArray(float *input, long size);

  //-----------------------------------------
  // Related to the object lists
  //
  /** Adds a single detection to the object list.*/
  void               addObject(Detection object);

  /** Adds all objects in a detection list to the object list. */
  void               addObjectList(vector <Detection> newlist);   
  /** Add pixel offsets to object coordinates. */
  void               addObjectOffsets(); 

  //-----------------------------------------
  // Parameter list related.
  //
  /** Output the Param set.*/
  void               showParam(std::ostream &stream){ stream << this->par; }; 
  /** Return the Param set.*/
  Param              getParam(){ return this->par; }; 
  /** Save a Param set to the Cube.*/
  void               saveParam(Param newpar){this->par = newpar;};
  /** Provides a reference to the Param set.*/
  Param&             pars(){ Param &rpar = this->par; return rpar; }; 
  /** Is the voxel number given by vox a BLANK value?*/
  bool               isBlank(int vox); 

  // --------------------------------------------
  // Statistics
  //
  /**  Returns the StatsContainer. */
  StatsContainer<float> getStats(){ return this->Stats; };

  /** Provides a reference to the StatsContainer. */
  StatsContainer<float>& stats(){ StatsContainer<float> &rstats = this->Stats;  return rstats;}; 
  /** Save a StatsContainer to the Cube. */
  void saveStats(StatsContainer<float> newStats){ this->Stats = newStats;};

  /** A detection test for value. */
  bool isDetection(float value);

  /**  A detection test for pixel.  */
  bool isDetection(long voxel);


  /** Output operator for DataArray.*/
  friend std::ostream& operator<< (std::ostream& theStream, DataArray &array);
 

protected:
  short int               numDim;     ///< Number of dimensions.
  long                   *axisDim;    ///< Array of dimensions of cube (ie. how large in each direction).
  long                    numPixels;  ///< Total number of pixels in cube.
  float                  *array;      ///< Array of data.
  vector <Detection>      objectList; ///< The list of detected objects.
  Param                   par;        ///< A parameter list.
  StatsContainer<float>   Stats;      ///< The statistics for the DataArray.
};



//===============================================================================

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
  /** Is the voxel number given by vox a BLANK value? */
  bool        isBlank(int vox){ return par.isBlank(array[vox]); };

  /** Is the voxel at (x,y,z) a BLANK value? */
  bool        isBlank(long x, long y, long z){ 
    return par.isBlank(array[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x]); };

  /** Does the Cube::recon array exist? */
  bool        isRecon(){ return reconExists; }; 

  float       getPixValue(long pos){ return array[pos]; };
  float       getPixValue(long x, long y, long z){ 
    return array[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x]; };
  short       getDetectMapValue(long pos){ return detectMap[pos]; };
  short       getDetectMapValue(long x, long y){ return detectMap[y*axisDim[0]+x]; };
  float       getReconValue(long pos){ return recon[pos]; };
  float       getReconValue(long x, long y, long z){ 
    return recon[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x];};
  float       getBaselineValue(long pos){ return baseline[pos]; };
  float       getBaselineValue(long x, long y, long z){ 
    return baseline[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x]; };
  void        setPixValue(long pos, float f){array[pos] = f;};;
  void        setPixValue(long x, long y, long z, float f){ 
    array[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x] = f;};
  void        setDetectMapValue(long pos, short f){ detectMap[pos] = f;};
  void        setDetectMapValue(long x, long y, short f){ detectMap[y*axisDim[0] + x] = f;};
  void        setReconValue(long pos, float f){recon[pos] = f;};
  void        setReconValue(long x, long y, long z, float f){ 
    recon[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x] = f; };
  void        setReconFlag(bool f){reconExists = f;};

  vector<Col> getLogCols(){return logCols;};
  void        setLogCols(vector<Col> C){logCols=C;};
  vector<Col> getFullCols(){return fullCols;};
  void        setFullCols(vector<Col> C){fullCols=C;};

  // additional functions -- in Cubes/cubes.cc
  /** Allocate memory correctly, with WCS defining the correct axes. */
  void        initialiseCube(long *dimensions);

  /** Read in a FITS file, with subsection correction. */
  int         getCube();

  /** Read in command-line options. */
  int         getopts(int argc, char ** argv);

  /** Read in reconstructed & smoothed arrays from FITS files on disk. */
  void        readSavedArrays();

  /** Save an external array to the Cube's pixel array */
  void        saveArray(float *input, long size);

  /** Save an external array to the Cube's reconstructed array. */
  void        saveRecon(float *input, long size);

  /** Save reconstructed array to an external array. */
  void        getRecon(float *output);

  /** Set Milky Way channels to zero. */
  void        removeMW();

  //------------------------
  // Statistics for cube
  //

  /** Calculate the statistics for the Cube (older version). */
  void        setCubeStatsOld();

  /** Calculate the statistics for the Cube. */
  void        setCubeStats();

  /** Set up thresholds for the False Discovery Rate routine. */
  int         setupFDR();

  /** A detection test for a given voxel. */
  bool        isDetection(long x, long y, long z);

  //-----------------------------
  // Dealing with the detections 
  //

  /** Calculate the WCS parameters for each Cube Detection. */
  void        calcObjectWCSparams();

  /** Sort the list of detections. */
  void        sortDetections();

  /** Update the map of detected pixels. */
  void        updateDetectMap();

  /** Update the map of detected pixels for a given Detection. */
  void        updateDetectMap(Detection obj);

  /** Find the flux enclosed by a Detection. */
  float       enclosedFlux(Detection obj);

  /** Set up the output column definitions for the Cube and its Detection list. */
  void        setupColumns();

  /** Is the object at the edge of the image? */
  bool        objAtSpatialEdge(Detection obj);

  /** Is the object at an end of the spectrum? */
  bool        objAtSpectralEdge(Detection obj);

  /** Set warning flags for the detections. */
  void        setObjectFlags();

  // ----------------------------
  // Dealing with the WCS
  //
  FitsHeader  getHead(){ return head; }; 
  void        setHead(FitsHeader F){head = F;};
  FitsHeader& header(){ FitsHeader &h = head; return h; };

  /** Convert a point from WCS to Pixel coords. */
  int         wcsToPix(const double *world, double *pix);

  /** Convert a set of points from WCS to Pixel coords. */
  int         wcsToPix(const double *world, double *pix, const int npts);

  /** Convert a point from Pixel to WCS coords. */
  int         pixToWCS(const double *pix, double *world);

  /** Convert a set of points from Pixel to WCS coords. */
  int         pixToWCS(const double *pix, double *world, const int npts);

  //-------------------------------------------
  // FITS-I/O related functions -- not in cubes.cc
  //
  /** Function to read in FITS file.*/
  int         getCube(string fname);  // in Cubes/getImage.cc

  /** Function to retrieve FITS data array */
  int         getFITSdata(string fname);   // in FitsIO/dataIO.cc

  /** Save Hanning-smoothed array to disk.*/
  void        saveSmoothedCube();       // in Cubes/saveImage.cc
  /** Save Reconstructed array to disk. */
  void        saveReconstructedCube();  // in Cubes/saveImage.cc

  /** Read in reconstructed array from FITS file. */
  int         readReconCube();  // in Cubes/readRecon.cc
  
  /** Read in Hanning-smoothed array from FITS file. */
  int         readSmoothCube();     // in Cubes/readSmooth.cc  

  //-------------------------------------
  // Functions that act on the cube
  //

  /** Remove excess BLANK pixels from spatial edge of cube. */
  void        trimCube();         // in Cubes/trimImage.cc

  /** Replace BLANK pixels to spatial edge of cube. */
  void        unTrimCube();       // in Cubes/trimImage.cc

  /** Removes the baselines from the spectra, and stores in Cube::baseline */
  void        removeBaseline();   // in Cubes/baseline.cc

  /** Replace the baselines stored in Cube::baseline */
  void        replaceBaseline();  // in Cubes/baseline.cc

  /** Multiply all pixel values by -1. */
  void        invert();           // in Cubes/invertCube.cc

  /** Undo the inversion, and invert fluxes of all detected objects. */
  void        reInvert();         // in Cubes/invertCube.cc

  //-------------------------------------
  // Reconstruction, Searching and Merging functions
  //
  // in ATrous/ReconSearch.cc
  /** Switcher to reconstruction functions */
  void        ReconCube();

  /** Front-end to reconstruction & searching functions.*/
  void        ReconSearch();
  /** Performs 1-dimensional a trous reconstruction on the Cube. */
  void        ReconCube1D();
  /** Performs 2-dimensional a trous reconstruction on the Cube. */
  void        ReconCube2D();
  /** Performs 3-dimensional a trous reconstruction on the Cube. */
  void        ReconCube3D();

  // in Cubes/CubicSearch.cc
  /** Front-end to the cubic searching routine. */
  void        CubicSearch();

  // in Cubes/smoothCube.cc 
  /** Front-end to the smoothing and searching functions. */
  void        SmoothSearch();
  /** A function to Hanning-smooth the cube. */
  void        SmoothCube();

  // in Cubes/Merger.cc
  /** Merge all objects in the detection list so that only distinct ones are left. */
  void        ObjectMerger();
  
  //-------------------------------------
  // Text outputting of detected objects.
  //   in Cubes/detectionIO.cc
  //
  /** Output detections to a Karma annotation file. */
  void        outputDetectionsKarma(std::ostream &stream);

  /** Output detections to a VOTable. */
  void        outputDetectionsVOTable(std::ostream &stream);

  /** Output detections to the output file and standard output. */
  void        outputDetectionList();

  /** Output detections to the log file. */
  void        logDetectionList();

  /** Output a single detection to the log file. */
  void        logDetection(Detection obj, int counter);


  // ----------------------------------
  // Graphical plotting of the cube and the detections.
  //
  /** Draw blank edges of cube. */
  void        plotBlankEdges();  // in cubes.cc

  //  in Cubes/plotting.cc
  /** Plot a spatial map of detections based on number of detected channels. */
  void        plotDetectionMap(string pgDestination);

  /** Plot a spatial map of detections based on 0th moment map of each object. */
  void        plotMomentMap(string pgDestination);

  /** Plot a spatial map of detections based on 0th moment map of each object to a number of PGPLOT devices. */
  void        plotMomentMap(vector<string> pgDestination);

  /** Draw WCS axes over a PGPLOT map. */
  void        plotWCSaxes();

  //  in Cubes/outputSpectra.cc
  /** Print spectra of each detected object. */
  void        outputSpectra();

  /** Print spectrum of a single object */
  void        plotSpectrum(Detection obj,Plot::SpectralPlot &plot);

  //  in Cubes/drawMomentCutout.cc
  /** Draw the 0th moment map for a single object. */
  void        drawMomentCutout(Detection &object);

  /** Draw a scale bar indicating angular size of the cutout. */
  void        drawScale(float xstart,float ystart,float channel);

  /** Draw a yellow line around the edge of the spatial extent of pixels. */
  void        drawFieldEdge();

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


//===============================================================================

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
  /** Save an external array to the Cube's pixel array */
  void      saveArray(float *input, long size);

  /** Extract a spectrum from an array and save to the local pixel array. */
  void      extractSpectrum(float *Array, long *dim, long pixel);

  /** Extract an image from an array and save to the local pixel array. */
  void      extractImage(float *Array, long *dim, long channel);

  /** Extract a spectrum from a cube and save to the local pixel array. */
  void      extractSpectrum(Cube &cube, long pixel);

  /** Extract an image from a cube and save to the local pixel array. */
  void      extractImage(Cube &cube, long channel);

  //--------------------
  // Accessing the data.
  float     getPixValue(long pos){ return array[pos]; };
  float     getPixValue(long x, long y){return array[y*axisDim[0] + x];};
  // the next few should have checks against array overflow...
  void      setPixValue(long pos, float f){array[pos] = f;};;
  void      setPixValue(long x, long y, float f){array[y*axisDim[0] + x] = f;};
  bool      isBlank(int vox){return par.isBlank(array[vox]);};
  bool      isBlank(long x,long y){return par.isBlank(array[y*axisDim[0]+x]);};

  //-----------------------
  // Detection-related
  /** Detect objects in a 2-D image */
  void      lutz_detect();            // in Detection/lutz_detect.cc

  /** Detect objects in a 1-D spectrum */
  void      spectrumDetect();         // in Detection/spectrumDetect.cc

  int       getMinSize(){return minSize;};
  void      setMinSize(int i){minSize=i;};
  // the rest are in Detection/thresholding_functions.cc

  /**  A detection test for a pixel location.  */
  bool      isDetection(long x, long y){
    /**
     * Test whether a pixel (x,y) is a statistically significant detection,
     * according to the set of statistics in the local StatsContainer object.
     */
    long voxel = y*axisDim[0] + x;
    if(isBlank(x,y)) return false;
    else return Stats.isDetection(array[voxel]);
  };  

  /** Blank out a set of channels marked as being Milky Way channels */
  void      removeMW();
  
private: 
  int        minSize;    ///< the minimum number of pixels for a detection to be accepted.
};


/****************************************************************/
//////////////////////////////////////////////////////
// Prototypes for functions that use above classes
//////////////////////////////////////////////////////

/** Search a reconstructed array for significant detections. */
vector <Detection> searchReconArray(long *dim, float *originalArray,
				    float *reconArray, Param &par,
				    StatsContainer<float> &stats);

/** Search a 3-dimensional array for significant detections. */
vector <Detection> search3DArray(long *dim, float *Array, Param &par,
				 StatsContainer<float> &stats);

/** Grow an object to a lower threshold */
void growObject(Detection &object, Cube &cube);

/** Draw the edge of the BLANK region on a map.*/
void drawBlankEdges(float *dataArray, int xdim, int ydim, Param &par);

#endif
