// -----------------------------------------------------------------------
// cubes.hh: Definitions of the DataArray, Cube and Image classes.
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
#ifndef CUBES_H
#define CUBES_H

#include <iostream>
#include <string>
#include <vector>

#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Detection/columns.hh>
#include <duchamp/Cubes/plots.hh>
#include <duchamp/Utils/Statistics.hh>
#include <duchamp/PixelMap/Scan.hh>
#include <duchamp/PixelMap/Object2D.hh>


namespace duchamp
{


  /// @brief Search a reconstructed array for significant detections. 
  std::vector <Detection> 
  searchReconArray(long *dim, float *originalArray, float *reconArray, 
		   Param &par, Statistics::StatsContainer<float> &stats);
  std::vector <Detection> 
  searchReconArraySimple(long *dim, float *originalArray, float *reconArray, 
			 Param &par, Statistics::StatsContainer<float> &stats);

  /// @brief Search a 3-dimensional array for significant detections. 
  std::vector <Detection> 
  search3DArray(long *dim, float *Array, Param &par,
		Statistics::StatsContainer<float> &stats);
  std::vector <Detection> 
  search3DArraySimple(long *dim, float *Array, Param &par,
		      Statistics::StatsContainer<float> &stats);

  //=========================================================================

  /// @brief Base class for the image container.
  /// 
  /// @details Definition of an n-dimensional data array: array of
  ///     pixel values, size & dimensions array of Detection objects

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
    long               getDimX(){if(numDim>0) return axisDim[0];else return 0;}; 
    long               getDimY(){if(numDim>1) return axisDim[1];else return 1;};
    long               getDimZ(){if(numDim>2) return axisDim[2];else return 1;};
    long               getSize(){ return numPixels; };
    short int          getNumDim(){ return numDim; };
    virtual float      getPixValue(long pos){ return array[pos]; };
    virtual void       setPixValue(long pos, float f){array[pos] = f;};
    Detection          getObject(long number){ return objectList->at(number); };
    Detection *        pObject(long number){ return &(objectList->at(number));};
    std::vector <Detection> getObjectList(){ return *objectList; };
    std::vector <Detection> *pObjectList(){ return objectList; };
    std::vector <Detection> &ObjectList(){ std::vector<Detection> &rlist = *objectList; return rlist; };
    long               getNumObj(){ return objectList->size(); };

    /// @brief Delete all objects from the list of detections. 
    void               clearDetectionList(){ 
      //objectList->clear(); 
      delete objectList;
      objectList = new std::vector<Detection>;
    };

    /// @brief Read a parameter set from file. 
    int                readParam(std::string paramfile){
      /// @brief 
      ///  Uses Param::readParams() to read parameters from a file.
      ///  \param paramfile The file to be read.
       
      return par.readParams(paramfile); 
    };

    //-----------------------------------------
    // Related to the various arrays
    // 
    /// @brief  Return first three dimensional axes. 
    void               getDim(long &x, long &y, long &z);
    /// @brief Return array of dimensional sizes.
    void               getDimArray(long *output);
    long *             getDimArray(){return axisDim;};

    /// @brief Return pixel value array. 
    void               getArray(float *output);
    float *            getArray(){return array;};

    /// @brief Save pixel value array. 
    virtual void       saveArray(float *input, long size);

    //-----------------------------------------
    // Related to the object lists
    //
    /// @brief Adds a single detection to the object list.
    void               addObject(Detection object);

    /// @brief Adds all objects in a detection list to the object list. 
    void               addObjectList(std::vector <Detection> newlist);   

    /// @brief Add pixel offsets to object coordinates. 
    void               addObjectOffsets(); 

    //-----------------------------------------
    // Parameter list related.
    //
    /// @brief Output the Param set.
    void               showParam(std::ostream &stream){ stream << par; }; 
    /// @brief Return the Param set.
    Param              getParam(){ return par; }; 
    /// @brief Save a Param set to the Cube.
    void               saveParam(Param &newpar){par = newpar;};
    /// @brief Provides a reference to the Param set.
    Param&             pars(){ Param &rpar = par; return rpar; }; 
    /// @brief Is the voxel number given by vox a BLANK value?
    bool               isBlank(int vox); 

    // --------------------------------------------
    // Statistics
    //
    /// @brief  Returns the StatsContainer. 
    Statistics::StatsContainer<float> getStats(){ return Stats; };

    /// @brief Provides a reference to the StatsContainer. 
    Statistics::StatsContainer<float>& stats(){ 
      Statistics::StatsContainer<float> &rstats = Stats;  return rstats;
    };
 
    /// @brief Save a StatsContainer to the Cube. 
    void saveStats(Statistics::StatsContainer<float> newStats){ Stats = newStats;};

    /// @brief A detection test for value. 
    bool isDetection(float value);

    /// @brief  A detection test for pixel.  
    bool isDetection(long voxel);


    /// @brief Output operator for DataArray.
    friend std::ostream& operator<< (std::ostream& theStream, DataArray &array);
 

  protected:
    short int                numDim;     ///< Number of dimensions.
    long                    *axisDim;    ///< Array of axis dimensions of cube
    bool                     axisDimAllocated; ///< has axisDim been allocated?
    long                     numPixels;  ///< Total number of pixels in cube.
    float                   *array;      ///< Array of data.
    bool                     arrayAllocated; ///< has the array been allocated?
    std::vector <Detection> *objectList; ///< The list of detected objects.
    Param                    par;        ///< A parameter list.
    Statistics::StatsContainer<float> Stats; ///< The statistics for the DataArray.
  };



  //=========================================================================

  /// @brief Definition of an data-cube object (3D):
  ///     a DataArray object limited to dim=3

  class Cube : public DataArray
  {
  public:
    Cube();                 ///< Basic Cube constructor.
    Cube(long nPix);        ///< Alternative Cube constructor.
    Cube(long *dimensions); ///< Alternative Cube constructor.
    virtual ~Cube();        ///< Basic Cube destructor.

    bool        is2D();
    void        checkDim(){head.set2D(is2D());};

    // INLINE functions -- definitions included after class declaration.
    /// @brief Is the voxel number given by vox a BLANK value? 
    bool        isBlank(int vox){ return par.isBlank(array[vox]); };

    /// @brief Is the voxel at (x,y,z) a BLANK value? 
    bool        isBlank(long x, long y, long z){ 
      return par.isBlank(array[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x]); };

    /// @brief Return a bool array masking blank pixels: 1=good, 0=blank 
    bool *      makeBlankMask(){return par.makeBlankMask(array, numPixels);};

    /// @brief Does the Cube::recon array exist? 
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
    void        setPixValue(long pos, float f){array[pos] = f;};
    void        setPixValue(long x, long y, long z, float f){ 
      array[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x] = f;};
    void        setDetectMapValue(long pos, short f){ detectMap[pos] = f;};
    void        setDetectMapValue(long x, long y, short f){ detectMap[y*axisDim[0] + x] = f;};
    void        setReconValue(long pos, float f){recon[pos] = f;};
    void        setReconValue(long x, long y, long z, float f){ 
      recon[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x] = f; };
    void        setReconFlag(bool f){reconExists = f;};

    std::vector<Column::Col> getLogCols(){return logCols;};
    void        setLogCols(std::vector<Column::Col> C){logCols=C;};
    std::vector<Column::Col> getFullCols(){return fullCols;};
    void        setFullCols(std::vector<Column::Col> C){fullCols=C;};

    // additional functions -- in Cubes/cubes.cc
    /// @brief Allocate memory correctly, with WCS defining the correct axes. 
    void        initialiseCube(long *dimensions, bool allocateArrays=true);

    /// @brief Read in a FITS file, with subsection correction. 
    int         getCube();
    /// @brief Read in a FITS file, with subsection correction. 
    int         getMetadata();

    /// @brief Read in command-line options. 
    //   int         getopts(int argc, char ** argv);
    int         getopts(int argc, char ** argv, std::string progname="Duchamp"){return par.getopts(argc,argv,progname);};

    /// @brief Read in reconstructed & smoothed arrays from FITS files on disk. 
    void        readSavedArrays();

    /// @brief Save an external array to the Cube's pixel array 
    void        saveArray(float *input, long size);

    /// @brief Save an external array to the Cube's pixel array 
    void        saveArray(std::vector<float> &input);

    /// @brief Save an external array to the Cube's reconstructed array. 
    void        saveRecon(float *input, long size);

    /// @brief Save reconstructed array to an external array. 
    void        getRecon(float *output);
    float *     getRecon(){return recon; };

    /// @brief Set Milky Way channels to zero. 
    void        removeMW();

    //------------------------
    // Statistics for cube
    //

    /// @brief Calculate the statistics for the Cube (older version). 
    void        setCubeStatsOld();

    /// @brief Calculate the statistics for the Cube. 
    void        setCubeStats();

    /// @brief Set up thresholds for the False Discovery Rate routine. 
    void        setupFDR();
    /// @brief Set up thresholds for the False Discovery Rate routine using a particular array. 
    void        setupFDR(float *input);

    /// @brief A detection test for a given voxel. 
    bool        isDetection(long x, long y, long z);

    //-----------------------------
    // Dealing with the detections 
    //

    /// @brief Calculate the object fluxes 
    void        calcObjectFluxes();

    /// @brief Calculate the WCS parameters for each Cube Detection. 
    void        calcObjectWCSparams();
    /// @brief Calculate the WCS parameters for each Cube Detection, using flux information in Voxels. 
    void        calcObjectWCSparams(std::vector< std::vector<PixelInfo::Voxel> > bigVoxList);

    /// @brief Sort the list of detections. 
    void        sortDetections();

    /// @brief Update the map of detected pixels. 
    void        updateDetectMap();

    /// @brief Update the map of detected pixels for a given Detection. 
    void        updateDetectMap(Detection obj);

    /// @brief Clear the map of detected pixels. 
    void        clearDetectMap(){
      for(int i=0;i<axisDim[0]*axisDim[1];i++) detectMap[i]=0;
    };

    /// @brief Find the flux enclosed by a Detection. 
    float       enclosedFlux(Detection obj);

    /// @brief Set up the output column definitions for the Cube and its Detection list. 
    void        setupColumns();

    /// @brief Is the object at the edge of the image? 
    bool        objAtSpatialEdge(Detection obj);

    /// @brief Is the object at an end of the spectrum? 
    bool        objAtSpectralEdge(Detection obj);

    /// @brief Set warning flags for the detections. 
    void        setObjectFlags();

    // ----------------------------
    // Dealing with the WCS
    //
    /// @brief Return the FitsHeader object. 
    FitsHeader  getHead(){ return head; }; 
    /// @brief Define the FitsHeader object. 
    void        setHead(FitsHeader F){head = F;};
    /// @brief Provide a reference to the FitsHeader object.
    FitsHeader& header(){ FitsHeader &h = head; return h; };

    /// @brief Convert a point from WCS to Pixel coords. 
    int         wcsToPix(const double *world, double *pix);

    /// @brief Convert a set of points from WCS to Pixel coords. 
    int         wcsToPix(const double *world, double *pix, const int npts);

    /// @brief Convert a point from Pixel to WCS coords. 
    int         pixToWCS(const double *pix, double *world);

    /// @brief Convert a set of points from Pixel to WCS coords. 
    int         pixToWCS(const double *pix, double *world, const int npts);

    //-------------------------------------------
    // FITS-I/O related functions -- not in cubes.cc
    //
    /// @brief Function to read in FITS file.
    int         getMetadata(std::string fname);  // in Cubes/getImage.cc
    int         getCube(std::string fname);  // in Cubes/getImage.cc

    /// @brief Convert the flux units to something user-specified.
    void        convertFluxUnits(); // in Cubes/getImage.cc

    /// @brief Function to retrieve FITS data array 
    int         getFITSdata(std::string fname);   // in FitsIO/dataIO.cc

    /// @brief Save a mask array to disk.
    void        saveMaskCube();       // in Cubes/saveImage.cc

    /// @brief Save Smoothed array to disk.
    void        saveSmoothedCube();       // in Cubes/saveImage.cc

    /// @brief Save Reconstructed array to disk. 
    void        saveReconstructedCube();  // in Cubes/saveImage.cc

    /// @brief Read in reconstructed array from FITS file. 
    int         readReconCube();  // in Cubes/readRecon.cc
  
    /// @brief Read in Hanning-smoothed array from FITS file. 
    int         readSmoothCube();     // in Cubes/readSmooth.cc  

    //-------------------------------------
    // Functions that act on the cube
    //

    /// @brief Remove excess BLANK pixels from spatial edge of cube. 
    void        trimCube();         // in Cubes/trimImage.cc

    /// @brief Replace BLANK pixels to spatial edge of cube. 
    void        unTrimCube();       // in Cubes/trimImage.cc

    /// @brief Removes the baselines from the spectra, and stores in Cube::baseline 
    void        removeBaseline();   // in Cubes/baseline.cc

    /// @brief Replace the baselines stored in Cube::baseline 
    void        replaceBaseline();  // in Cubes/baseline.cc

    /// @brief Multiply all pixel values by -1. 
    void        invert();           // in Cubes/invertCube.cc

    /// @brief Undo the inversion, and invert fluxes of all detected objects. 
    void        reInvert();         // in Cubes/invertCube.cc

    //-------------------------------------
    // Reconstruction, Searching and Merging functions
    //
    // in ATrous/ReconSearch.cc
    /// @brief Front-end to reconstruction & searching functions.
    void        ReconSearch();
    /// @brief Switcher to reconstruction functions 
    void        ReconCube();
    /// @brief Performs 1-dimensional a trous reconstruction on the Cube. 
    void        ReconCube1D();
    /// @brief Performs 2-dimensional a trous reconstruction on the Cube. 
    void        ReconCube2D();
    /// @brief Performs 3-dimensional a trous reconstruction on the Cube. 
    void        ReconCube3D();

    // in Cubes/CubicSearch.cc
    /// @brief Front-end to the cubic searching routine. 
    void        CubicSearch();

    // in Cubes/smoothCube.cc 
    /// @brief Smooth the cube with the requested method.
    void        SmoothCube();
    /// @brief Front-end to the smoothing and searching functions. 
    void        SmoothSearch();
    /// @brief A function to spatially smooth the cube and search the result. 
    void        SpatialSmoothNSearch();
    /// @brief A function to Hanning-smooth the cube. 
    void        SpectralSmooth();
    /// @brief A function to spatially-smooth the cube. 
    void        SpatialSmooth();

    void        Simple3DSearch(){
      /// @brief Basic front-end to the simple 3d searching function -- does
      /// nothing more. 
      /// 
      /// @details This function just runs the search3DArraySimple function,
      /// storing the results in the Cube::objectList vector. No stats
      /// are calculated beforehand, and no logging or detection map
      /// updating is done.
      *objectList = search3DArraySimple(axisDim,array,par,Stats);
    };

    void        Simple3DSearchRecon(){
      /// @brief Basic front-end to the 3d searching function used in the
      /// reconstruction case -- does nothing more.
      /// 
      /// @details This function just runs the searchReconArraySimple
      /// function, storing the results in the Cube::objectList
      /// vector. No stats are calculated beforehand, and no logging
      /// or detection map updating is done. The recon array is
      /// assumed to have been defined already.

      *objectList = searchReconArraySimple(axisDim,array,recon,par,Stats);
    };

    void        Simple3DSearchSmooth(){
      /// @brief Basic front-end to the simple 3d searching function
      /// run on the smoothed array -- does nothing more.
      /// 
      /// @details This function just runs the search3DArraySimple
      /// function on the recon array (which contains the smoothed
      /// array), storing the results in the Cube::objectList
      /// vector. No stats are calculated beforehand, and no logging
      /// or detection map updating is done. The recon array is
      /// assumed to have been defined already.

      *objectList = search3DArraySimple(axisDim,recon,par,Stats);
    };

    // in Cubes/Merger.cc
    /// @brief Merge all objects in the detection list so that only distinct ones are left. 
    void        ObjectMerger();
  
    // in Cubes/existingDetections.cc
    /// @brief Read a previously-created log file to get the detections without searching 
    int getExistingDetections();

    //-------------------------------------
    // Text outputting of detected objects.
    //   in Cubes/detectionIO.cc
    //
    /// @brief Set up the output file with parameters and stats. 
    void        prepareOutputFile();

    /// @brief Write the statistical information to the output file. 
    void        outputStats();

    /// @brief Output detections to the output file and standard output. 
    void        outputDetectionList();

    /// @brief Prepare the log file for output. 
    void        prepareLogFile(int argc, char *argv[]);

    /// @brief Output detections to the log file. 
    void        logDetectionList();

    /// @brief Output a single detection to the log file. 
    void        logDetection(Detection obj, int counter);

    /// @brief Output detections to a Karma annotation file. 
    void        outputDetectionsKarma(std::ostream &stream);

    /// @brief Output detections to a VOTable. 
    void        outputDetectionsVOTable(std::ostream &stream);

    // ----------------------------------
    // Graphical plotting of the cube and the detections.
    //
    //  in Cubes/plotting.cc
    /// @brief Plot a spatial map of detections based on number of detected channels. 
    void        plotDetectionMap(std::string pgDestination);

    /// @brief Plot a spatial map of detections based on 0th moment map of each object. 
    void        plotMomentMap(std::string pgDestination);

    /// @brief Plot a spatial map of detections based on 0th moment map of each object to a number of PGPLOT devices. 
    void        plotMomentMap(std::vector<std::string> pgDestination);

    /// @brief Draw WCS axes over a PGPLOT map. 
    void        plotWCSaxes(){duchamp::plotWCSaxes(head.getWCS(),axisDim);};

    //  in Cubes/outputSpectra.cc
    /// @brief Print spectra of each detected object. 
    void        outputSpectra();

    /// @brief Write out text file of all spectra. 
    void        writeSpectralData();

    /// @brief Print spectrum of a single object 
    void        plotSpectrum(int objNum, Plot::SpectralPlot &plot);
    /// @brief Plot the image cutout for a single object 
    void        plotSource(Detection obj, Plot::CutoutPlot &plot);

    /// @brief Get the spectral arrays 
    void        getSpectralArrays(int objNumber, float *specx, float *specy, float *specRecon, float *specBase);

    //  in Cubes/drawMomentCutout.cc
    /// @brief Draw the 0th moment map for a single object. 
    void        drawMomentCutout(Detection &object);

    /// @brief Draw a scale bar indicating angular size of the cutout. 
    void        drawScale(float xstart,float ystart,float channel);

    /// @brief Draw a yellow line around the edge of the spatial extent of pixels. 
    void        drawFieldEdge();

  private: 
    float      *recon;            ///< reconstructed array - used when doing a trous reconstruction.
    bool        reconExists;      ///< flag saying whether there is a reconstruction
    short      *detectMap;        ///< "moment map" - x,y locations of detected pixels
    float      *baseline;         ///< array of spectral baseline values.
			     
    bool        reconAllocated;   ///< have we allocated memory for the recon array?
    bool        baselineAllocated;///< have we allocated memory for the baseline array?
    FitsHeader  head;             ///< the WCS and other header information.
    std::vector<Column::Col> fullCols;    ///< the list of all columns as printed in the results file
    std::vector<Column::Col> logCols;     ///< the list of columns as printed in the log file

  };

  ////////////
  //// Cube inline function definitions
  ////////////

  inline int Cube::wcsToPix(const double *world, double *pix)
  {
    ///  @brief Use the WCS in the FitsHeader to convert from WCS to pixel coords for
    ///   a single point.
    ///  \param world The world coordinates.
    ///  \param pix The returned pixel coordinates.
    /// 
    return this->head.wcsToPix(world,pix);
  } 
  inline int Cube::wcsToPix(const double *world, double *pix, const int npts)
  {
    ///  @brief Use the WCS in the FitsHeader to convert from WCS to pixel coords for
    ///   a set of points.
    ///  \param world The world coordinates.
    ///  \param pix The returned pixel coordinates.
    ///  \param npts The number of points being converted.

    return this->head.wcsToPix(world,pix,npts);
  }
  inline int Cube::pixToWCS(const double *pix, double *world)
  {
    ///  @brief Use the WCS in the FitsHeader to convert from pixel to WCS coords for
    ///   a single point.
    ///  \param pix The pixel coordinates.
    ///  \param world The returned world coordinates.

    return this->head.pixToWCS(pix,world);
  }
  inline int Cube::pixToWCS(const double *pix, double *world, const int npts)
  {
    ///  @brief Use the WCS in the FitsHeader to convert from pixel to WCS coords for
    ///   a set of points.
    ///  \param pix The pixel coordinates.
    ///  \param world The returned world coordinates.
    ///  \param npts The number of points being converted.

    return this->head.pixToWCS(pix,world,npts);
  }


  //============================================================================

  ///  @brief A DataArray object limited to two dimensions, and with some additional
  ///   special functions. 
  /// 
  ///  @details It is used primarily for searching a 1- or 2-D array with 
  ///   lutz_detect() and spectrumDetect().

  class Image : public DataArray
  {
  public:
    Image(){numPixels=0; numDim=2;};
    Image(long nPix);
    Image(long *dimensions);
    virtual ~Image(){};

    //--------------------
    // Defining the array
    //
    /// @brief Save an external array to the Cube's pixel array 
    void      saveArray(float *input, long size);

    /// @brief Extract a spectrum from an array and save to the local pixel array. 
    void      extractSpectrum(float *Array, long *dim, long pixel);

    /// @brief Extract an image from an array and save to the local pixel array. 
    void      extractImage(float *Array, long *dim, long channel);

    /// @brief Extract a spectrum from a cube and save to the local pixel array. 
    void      extractSpectrum(Cube &cube, long pixel);

    /// @brief Extract an image from a cube and save to the local pixel array. 
    void      extractImage(Cube &cube, long channel);

    //--------------------
    // Accessing the data.
    //
    float     getPixValue(long pos){ return array[pos]; };
    float     getPixValue(long x, long y){return array[y*axisDim[0] + x];};
    // the next few should have checks against array overflow...
    void      setPixValue(long pos, float f){array[pos] = f;};
    void      setPixValue(long x, long y, float f){array[y*axisDim[0] + x] = f;};
    bool      isBlank(int vox){return par.isBlank(array[vox]);};
    bool      isBlank(long x,long y){return par.isBlank(array[y*axisDim[0]+x]);};

    //-----------------------
    // Detection-related
    //
    // in Detection/lutz_detect.cc:
    /// @brief Detect objects in a 2-D image 
    std::vector<PixelInfo::Object2D> lutz_detect();

    // in Detection/spectrumDetect.cc:
    /// @brief Detect objects in a 1-D spectrum 
    std::vector<PixelInfo::Scan> spectrumDetect();

    unsigned int getMinSize(){return minSize;};
    void         setMinSize(int i){minSize=i;};

    /// @brief  A detection test for a pixel location.  
    bool      isDetection(long x, long y){
      /// @details Test whether a pixel (x,y) is a statistically
      /// significant detection, according to the set of statistics in
      /// the local StatsContainer object.

      long voxel = y*axisDim[0] + x;
      if(isBlank(x,y)) return false;
      else return Stats.isDetection(array[voxel]);
    };  

    /// @brief Blank out a set of channels marked as being Milky Way channels 
    void      removeMW();

  private: 
    unsigned int minSize;    ///< the minimum number of pixels for a detection to be accepted.
  };


}

#endif
