// -----------------------------------------------------------------------
// cubes.cc: Member functions for the DataArray, Cube and Image classes.
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
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <string>
#include <math.h>

#include <wcslib/wcs.h>

#include <duchamp/pgheader.hh>

#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/PixelMap/Voxel.hh>
#include <duchamp/PixelMap/Object3D.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Detection/columns.hh>
#include <duchamp/Detection/finders.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/feedback.hh>
#include <duchamp/Utils/mycpgplot.hh>
#include <duchamp/Utils/Statistics.hh>
#include <duchamp/FitsIO/DuchampBeam.hh>

using namespace mycpgplot;
using namespace Statistics;
using namespace PixelInfo;

#ifdef TEST_DEBUG
const bool TESTING=true;
#else
const bool TESTING=false;
#endif

namespace duchamp
{

  using namespace Column;

  /****************************************************************/
  ///////////////////////////////////////////////////
  //// Functions for DataArray class:
  ///////////////////////////////////////////////////

  DataArray::DataArray(){
    /// Fundamental constructor for DataArray.
    /// Number of dimensions and pixels are set to 0. Nothing else allocated.

    this->numDim=0; 
    this->numPixels=0;
    this->objectList = new std::vector<Detection>;
    this->axisDimAllocated = false;
    this->arrayAllocated = false;
  }
  //--------------------------------------------------------------------

  DataArray::DataArray(const DataArray &d){
    operator=(d);
  }

  DataArray& DataArray::operator=(const DataArray &d){
    if(this==&d) return *this;
    this->numDim = d.numDim;
    if(this->axisDimAllocated) delete [] this->axisDim;
    this->axisDimAllocated = d.axisDimAllocated;
    if(this->axisDimAllocated){
      this->axisDim = new size_t[this->numDim];
      for(size_t i=0;i<size_t(this->numDim);i++) this->axisDim[i] = d.axisDim[i];
    }
    this->numPixels = d.numPixels;
    if(this->arrayAllocated) delete [] this->array;
    this->arrayAllocated = d.arrayAllocated;
    if(this->arrayAllocated) {
      this->array = new float[this->numPixels];
      for(size_t i=0;i<size_t(this->numPixels);i++) this->array[i] = d.array[i];
    }
    this->objectList = d.objectList;
    this->par = d.par;
    this->Stats = d.Stats;
    return *this;
  }


  DataArray::DataArray(short int nDim){
    /// @details
    /// N-dimensional constructor for DataArray.
    /// Number of dimensions defined, and dimension array allocated.
    /// Number of pixels are set to 0. 
    /// \param nDim Number of dimensions.

    this->axisDimAllocated = false;
    this->arrayAllocated = false;
    if(nDim>0){
      this->axisDim = new size_t[nDim];
      this->axisDimAllocated = true;
    }
    this->numDim=nDim; 
    this->numPixels=0;
    this->objectList = new std::vector<Detection>;
  } 
  //--------------------------------------------------------------------

  DataArray::DataArray(short int nDim, size_t size){
    /// @details
    /// N-dimensional constructor for DataArray.
    /// Number of dimensions and number of pixels defined. 
    /// Arrays allocated based on these values.
    /// \param nDim Number of dimensions. 
    /// \param size Number of pixels. 
    /// 
    /// Note that we can assign values to the dimension array.

    this->axisDimAllocated = false;
    this->arrayAllocated = false;
    if(size<0)
      duchampError("DataArray(nDim,size)",
		   "Negative size -- could not define DataArray");
    else if(nDim<0)
      duchampError("DataArray(nDim,size)",
		   "Negative number of dimensions: could not define DataArray");
    else {
      if(size>0){
	this->array = new float[size];
	this->arrayAllocated = true;
      }
      this->numPixels = size;
      if(nDim>0){
	this->axisDim = new size_t[nDim];
	this->axisDimAllocated = true;
      }
      this->numDim = nDim;
    }
    this->objectList = new std::vector<Detection>;
  }
  //--------------------------------------------------------------------

  DataArray::DataArray(short int nDim, size_t *dimensions)
  {
    /// @details
    /// Most robust constructor for DataArray.
    /// Number and sizes of dimensions are defined, and hence the number of 
    /// pixels. Arrays allocated based on these values.
    /// \param nDim Number of dimensions. 
    /// \param dimensions Array giving sizes of dimensions.

    this->axisDimAllocated = false;
    this->arrayAllocated = false;
    if(nDim<0)
      duchampError("DataArray(nDim,dimArray)",
		   "Negative number of dimensions: could not define DataArray");
    else {
      int size = dimensions[0];
      for(int i=1;i<nDim;i++) size *= dimensions[i];
      if(size<0)
	duchampError("DataArray(nDim,dimArray)",
		     "Negative size: could not define DataArray");
      else{
	this->numPixels = size;
	if(size>0){
	  this->array = new float[size];
	  this->arrayAllocated = true;
	}
	this->numDim=nDim;
	if(nDim>0){
	  this->axisDim = new size_t[nDim];
	  this->axisDimAllocated = true;
	}
	for(int i=0;i<nDim;i++) this->axisDim[i] = dimensions[i];
      }
    }
  }
  //--------------------------------------------------------------------

  DataArray::~DataArray()
  {
    ///  @details
    ///  Destructor -- arrays deleted if they have been allocated, and the 
    ///   object list is deleted.

    if(this->numPixels>0 && this->arrayAllocated){
      delete [] this->array;
      this->arrayAllocated = false;
    }
    if(this->numDim>0 && this->axisDimAllocated){
      delete [] this->axisDim;
      this->axisDimAllocated = false;
    }
    delete this->objectList;
  }
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------

  void DataArray::getDim(size_t &x, size_t &y, size_t &z)
  {
    /// @details
    /// The sizes of the first three dimensions (if they exist) are returned.
    /// \param x The first dimension. Defaults to 0 if numDim \f$\le\f$ 0.
    /// \param y The second dimension. Defaults to 1 if numDim \f$\le\f$ 1.
    /// \param z The third dimension. Defaults to 1 if numDim \f$\le\f$ 2.

    if(this->numDim>0) x=this->axisDim[0]; 
    else x=0;
    if(this->numDim>1) y=this->axisDim[1]; 
    else y=1;
    if(this->numDim>2) z=this->axisDim[2]; 
    else z=1;
  }
  //--------------------------------------------------------------------

  void DataArray::getDimArray(size_t *output)
  {
    /// @details
    /// The axisDim array is written to output. This needs to be defined 
    ///  beforehand: no checking is done on the memory.
    /// \param output The array that is written to.

    for(int i=0;i<this->numDim;i++) output[i] = this->axisDim[i];
  }
  //--------------------------------------------------------------------

  void DataArray::getArray(float *output)
  {
    /// @details
    /// The pixel value array is written to output. This needs to be defined 
    ///  beforehand: no checking is done on the memory.
    /// \param output The array that is written to.

    for(int i=0;i<this->numPixels;i++) output[i] = this->array[i];
  }
  //--------------------------------------------------------------------

  void DataArray::saveArray(float *input, size_t size)
  {
    /// @details
    /// Saves the array in input to the pixel array DataArray::array.
    /// The size of the array given must be the same as the current number of
    /// pixels, else an error message is returned and nothing is done.
    /// \param input The array of values to be saved.
    /// \param size The size of input.

    if(size != this->numPixels)
      duchampError("DataArray::saveArray",
		   "Input array different size to existing array. Cannot save.");
    else {
      if(this->numPixels>0 && this->arrayAllocated) delete [] this->array;
      this->numPixels = size;
      this->array = new float[size];
      this->arrayAllocated = true;
      for(int i=0;i<size;i++) this->array[i] = input[i];
    }
  }
  //--------------------------------------------------------------------

  void DataArray::addObject(Detection object)
  { 
    /// \param object The object to be added to the object list.

    // objectList is a vector, so just use push_back()
    this->objectList->push_back(object);
  }
  //--------------------------------------------------------------------

  void DataArray::addObjectList(std::vector <Detection> newlist) 
  {
    /// \param newlist The list of objects to be added to the object list.

    std::vector<Detection>::iterator obj;
    for(obj=newlist.begin();obj<newlist.end();obj++) this->objectList->push_back(*obj);
  }
  //--------------------------------------------------------------------

  bool DataArray::isDetection(float value)
  {
    ///  @details
    /// Is a given value a detection, based on the statistics in the 
    /// DataArray's StatsContainer? 
    /// \param value The pixel value to test.

    if(par.isBlank(value)) return false;
    else return Stats.isDetection(value);
  }
  //--------------------------------------------------------------------

  bool DataArray::isDetection(size_t voxel)
  {
    ///  @details
    /// Is a given pixel a detection, based on the statistics in the 
    /// DataArray's StatsContainer? 
    /// If the pixel lies outside the valid range for the data array, return false.
    /// \param voxel Location of the DataArray's pixel to be tested.

    if((voxel<0)||(voxel>this->numPixels)) return false;
    else if(par.isBlank(this->array[voxel])) return false;
    else return Stats.isDetection(this->array[voxel]);
  }  
  //--------------------------------------------------------------------

  std::ostream& operator<< ( std::ostream& theStream, DataArray &array)
  {
    /// @details
    /// A way to print out the pixel coordinates & flux values of the 
    /// list of detected objects belonging to the DataArray.
    /// These are formatted nicely according to the << operator for Detection,
    ///  with a line indicating the number of detections at the start.
    /// \param theStream The ostream object to which the output should be sent.
    /// \param array The DataArray containing the list of objects.

    for(int i=0;i<array.numDim;i++){
      if(i>0) theStream<<"x";
      theStream<<array.axisDim[i];
    }
    theStream<<std::endl;

    theStream<<"Threshold\tmiddle\tspread\trobust\n" << array.stats().getThreshold() << "\t";
    if(array.pars().getFlagUserThreshold())
      theStream << "0.0000\t" << array.stats().getThreshold() << "\t"; 
    else
      theStream << array.stats().getMiddle() << " " << array.stats().getSpread() << "\t";
    theStream << array.stats().getRobust()<<"\n";

    theStream<<array.objectList->size()<<" detections:\n--------------\n";
    std::vector<Detection>::iterator obj;
    for(obj=array.objectList->begin();obj<array.objectList->end();obj++){
      theStream << "Detection #" << obj->getID()<<std::endl;
      Detection *newobj = new Detection;
      *newobj = *obj;
      newobj->addOffsets();
      theStream<<*newobj;
      delete newobj;
    }
    theStream<<"--------------\n";
    return theStream;
  }

  /****************************************************************/
  /////////////////////////////////////////////////////////////
  //// Functions for Cube class
  /////////////////////////////////////////////////////////////

  Cube::Cube()
  {
    /// @details
    /// Basic Constructor for Cube class.
    /// numDim set to 3, but numPixels to 0 and all bool flags to false.
    /// No allocation done.

    numPixels=0; numDim=3; 
    reconExists = false; reconAllocated = false; baselineAllocated = false;
  }
  //--------------------------------------------------------------------

  Cube::Cube(size_t size)
  {
    /// @details
    /// Alternative Cube constructor, where size is given but not individual
    ///  dimensions. Arrays are allocated as appropriate (according to the 
    ///  relevant flags in Param set), but the Cube::axisDim array is not.

    this->reconAllocated = false;
    this->baselineAllocated = false;
    this->axisDimAllocated = false;
    this->arrayAllocated = false;
    this->numPixels = this->numDim = 0;
    if(size<0)
      duchampError("Cube(size)","Negative size -- could not define Cube");
    else{
      if(size>0){
	this->array = new float[size];
	this->arrayAllocated = true;
	if(this->par.getFlagATrous()||this->par.getFlagSmooth()){
	  this->recon = new float[size];
	  this->reconAllocated = true;
	}
	if(this->par.getFlagBaseline()){
	  this->baseline = new float[size];
	  this->baselineAllocated = true;
	}
      }
      this->numPixels = size;
      this->axisDim = new size_t[3];
      this->axisDimAllocated = true;
      this->numDim = 3;
      this->reconExists = false;
    }
  }
  //--------------------------------------------------------------------

  Cube::Cube(size_t *dimensions)
  {
    /// Alternative Cube constructor, where sizes of dimensions are given. 
    /// Arrays are allocated as appropriate (according to the 
    ///  relevant flags in Param set), as is the Cube::axisDim array.

    int size   = dimensions[0] * dimensions[1] * dimensions[2];
    int imsize = dimensions[0] * dimensions[1];
    this->reconAllocated = false;
    this->baselineAllocated = false;
    this->axisDimAllocated = false;
    this->arrayAllocated = false;
    this->numPixels = this->numDim = 0;
    if((size<0) || (imsize<0) )
      duchampError("Cube(dimArray)","Negative size -- could not define Cube");
    else{
      this->numPixels = size;
      if(size>0){
	this->array      = new float[size];
	this->arrayAllocated = true;
	this->detectMap  = new short[imsize];
	if(this->par.getFlagATrous()||this->par.getFlagSmooth()){
	  this->recon    = new float[size];
	  this->reconAllocated = true;
	}
	if(this->par.getFlagBaseline()){
	  this->baseline = new float[size];
	  this->baselineAllocated = true;
	}
      }
      this->numDim  = 3;
      this->axisDim = new size_t[3];
      this->axisDimAllocated = true;
      for(int i=0;i<3     ;i++) this->axisDim[i]   = dimensions[i];
      for(int i=0;i<imsize;i++) this->detectMap[i] = 0;
      this->reconExists = false;
    }
  }
  //--------------------------------------------------------------------

  Cube::~Cube()
  {
    /// @details
    ///  The destructor deletes the memory allocated for Cube::detectMap, and,
    ///  if these have been allocated, Cube::recon and Cube::baseline.

    if(this->numPixels>0 && this->arrayAllocated) delete [] this->detectMap;
    if(this->reconAllocated)    delete [] this->recon;
    if(this->baselineAllocated) delete [] this->baseline;
  }
  //--------------------------------------------------------------------

  Cube::Cube(const Cube &c):
    DataArray(c)
  {
    this->operator=(c);
  }
  //--------------------------------------------------------------------

  Cube& Cube::operator=(const Cube &c)
  {
    if(this==&c) return *this;
    if(this->arrayAllocated) delete [] this->detectMap;
    ((DataArray &) *this) = c;
    this->reconExists = c.reconExists;
    if(this->reconAllocated) delete [] this->recon;
    this->reconAllocated = c.reconAllocated;
    if(this->reconAllocated) {
      this->recon = new float[this->numPixels];
      for(size_t i=0;i<size_t(this->numPixels);i++) this->recon[i] = c.recon[i];
    }
    if(this->arrayAllocated){
      this->detectMap = new short[this->axisDim[0]*this->axisDim[1]];
      for(size_t i=0;i<size_t(this->axisDim[0]*this->axisDim[1]);i++) this->detectMap[i] = c.detectMap[i];
    }
    if(this->baselineAllocated) delete [] this->baseline;
    this->baselineAllocated = c.baselineAllocated;
    if(this->baselineAllocated){
      this->baseline = new float[this->numPixels];
      for(size_t i=0;i<size_t(this->numPixels);i++) this->baseline[i] = c.baseline[i];
    }
    this->head = c.head;
    this->fullCols = c.fullCols;
    this->logCols = c.logCols;
    return *this;
  }
  //--------------------------------------------------------------------

  Cube* Cube::slice(Section subsection)
  {
    Cube *output = new Cube;
    Section thisSection;
    std::string nullsec=nullSection(this->numDim);
    if(this->par.section().isParsed()) thisSection=this->par.section();
    else{
      thisSection = Section(nullsec);
      thisSection.parse(this->axisDim, this->numDim);
    }

    subsection.parse(this->axisDim, this->numDim);
    if(subsection.isValid()){
      output->par = this->par;
      output->par.section() = thisSection * subsection;
      output->par.setXOffset(output->par.getXOffset()+subsection.getStart(0));
      output->par.setYOffset(output->par.getYOffset()+subsection.getStart(1));
      output->par.setZOffset(output->par.getZOffset()+subsection.getStart(2));
      output->head = this->head;
      // correct the reference pixel in the WCS
      output->head.WCS().crpix[output->head.WCS().lng] -= subsection.getStart(output->head.WCS().lng);
      output->head.WCS().crpix[output->head.WCS().lat] -= subsection.getStart(output->head.WCS().lat);
      if(output->head.WCS().spec>0)
	output->head.WCS().crpix[output->head.WCS().spec] -= subsection.getStart(output->head.WCS().spec);
      output->Stats = this->Stats;
      output->fullCols = this->fullCols;
      output->logCols = this->logCols;
      size_t *dims = new size_t[3];
      for(size_t i=0;i<3;i++){
	dims[i] = subsection.getDimList()[i];
	std::cout << "Dim " << i+1 << " = " << dims[i] << "\n";
      }
     
      output->initialiseCube(dims,true);
      for(size_t z=0;z<output->axisDim[2];z++){
	for(size_t y=0;y<output->axisDim[1];y++){
	  for(size_t x=0;x<output->axisDim[0];x++){
	    size_t impos=x+y*output->axisDim[0];
	    size_t pos=impos+z*output->axisDim[0]*output->axisDim[1]; 
	    if(pos>=output->numPixels) duchampError("cube slicer","Out of bounds in new Cube");
	    size_t imposIn=(x+subsection.getStart(0)) + (y+subsection.getStart(1))*this->axisDim[0];
	    size_t posIn=imposIn + (z+subsection.getStart(2))*this->axisDim[0]*this->axisDim[1];
	    if(posIn>=this->numPixels) duchampError("cube slicer","Out of bounds in new Cube");
	    output->array[pos] = this->array[posIn];
	    output->detectMap[impos] = this->detectMap[imposIn];
	    if(this->reconAllocated) output->recon[pos] = this->recon[posIn];
	    if(this->baselineAllocated) output->baseline[pos] = this->baseline[posIn];
	  }
	}
      }
       std::cout << this->par << "\n"<<output->par <<"\n";
    }
    else{
      duchampError("cube slicer","Subsection does not parse");
    }

    return output;

  }
  //--------------------------------------------------------------------

  OUTCOME Cube::initialiseCube(long *dimensions, bool allocateArrays)
  {
    int numAxes = this->head.getNumAxes();
    if(numAxes<=0) numAxes=3;
    size_t *dim = new size_t[numAxes];
    for(int i=0;i<numAxes;i++) dim[i]=dimensions[i];
    this->initialiseCube(dim,allocateArrays);
    delete dim;
  }


  OUTCOME Cube::initialiseCube(size_t *dimensions, bool allocateArrays)
  {
    /// @details
    ///  This function will set the sizes of all arrays that will be used by Cube.
    ///  It will also define the values of the axis dimensions: this will be done 
    ///   using the WCS in the FitsHeader class, so the WCS needs to be good and 
    ///   have three axes. If this is not the case, the axes are assumed to be 
    ///   ordered in the sense of lng,lat,spc.
    /// 
    ///  \param dimensions An array of values giving the dimensions (sizes) for 
    ///  all axes.  
    ///  \param allocateArrays A flag indicating whether to allocate
    ///  the memory for the data arrays: the default is true. The
    ///  dimension arrays will be allocated and filled.

    int lng,lat,spc;
    size_t size,imsize;
  
    int numAxes = this->head.getNumAxes();
    if(numAxes<=0) numAxes=3;

    if(this->head.isWCS() && (numAxes>=3) && (this->head.WCS().spec>=0)){
      // if there is a WCS and there is at least 3 axes
      lng = this->head.WCS().lng;
      lat = this->head.WCS().lat;
      spc = this->head.WCS().spec;
    }
    else{
      // just take dimensions[] at face value
      lng = 0;
      lat = 1;
      spc = 2;
    }

    size   = dimensions[lng];
    if(numAxes>1) size *= dimensions[lat];
    if(this->head.canUseThirdAxis() && numAxes>spc) size *= dimensions[spc];

    imsize = dimensions[lng];
    if(numAxes>1) imsize *= dimensions[lat];

    this->reconAllocated = false;
    this->baselineAllocated = false;

    if(this->axisDimAllocated){
      delete [] this->axisDim;
      this->axisDimAllocated = false;
    }

    if(this->arrayAllocated){
      delete [] this->array;
      delete [] this->detectMap;
      this->arrayAllocated = false;
    }
    if(this->reconAllocated){
      delete [] this->recon;
      this->reconAllocated = false;
    }
    if(this->baselineAllocated){
      delete [] this->baseline;
      this->baselineAllocated = false;
    }

    if((size<0) || (imsize<0) ) {
      duchampError("Cube::initialiseCube(dimArray)",
		   "Negative size -- could not define Cube.\n");
      return FAILURE;
    }
    else{
      this->numPixels = size;
      this->numDim  = 3;

      this->axisDim = new size_t[this->numDim];
      this->axisDimAllocated = true;
      this->axisDim[0] = dimensions[lng];
      if(numAxes>1) this->axisDim[1] = dimensions[lat];
      else this->axisDim[1] = 1;
      if(this->head.canUseThirdAxis() && numAxes>spc) this->axisDim[2] = dimensions[spc];
      else this->axisDim[2] = 1;

      int numNondegDim=0;
      for(int i=0;i<3;i++) if(this->axisDim[i]>1) numNondegDim++;

      if(this->par.getFlagSmooth()){
	if(this->par.getSmoothType()=="spectral" && numNondegDim==2){
	  duchampWarning("Cube::initialiseCube", "Spectral smooth requested, but have a 2D image. Setting flagSmooth=false");
	  this->par.setFlagSmooth(false);
	}
	if(this->par.getSmoothType()=="spatial" && numNondegDim==1){
	  duchampWarning("Cube::initialiseCube", "Spatial smooth requested, but have a 1D image. Setting flagSmooth=false");
	  this->par.setFlagSmooth(false);
	}
      }
      if(this->par.getFlagATrous()){
	for(int d=3; d>=1; d--){
	  if(this->par.getReconDim()==d && numNondegDim==(d-1)){
	    std::stringstream ss;
	    ss << d << "D reconstruction requested, but image is " << d-1 <<"D. Setting flagAtrous=false";
	    duchampWarning("Cube::initialiseCube", ss.str());
	    this->par.setFlagATrous(false);
	  }
	}
      }

      if(allocateArrays && this->par.isVerbose()) this->reportMemorySize(std::cout,allocateArrays);

      this->reconExists = false;
      if(size>0 && allocateArrays){
	this->array      = new float[size];
	this->arrayAllocated = true;
	this->detectMap  = new short[imsize];
	for(int i=0;i<imsize;i++) this->detectMap[i] = 0;
	if(this->par.getFlagATrous() || this->par.getFlagSmooth()){
	  this->recon    = new float[size];
	  this->reconAllocated = true;
	  for(int i=0;i<size;i++) this->recon[i] = 0.;
	}
	if(this->par.getFlagBaseline()){
	  this->baseline = new float[size];
	  this->baselineAllocated = true;
	  for(int i=0;i<size;i++) this->baseline[i] = 0.;
	}
      }
    }
    return SUCCESS;
  }
  //--------------------------------------------------------------------

  void Cube::reportMemorySize(std::ostream &theStream, bool allocateArrays)
  {
    std::string unitlist[4]={"kB","MB","GB","TB"};
    size_t size=axisDim[0]*axisDim[1]*axisDim[2];
    size_t imsize=axisDim[0]*axisDim[1];
    
      // Calculate and report the total size of memory to be allocated.
      float allocSize=3*sizeof(size_t);  // axisDim
      float arrAllocSize=0.;
      if(size>0 && allocateArrays){
	allocSize += size * sizeof(float); // array
	arrAllocSize = size*sizeof(float);
	allocSize += imsize * sizeof(short); // detectMap
	if(this->par.getFlagATrous() || this->par.getFlagSmooth()) 
	  allocSize += size * sizeof(float); // recon
	if(this->par.getFlagBaseline())
	  allocSize += size * sizeof(float); // baseline
      }
      std::string units="bytes";
      for(int i=0;i<4 && allocSize>1024.;i++){
	allocSize/=1024.;
	arrAllocSize /= 1024.;
	units=unitlist[i];
      }

      theStream << "\n About to allocate " << allocSize << units;
      if(arrAllocSize > 0.) theStream << " of which " << arrAllocSize << units << " is for the image\n";
      else theStream << "\n";
  }


  bool Cube::is2D()
  {
    /// @details
    ///   Check whether the image is 2-dimensional, by counting
    ///   the number of dimensions that are greater than 1

    if(this->head.WCS().naxis==2) return true;
    else{
      int numDim=0;
      for(int i=0;i<this->numDim;i++) if(axisDim[i]>1) numDim++;
      return numDim<=2;
    }

  }
  //--------------------------------------------------------------------

  OUTCOME Cube::getCube()
  {  
    ///  @details
    /// A front-end to the Cube::getCube() function, that does 
    ///  subsection checks.
    /// Assumes the Param is set up properly.

    std::string fname = par.getImageFile();
    if(par.getFlagSubsection()) fname+=par.getSubsection();
    return getCube(fname);
  }
  //--------------------------------------------------------------------

  void Cube::saveArray(float *input, size_t size)
  {
    if(size != this->numPixels){
      std::stringstream errmsg;
      errmsg << "Input array different size to existing array ("
	     << size << " cf. " << this->numPixels << "). Cannot save.\n";
      duchampError("Cube::saveArray",errmsg.str());
    }
    else {
      if(this->numPixels>0 && this->arrayAllocated) delete [] array;
      this->numPixels = size;
      this->array = new float[size];
      this->arrayAllocated = true;
      for(int i=0;i<size;i++) this->array[i] = input[i];
    }
  }
  //--------------------------------------------------------------------

  void Cube::saveArray(std::vector<float> &input)
  {
    /// @details
    /// Saves the array in input to the pixel array Cube::array.
    /// The size of the array given must be the same as the current number of
    /// pixels, else an error message is returned and nothing is done.
    /// \param input The array of values to be saved.

    if(input.size() != this->numPixels){
      std::stringstream errmsg;
      errmsg << "Input array different size to existing array ("
	     << input.size() << " cf. " << this->numPixels << "). Cannot save.\n";
      duchampError("Cube::saveArray",errmsg.str());
    }
    else {
      if(this->numPixels>0 && this->arrayAllocated) delete [] this->array;
      this->numPixels = input.size();
      this->array = new float[input.size()];
      this->arrayAllocated = true;
      for(size_t i=0;i<input.size();i++) this->array[i] = input[i];
    }
  }
  //--------------------------------------------------------------------

  void Cube::saveRecon(float *input, size_t size)
  {
    /// @details
    /// Saves the array in input to the reconstructed array Cube::recon
    /// The size of the array given must be the same as the current number of
    /// pixels, else an error message is returned and nothing is done.
    /// If the recon array has already been allocated, it is deleted first, and 
    /// then the space is allocated.
    /// Afterwards, the appropriate flags are set.
    /// \param input The array of values to be saved.
    /// \param size The size of input.

    if(size != this->numPixels){
      std::stringstream errmsg;
      errmsg << "Input array different size to existing array ("
	     << size << " cf. " << this->numPixels << "). Cannot save.\n";
      duchampError("Cube::saveRecon",errmsg.str());
    }
    else {
      if(this->numPixels>0){
	if(this->reconAllocated) delete [] this->recon;
	this->numPixels = size;
	this->recon = new float[size];
	this->reconAllocated = true;
	for(int i=0;i<size;i++) this->recon[i] = input[i];
	this->reconExists = true;
      }
    }
  }
  //--------------------------------------------------------------------

  void Cube::getRecon(float *output)
  {
    /// @details
    /// The reconstructed array is written to output. The output array needs to 
    ///  be defined beforehand: no checking is done on the memory.
    /// \param output The array that is written to.

    // Need check for change in number of pixels!
    for(int i=0;i<this->numPixels;i++){
      if(this->reconExists) output[i] = this->recon[i];
      else output[i] = 0.;
    }
  }
  //--------------------------------------------------------------------

  void Cube::removeMW()
  {
    /// @details
    /// The channels corresponding to the Milky Way range (as given by the Param
    ///  set) are all set to 0 in the pixel array.
    /// Only done if the appropriate flag is set, and the pixels are not BLANK.
    /// \deprecated

    if(this->par.getFlagMW()){
      for(int pix=0;pix<this->axisDim[0]*this->axisDim[1];pix++){
	for(int z=0;z<this->axisDim[2];z++){
	  int pos = z*this->axisDim[0]*this->axisDim[1] + pix;
	  if(!this->isBlank(pos) && this->par.isInMW(z)) this->array[pos]=0.;
	}
      }
    }
  }
  //--------------------------------------------------------------------

  void Cube::setCubeStats()
  {
    ///   @details
    ///   Calculates the full statistics for the cube:
    ///     mean, rms, median, madfm
    ///   Only do this if the threshold has not been defined (ie. is still 0.,
    ///    its default). 
    ///   Also work out the threshold and store it in the par set.
    ///   
    ///   Different from Cube::setCubeStatsOld() as it doesn't use the 
    ///    getStats functions but has own versions of them hardcoded to 
    ///    ignore BLANKs and MW channels. This saves on memory usage -- necessary
    ///    for dealing with very big files.
    /// 
    ///   Three cases exist:
    ///  <ul><li>Simple case, with no reconstruction/smoothing: all stats 
    ///          calculated from the original array.
    ///      <li>Wavelet reconstruction: mean & median calculated from the 
    ///          original array, and stddev & madfm from the residual.
    ///      <li>Smoothing: all four stats calculated from the recon array 
    ///          (which holds the smoothed data).
    ///  </ul>

    if(this->par.getFlagUserThreshold() ){
      // if the user has defined a threshold, set this in the StatsContainer
      this->Stats.setThreshold( this->par.getThreshold() );
    }
    else{
      // only work out the stats if we need to.
      // the only reason we don't is if the user has specified a threshold.
    
      this->Stats.setRobust(this->par.getFlagRobustStats());

      if(this->par.isVerbose())
	std::cout << "Calculating the cube statistics... " << std::flush;
    
      // size_t xysize = this->axisDim[0]*this->axisDim[1];

      bool *mask = new bool[this->numPixels];
      int vox=0,goodSize = 0;
      for(int z=0;z<this->axisDim[2];z++){
	for(int y=0;y<this->axisDim[1];y++){
	  for(int x=0;x<this->axisDim[0];x++){
	    //	    vox = z * xysize + y*this->axisDim[0] + x;
	    bool isBlank=this->isBlank(vox);
	    bool isMW = this->par.isInMW(z);
	    bool statOK = this->par.isStatOK(x,y,z);
	    mask[vox] = (!isBlank && !isMW && statOK );
	    if(mask[vox]) goodSize++;
	    vox++;
	  }
	}
      }

      //      float mean,median,stddev,madfm;
      if( this->par.getFlagATrous() ){
	// Case #2 -- wavelet reconstruction
	// just get mean & median from orig array, and rms & madfm from
	// residual recompute array values to be residuals & then find
	// stddev & madfm
	if(!this->reconExists)
	  duchampError("setCubeStats",
		       "Reconstruction not yet done!\nCannot calculate stats!\n");
	else{
	  float *tempArray = new float[goodSize];

	  goodSize=0;
	  vox=0;
	  for(int z=0;z<this->axisDim[2];z++){
	    for(int y=0;y<this->axisDim[1];y++){
	      for(int x=0;x<this->axisDim[0];x++){
		//		vox = z * xysize + y*this->axisDim[0] + x;
		if(mask[vox]) tempArray[goodSize++] = this->array[vox];
		vox++;
	      }
	    }
	  }

	  // First, find the mean of the original array. Store it.
	  this->Stats.setMean( findMean<float>(tempArray, goodSize) );
	
	  // Now sort it and find the median. Store it.
	  this->Stats.setMedian( findMedian<float>(tempArray, goodSize, true) );

	  // Now calculate the residuals and find the mean & median of
	  // them. We don't store these, but they are necessary to find
	  // the sttdev & madfm.
	  goodSize = 0;
	  //	  for(int p=0;p<xysize;p++){
	  vox=0;
	  for(int z=0;z<this->axisDim[2];z++){
	    for(int y=0;y<this->axisDim[1];y++){
	      for(int x=0;x<this->axisDim[0];x++){
		//	      vox = z * xysize + p;
	      if(mask[vox])
		tempArray[goodSize++] = this->array[vox] - this->recon[vox];
	      vox++;
	      }
	    }
	  }
	    
	  this->Stats.setStddev( findStddev<float>(tempArray, goodSize) );

	  // Now find the madfm of the residuals. Store it.
	  this->Stats.setMadfm( findMADFM(tempArray, goodSize, true) );

	  delete [] tempArray;
	}
      }
      else if(this->par.getFlagSmooth()) {
	// Case #3 -- smoothing
	// get all four stats from the recon array, which holds the
	// smoothed data. This can just be done with the
	// StatsContainer::calculate function, using the mask generated
	// earlier.
	if(!this->reconExists)
	  duchampError("setCubeStats","Smoothing not yet done!\nCannot calculate stats!\n");
	else this->Stats.calculate(this->recon,this->numPixels,mask);
      }
      else{
	// Case #1 -- default case, with no smoothing or reconstruction.
	// get all four stats from the original array. This can just be
	// done with the StatsContainer::calculate function, using the
	// mask generated earlier.
	this->Stats.calculate(this->array,this->numPixels,mask);
      }

      this->Stats.setUseFDR( this->par.getFlagFDR() );
      // If the FDR method has been requested, define the P-value
      // threshold
      if(this->par.getFlagFDR())  this->setupFDR();
      else{
	// otherwise, calculate threshold based on the requested SNR cut
	// level, and then set the threshold parameter in the Par set.
	this->Stats.setThresholdSNR( this->par.getCut() );
	this->par.setThreshold( this->Stats.getThreshold() );
      }
    
      delete [] mask;

    }

    if(this->par.isVerbose()){
      std::cout << "Using ";
      if(this->par.getFlagFDR()) std::cout << "effective ";
      std::cout << "flux threshold of: ";
      float thresh = this->Stats.getThreshold();
      if(this->par.getFlagNegative()) thresh *= -1.;
      std::cout << thresh;
      if(this->par.getFlagGrowth()){
	std::cout << " and growing to threshold of: ";
	if(this->par.getFlagUserGrowthThreshold()) thresh= this->par.getGrowthThreshold();
	else thresh= this->Stats.snrToValue(this->par.getGrowthCut());
	if(this->par.getFlagNegative()) thresh *= -1.;
	std::cout << thresh;
      }
      std::cout << std::endl;
    }

  }
  //--------------------------------------------------------------------

  void Cube::setupFDR()
  {
    /// @details
    ///  Call the setupFDR(float *) function on the pixel array of the
    ///  cube. This is the usual way of running it.
    /// 
    ///  However, if we are in smoothing mode, we calculate the FDR
    ///  parameters using the recon array, which holds the smoothed
    ///  data. Gives an error message if the reconExists flag is not set.

    if(this->par.getFlagSmooth()) 
      if(this->reconExists) this->setupFDR(this->recon);
      else{
	duchampError("setupFDR",
		     "Smoothing not done properly! Using original array for defining threshold.\n");
	this->setupFDR(this->array);
      }
    else if( this->par.getFlagATrous() ){
      this->setupFDR(this->recon);
    }
    else{
      this->setupFDR(this->array);
    }
  }
  //--------------------------------------------------------------------

  void Cube::setupFDR(float *input)
  {
    ///   @details
    ///   Determines the critical Probability value for the False
    ///   Discovery Rate detection routine. All pixels in the given arry
    ///   with Prob less than this value will be considered detections.
    /// 
    ///   Note that the Stats of the cube need to be calculated first.
    /// 
    ///   The Prob here is the probability, assuming a Normal
    ///   distribution, of obtaining a value as high or higher than the
    ///   pixel value (ie. only the positive tail of the PDF).
    /// 
    ///   The probabilities are calculated using the
    ///   StatsContainer::getPValue(), which calculates the z-statistic,
    ///   and then the probability via
    ///   \f$0.5\operatorname{erfc}(z/\sqrt{2})\f$ -- giving the positive
    ///   tail probability.

    // first calculate p-value for each pixel -- assume Gaussian for now.

    float *orderedP = new float[this->numPixels];
    int count = 0;
    for(int x=0;x<this->axisDim[0];x++){
      for(int y=0;y<this->axisDim[1];y++){
	for(int z=0;z<this->axisDim[2];z++){
	  int pix = z * this->axisDim[0]*this->axisDim[1] + 
	    y*this->axisDim[0] + x;

	  if(!(this->par.isBlank(this->array[pix])) && !this->par.isInMW(z)){ 
	    // only look at non-blank, valid pixels 
	    //  	  orderedP[count++] = this->Stats.getPValue(this->array[pix]);
	    orderedP[count++] = this->Stats.getPValue(input[pix]);
	  }
	}
      }
    }

    // now order them 
    std::stable_sort(orderedP,orderedP+count);
  
    // now find the maximum P value.
    int max = 0;
    double cN = 0.;
    // Calculate number of correlated pixels. Assume all spatial
    // pixels within the beam are correlated, and multiply this by the
    // number of correlated pixels as determined by the beam
    int numVox;
    if(this->head.beam().isDefined()) numVox = int(ceil(this->head.beam().area()));
    else  numVox = 1;
    if(this->head.canUseThirdAxis()) numVox *= this->par.getFDRnumCorChan();
    for(int psfCtr=1;psfCtr<=numVox;psfCtr++) cN += 1./float(psfCtr);

    double slope = this->par.getAlpha()/cN;
    for(int loopCtr=0;loopCtr<count;loopCtr++) {
      if( orderedP[loopCtr] < (slope * double(loopCtr+1)/ double(count)) ){
	max = loopCtr;
      }
    }

    this->Stats.setPThreshold( orderedP[max] );


    // Find real value of the P threshold by finding the inverse of the 
    //  error function -- root finding with brute force technique 
    //  (relatively slow, but we only do it once).
    double zStat     = 0.;
    double deltaZ    = 0.1;
    double tolerance = 1.e-6;
    double initial   = 0.5 * erfc(zStat/M_SQRT2) - this->Stats.getPThreshold();
    do{
      zStat+=deltaZ;
      double current = 0.5 * erfc(zStat/M_SQRT2) - this->Stats.getPThreshold();
      if((initial*current)<0.){
	zStat-=deltaZ;
	deltaZ/=2.;
      }
    }while(deltaZ>tolerance);
    this->Stats.setThreshold( zStat*this->Stats.getSpread() + 
			      this->Stats.getMiddle() );

    ///////////////////////////
    //   if(TESTING){
    //     std::stringstream ss;
    //     float *xplot = new float[2*max];
    //     for(int i=0;i<2*max;i++) xplot[i]=float(i)/float(count);
    //     cpgopen("latestFDR.ps/vcps");
    //     cpgpap(8.,1.);
    //     cpgslw(3);
    //     cpgenv(0,float(2*max)/float(count),0,orderedP[2*max],0,0);
    //     cpglab("i/N (index)", "p-value","");
    //     cpgpt(2*max,xplot,orderedP,DOT);

    //     ss.str("");
    //     ss << "\\gm = " << this->Stats.getMiddle();
    //     cpgtext(max/(4.*count),0.9*orderedP[2*max],ss.str().c_str());
    //     ss.str("");
    //     ss << "\\gs = " << this->Stats.getSpread();
    //     cpgtext(max/(4.*count),0.85*orderedP[2*max],ss.str().c_str());
    //     ss.str("");
    //     ss << "Slope = " << slope;
    //     cpgtext(max/(4.*count),0.8*orderedP[2*max],ss.str().c_str());
    //     ss.str("");
    //     ss << "Alpha = " << this->par.getAlpha();
    //     cpgtext(max/(4.*count),0.75*orderedP[2*max],ss.str().c_str());
    //     ss.str("");
    //     ss << "c\\dN\\u = " << cN;
    //     cpgtext(max/(4.*count),0.7*orderedP[2*max],ss.str().c_str());
    //     ss.str("");
    //     ss << "max = "<<max << " (out of " << count << ")";
    //     cpgtext(max/(4.*count),0.65*orderedP[2*max],ss.str().c_str());
    //     ss.str("");
    //     ss << "Threshold = "<<zStat*this->Stats.getSpread()+this->Stats.getMiddle();
    //     cpgtext(max/(4.*count),0.6*orderedP[2*max],ss.str().c_str());
  
    //     cpgslw(1);
    //     cpgsci(RED);
    //     cpgmove(0,0);
    //     cpgdraw(1,slope);
    //     cpgsci(BLUE);
    //     cpgsls(DOTTED);
    //     cpgmove(0,orderedP[max]);
    //     cpgdraw(2*max/float(count),orderedP[max]);
    //     cpgmove(max/float(count),0);
    //     cpgdraw(max/float(count),orderedP[2*max]);
    //     cpgsci(GREEN);
    //     cpgsls(SOLID);
    //     for(int i=1;i<=10;i++) {
    //       ss.str("");
    //       ss << float(i)/2. << "\\gs";
    //       float prob = 0.5*erfc((float(i)/2.)/M_SQRT2);
    //       cpgtick(0, 0, 0, orderedP[2*max],
    // 	      prob/orderedP[2*max],
    // 	      0, 1, 1.5, 90., ss.str().c_str());
    //     }
    //     cpgend();
    //     delete [] xplot;
    //   }
    delete [] orderedP;

  }
  //--------------------------------------------------------------------

  void Cube::Search()
  {
    /// @details
    /// This acts as a switching function to select the correct searching function based on the user's parameters.
    /// @param verboseFlag If true, text is written to stdout describing the search function being used.
    if(this->par.getFlagATrous()){
      if(this->par.isVerbose()) std::cout<<"Commencing search in reconstructed cube..."<<std::endl;
      this->ReconSearch();
    }  
    else if(this->par.getFlagSmooth()){
      if(this->par.isVerbose()) std::cout<<"Commencing search in smoothed cube..."<<std::endl;
      this->SmoothSearch();
    }
    else{
      if(this->par.isVerbose()) std::cout<<"Commencing search in cube..."<<std::endl;
      this->CubicSearch();
    }

  }

  bool Cube::isDetection(size_t x, size_t y, size_t z)
  {
    ///  @details
    /// Is a given voxel at position (x,y,z) a detection, based on the statistics
    ///  in the Cube's StatsContainer? 
    /// If the pixel lies outside the valid range for the data array,
    /// return false.
    /// \param x X-value of the Cube's voxel to be tested.
    /// \param y Y-value of the Cube's voxel to be tested.
    /// \param z Z-value of the Cube's voxel to be tested.

    size_t voxel = z*axisDim[0]*axisDim[1] + y*axisDim[0] + x;
    return DataArray::isDetection(array[voxel]);
  }
  //--------------------------------------------------------------------

  void Cube::calcObjectFluxes()
  {
    /// @details
    ///  A function to calculate the fluxes and centroids for each
    ///  object in the Cube's list of detections. Uses
    ///  Detection::calcFluxes() for each object.

    std::vector<Detection>::iterator obj;
    for(obj=this->objectList->begin();obj<this->objectList->end();obj++){
      obj->calcFluxes(this->array, this->axisDim);
      if(this->par.getFlagUserThreshold())
	obj->setPeakSNR( obj->getPeakFlux() / this->Stats.getThreshold() );
      else
	obj->setPeakSNR( (obj->getPeakFlux() - this->Stats.getMiddle()) / this->Stats.getSpread() );
    }
  }
  //--------------------------------------------------------------------

  void Cube::calcObjectWCSparams()
  {
    ///  @details
    ///  A function that calculates the WCS parameters for each object in the 
    ///  Cube's list of detections.
    ///  Each object gets an ID number assigned to it (which is simply its order 
    ///   in the list), and if the WCS is good, the WCS paramters are calculated.

    std::vector<Detection>::iterator obj;
    int ct=0;
    ProgressBar bar;
    if(this->par.isVerbose()) bar.init(this->objectList->size());
    for(obj=this->objectList->begin();obj<this->objectList->end();obj++){
      //      std::cerr << ct << ' ' << this->array << '\n';
      if(this->par.isVerbose()) bar.update(ct);
      obj->setID(ct++);
      if(!obj->hasParams()){
	obj->setCentreType(this->par.getPixelCentre());
	obj->calcFluxes(this->array,this->axisDim);
	//      obj->calcWCSparams(this->array,this->axisDim,this->head);
	obj->calcWCSparams(this->head);
	obj->calcIntegFlux(this->array,this->axisDim,this->head);
	
	if(this->par.getFlagUserThreshold())
	  obj->setPeakSNR( obj->getPeakFlux() / this->Stats.getThreshold() );
	else
	  obj->setPeakSNR( (obj->getPeakFlux() - this->Stats.getMiddle()) / this->Stats.getSpread() );
      }
    }  
    if(this->par.isVerbose()) bar.remove();

    if(!this->head.isWCS()){ 
      // if the WCS is bad, set the object names to Obj01 etc
      int numspaces = int(log10(this->objectList->size())) + 1;
      std::stringstream ss;
      for(size_t i=0;i<this->objectList->size();i++){
	ss.str("");
	ss << "Obj" << std::setfill('0') << std::setw(numspaces) << i+1;
	this->objectList->at(i).setName(ss.str());
      }
    }
  
  }
  //--------------------------------------------------------------------

  void Cube::calcObjectWCSparams(std::vector< std::vector<PixelInfo::Voxel> > bigVoxList)
  {
    ///  @details
    ///  A function that calculates the WCS parameters for each object in the 
    ///  Cube's list of detections.
    ///  Each object gets an ID number assigned to it (which is simply its order 
    ///   in the list), and if the WCS is good, the WCS paramters are calculated.
    /// 
    ///  This version uses vectors of Voxels to define the fluxes.
    /// 
    /// \param bigVoxList A vector of vectors of Voxels, with the same
    /// number of elements as this->objectList, where each element is a
    /// vector of Voxels corresponding to the same voxels in each
    /// detection and indicating the flux of each voxel.
  
    std::vector<Detection>::iterator obj;
    int ct=0;
    for(obj=this->objectList->begin();obj<this->objectList->end();obj++){
      obj->setID(ct+1);
      if(!obj->hasParams()){
	obj->setCentreType(this->par.getPixelCentre());
	obj->calcFluxes(bigVoxList[ct]);
	obj->calcWCSparams(this->head);
	obj->calcIntegFlux(this->axisDim[2],bigVoxList[ct],this->head);
	
	if(this->par.getFlagUserThreshold())
	  obj->setPeakSNR( obj->getPeakFlux() / this->Stats.getThreshold() );
	else
	  obj->setPeakSNR( (obj->getPeakFlux() - this->Stats.getMiddle()) / this->Stats.getSpread() );
      }
      ct++;
    }  

    if(!this->head.isWCS()){ 
      // if the WCS is bad, set the object names to Obj01 etc
      int numspaces = int(log10(this->objectList->size())) + 1;
      std::stringstream ss;
      for(size_t i=0;i<this->objectList->size();i++){
	ss.str("");
	ss << "Obj" << std::setfill('0') << std::setw(numspaces) << i+1;
	this->objectList->at(i).setName(ss.str());
      }
    }
  
  }
  //--------------------------------------------------------------------

  void Cube::calcObjectWCSparams(std::map<PixelInfo::Voxel,float> &voxelMap)
  {
    ///  @details
    ///  A function that calculates the WCS parameters for each object in the 
    ///  Cube's list of detections.
    ///  Each object gets an ID number assigned to it (which is simply its order 
    ///   in the list), and if the WCS is good, the WCS paramters are calculated.
    /// 
    ///  This version uses vectors of Voxels to define the fluxes.
    /// 
    /// \param bigVoxList A vector of vectors of Voxels, with the same
    /// number of elements as this->objectList, where each element is a
    /// vector of Voxels corresponding to the same voxels in each
    /// detection and indicating the flux of each voxel.
  
    std::vector<Detection>::iterator obj;
    int ct=0;
    for(obj=this->objectList->begin();obj<this->objectList->end();obj++){
      obj->setID(ct+1);
      if(!obj->hasParams()){
	obj->setCentreType(this->par.getPixelCentre());
	obj->calcFluxes(voxelMap);
	obj->calcWCSparams(this->head);
	obj->calcIntegFlux(this->axisDim[2],voxelMap,this->head);
	
	if(this->par.getFlagUserThreshold())
	  obj->setPeakSNR( obj->getPeakFlux() / this->Stats.getThreshold() );
	else
	  obj->setPeakSNR( (obj->getPeakFlux() - this->Stats.getMiddle()) / this->Stats.getSpread() );
      }
      ct++;
    }  

    if(!this->head.isWCS()){ 
      // if the WCS is bad, set the object names to Obj01 etc
      int numspaces = int(log10(this->objectList->size())) + 1;
      std::stringstream ss;
      for(size_t i=0;i<this->objectList->size();i++){
	ss.str("");
	ss << "Obj" << std::setfill('0') << std::setw(numspaces) << i+1;
	this->objectList->at(i).setName(ss.str());
      }
    }
  
  }
  //--------------------------------------------------------------------

  void Cube::updateDetectMap()
  {
    /// @details A function that, for each detected object in the
    ///  cube's list, increments the cube's detection map by the
    ///  required amount at each pixel. Uses
    ///  updateDetectMap(Detection).

    std::vector<Detection>::iterator obj;
    for(obj=this->objectList->begin();obj<this->objectList->end();obj++){
      this->updateDetectMap(*obj);
    }

  }
  //--------------------------------------------------------------------

  void Cube::updateDetectMap(Detection obj)
  {
    ///  @details
    ///  A function that, for the given object, increments the cube's
    ///  detection map by the required amount at each pixel.
    /// 
    ///  \param obj A Detection object that is being incorporated into the map.

    std::vector<Voxel> vlist = obj.getPixelSet();
    for(std::vector<Voxel>::iterator vox=vlist.begin();vox<vlist.end();vox++) 
      this->detectMap[vox->getX()+vox->getY()*this->axisDim[0]]++;

  }
  //--------------------------------------------------------------------

  float Cube::enclosedFlux(Detection obj)
  {
    ///  @details
    ///   A function to calculate the flux enclosed by the range
    ///    of pixels detected in the object obj (not necessarily all
    ///    pixels will have been detected).
    /// 
    ///   \param obj The Detection under consideration.

    obj.calcFluxes(this->array, this->axisDim);
    int xsize = obj.getXmax()-obj.getXmin()+1;
    int ysize = obj.getYmax()-obj.getYmin()+1;
    int zsize = obj.getZmax()-obj.getZmin()+1; 
    std::vector <float> fluxArray(xsize*ysize*zsize,0.);
    for(int x=0;x<xsize;x++){
      for(int y=0;y<ysize;y++){
	for(int z=0;z<zsize;z++){
	  fluxArray[x+y*xsize+z*ysize*xsize] = 
	    this->getPixValue(x+obj.getXmin(),
			      y+obj.getYmin(),
			      z+obj.getZmin());
	  if(this->par.getFlagNegative()) 
	    fluxArray[x+y*xsize+z*ysize*xsize] *= -1.;
	}
      }
    }
    float sum = 0.;
    for(size_t i=0;i<fluxArray.size();i++) 
      if(!this->par.isBlank(fluxArray[i])) sum+=fluxArray[i];
    return sum;
  }
  //--------------------------------------------------------------------

  void Cube::setupColumns()
  {
    /// @details
    ///  A front-end to the two setup routines in columns.cc.  
    /// 
    ///  This first gets the starting precisions, which may be from
    ///  the input parameters. It then sets up the columns (calculates
    ///  their widths and precisions and so on based on the values
    ///  within). The precisions are also stored in each Detection
    ///  object.
    /// 
    ///  Need to have called calcObjectWCSparams() somewhere
    ///  beforehand.

    std::vector<Detection>::iterator obj;
    for(obj=this->objectList->begin();obj<this->objectList->end();obj++){
      obj->setVelPrec( this->par.getPrecVel() );
      obj->setFpeakPrec( this->par.getPrecFlux() );
      obj->setXYZPrec( Column::prXYZ );
      obj->setPosPrec( Column::prWPOS );
      obj->setFintPrec( this->par.getPrecFlux() );
      obj->setSNRPrec( this->par.getPrecSNR() );
    }
  
    this->fullCols.clear();
    this->fullCols = getFullColSet(*(this->objectList), this->head);

    this->logCols.clear();
    this->logCols = getLogColSet(*(this->objectList), this->head);

    int vel,fpeak,fint,pos,xyz,snr;
    vel = fullCols[VEL].getPrecision();
    fpeak = fullCols[FPEAK].getPrecision();
    snr = fullCols[SNRPEAK].getPrecision();
    xyz = fullCols[X].getPrecision();
    xyz = std::max(xyz, fullCols[Y].getPrecision());
    xyz = std::max(xyz, fullCols[Z].getPrecision());
    if(this->head.isWCS()) fint = fullCols[FINT].getPrecision();
    else fint = fullCols[FTOT].getPrecision();
    pos = fullCols[WRA].getPrecision();
    pos = std::max(pos, fullCols[WDEC].getPrecision());
  
    for(obj=this->objectList->begin();obj<this->objectList->end();obj++){
      obj->setVelPrec(vel);
      obj->setFpeakPrec(fpeak);
      obj->setXYZPrec(xyz);
      obj->setPosPrec(pos);
      obj->setFintPrec(fint);
      obj->setSNRPrec(snr);
    }

  }
  //--------------------------------------------------------------------

  bool Cube::objAtSpatialEdge(Detection obj)
  {
    ///  @details
    ///   A function to test whether the object obj
    ///    lies at the edge of the cube's spatial field --
    ///    either at the boundary, or next to BLANKs.
    /// 
    ///   \param obj The Detection under consideration.

    bool atEdge = false;

    size_t pix = 0;
    std::vector<Voxel> voxlist = obj.getPixelSet();
    while(!atEdge && pix<voxlist.size()){
      // loop over each pixel in the object, until we find an edge pixel.
      for(int dx=-1;dx<=1;dx+=2){
	if( ((voxlist[pix].getX()+dx)<0) || 
	    ((voxlist[pix].getX()+dx)>=this->axisDim[0]) ) 
	  atEdge = true;
	else if(this->isBlank(voxlist[pix].getX()+dx,
			      voxlist[pix].getY(),
			      voxlist[pix].getZ())) 
	  atEdge = true;
      }
      for(int dy=-1;dy<=1;dy+=2){
	if( ((voxlist[pix].getY()+dy)<0) || 
	    ((voxlist[pix].getY()+dy)>=this->axisDim[1]) ) 
	  atEdge = true;
	else if(this->isBlank(voxlist[pix].getX(),
			      voxlist[pix].getY()+dy,
			      voxlist[pix].getZ())) 
	  atEdge = true;
      }
      pix++;
    }

    return atEdge;
  }
  //--------------------------------------------------------------------

  bool Cube::objAtSpectralEdge(Detection obj)
  {
    ///   @details
    ///   A function to test whether the object obj
    ///    lies at the edge of the cube's spectral extent --
    ///    either at the boundary, or next to BLANKs.
    /// 
    ///   \param obj The Detection under consideration.

    bool atEdge = false;

    size_t pix = 0;
    std::vector<Voxel> voxlist = obj.getPixelSet();
    while(!atEdge && pix<voxlist.size()){
      // loop over each pixel in the object, until we find an edge pixel.
      for(int dz=-1;dz<=1;dz+=2){
	if( ((voxlist[pix].getZ()+dz)<0) || 
	    ((voxlist[pix].getZ()+dz)>=this->axisDim[2])) 
	  atEdge = true;
	else if(this->isBlank(voxlist[pix].getX(),
			      voxlist[pix].getY(),
			      voxlist[pix].getZ()+dz)) 
	  atEdge = true;
      }
      pix++;
    }

    return atEdge;
  }
  //--------------------------------------------------------------------

  void Cube::setObjectFlags()
  {
    /// @details
    ///   A function to set any warning flags for all the detected objects
    ///    associated with the cube.
    ///   Flags to be looked for:
    ///    <ul><li> Negative enclosed flux (N)
    ///        <li> Detection at edge of field (spatially) (E)
    ///        <li> Detection at edge of spectral region (S)
    ///    </ul>

    std::vector<Detection>::iterator obj;
    for(obj=this->objectList->begin();obj<this->objectList->end();obj++){

      if( this->enclosedFlux(*obj) < 0. )  
	obj->addToFlagText("N");

      if( this->objAtSpatialEdge(*obj) ) 
	obj->addToFlagText("E");

      if( this->objAtSpectralEdge(*obj) && (this->axisDim[2] > 2)) 
	obj->addToFlagText("S");

      if(obj->getFlagText()=="") obj->addToFlagText("-");

    }

  }
  //--------------------------------------------------------------------

  /****************************************************************/
  /////////////////////////////////////////////////////////////
  //// Functions for Image class
  /////////////////////////////////////////////////////////////

  Image::Image(size_t size)
  {
    // need error handling in case size<0 !!!
    this->numPixels = this->numDim = 0;
    this->minSize = 2;
    if(size<0)
      duchampError("Image(size)","Negative size -- could not define Image");
    else{
      if(size>0 && !this->arrayAllocated){
	this->array = new float[size];
	this->arrayAllocated = true;
      }
      this->numPixels = size;
      this->axisDim = new size_t[2];
      this->axisDimAllocated = true;
      this->numDim = 2;
    }
  }
  //--------------------------------------------------------------------

  Image::Image(size_t *dimensions)
  {
    this->numPixels = this->numDim = 0;
    this->minSize = 2;
    int size = dimensions[0] * dimensions[1];
    if(size<0)
      duchampError("Image(dimArray)","Negative size -- could not define Image");
    else{
      this->numPixels = size;
      if(size>0){
	this->array = new float[size];
	this->arrayAllocated = true;
      }
      this->numDim=2;
      this->axisDim = new size_t[2];
      this->axisDimAllocated = true;
      for(int i=0;i<2;i++) this->axisDim[i] = dimensions[i];
    }
  }
  //--------------------------------------------------------------------
  Image::Image(const Image &i):
    DataArray(i)
  {
    this->operator=(i);
  }

  Image& Image::operator=(const Image &i)
  {
    if(this==&i) return *this;
    ((DataArray &) *this) = i;
    this->minSize = i.minSize;
    return *this;
  }

  //--------------------------------------------------------------------

  void Image::saveArray(float *input, size_t size)
  {
    /// @details
    /// Saves the array in input to the pixel array Image::array.
    /// The size of the array given must be the same as the current number of
    /// pixels, else an error message is returned and nothing is done.
    /// \param input The array of values to be saved.
    /// \param size The size of input.

    if(size != this->numPixels)
      duchampError("Image::saveArray",
		   "Input array different size to existing array. Cannot save.");
    else {
      if(this->numPixels>0 && this->arrayAllocated) delete [] array;
      this->numPixels = size;
      if(this->numPixels>0){
	this->array = new float[size];
	this->arrayAllocated = true;
	for(int i=0;i<size;i++) this->array[i] = input[i];
      }
    }
  }
  //--------------------------------------------------------------------

  void Image::extractSpectrum(float *Array, size_t *dim, size_t pixel)
  {
    /// @details
    ///  A function to extract a 1-D spectrum from a 3-D array.
    ///  The array is assumed to be 3-D with the third dimension the spectral one.
    ///  The spectrum extracted is the one lying in the spatial pixel referenced
    ///    by the third argument.
    ///  The extracted spectrum is stored in the pixel array Image::array.
    /// \param Array The array containing the pixel values, from which
    ///               the spectrum is extracted.
    /// \param dim The array of dimension values.
    /// \param pixel The spatial pixel that contains the desired spectrum.

    if((pixel<0)||(pixel>=dim[0]*dim[1]))
      duchampError("Image::extractSpectrum",
		   "Requested spatial pixel outside allowed range. Cannot save.");
    else if(dim[2] != this->numPixels)
      duchampError("Image::extractSpectrum",
		   "Input array different size to existing array. Cannot save.");
    else {
      if(this->numPixels>0 && this->arrayAllocated) delete [] array;
      this->numPixels = dim[2];
      if(this->numPixels>0){
	this->array = new float[dim[2]];
	this->arrayAllocated = true;
	for(int z=0;z<dim[2];z++) this->array[z] = Array[z*dim[0]*dim[1] + pixel];
      }
    }
  }
  //--------------------------------------------------------------------

  void Image::extractSpectrum(Cube &cube, size_t pixel)
  {
    /// @details
    ///  A function to extract a 1-D spectrum from a Cube class
    ///  The spectrum extracted is the one lying in the spatial pixel referenced
    ///    by the second argument.
    ///  The extracted spectrum is stored in the pixel array Image::array.
    /// \param cube The Cube containing the pixel values, from which the spectrum is extracted.
    /// \param pixel The spatial pixel that contains the desired spectrum.

    size_t zdim = cube.getDimZ();
    size_t spatSize = cube.getDimX()*cube.getDimY();
    if((pixel<0)||(pixel>=spatSize))
      duchampError("Image::extractSpectrum",
		   "Requested spatial pixel outside allowed range. Cannot save.");
    else if(zdim != this->numPixels)
      duchampError("Image::extractSpectrum",
		   "Input array different size to existing array. Cannot save.");
    else {
      if(this->numPixels>0 && this->arrayAllocated) delete [] array;
      this->numPixels = zdim;
      if(this->numPixels>0){
	this->array = new float[zdim];
	this->arrayAllocated = true;
	for(int z=0;z<zdim;z++) 
	  this->array[z] = cube.getPixValue(z*spatSize + pixel);
      }
    }
  }
  //--------------------------------------------------------------------

  void Image::extractImage(float *Array, size_t *dim, size_t channel)
  {
    /// @details
    ///  A function to extract a 2-D image from a 3-D array.
    ///  The array is assumed to be 3-D with the third dimension the spectral one.
    ///  The dimensions of the array are in the dim[] array.
    ///  The image extracted is the one lying in the channel referenced
    ///    by the third argument.
    ///  The extracted image is stored in the pixel array Image::array.
    /// \param Array The array containing the pixel values, from which the image is extracted.
    /// \param dim The array of dimension values.
    /// \param channel The spectral channel that contains the desired image.

    size_t spatSize = dim[0]*dim[1];
    if((channel<0)||(channel>=dim[2]))
      duchampError("Image::extractImage",
		   "Requested channel outside allowed range. Cannot save.");
    else if(spatSize != this->numPixels)
      duchampError("Image::extractImage",
		   "Input array different size to existing array. Cannot save.");
    else {
      if(this->numPixels>0 && this->arrayAllocated) delete [] array;
      this->numPixels = spatSize;
      if(this->numPixels>0){
	this->array = new float[spatSize];
	this->arrayAllocated = true;
	for(int npix=0; npix<spatSize; npix++)
	  this->array[npix] = Array[channel*spatSize + npix];
      }
    }
  }
  //--------------------------------------------------------------------

  void Image::extractImage(Cube &cube, size_t channel)
  {
    /// @details
    ///  A function to extract a 2-D image from Cube class.
    ///  The image extracted is the one lying in the channel referenced
    ///    by the second argument.
    ///  The extracted image is stored in the pixel array Image::array.
    /// \param cube The Cube containing the pixel values, from which the image is extracted.
    /// \param channel The spectral channel that contains the desired image.

    size_t spatSize = cube.getDimX()*cube.getDimY();
    if((channel<0)||(channel>=cube.getDimZ()))
      duchampError("Image::extractImage",
		   "Requested channel outside allowed range. Cannot save.");
    else if(spatSize != this->numPixels)
      duchampError("Image::extractImage",
		   "Input array different size to existing array. Cannot save.");
    else {
      if(this->numPixels>0 && this->arrayAllocated) delete [] array;
      this->numPixels = spatSize;
      if(this->numPixels>0){
	this->array = new float[spatSize];
	this->arrayAllocated = true;
	for(int npix=0; npix<spatSize; npix++) 
	  this->array[npix] = cube.getPixValue(channel*spatSize + npix);
      }
    }
  }
  //--------------------------------------------------------------------

  void Image::removeMW()
  {
    /// @details
    ///  A function to remove the Milky Way range of channels from a 1-D spectrum.
    ///  The array in this Image is assumed to be 1-D, with only the first axisDim
    ///    equal to 1.
    ///  The values of the MW channels are set to 0, unless they are BLANK.

    if(this->par.getFlagMW() && (this->axisDim[1]==1) ){
      for(int z=0;z<this->axisDim[0];z++){
	if(!this->isBlank(z) && this->par.isInMW(z)) this->array[z]=0.;
      }
    }
  }
  //--------------------------------------------------------------------

  std::vector<Object2D> Image::findSources2D() 
  {
    std::vector<bool> thresholdedArray(this->axisDim[0]*this->axisDim[1]);
    for(int posY=0;posY<this->axisDim[1];posY++){
      for(int posX=0;posX<this->axisDim[0];posX++){
	int loc = posX + this->axisDim[0]*posY;
	thresholdedArray[loc] = this->isDetection(posX,posY);
      }
    }
    return lutz_detect(thresholdedArray, this->axisDim[0], this->axisDim[1], this->minSize);
  }

  std::vector<Scan> Image::findSources1D() 
  {
    std::vector<bool> thresholdedArray(this->axisDim[0]);
    for(int posX=0;posX<this->axisDim[0];posX++){
      thresholdedArray[posX] = this->isDetection(posX,0);
    }
    return spectrumDetect(thresholdedArray, this->axisDim[0], this->minSize);
  }


  std::vector< std::vector<PixelInfo::Voxel> > Cube::getObjVoxList()
  {
    
    std::vector< std::vector<PixelInfo::Voxel> > biglist;
    
    std::vector<Detection>::iterator obj;
    for(obj=this->objectList->begin(); obj<this->objectList->end(); obj++) {

      Cube *subcube = new Cube;
      subcube->pars() = this->par;
      subcube->pars().setVerbosity(false);
      subcube->pars().setFlagSubsection(true);
      duchamp::Section sec = obj->getBoundingSection();
      subcube->pars().setSubsection( sec.getSection() );
      if(subcube->pars().verifySubsection() == FAILURE)
	duchampError("get object voxel list","Unable to verify the subsection - something's wrong!");
      if(subcube->getCube() == FAILURE)
	duchampError("get object voxel list","Unable to read the FITS file - something's wrong!");
      std::vector<PixelInfo::Voxel> voxlist = obj->getPixelSet();
      std::vector<PixelInfo::Voxel>::iterator vox;
      for(vox=voxlist.begin(); vox<voxlist.end(); vox++){
	size_t pix = (vox->getX()-subcube->pars().getXOffset()) +
	  subcube->getDimX()*(vox->getY()-subcube->pars().getYOffset()) +
	  subcube->getDimX()*subcube->getDimY()*(vox->getZ()-subcube->pars().getZOffset());
	vox->setF( subcube->getPixValue(pix) );
      }
      biglist.push_back(voxlist);
      delete subcube;

    }

    return biglist;

  }

}
