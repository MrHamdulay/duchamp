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
#include <duchamp/Outputs/columns.hh>
#include <duchamp/Outputs/CatalogueSpecification.hh>
#include <duchamp/Plotting/SpectralPlot.hh>
#include <duchamp/Plotting/CutoutPlot.hh>
#include <duchamp/Utils/Statistics.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/PixelMap/Scan.hh>
#include <duchamp/PixelMap/Object2D.hh>


namespace duchamp
{


  /// @brief Search a reconstructed array for significant detections. 
  std::vector <Detection> searchReconArray(size_t *dim, float *originalArray, float *reconArray, 
					   Param &par, Statistics::StatsContainer<float> &stats);
  std::vector <Detection> searchReconArraySpectral(size_t *dim, float *originalArray, float *reconArray, 
						   Param &par, Statistics::StatsContainer<float> &stats);
  std::vector <Detection> searchReconArraySpatial(size_t *dim, float *originalArray, float *reconArray, 
						  Param &par, Statistics::StatsContainer<float> &stats);

  /// @brief Search a 3-dimensional array for significant detections. 
  std::vector <Detection> search3DArray(size_t *dim, float *Array, Param &par,
					Statistics::StatsContainer<float> &stats);
  std::vector <Detection> search3DArraySpectral(size_t *dim, float *Array, Param &par,
						Statistics::StatsContainer<float> &stats);
  std::vector <Detection> search3DArraySpatial(size_t *dim, float *Array, Param &par,
					       Statistics::StatsContainer<float> &stats);


  //=========================================================================

  /// An enumeration allowing us to refer to a particular array within functions
  enum ARRAYREF {ARRAY=0, RECON, BASELINE};


  /// @brief Base class for the image container.
  /// 
  /// @details Definition of an n-dimensional data array: array of
  ///     pixel values, size & dimensions array of Detection objects

  class DataArray
  {
  public:
    DataArray();  ///< Basic DataArray constructor
    DataArray(short int nDim); ///< Basic nDim-dimensional DataArray constructor
    DataArray(short int nDim, size_t size);///< Basic nDim-dimensional DataArray constructor, specifying size.
    DataArray(short int nDim, size_t *dimensions); ///< Basic nDim-dimensional DataArray constructor, specifying size of dimensions.
    virtual ~DataArray(); ///< Basic DataArray constructor
    DataArray(const DataArray &d);
    DataArray& operator=(const DataArray &d);

    //-----------------------------------------
    // Obvious inline accessor functions.
    //
    size_t             getDimX(){if(numDim>0) return axisDim[0];else return 0;}; 
    size_t             getDimY(){if(numDim>1) return axisDim[1];else return 1;};
    size_t             getDimZ(){if(numDim>2) return axisDim[2];else return 1;};
    size_t             getSize(){ return numPixels; };
    size_t             getSpatialSize(){if(numDim>1) return axisDim[0]*axisDim[1]; else if(numDim>0) return axisDim[0]; else return 0;};
    short int          getNumDim(){ return numDim; };
    virtual float      getPixValue(size_t pos){ return array[pos]; };
    virtual void       setPixValue(size_t pos, float f){array[pos] = f;};
    Detection          getObject(size_t number){ return objectList->at(number); };
    Detection *        pObject(size_t number){ return &(objectList->at(number));};
    std::vector <Detection> getObjectList(){ return *objectList; };
    std::vector <Detection> *pObjectList(){ return objectList; };
    std::vector <Detection> &ObjectList(){ std::vector<Detection> &rlist = *objectList; return rlist; };
    size_t              getNumObj(){ return objectList->size(); };

    /// @brief Delete all objects from the list of detections. 
    void               clearDetectionList(){ 
      //objectList->clear(); 
      delete objectList;
      objectList = new std::vector<Detection>;
    };

    /// @brief Read a parameter set from file. 
    OUTCOME readParam(std::string paramfile){
      /// @brief 
      ///  Uses Param::readParams() to read parameters from a file.
      ///  \param paramfile The file to be read.
       
      return par.readParams(paramfile); 
    };

    //-----------------------------------------
    // Related to the various arrays
    // 
    /// @brief  Return first three dimensional axes. 
    void               getDim(size_t &x, size_t &y, size_t &z);
    /// @brief Return array of dimensional sizes.
    void               getDimArray(size_t *output);
    size_t *           getDimArray(){return axisDim;};

    /// @brief Return pixel value array. 
    void               getArray(float *output);
    float *            getArray(){return array;};

    /// @brief Save pixel value array. 
    virtual void       saveArray(float *input, size_t size);

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
    bool               isBlank(int vox){return par.isBlank(array[vox]);}; 

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
    bool isDetection(size_t voxel);


    /// @brief Output operator for DataArray.
    friend std::ostream& operator<< (std::ostream& theStream, DataArray &array);
 

  protected:
    short int                numDim;     ///< Number of dimensions.
    size_t                  *axisDim;    ///< Array of axis dimensions of cube
    bool                     axisDimAllocated; ///< has axisDim been allocated?
    size_t                   numPixels;  ///< Total number of pixels in cube.
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
    Cube(size_t nPix);        ///< Alternative Cube constructor.
    Cube(size_t *dimensions); ///< Alternative Cube constructor.
    virtual ~Cube();        ///< Basic Cube destructor.
    Cube(const Cube &c);    ///< Copy constructor
    Cube& operator=(const Cube &c); ///< Copy operator

    /// @brief Return a Cube holding only a subsection of the original
    Cube* slice(Section subsection);
    

    short int   nondegDim(){return numNondegDim;};
    bool        is2D();
    void        checkDim(){head.set2D(is2D());};
    void        reportMemorySize(std::ostream &theStream, bool allocateArrays);

    // INLINE functions -- definitions included after class declaration.
    /// @brief Is the voxel number given by vox a BLANK value? 
    bool        isBlank(size_t vox){ return par.isBlank(array[vox]); };

    /// @brief Is the voxel at (x,y,z) a BLANK value? 
    bool        isBlank(size_t x, size_t y, size_t z){ 
      return par.isBlank(array[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x]); };

    /// @brief Return a bool array masking blank pixels: 1=good, 0=blank 
    bool *      makeBlankMask(){return par.makeBlankMask(array, numPixels);};

    /// @brief Does the Cube::recon array exist? 
    bool        isRecon(){ return reconExists; }; 

    float       getPixValue(size_t pos){ return array[pos]; };
    float       getPixValue(size_t x, size_t y, size_t z){ 
      return array[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x]; };
    short       getDetectMapValue(size_t pos){ return detectMap[pos]; };
    short       getDetectMapValue(size_t x, size_t y){ return detectMap[y*axisDim[0]+x]; };
    float       getReconValue(size_t pos){ return recon[pos]; };
    float       getReconValue(size_t x, size_t y, size_t z){ 
      return recon[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x];};
    float       getBaselineValue(size_t pos){ return baseline[pos]; };
    float       getBaselineValue(size_t x, size_t y, size_t z){ 
      return baseline[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x]; };
    void        setPixValue(size_t pos, float f){array[pos] = f;};
    void        setPixValue(size_t x, size_t y, size_t z, float f){ 
      array[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x] = f;};
    void        setDetectMapValue(size_t pos, short f){ detectMap[pos] = f;};
    void        setDetectMapValue(size_t x, size_t y, short f){ detectMap[y*axisDim[0] + x] = f;};
    void        incrementDetectMap(size_t x, size_t y){detectMap[y*axisDim[0]+x]++;};
    void        incrementDetectMap(size_t pos){detectMap[pos]++;};
    void        setReconValue(size_t pos, float f){recon[pos] = f;};
    void        setReconValue(size_t x, size_t y, size_t z, float f){ 
      recon[z*axisDim[0]*axisDim[1] + y*axisDim[0] + x] = f; };
    void        setReconFlag(bool f){reconExists = f;};

    Catalogues::CatalogueSpecification getFullCols(){return fullCols;};
    Catalogues::CatalogueSpecification *pFullCols(){return &fullCols;};
    void setFullCols(Catalogues::CatalogueSpecification C){fullCols=C;};

    // additional functions -- in Cubes/cubes.cc
    /// @brief Allocate memory correctly, with WCS defining the correct axes. 
    OUTCOME     initialiseCube(size_t *dimensions, bool allocateArrays=true);
    OUTCOME     initialiseCube(long *dimensions, bool allocateArrays=true);

    /// @brief Read in a FITS file, with subsection correction. 
    OUTCOME     getCube();
    /// @brief Read in a FITS file, with subsection correction. 
    OUTCOME     getMetadata();

    /// @brief Read in command-line options. 
    //   int         getopts(int argc, char ** argv);
    int         getopts(int argc, char ** argv, std::string progname="Duchamp"){return par.getopts(argc,argv,progname);};

    /// @brief Read in reconstructed & smoothed arrays from FITS files on disk. 
    void        readSavedArrays();

    /// @brief Save an external array to the Cube's pixel array 
    void        saveArray(float *input, size_t size);

    /// @brief Save an external array to the Cube's pixel array 
    void        saveArray(std::vector<float> &input);

    /// @brief Save an external array to the Cube's reconstructed array. 
    void        saveRecon(float *input, size_t size);

    /// @brief Save reconstructed array to an external array. 
    void        getRecon(float *output);
    /// @brief Return the pointer to the reconstructed array.
    float *     getRecon(){return recon; };

    /// @brief Save baseline array to an external array. 
    void        getBaseline(float *output);
    /// @brief Return the pointer to the baseline array.
    float *     getBaseline(){return baseline; };

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
    bool        isDetection(size_t x, size_t y, size_t z);

    //-----------------------------
    // Dealing with the detections 
    //
    
    /// @brief Get the set of voxels pertaining to the detected objects.
    std::vector< std::vector<PixelInfo::Voxel> > getObjVoxList();

    /// @brief Calculate the object fluxes 
    void        calcObjectFluxes();

    /// @brief Calculate the WCS parameters for each Cube Detection. 
    void        calcObjectWCSparams();
    /// @brief Calculate the WCS parameters for each Cube Detection, using flux information in Voxels. 
    void        calcObjectWCSparams(std::vector< std::vector<PixelInfo::Voxel> > bigVoxList);
    void        calcObjectWCSparams(std::map<PixelInfo::Voxel,float> &voxelMap);
  
    /// @brief Sort the list of detections. 
    void        sortDetections();

    short      *getDetectMap(){return detectMap;};

    /// @brief Update the map of detected pixels. 
    void        updateDetectMap();

    /// @brief Update the map of detected pixels for a given Detection. 
    void        updateDetectMap(Detection obj);

    /// @brief Clear the map of detected pixels. 
    void        clearDetectMap(){
      for(size_t i=0;i<axisDim[0]*axisDim[1];i++) detectMap[i]=0;
    };

    /// @brief Find the flux enclosed by a Detection. 
    float       enclosedFlux(Detection obj);

    /// @brief Set up the output column definitions for the Cube and its Detection list. 
    void        setupColumns();

    /// @brief Is the object at the edge of the image? 
    bool        objAtSpatialEdge(Detection obj);

    /// @brief Is the object at an end of the spectrum? 
    bool        objAtSpectralEdge(Detection obj);

      /// @brief Is the object next to or enclosing flagged channels?
      bool      objNextToFlaggedChan(Detection &obj);

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
    /// @brief Provide a pointer to the FitsHeader object
    FitsHeader *pHeader(){return &head;};

    //-------------------------------------------
    // FITS-I/O related functions -- not in cubes.cc
    //
    /// @brief Function to read in FITS file.
    OUTCOME     getMetadata(std::string fname);  // in Cubes/getImage.cc
    OUTCOME     getCube(std::string fname);  // in Cubes/getImage.cc

    /// @brief Convert the flux units to something user-specified.
    void        convertFluxUnits(std::string oldUnit, std::string newUnit, ARRAYREF whichArray=ARRAY); // in Cubes/getImage.cc

    /// @brief Function to retrieve FITS data array 
    OUTCOME         getFITSdata(std::string fname);   // in FitsIO/dataIO.cc
    OUTCOME         getFITSdata();   // in FitsIO/dataIO.cc
    OUTCOME         getFITSdata(fitsfile *fptr);   // in FitsIO/dataIO.cc

    OUTCOME         writeBasicHeader(fitsfile *fptr, int bitpix, bool is2D=false);

    /// @brief Handle all the writing to FITS files.
    void            writeToFITS();

     /// @brief Save the moment map to a FITS file
    OUTCOME        saveMomentMapImage();

     /// @brief Save the moment map mask to a FITS file
    OUTCOME        saveMomentMask();

    /// @brief Save a mask array to disk.
    OUTCOME        saveMaskCube(); 

    /// @brief Save Smoothed array to disk.
    OUTCOME        saveSmoothedCube();

    /// @brief Save Reconstructed array to disk. 
    OUTCOME        saveReconstructedCube(); 

    /// @brief Save baseline array to disk. 
    OUTCOME        saveBaselineCube();

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
    // in cubes.cc
    /// @brief A front-end to all the searching functions
    void        Search();

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
      *objectList = search3DArray(axisDim,array,par,Stats);
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

      *objectList = searchReconArray(axisDim,array,recon,par,Stats);
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

      *objectList = search3DArray(axisDim,recon,par,Stats);
    };

    // in Cubes/Merger.cc
    /// @brief Merge all objects in the detection list so that only distinct ones are left. 
    void        ObjectMerger();
    /// @brief Grow the sources out to the secondary threshold
    void        growSources(std::vector <Detection> &currentList);

    // in Cubes/existingDetections.cc
    /// @brief Read a previously-created log file to get the detections without searching 
    OUTCOME     getExistingDetections();

    //-------------------------------------
    // Text outputting of detected objects.
    //   in Cubes/detectionIO.cc
    //

    void        outputCatalogue();

    /// @brief Set up the output file with parameters and stats. 
    void        prepareOutputFile();

    /// @brief Write the statistical information to the output file. 
    void        outputStats();

    /// @brief Prepare the log file for output. 
    void        prepareLogFile(int argc, char *argv[]);

    /// @brief Output detections to the log file. 
    void        logDetectionList(bool calcFluxes=true);

    void        logSummary();

    /// @brief Write set of detections and metadata to a binary catalogue
    void        writeBinaryCatalogue();
    OUTCOME     readBinaryCatalogue();


    /// @brief Output detections to a Karma annotation file. 
    void        outputDetectionsKarma();

    /// @brief Output detections to a DS9 annotation file. 
    void        outputDetectionsDS9();

    /// @brief Output detections to a CASA region file. 
    void        outputDetectionsCasa();

    /// @brief Write annotation files
    void        outputAnnotations();

    /// @brief Output detections to a VOTable. 
    void        outputDetectionsVOTable();


    // ----------------------------------
    // Calculation of overall moment maps

    /// @brief Return a moment-0 map plus a map of where the object pixels are
    std::vector<bool> getMomentMap(float *momentMap);
    /// @brief Return a moment-0 map formatted for logarithmic greyscale plotting, with greyscale limits
    void       getMomentMapForPlot(float *momentMap, float &z1, float &z2);

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
    void        plotWCSaxes(){wcsAxes(head.getWCS(),axisDim,DUCHAMP_ID_TEXT_COLOUR,DUCHAMP_WCS_AXIS_COLOUR);};

    //  in Cubes/outputSpectra.cc
    /// @brief Print spectra of each detected object. 
    void        outputSpectra();

    /// @brief Write out text file of all spectra. 
    void        writeSpectralData();

    /// @brief Print spectrum of a single object 
    void        plotSpectrum(int objNum, Plot::SpectralPlot &plot);
    /// @brief Plot the image cutout for a single object 
    void        plotSource(Detection obj, Plot::CutoutPlot &plot);
    /// @brief Mark channels that have been flagged by the user.
      void        drawFlaggedChannels(Plot::SpectralPlot &plot, double xval, double yval);


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
    short int   numNondegDim;     ///< Number of non-degenerate dimensions (ie. with size>1)
    float      *recon;            ///< reconstructed array - used when doing a trous reconstruction.
    bool        reconExists;      ///< flag saying whether there is a reconstruction
    short      *detectMap;        ///< "moment map" - x,y locations of detected pixels
    float      *baseline;         ///< array of spectral baseline values.
			     
    bool        reconAllocated;   ///< have we allocated memory for the recon array?
    bool        baselineAllocated;///< have we allocated memory for the baseline array?
    FitsHeader  head;             ///< the WCS and other header information.
    Catalogues::CatalogueSpecification fullCols;    ///< the list of all columns as printed in the results file
  };


  //============================================================================

  ///  @brief A DataArray object limited to two dimensions, and with some additional
  ///   special functions. 
  /// 
  ///  @details It is used primarily for searching a 1- or 2-D array with 
  ///   lutz_detect() and spectrumDetect().

  class Image : public DataArray
  {
  public:
    Image(){numPixels=0; numDim=2; minSize=2;};
    Image(size_t nPix);
    Image(size_t *dimensions);
    virtual ~Image(){};
    Image(const Image &i);
    Image& operator=(const Image &i);

    //--------------------
    // Defining the array
    //
    /// @brief Save an external array to the Cube's pixel array 
    void      saveArray(float *input, size_t size);

    /// @brief Extract a spectrum from an array and save to the local pixel array. 
    void      extractSpectrum(float *Array, size_t *dim, size_t pixel);

    /// @brief Extract an image from an array and save to the local pixel array. 
    void      extractImage(float *Array, size_t *dim, size_t channel);

    /// @brief Extract a spectrum from a cube and save to the local pixel array. 
    void      extractSpectrum(Cube &cube, size_t pixel);

    /// @brief Extract an image from a cube and save to the local pixel array. 
    void      extractImage(Cube &cube, size_t channel);

    //--------------------
    // Accessing the data.
    //
    float     getPixValue(size_t pos){ return array[pos]; };
    float     getPixValue(size_t x, size_t y){return array[y*axisDim[0] + x];};
    // the next few should have checks against array overflow...
    void      setPixValue(size_t pos, float f){array[pos] = f;};
    void      setPixValue(size_t x, size_t y, float f){array[y*axisDim[0] + x] = f;};
    bool      isBlank(int vox){return par.isBlank(array[vox]);};
    bool      isBlank(size_t x,size_t y){return par.isBlank(array[y*axisDim[0]+x]);};

    //-----------------------
    // Detection-related
    //
    /// @brief Detect objects in a 2-D image 
    std::vector<PixelInfo::Object2D> findSources2D();

    /// @brief Detect objects in a 1-D spectrum 
    std::vector<PixelInfo::Scan> findSources1D();

    unsigned int getMinSize(){return minSize;};
    void         setMinSize(int i){minSize=i;};

    /// @brief  A detection test for a pixel location.  
    bool      isDetection(size_t x, size_t y){
      /// @details Test whether a pixel (x,y) is a statistically
      /// significant detection, according to the set of statistics in
      /// the local StatsContainer object.

      size_t voxel = y*axisDim[0] + x;
      if(isBlank(x,y)) return false;
      else return Stats.isDetection(array[voxel]);
    };  

    /// @brief Blank out a set of channels marked as flagged
      void removeFlaggedChannels();

  private: 
    unsigned int minSize;    ///< the minimum number of pixels for a detection to be accepted.
  };


}

#endif
