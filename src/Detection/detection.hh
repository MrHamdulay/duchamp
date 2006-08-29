#ifndef DETECTION_H
#define DETECTION_H

#include <iostream>
#include <string>
#include <vector>
#include <param.hh>
#ifndef COLUMNS_H
#include <Detection/columns.hh>
#endif
#include <Utils/utils.hh>

using std::string;
using std::vector;
using namespace Column;

/**
 * Voxel class
 *  A 3-dimensional pixel, with x,y,z position + flux
 */

class Voxel
{
public:
  Voxel(){};
  Voxel(long x, long y, long z, float f){ itsX=x; itsY=y; itsZ=z; itsF=f;};
  virtual ~Voxel(){};
  friend class Detection;
  // accessor functions
  void   setX(long x){itsX = x;};
  void   setY(long y){itsY = y;};
  void   setZ(long z){itsZ = z;};
  void   setF(float f){itsF = f;};
  void   setXY(long x, long y){itsX = x; itsY = y;};
  void   setXYZ(long x, long y, long z){itsX = x; itsY = y; itsZ = z;};
  void   setXYF(long x, long y, float f){itsX = x; itsY = y; itsF = f;};
  void   setXYZF(long x, long y, long z, float f){itsX = x; itsY = y; itsZ = z; itsF = f;};
  long   getX(){return itsX;};
  long   getY(){return itsY;};
  long   getZ(){return itsZ;};
  float  getF(){return itsF;};
  //
  friend std::ostream& operator<< ( std::ostream& theStream, Voxel& vox);
  //
protected:
  long  itsX;         // x-position of pixel
  long  itsY;         // y-position of pixel
  long  itsZ;         // z-position of pixel
  float itsF;         // flux of pixel
};

/**
 * Pixel class
 *  A 2-dimensional type of voxel, with just x & y position + flux
 */


class Pixel : public Voxel
{
public:
  Pixel(){itsZ=0;};
  Pixel(long x, long y, float f){ itsX=x; itsY=y; itsF=f; itsZ=0;};
  virtual ~Pixel(){};
  // accessor functions
  void  setXY(long x, long y){itsX = x; itsY = y;};
  void  setXYF(long x, long y, float f){itsX = x; itsY = y; itsF = f;};

};

/**
 * Detection class
 *  A detected object, featuring:
 *   a vector of voxels, centroid positions, total & peak fluxes
 *   the possibility of WCS-calculated parameters (RA, Dec, velocity) etc.
 */

class Detection
{
public:
  Detection(){flagWCS=false; negativeSource = false; flagText="";};
  Detection(long numPix){vector <Voxel> pix(numPix); flagWCS = false; negativeSource = false;};
  virtual ~Detection(){};

  void   addPixel(Voxel point){pix.push_back(point);};
  Voxel  getPixel(long pixNum){return pix[pixNum];};
  int    getSize(){return pix.size();};
  int    getSpatialSize();// in detection.cc
  //
  long   getX(long pixCount){return pix[pixCount].getX();};
  void   setX(long pixCount, long x){pix[pixCount].setX(x);};
  long   getY(long pixCount){return pix[pixCount].getY();};
  void   setY(long pixCount, long y){pix[pixCount].setY(y);};
  long   getZ(long pixCount){return pix[pixCount].getZ();};
  void   setZ(long pixCount, long z){pix[pixCount].setZ(z);};
  float  getF(long pixCount){return pix[pixCount].getF();};
  void   setF(long pixCount, float f){pix[pixCount].setF(f);};
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
  bool   isNegative(){return negativeSource;};
  void   setNegative(bool f){negativeSource = f;};
  string getFlagText(){return flagText;};
  void   setFlagText(string s){flagText = s;};
  void   addToFlagText(string s){flagText += s;};
  //
  long   getXmin(){return xmin;};
  long   getYmin(){return ymin;};
  long   getZmin(){return zmin;};
  long   getXmax(){return xmax;};
  long   getYmax(){return ymax;};
  long   getZmax(){return zmax;};
  //
  bool   isWCS(){return flagWCS;};
  string getName(){return name;};
  void   setName(string s){name = s;};
  string getRAs(){return raS;};
  void   setRAs(string s){raS = s;};
  string getDecs(){return decS;};
  void   setDecs(string s){decS = s;};
  float  getRA(){return ra;};
  void   setRA(float f){ra = f;};
  float  getDec(){return dec;};
  void   setDec(float f){dec = f;};
  float  getRAWidth(){return raWidth;};
  void   setRAWidth(float f){raWidth = f;};
  float  getDecWidth(){return decWidth;};
  void   setDecWidth(float f){decWidth = f;};
//   string getLNGtype(){return lngtype;};
//   void   setLNGtype(string s){lngtype = s;};
//   string getLATtype(){return lattype;};
//   void   setLATtype(string s){lattype = s;};
//   string getZtype(){return ztype;};
//   void   setZtype(string s){ztype = s;};
//   float  getNuRest(){return nuRest;};
//   void   setNuRest(float f){nuRest = f;};
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
  void   addAnObject(Detection &toAdd);
  void   calcWCSparams(FitsHeader &head);
  float  getIntegFlux(FitsHeader &head);
  void   calcParams();
  void   clearDetection(){this->pix.clear();};
  void   addOffsets(Param &par);
  void   SortByZ();   // in Detection/sorting.cc
  bool   hasEnoughChannels(int minNumber); 
  // Text Output -- all in Detection/outputDetection.cc
  string outputLabelWCS();
  string outputLabelPix();
  string outputLabelInfo();
  void   outputDetectionTextWCS(std::ostream &stream, ColSet columns);
  void   outputDetectionText(std::ostream &stream, ColSet columns, int idNumber);
  // For plotting routines...
  void   drawBorders(int xoffset, int yoffset);  // in Cubes/drawMomentCutout.cc
  //
  friend std::ostream& operator<< ( std::ostream& theStream, Detection& obj);
  //
private:
  vector <Voxel> pix;         // array of pixels
  float          xcentre;     // x-value of centre pixel of object
  float          ycentre;     // y-value of centre pixel of object
  float          zcentre;     // z-value of centre pixel of object
  long           xmin,xmax;   // min and max x-values of object
  long           ymin,ymax;   // min and max y-values of object
  long           zmin,zmax;   // min and max z-values of object
  // Subsection offsets
  long           xSubOffset;  // The offset in the x-direction from the subsectioned cube
  long           ySubOffset;  // The offset in the y-direction from the subsectioned cube
  long           zSubOffset;  // The offset in the z-direction from the subsectioned cube
  // Flux related
  float          totalFlux;   // sum of the fluxes of all the pixels
  float          intFlux;     // integrated flux --> involves integration over velocity.
  float          peakFlux;    // maximum flux over all the pixels
  long           xpeak,ypeak,zpeak; // location of peak flux
  bool           negativeSource; // is the source defined as a negative feature?
  string         flagText;    // any warning flags about the quality of the detection.
  // WCS related
  int            id;          // ID -- generally number in list
  string         name;	      // IAU-style name (based on position)
  bool           flagWCS;     // A flag indicating whether the WCS parameters have been set.
  string         raS;	      // Central Right Ascension (or Longitude) in form 12:34:23
  string         decS;	      // Central Declination(or Latitude), in form -12:23:34
  float          ra;	      // Central Right Ascension in degrees
  float          dec;	      // Central Declination in degrees
  float          raWidth;     // Width of detection in RA direction in arcmin
  float          decWidth;    // Width of detection in Dec direction in arcmin
  string         specUnits;   // Units of the spectral dimension
  string         fluxUnits;   // Units of flux
  string         intFluxUnits;// Units of integrated flux
  string         lngtype;     // Type of longitude axis (RA/GLON)
  string         lattype;     // Type of latitude axis (DEC/GLAT)
//   string         ztype;	      // Type of z-axis (FREQ/VEL/...)
//   float          nuRest;      // Rest frequency
  float          vel;	      // Central velocity (from zCentre)
  float          velWidth;    // Full velocity width
  float          velMin;      // Minimum velocity
  float          velMax;      // Maximum velocity

};

/****************************************************************/
//////////////////////////////////////////////////////
// Prototypes for functions that use above classes
//////////////////////////////////////////////////////

// void outputDetectionTextWCSHeader(std::ostream &stream, wcsprm *wcs);
void outputDetectionTextWCSHeader(std::ostream &stream, ColSet columns);
void outputDetectionTextHeader(std::ostream &stream, ColSet columns);

void SortByZ(vector <Detection> &inputList);
void SortByVel(vector <Detection> &inputList);
Detection combineObjects(Detection &first, Detection &second);
vector <Detection> combineLists(vector <Detection> &first, vector <Detection> &second);

bool areClose(Detection &object1, Detection &object2, Param &par);
void mergeIntoList(Detection &object, vector <Detection> &objList, Param &par);
void mergeList(vector<Detection> &objList, Param &par);
void finaliseList(vector<Detection> &objList, Param &par);

// A GENERIC DETECTION TEST  -- in thresholding_functions.cc
bool isDetection(float value, float mean, float sigma, float cut);

#endif
