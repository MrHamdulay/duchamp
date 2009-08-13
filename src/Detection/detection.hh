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
#include <duchamp/param.hh>
#include <duchamp/PixelMap/Voxel.hh>
#include <duchamp/PixelMap/Object3D.hh>
#include <duchamp/Detection/columns.hh>

using namespace PixelInfo;

namespace duchamp
{


  /// Class to represent a contiguous set of detected voxels.
  ///  This is a detected object, which features:
  ///   a vector of voxels, average and centroid positions, total & peak fluxes,
  ///   the possibility of WCS-calculated parameters (RA, Dec, velocity, 
  ///     related widths).
  ///  Also many functions with which to manipulate the Detections.

  class Detection : public Object3D
  {
  public:
    Detection();
    Detection(const Object3D& o);
    Detection(const Detection& d);
    Detection& operator= (const Detection& d);
    virtual ~Detection(){};
    void defaultDetection();
    //------------------------------
    // These are functions in detection.cc. 
    //

    friend Detection operator+ (Detection lhs, Detection rhs);


    /// @brief Test whether voxel lists match 
    bool voxelListsMatch(std::vector<PixelInfo::Voxel> voxelList);

    /// @brief Test whether a voxel list contains all detected voxels 
    bool voxelListCovered(std::vector<PixelInfo::Voxel> voxelList);

    /// @brief Calculate flux-related parameters of the Detection. 
    void   calcFluxes(float *fluxArray, long *dim); 
    /// @brief Calculate flux-related parameters of the Detection. 
    void   calcFluxes(std::vector<PixelInfo::Voxel> voxelList); 

    /// @brief Calculate parameters related to the World Coordinate System. 
    //    void   calcWCSparams(float *fluxArray, long *dim, FitsHeader &head); 
    void   calcWCSparams(FitsHeader &head); 

    /// @brief Calculate the integrated flux over the entire Detection. 
    void   calcIntegFlux(float *fluxArray, long *dim, FitsHeader &head); 
    /// @brief Calculate the integrated flux over the entire Detection. 
    void   calcIntegFlux(std::vector<PixelInfo::Voxel> voxelList, FitsHeader &head); 

    /// @brief Calculate the 20%/50% peak flux widths 
    void calcVelWidths(float *fluxArray, long *dim, FitsHeader &head);
    /// @brief Calculate the 20%/50% peak flux widths 
    void calcVelWidths(std::vector<PixelInfo::Voxel> voxelList, FitsHeader &head);

    /// @brief Set the values of the axis offsets from the cube. 
    void   setOffsets(Param &par); 

    /// @brief Add the offset values to the pixel locations 
    void   addOffsets(long xoff, long yoff, long zoff){Object3D::addOffsets(xoff,yoff,zoff);};
    void   addOffsets(){
      Object3D::addOffsets(xSubOffset,ySubOffset,zSubOffset);
      xpeak+=xSubOffset; ypeak+=ySubOffset; zpeak+=zSubOffset;
      xCentroid+=xSubOffset; yCentroid+=ySubOffset; zCentroid+=zSubOffset;
    };

    //
    //---------------------------------
    // Text Output -- all in Detection/outputDetection.cc
    //
    /// @brief The spectral output label that contains info on the WCS position & velocity.
    std::string outputLabelWCS();  

    /// @brief The spectral output label that contains info on the pixel location. 
    std::string outputLabelPix(); 

    /// @brief The spectral output label that contains info on fluxes of the Detection. 
    std::string outputLabelFluxes(); 

    /// @brief The spectral output label that contains info on widths of the Detection. 
    std::string outputLabelWidths(); 

    /// @brief Print all required values for the Detection to a table.
    void printTableRow(std::ostream &stream, std::vector<Column::Col> columns, std::string tableType);

    /// @brief Print a particular value for the Detection to a table.
    void printTableEntry(std::ostream &stream, Column::Col column);

    //---------------------------------- 
    // For plotting routines... in Cubes/drawMomentCutout.cc
    //
    /// @brief Get the location of the spatial borders. 
    std::vector<int> getVertexSet();
    //
    /// @brief Draw spatial borders for a particular Detection. 
    void   drawBorders(int xoffset, int yoffset); 
    //
    
    //
    //----------------------------------
    // Basic functions
    //

    /// @brief Add a single voxel to the pixel list.
    void   addPixel(long x, long y, long z){Object3D::addPixel(x,y,z);};
    
    /// @brief Add a single voxel to the pixel list.
    void   addPixel(PixelInfo::Voxel point){
      /// @brief This one adds the pixel to the pixelArray, and updates the fluxes according to the Voxel's flux information 
      Object3D::addPixel(point.getX(),point.getY(),point.getZ());
      totalFlux += point.getF();
      if(point.getF()>peakFlux){
	peakFlux = point.getF();
	xpeak = point.getX(); ypeak = point.getY(); zpeak = point.getZ();
      }
    };

    /// @brief How many channels does the Detection have? 
    long   getNumChannels(){return Object3D::getNumDistinctZ();};

    /// @brief Is there at least the acceptable minimum number of channels in the Detection?  
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
    float       getXcentre();
    float       getYcentre();
    float       getZcentre();
    float       getTotalFlux(){return totalFlux;};
    void        setTotalFlux(float f){totalFlux=f;};
    double      getIntegFlux(){return intFlux;};
    void        setIntegFlux(double f){intFlux=f;};
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
    /// @brief Is the WCS good enough to be used? 
    ///	\return Detection::flagWCS =  True/False 
    bool        isWCS(){return flagWCS;};
    bool        isSpecOK(){return specOK;};
    void        setSpecOK(bool b){specOK=b;};
    std::string getName(){return name;};
    void        setName(std::string s){name=s;};
    std::string getRAs(){return raS;};
    std::string getDecs(){return decS;};
    float       getRA(){return ra;};
    float       getDec(){return dec;};
    float       getRAWidth(){return raWidth;};
    float       getDecWidth(){return decWidth;};
    float       getMajorAxis(){return majorAxis;};
    float       getMinorAxis(){return minorAxis;};
    float       getPositionAngle(){return posang;};
    float       getVel(){return vel;};
    float       getVelWidth(){return velWidth;};
    float       getVelMin(){return velMin;};
    float       getVelMax(){return velMax;};
    float       getW20(){return w20;};
    float       getV20Min(){return v20min;};
    float       getV20Max(){return v20max;};
    float       getW50(){return w50;};
    float       getV50Min(){return v50min;};
    float       getV50Max(){return v50max;};
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
  protected:
    // Subsection offsets
    long           xSubOffset;     ///< The x-offset, from subsectioned cube
    long           ySubOffset;     ///< The y-offset, from subsectioned cube
    long           zSubOffset;     ///< The z-offset, from subsectioned cube
    // Flux related
    float          totalFlux;      ///< sum of the fluxes of all the pixels
    double         intFlux;        ///< integrated flux : involves integration over velocity.
    float          peakFlux;       ///< maximum flux over all the pixels
    long           xpeak;          ///< x-pixel location of peak flux
    long           ypeak;          ///< y-pixel location of peak flux
    long           zpeak;          ///< z-pixel location of peak flux
    float          peakSNR;        ///< signal-to-noise ratio at peak
    float          xCentroid;      ///< x-pixel location of centroid
    float          yCentroid;      ///< y-pixel location of centroid
    float          zCentroid;      ///< z-pixel location of centroid
    std::string    centreType;     ///< which type of pixel centre to report: "average", "centroid", or "peak" (flux)
    bool           negSource;      ///< is the source a negative feature?
    std::string    flagText;       ///< any warning flags about the quality of the detection.
    // WCS related
    int            id;             ///< ID -- generally number in list
    std::string    name;	   ///< IAU-style name (based on position)
    bool           flagWCS;        ///< A flag indicating whether the WCS parameters have been set.
    std::string    raS;	           ///< Right Ascension (or Longitude) of pixel centre in form 12:34:23
    std::string    decS;	   ///< Declination (or Latitude) of pixel centre, in form -12:23:34
    float          ra;	           ///< Central Right Ascension in degrees
    float          dec;	           ///< Central Declination in degrees
    float          raWidth;        ///< Width of detection in RA direction in arcmin
    float          decWidth;       ///< Width of detection in Dec direction in arcmin
    float          majorAxis;      ///< Major axis length in arcmin
    float          minorAxis;      ///< Minor axis length in arcmin
    float          posang;         ///< Position angle of the major axis, in degrees
    bool           specOK;         ///< Is the spectral dimension valid?
    std::string    specUnits;      ///< Units of the spectral dimension
    std::string    fluxUnits;      ///< Units of flux
    std::string    intFluxUnits;   ///< Units of integrated flux
    std::string    lngtype;        ///< Type of longitude axis (RA/GLON)
    std::string    lattype;        ///< Type of latitude axis (DEC/GLAT)
    float          vel;	           ///< Central velocity (from zCentre)
    float          velWidth;       ///< Full velocity width
    float          velMin;         ///< Minimum velocity
    float          velMax;         ///< Maximum velocity
    float          v20min;         ///< Minimum velocity at 20% of peak flux
    float          v20max;         ///< Maximum velocity at 20% of peak flux
    float          w20;            ///< Velocity width at 20% of peak flux  
    float          v50min;         ///< Minimum velocity at 50% of peak flux
    float          v50max;         ///< Maximum velocity at 50% of peak flux
    float          w50;            ///< Velocity width at 50% of peak flux  
    /// @brief  The next six are the precision of values printed in the headers of the spectral plots
    ///	@name 
    /// @{ 
    int            posPrec;        ///< Precision of WCS positional values 
    int            xyzPrec;        ///< Precision of pixel positional values
    int            fintPrec;       ///< Precision of F_int/F_tot values
    int            fpeakPrec;      ///< Precision of F_peak values
    int            velPrec;        ///< Precision of velocity values.
    int            snrPrec;        ///< Precision of S/N_max values.
    /// @} 
  };

  //==========================================================================

  //////////////////////////////////////////////////////
  // Prototypes for functions that use above classes
  //////////////////////////////////////////////////////

  //----------------
  // These are in sorting.cc
  //
  /// @brief Sort a list of Detections by Z-pixel value. 
  void SortByZ(std::vector <Detection> &inputList);

  /// @brief Sort a list of Detections by Velocity.
  void SortByVel(std::vector <Detection> &inputList);

  /// @brief Sort a list of Detections by a nominated parameter
  void SortDetections(std::vector <Detection> &inputList, std::string parameter);

  //----------------
  // This is in areClose.cc
  //
  /// @brief Determine whether two objects are close according to set parameters.
  bool areClose(Detection &object1, Detection &object2, Param &par);

  //----------------
  // This is in mergeIntoList.cc
  //
  /// @brief Add an object into a list, combining with adjacent objects if need be. 
  void mergeIntoList(Detection &object, std::vector <Detection> &objList, 
		     Param &par);

  //----------------
  // These are in Cubes/Merger.cc
  //
  /// @brief Merge a list of Detections so that all adjacent voxels are in the same Detection. 
  void mergeList(std::vector<Detection> &objList, Param &par);   
  /// @brief Culls a list of Detections that do not meet minimum requirements. 
  void finaliseList(std::vector<Detection> &objList, Param &par);
  /// @brief Manage both the merging and the cleaning up of the list. 
  void ObjectMerger(std::vector<Detection> &objList, Param &par);

  /// @brief Print the header information to a particular table 
  void outputTableHeader(std::ostream &stream, std::vector<Column::Col> columns, std::string tableType, bool flagWCS);

}

#endif
