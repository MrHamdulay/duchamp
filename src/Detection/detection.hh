#ifndef DETECTION_H
#define DETECTION_H

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <param.hh>
#include <Detection/voxel.hh>
#include <Detection/columns.hh>
#include <Utils/utils.hh>

using namespace Column;

//==========================================================================

/**
 * Class to represent a contiguous set of detected voxels.
 *  This is a detected object, which features:
 *   a vector of voxels, centroid positions, total & peak fluxes,
 *   the possibility of WCS-calculated parameters (RA, Dec, velocity, related widths).
 *  Also many functions with which to manipulate the Detections.
 */

class Detection
{
public:
  Detection(){
    flagWCS=false; negativeSource = false; flagText="";
    xcentre = ycentre = zcentre = totalFlux = peakFlux = 0.;
    xmin=xmax=ymin=ymax=zmin=zmax=0;
  };
  Detection(long numPix){
    std::vector <Voxel> pix(numPix); flagWCS = false; negativeSource = false;
  };
  Detection(const Detection& d);
  Detection& operator= (const Detection& d);
  virtual ~Detection(){};
  //------------------------------
  // These are functions in detection.cc. 
  //
  void   addAnObject(Detection &toAdd); ///< Add all voxels of one object to another.
  void   calcWCSparams(FitsHeader &head); ///< Calculate parameters related to the World Coordinate System.
  float  getIntegFlux(FitsHeader &head); ///< Calculate the integrated flux over the entire Detection.
  void   calcParams();   ///< Calculate basic parameters of the Detection.
  void   setOffsets(Param &par); ///< Set the values of the axis offsets from the cube.
  bool   hasEnoughChannels(int minNumber);  ///< Is there at least the acceptable minimum number of channels in the Detection?
  //
  friend std::ostream& operator<< ( std::ostream& theStream, Detection& obj);
  //---------------------------------
  // Text Output -- all in Detection/outputDetection.cc
  //
  /** The spectral output label that contains info on the WCS position & velocity.*/
  std::string outputLabelWCS();  

  /** The spectral output label that contains info on the pixel location. */
  std::string outputLabelPix(); 

  /** The spectral output label that contains info on widths & fluxes of the Detection. */
  std::string outputLabelInfo(); 

  /** Prints the column headers. */
  void   outputDetectionTextHeader(std::ostream &stream, std::vector<Col> columns); 

  /** Prints the full set of columns, including the WCS information. */
  void   outputDetectionTextWCS(std::ostream &stream, std::vector<Col> columns); 

  /** Prints a limited set of columns, excluding any WCS information. */
  void   outputDetectionText(std::ostream &stream, std::vector<Col> columns, int idNumber); 
  //---------------------------------- 
  // For plotting routines... in Cubes/drawMomentCutout.cc
  //
  /** Draw spatial borders for a particular Detection. */
  void   drawBorders(int xoffset, int yoffset); 
  //---------------------------------- 
  // For sorting routines... in Detection/sorting.cc
  //
  /** Sort the pixels by central z-value. */
  void   SortByZ();

  /** Order the pixels by z, then by y, and last by x */
  void order(){std::stable_sort(pix.begin(),pix.end());}
  //
  //----------------------------------
  // Basic functions
  //
  /** Delete all pixel information from Detection. Does not clear other parameters. */
  void   clearDetection(){this->pix.clear();};

  /** Add a single voxel to the pixel list. Does not re-calculate parameters.*/

  void   addPixel(Voxel point){pix.push_back(point);};

  /** Return the nth voxel in the list */
  Voxel  getPixel(long pixNum){return pix[pixNum];};

  /** How many voxels are in the Detection? */
  int    getSize(){return pix.size();};

  /** How many distinct spatial pixels are there? */
  int    getSpatialSize();
  //-----------------------------------
  // Basic accessor functions for private members follow...
  //
  long   getX(long pixCount){return pix[pixCount].itsX;};
  void   setX(long pixCount, long x){pix[pixCount].itsX=x;};
  long   getY(long pixCount){return pix[pixCount].itsY;};
  void   setY(long pixCount, long y){pix[pixCount].itsY=y;};
  long   getZ(long pixCount){return pix[pixCount].itsZ;};
  void   setZ(long pixCount, long z){pix[pixCount].itsZ=z;};
  float  getF(long pixCount){return pix[pixCount].itsF;};
  void   setF(long pixCount, float f){pix[pixCount].itsF=f;};
  //
  long   getXOffset(){return xSubOffset;};
  void   setXOffset(long o){xSubOffset = o;};
  long   getYOffset(){return ySubOffset;};
  void   setYOffset(long o){ySubOffset = o;};
  long   getZOffset(){return zSubOffset;};
  void   setZOffset(long o){zSubOffset = o;};
  //
  float  getXcentre(){return xcentre;};
  void   setXcentre(float x){xcentre = x;};
  float  getYcentre(){return ycentre;};
  void   setYcentre(float y){ycentre = y;};
  float  getZcentre(){return zcentre;};
  void   setCentre(float x, float y){xcentre = x; ycentre = y;};
  float  getTotalFlux(){return totalFlux;};
  void   setTotalFlux(float f){totalFlux = f;};
  float  getIntegFlux(){return intFlux;};
  void   setIntegFlux(float f){intFlux = f;};
  float  getPeakFlux(){return peakFlux;};
  void   setPeakFlux(float f){peakFlux = f;};
  long   getXPeak(){return xpeak;};
  void   setXPeak(long x){xpeak = x;};
  long   getYPeak(){return ypeak;};
  void   setYPeak(long y){ypeak = y;};
  long   getZPeak(){return zpeak;};
  void   setZPeak(long z){zpeak = z;};
  float  getPeakSNR(){return peakSNR;};
  void   setPeakSNR(float f){peakSNR=f;};
  bool   isNegative(){return negativeSource;};
  void   setNegative(bool f){negativeSource = f;};
  std::string getFlagText(){return flagText;};
  void   setFlagText(std::string s){flagText = s;};
  void   addToFlagText(std::string s){flagText += s;};
  //
  long   getXmin(){return xmin;};
  long   getYmin(){return ymin;};
  long   getZmin(){return zmin;};
  long   getXmax(){return xmax;};
  long   getYmax(){return ymax;};
  long   getZmax(){return zmax;};
  //
  /** Is the WCS good enough to be used? \return Detection::flagWCS True/False */
  bool   isWCS(){return flagWCS;};
  std::string getName(){return name;};
  void   setName(std::string s){name = s;};
  std::string getRAs(){return raS;};
  void   setRAs(std::string s){raS = s;};
  std::string getDecs(){return decS;};
  void   setDecs(std::string s){decS = s;};
  float  getRA(){return ra;};
  void   setRA(float f){ra = f;};
  float  getDec(){return dec;};
  void   setDec(float f){dec = f;};
  float  getRAWidth(){return raWidth;};
  void   setRAWidth(float f){raWidth = f;};
  float  getDecWidth(){return decWidth;};
  void   setDecWidth(float f){decWidth = f;};
  float  getVel(){return vel;};
  void   setVel(float f){vel = f;};
  float  getVelWidth(){return velWidth;};
  void   setVelWidth(float f){velWidth = f;};
  float  getVelMin(){return velMin;};
  void   setVelMin(float f){velMin = f;};
  float  getVelMax(){return velMax;};
  void   setVelMax(float f){velMax = f;};
  int    getID(){return id;};
  void   setID(int i){id = i;};
  //
  int    getPosPrec(){return posPrec;};
  void   setPosPrec(int i){posPrec=i;};
  int    getXYZPrec(){return xyzPrec;};
  void   setXYZPrec(int i){xyzPrec=i;};
  int    getFintPrec(){return fintPrec;};
  void   setFintPrec(int i){fintPrec=i;};
  int    getFpeakPrec(){return fpeakPrec;};
  void   setFpeakPrec(int i){fpeakPrec=i;};
  int    getVelPrec(){return velPrec;};
  void   setVelPrec(int i){velPrec=i;};
  int    getSNRPrec(){return snrPrec;};
  void   setSNRPrec(int i){snrPrec=i;};
  //
private:
  std::vector <Voxel> pix;            ///< array of pixels
  float          xcentre;        ///< x-value of centre pixel of object
  float          ycentre;        ///< y-value of centre pixel of object
  float          zcentre;        ///< z-value of centre pixel of object
  long           xmin,xmax;      ///< min and max x-values of object
  long           ymin,ymax;      ///< min and max y-values of object
  long           zmin,zmax;      ///< min and max z-values of object
  // Subsection offsets
  long           xSubOffset;     ///< The x-offset, taken from the subsectioned cube
  long           ySubOffset;     ///< The y-offset, taken from the subsectioned cube
  long           zSubOffset;     ///< The z-offset, taken from the subsectioned cube
  // Flux related
  float          totalFlux;      ///< sum of the fluxes of all the pixels
  float          intFlux;        ///< integrated flux 
                                 ///<   --> involves integration over velocity.
  float          peakFlux;       ///< maximum flux over all the pixels
  long           xpeak;          ///< x-pixel location of peak flux
  long           ypeak;          ///< y-pixel location of peak flux
  long           zpeak;          ///< z-pixel location of peak flux
  float          peakSNR;        ///< signal-to-noise ratio at peak
  bool           negativeSource; ///< is the source a negative feature?
  std::string         flagText;       ///< any warning flags about the quality of the detection.
  // WCS related
  int            id;             ///< ID -- generally number in list
  std::string         name;	         ///< IAU-style name (based on position)
  bool           flagWCS;        ///< A flag indicating whether the WCS parameters have been set.
  std::string         raS;	         ///< Central Right Ascension (or Longitude) in form 12:34:23
  std::string         decS;	         ///< Central Declination(or Latitude), in form -12:23:34
  float          ra;	         ///< Central Right Ascension in degrees
  float          dec;	         ///< Central Declination in degrees
  float          raWidth;        ///< Width of detection in RA direction in arcmin
  float          decWidth;       ///< Width of detection in Dec direction in arcmin
  std::string         specUnits;      ///< Units of the spectral dimension
  std::string         fluxUnits;      ///< Units of flux
  std::string         intFluxUnits;   ///< Units of integrated flux
  std::string         lngtype;        ///< Type of longitude axis (RA/GLON)
  std::string         lattype;        ///< Type of latitude axis (DEC/GLAT)
  float          vel;	         ///< Central velocity (from zCentre)
  float          velWidth;       ///< Full velocity width
  float          velMin;         ///< Minimum velocity
  float          velMax;         ///< Maximum velocity
  //  The next six are the precision of values printed in the headers of the spectral plots
  int            posPrec;        ///< Precision of WCS positional values 
  int            xyzPrec;        ///< Precision of pixel positional values 
  int            fintPrec;       ///< Precision of F_int/F_tot values
  int            fpeakPrec;      ///< Precision of F_peak values
  int            velPrec;        ///< Precision of velocity values.
  int            snrPrec;        ///< Precision of S/N_max values.

};

//==========================================================================

//////////////////////////////////////////////////////
// Prototypes for functions that use above classes
//////////////////////////////////////////////////////

//----------------
// These are in detection.cc
//
/** Combine two Detections to form a new one. */
Detection combineObjects(Detection &first, Detection &second);

/** Combine two lists Detections to form a new one. */
std::vector <Detection> combineLists(std::vector <Detection> &first, 
				     std::vector <Detection> &second);

//----------------
// These are in sorting.cc
//
/** Sort a list of Detections by Z-pixel value. */
void SortByZ(std::vector <Detection> &inputList);

/** Sort a list of Detections by Velocity.*/
void SortByVel(std::vector <Detection> &inputList);

//----------------
// This is in areClose.cc
//
/** Determine whether two objects are close according to set parameters.*/
bool areClose(Detection &object1, Detection &object2, Param &par);

//----------------
// This is in mergeIntoList.cc
//
/** Add an object into a list, combining with adjacent objects if need be. */
void mergeIntoList(Detection &object, std::vector <Detection> &objList, 
		   Param &par);

//----------------
// These are in Cubes/Merger.cc
//
/** Merge a list of Detections so that all adjacent voxels are in the same Detection. */
void mergeList(std::vector<Detection> &objList, Param &par);   
/** Culls a list of Detections that do not meet minimum requirements. */
void finaliseList(std::vector<Detection> &objList, Param &par);
/** Manage both the merging and the cleaning up of the list. */
void ObjectMerger(std::vector<Detection> &objList, Param &par);


#endif
