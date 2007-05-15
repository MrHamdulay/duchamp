// -----------------------------------------------------------------------
// detection.hh: Definition of the Detection class.
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
#ifndef DETECTION_H
#define DETECTION_H

#include <iostream>
#include <string>
#include <vector>
#include <param.hh>
#include <PixelMap/Voxel.hh>
#include <PixelMap/Object3D.hh>
#include <Detection/columns.hh>

/**
 * Class to represent a contiguous set of detected voxels.
 *  This is a detected object, which features:
 *   a vector of voxels, average and centroid positions, total & peak fluxes,
 *   the possibility of WCS-calculated parameters (RA, Dec, velocity, 
 *     related widths).
 *  Also many functions with which to manipulate the Detections.
 */

class Detection
{
public:
  Detection(){
    flagWCS=false; negSource = false; flagText="";
    totalFlux = peakFlux = 0.;
    centreType="centroid";
  };
  Detection(const Detection& d);
  Detection& operator= (const Detection& d);
  virtual ~Detection(){};
  //------------------------------
  // These are functions in detection.cc. 
  //
  /** Add all voxels of one object to another. */
  friend Detection operator+ (Detection lhs, Detection rhs);
  friend Detection operator+= (Detection lhs, Detection rhs){
    lhs = lhs + rhs;
    return lhs;
  }

  /** Provides a reference to the StatsContainer. */
  PixelInfo::Object3D& pixels(){ 
    PixelInfo::Object3D &rpix = this->pixelArray; 
    return rpix;
  }; 

  /** Calculate basic parameters of the Detection. */
  void   calcParams(){pixelArray.calcParams();}; 

  /** Calculate flux-related parameters of the Detection. */
  void   calcFluxes(float *fluxArray, long *dim); 

  /** Calculate parameters related to the World Coordinate System. */
  void   calcWCSparams(float *fluxArray, long *dim, FitsHeader &head); 

  /** Calculate the integrated flux over the entire Detection. */
  void   calcIntegFlux(float *fluxArray, long *dim, FitsHeader &head); 

  /** Set the values of the axis offsets from the cube. */
  void   setOffsets(Param &par); 

  /** Add the offset values to the pixel locations */
  void   addOffsets(){
    pixelArray.addOffsets(xSubOffset,ySubOffset,zSubOffset);
  };

  //
  friend std::ostream& operator<< ( std::ostream& theStream, Detection& obj);
  //---------------------------------
  // Text Output -- all in Detection/outputDetection.cc
  //
  /** The spectral output label that contains info on the WCS position
      & velocity.*/
  std::string outputLabelWCS();  

  /** The spectral output label that contains info on the pixel
      location. */
  std::string outputLabelPix(); 

  /** The spectral output label that contains info on fluxes of the
      Detection. */
  std::string outputLabelFluxes(); 

  /** The spectral output label that contains info on widths of the
      Detection. */
  std::string outputLabelWidths(); 

  /** Prints the column headers, except for the different pixel centres. */
  void   outputDetectionTextHeader(std::ostream &stream, 
				   std::vector<Column::Col> columns); 

  /** Prints all the column headers. */
  void   outputDetectionTextHeaderFull(std::ostream &stream, 
				       std::vector<Column::Col> columns); 

  /** Prints the full set of columns, including the WCS
      information, but not the different pixel centres. */
  void   outputDetectionTextWCS(std::ostream &stream, 
				std::vector<Column::Col> columns); 

  /** Prints the full set of columns, including the WCS
      information. */
  void   outputDetectionTextWCSFull(std::ostream &stream, 
				    std::vector<Column::Col> columns); 

  /** Prints a limited set of columns, excluding any WCS
      information. */
  void   outputDetectionText(std::ostream &stream, 
			     std::vector<Column::Col> columns, 
			     int idNumber); 
  //---------------------------------- 
  // For plotting routines... in Cubes/drawMomentCutout.cc
  //
  /** Draw spatial borders for a particular Detection. */
  void   drawBorders(int xoffset, int yoffset); 
  //
  /** Sort the pixels by central z-value. */
  void   SortByZ(){pixelArray.order();};
  //
  //----------------------------------
  // Basic functions
  //
  /** Delete all pixel information from Detection. Does not clear
      other parameters. */
  //  void   clearDetection(){this->pix.clear();};

  /** Add a single voxel to the pixel list.*/
  void   addPixel(long x, long y, long z){pixelArray.addPixel(x,y,z);};
  /** Add a single voxel to the pixel list.*/
  void   addPixel(PixelInfo::Voxel point){
    /** This one adds the pixel to the pixelArray, and updates the
	fluxes according to the Voxel's flux information */
    pixelArray.addPixel(point);
    totalFlux += point.getF();
    if(point.getF()>peakFlux){
      peakFlux = point.getF();
      xpeak = point.getX(); ypeak = point.getY(); zpeak = point.getZ();
    }
  };

  /** Return a single voxel. */
  PixelInfo::Voxel getPixel(int i){return pixelArray.getPixel(i);};

  /** How many voxels are in the Detection? */
  int    getSize(){return pixelArray.getSize();};

  /** How many distinct spatial pixels are there? */
  int    getSpatialSize(){return pixelArray.getSpatialSize();};

  /** How many channels does the Detection have? */
  long   getNumChannels(){return pixelArray.getNumDistinctZ();};

  /** Is there at least the acceptable minimum number of channels in
      the Detection?  */
  bool   hasEnoughChannels(int minNumber);
 
  //-----------------------------------
  // Basic accessor functions for private members follow...
  //
  long        getXOffset(){return xSubOffset;};
  void        setXOffset(long o){xSubOffset = o;};
  long        getYOffset(){return ySubOffset;};
  void        setYOffset(long o){ySubOffset = o;};
  long        getZOffset(){return zSubOffset;};
  void        setZOffset(long o){zSubOffset = o;};
  //	      
  float       getXcentre(){
    if(centreType=="peak") return xpeak;
    else if(centreType=="average") return pixelArray.getXcentre();
    else return xCentroid;
  };
  float       getYcentre(){
    if(centreType=="peak") return ypeak;
    else if(centreType=="average") return pixelArray.getYcentre();
    else return yCentroid;
  };
  float       getZcentre(){
    if(centreType=="peak") return zpeak;
    else if(centreType=="average") return pixelArray.getZcentre();
    else return zCentroid;
  };
  float       getXAverage(){return pixelArray.getXcentre();};
  float       getYAverage(){return pixelArray.getYcentre();};
  float       getZAverage(){return pixelArray.getZcentre();};
  float       getTotalFlux(){return totalFlux;};
  void        setTotalFlux(float f){totalFlux=f;};
  float       getIntegFlux(){return intFlux;};
  void        setIntegFlux(float f){intFlux=f;};
  float       getPeakFlux(){return peakFlux;};
  void        setPeakFlux(float f){peakFlux=f;};
  long        getXPeak(){return xpeak;};
  long        getYPeak(){return ypeak;};
  long        getZPeak(){return zpeak;};
  float       getPeakSNR(){return peakSNR;};
  void        setPeakSNR(float f){peakSNR = f;};
  float       getXCentroid(){return xCentroid;};
  float       getYCentroid(){return yCentroid;};
  float       getZCentroid(){return zCentroid;};
  std::string getCentreType(){return centreType;};
  void        setCentreType(std::string s){centreType=s;};
  bool        isNegative(){return negSource;};
  void        setNegative(bool f){negSource = f;};
  std::string getFlagText(){return flagText;};
  void        setFlagText(std::string s){flagText = s;};
  void        addToFlagText(std::string s){flagText += s;};
  //	      
  long        getXmin(){return pixelArray.getXmin();};
  long        getYmin(){return pixelArray.getYmin();};
  long        getZmin(){return pixelArray.getZmin();};
  long        getXmax(){return pixelArray.getXmax();};
  long        getYmax(){return pixelArray.getYmax();};
  long        getZmax(){return pixelArray.getZmax();};
  //
  /** Is the WCS good enough to be used? 
      \return Detection::flagWCS =  True/False */
  bool        isWCS(){return flagWCS;};
  int         getNumAxes(){return numAxes;};
  void        setNumAxes(int i){numAxes=i;};
  std::string getName(){return name;};
  void        setName(std::string s){name=s;};
  std::string getRAs(){return raS;};
  std::string getDecs(){return decS;};
  float       getRA(){return ra;};
  float       getDec(){return dec;};
  float       getRAWidth(){return raWidth;};
  float       getDecWidth(){return decWidth;};
  float       getVel(){return vel;};
  float       getVelWidth(){return velWidth;};
  float       getVelMin(){return velMin;};
  float       getVelMax(){return velMax;};
  int         getID(){return id;};
  void        setID(int i){id = i;};
  //
  int         getPosPrec(){return posPrec;};
  void        setPosPrec(int i){posPrec=i;};
  int         getXYZPrec(){return xyzPrec;};
  void        setXYZPrec(int i){xyzPrec=i;};
  int         getFintPrec(){return fintPrec;};
  void        setFintPrec(int i){fintPrec=i;};
  int         getFpeakPrec(){return fpeakPrec;};
  void        setFpeakPrec(int i){fpeakPrec=i;};
  int         getVelPrec(){return velPrec;};
  void        setVelPrec(int i){velPrec=i;};
  int         getSNRPrec(){return snrPrec;};
  void        setSNRPrec(int i){snrPrec=i;};
  //
private:
  PixelInfo::Object3D pixelArray;     ///< The pixel locations
  // Subsection offsets
  long           xSubOffset;     ///< The x-offset, from subsectioned cube
  long           ySubOffset;     ///< The y-offset, from subsectioned cube
  long           zSubOffset;     ///< The z-offset, from subsectioned cube
  // Flux related
  float          totalFlux;      ///< sum of the fluxes of all the pixels
  float          intFlux;        ///< integrated flux : involves
				 ///    integration over velocity.
  float          peakFlux;       ///< maximum flux over all the pixels
  long           xpeak;          ///< x-pixel location of peak flux
  long           ypeak;          ///< y-pixel location of peak flux
  long           zpeak;          ///< z-pixel location of peak flux
  float          peakSNR;        ///< signal-to-noise ratio at peak
  float          xCentroid;      ///< x-pixel location of centroid
  float          yCentroid;      ///< y-pixel location of centroid
  float          zCentroid;      ///< z-pixel location of centroid
  std::string    centreType;     ///< which type of pixel centre to
				 ///   report: "average", "centroid", or
				 ///   "peak" (flux)
  bool           negSource;      ///< is the source a negative feature?
  std::string    flagText;       ///< any warning flags about the
				 ///    quality of the detection.
  // WCS related
  int            id;             ///< ID -- generally number in list
  std::string    name;	         ///< IAU-style name (based on position)
  bool           flagWCS;        ///< A flag indicating whether the
				 ///    WCS parameters have been set.
  int            numAxes;        ///< The number of axes in the WCS
  std::string    raS;	         ///< Central Right Ascension (or
				 ///    Longitude) in form 12:34:23
  std::string    decS;	         ///< Central Declination(or
				 ///    Latitude), in form -12:23:34
  float          ra;	         ///< Central Right Ascension in degrees
  float          dec;	         ///< Central Declination in degrees
  float          raWidth;        ///< Width of detection in RA
				 ///   direction in arcmin
  float          decWidth;       ///< Width of detection in Dec
				 ///   direction in arcmin
  std::string    specUnits;      ///< Units of the spectral dimension
  std::string    fluxUnits;      ///< Units of flux
  std::string    intFluxUnits;   ///< Units of integrated flux
  std::string    lngtype;        ///< Type of longitude axis (RA/GLON)
  std::string    lattype;        ///< Type of latitude axis (DEC/GLAT)
  float          vel;	         ///< Central velocity (from zCentre)
  float          velWidth;       ///< Full velocity width
  float          velMin;         ///< Minimum velocity
  float          velMax;         ///< Maximum velocity
  //  The next six are the precision of values printed in the headers
  //  of the spectral plots
  int            posPrec;        ///< Precision of WCS positional values 
  int            xyzPrec;        ///< Precision of pixel positional
				 ///   values
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
