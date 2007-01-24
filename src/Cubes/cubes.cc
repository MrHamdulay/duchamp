#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <string>
#include <math.h>

#include <wcs.h>

#include <duchamp.hh>
#include <param.hh>
#include <Cubes/cubes.hh>
#include <Detection/detection.hh>
#include <Detection/columns.hh>
#include <Utils/utils.hh>
#include <Utils/mycpgplot.hh>
#include <Utils/Statistics.hh>
using namespace Column;
using namespace mycpgplot;
using namespace Statistics;

/****************************************************************/
///////////////////////////////////////////////////
//// Functions for DataArray class:
///////////////////////////////////////////////////

DataArray::DataArray(){
  /**
   * Fundamental constructor for DataArray.
   * Number of dimensions and pixels are set to 0. Nothing else allocated.
   */
  this->numDim=0; 
  this->numPixels=0;
}; 
//--------------------------------------------------------------------

DataArray::DataArray(short int nDim){
  /**
   * N-dimensional constructor for DataArray.
   * Number of dimensions defined, and dimension array allocated.
   * Number of pixels are set to 0. 
   * \param nDim Number of dimensions.
   */
  if(nDim>0) this->axisDim = new long[nDim];
  this->numDim=nDim; 
  this->numPixels=0;
}; 
//--------------------------------------------------------------------

DataArray::DataArray(short int nDim, long size){
  /**
   * N-dimensional constructor for DataArray.
   * Number of dimensions and number of pixels defined. 
   * Arrays allocated based on these values.
   * \param nDim Number of dimensions. 
   * \param size Number of pixels. 
   *
   * Note that we can assign values to the dimension array.
   */

  if(size<0)
    duchampError("DataArray(nDim,size)",
		 "Negative size -- could not define DataArray");
  else if(nDim<0)
    duchampError("DataArray(nDim,size)",
		 "Negative number of dimensions: could not define DataArray");
  else {
    if(size>0) this->array = new float[size];
    this->numPixels = size;
    if(nDim>0) this->axisDim = new long[nDim];
    this->numDim = nDim;
  }
}
//--------------------------------------------------------------------

DataArray::DataArray(short int nDim, long *dimensions)
{
  /**
   * Most robust constructor for DataArray.
   * Number and sizes of dimensions are defined, and hence the number of 
   * pixels. Arrays allocated based on these values.
   * \param nDim Number of dimensions. 
   * \param dimensions Array giving sizes of dimensions.
   */
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
      if(size>0) this->array = new float[size];
      this->numDim=nDim;
      if(nDim>0) this->axisDim = new long[nDim];
      for(int i=0;i<nDim;i++) this->axisDim[i] = dimensions[i];
    }
  }
}
//--------------------------------------------------------------------

DataArray::~DataArray()
{
  /** 
   *  Destructor -- arrays deleted if they have been allocated, and the 
   *   object list is cleared.
   */
  if(this->numPixels>0) delete [] this->array;
  if(this->numDim>0)    delete [] this->axisDim;
  this->objectList.clear();
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------

void DataArray::getDim(long &x, long &y, long &z){
  /**
   * The sizes of the first three dimensions (if they exist) are returned.
   * \param x The first dimension. Defaults to 0 if numDim \f$\le\f$ 0.
   * \param y The second dimension. Defaults to 1 if numDim \f$\le\f$ 1.
   * \param z The third dimension. Defaults to 1 if numDim \f$\le\f$ 2.
   */
  if(this->numDim>0) x=this->axisDim[0]; 
  else x=0;
  if(this->numDim>1) y=this->axisDim[1]; 
  else y=1;
  if(this->numDim>2) z=this->axisDim[2]; 
  else z=1;
}
//--------------------------------------------------------------------

void DataArray::getDimArray(long *output){
  /**
   * The axisDim array is written to output. This needs to be defined 
   *  beforehand: no checking is done on the memory.
   * \param output The array that is written to.
   */
  for(int i=0;i<this->numDim;i++) output[i] = this->axisDim[i];
}
//--------------------------------------------------------------------

void DataArray::getArray(float *output){
  /**
   * The pixel value array is written to output. This needs to be defined 
   *  beforehand: no checking is done on the memory.
   * \param output The array that is written to.
   */
  for(int i=0;i<this->numPixels;i++) output[i] = this->array[i];
}
//--------------------------------------------------------------------

void DataArray::saveArray(float *input, long size){
  /**
   * Saves the array in input to the pixel array DataArray::array.
   * The size of the array given must be the same as the current number of
   * pixels, else an error message is returned and nothing is done.
   * \param input The array of values to be saved.
   * \param size The size of input.
   */
  if(size != this->numPixels)
    duchampError("DataArray::saveArray",
		 "Input array different size to existing array. Cannot save.");
  else {
    if(this->numPixels>0) delete [] this->array;
    this->numPixels = size;
    this->array = new float[size];
    for(int i=0;i<size;i++) this->array[i] = input[i];
  }
}
//--------------------------------------------------------------------

void DataArray::addObject(Detection object){ 
  /**
   * \param object The object to be added to the object list.
   */
  // objectList is a vector, so just use push_back()
  this->objectList.push_back(object);
}
//--------------------------------------------------------------------

void DataArray::addObjectList(vector <Detection> newlist) {
  /**
   * \param newlist The list of objects to be added to the object list.
   */
  for(int i=0;i<newlist.size();i++) this->objectList.push_back(newlist[i]);
}
//--------------------------------------------------------------------

void DataArray::addObjectOffsets(){
  /**
   * Add the pixel offsets (that is, offsets from the corner of the cube to the
   *  corner of the utilised part) that are stored in the Param set to the
   *  coordinate values of each object in the object list.
   */
  for(int i=0;i<this->objectList.size();i++){
    for(int p=0;p<this->objectList[i].getSize();p++){
      this->objectList[i].setX(p,this->objectList[i].getX(p)+
			       this->par.getXOffset());
      this->objectList[i].setY(p,this->objectList[i].getY(p)+
			       this->par.getYOffset());
      this->objectList[i].setZ(p,this->objectList[i].getZ(p)+
			       this->par.getZOffset());
    }
  }
}
//--------------------------------------------------------------------

bool DataArray::isDetection(float value){
  /** 
   * Is a given value a detection, based on the statistics in the 
   * DataArray's StatsContainer? 
   * \param value The pixel value to test.
   */
  if(par.isBlank(value)) return false;
  else return Stats.isDetection(value);
};  
//--------------------------------------------------------------------

bool DataArray::isDetection(long voxel){
  /** 
   * Is a given pixel a detection, based on the statistics in the 
   * DataArray's StatsContainer? 
   * If the pixel lies outside the valid range for the data array, return false.
   * \param voxel Location of the DataArray's pixel to be tested.
   */
  if((voxel<0)||(voxel>this->numPixels)) return false;
  else if(par.isBlank(this->array[voxel])) return false;
  else return Stats.isDetection(this->array[voxel]);
};  
//--------------------------------------------------------------------

std::ostream& operator<< ( std::ostream& theStream, DataArray &array)
{
  /**
   * A way to print out the pixel coordinates & flux values of the 
   * list of detected objects belonging to the DataArray.
   * These are formatted nicely according to the << operator for Detection,
   *  with a line indicating the number of detections at the start.
   * \param theStream The ostream object to which the output should be sent.
   * \param array The DataArray containing the list of objects.
   */
  for(int i=0;i<array.numDim;i++){
    if(i>0) theStream<<"x";
    theStream<<array.axisDim[i];
  }
  theStream<<std::endl;
  theStream<<array.objectList.size()<<" detections:\n--------------\n";
  for(int i=0;i<array.objectList.size();i++){
    theStream << "Detection #" << array.objectList[i].getID()<<std::endl;
    Detection *obj = new Detection;
    *obj = array.objectList[i];
    for(int j=0;j<obj->getSize();j++){
      obj->setX(j,obj->getX(j)+obj->getXOffset());
      obj->setY(j,obj->getY(j)+obj->getYOffset());
      obj->setZ(j,obj->getZ(j)+obj->getZOffset());
    }
    theStream<<*obj;
    delete obj;
  }
  theStream<<"--------------\n";
}

/****************************************************************/
/////////////////////////////////////////////////////////////
//// Functions for Cube class
/////////////////////////////////////////////////////////////

Cube::Cube(){
  /**
   * Basic Constructor for Cube class.
   * numDim set to 3, but numPixels to 0 and all bool flags to false.
   * No allocation done.
   */
    numPixels=0; numDim=3; 
    reconExists = false; reconAllocated = false; baselineAllocated = false;
  };
//--------------------------------------------------------------------

Cube::Cube(long size){
  /**
   * Alternative Cube constructor, where size is given but not individual
   *  dimensions. Arrays are allocated as appropriate (according to the 
   *  relevant flags in Param set), but the Cube::axisDim array is not.
   */
  this->reconAllocated = false;
  this->baselineAllocated = false;
  this->numPixels = this->numDim = 0;
  if(size<0)
    duchampError("Cube(size)","Negative size -- could not define Cube");
  else{
    if(size>0){
      this->array = new float[size];
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
    this->axisDim = new long[2];
    this->numDim = 3;
    this->reconExists = false;
  }
}
//--------------------------------------------------------------------

Cube::Cube(long *dimensions){
  /**
   * Alternative Cube constructor, where sizes of dimensions are given. 
   * Arrays are allocated as appropriate (according to the 
   *  relevant flags in Param set), as is the Cube::axisDim array.
   */
  int size   = dimensions[0] * dimensions[1] * dimensions[2];
  int imsize = dimensions[0] * dimensions[1];
  this->reconAllocated = false;
  this->baselineAllocated = false;
  this->numPixels = this->numDim = 0;
  if((size<0) || (imsize<0) )
    duchampError("Cube(dimArray)","Negative size -- could not define Cube");
  else{
    this->numPixels = size;
    if(size>0){
      this->array      = new float[size];
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
    this->axisDim = new long[3];
    for(int i=0;i<3     ;i++) this->axisDim[i]   = dimensions[i];
    for(int i=0;i<imsize;i++) this->detectMap[i] = 0;
    this->reconExists = false;
  }
}
//--------------------------------------------------------------------

Cube::~Cube()
{
  /**
   *  The destructor deletes the memory allocated for Cube::detectMap, and,
   *  if these have been allocated, Cube::recon and Cube::baseline.
   */
  delete [] this->detectMap;
  if(this->reconAllocated)    delete [] this->recon;
  if(this->baselineAllocated) delete [] this->baseline;
}
//--------------------------------------------------------------------

void Cube::initialiseCube(long *dimensions)
{
  /**
   *  This function will set the sizes of all arrays that will be used by Cube.
   *  It will also define the values of the axis dimensions: this will be done 
   *   using the WCS in the FitsHeader class, so the WCS needs to be good and 
   *   have three axes. If this is not the case, the axes are assumed to be 
   *   ordered in the sense of lng,lat,spc.
   *
   *  \param dimensions An array of values giving the dimensions (sizes) for 
   *  all axes.  
   */ 

  int lng,lat,spc,size,imsize;

  if(this->head.isWCS() && (this->head.getWCS()->naxis>=3)){
    // if there is a WCS and there is at least 3 axes
    lng = this->head.getWCS()->lng;
    lat = this->head.getWCS()->lat;
    spc = this->head.getWCS()->spec;
  }
  else{
    // just take dimensions[] at face value
    lng = 0;
    lat = 1;
    spc = 2;
  }
  size   = dimensions[lng] * dimensions[lat] * dimensions[spc];
  imsize = dimensions[lng] * dimensions[lat];

  this->reconAllocated = false;
  this->baselineAllocated = false;

  if((size<0) || (imsize<0) )
    duchampError("Cube::initialiseCube(dimArray)",
		 "Negative size -- could not define Cube.\n");
  else{
    this->numPixels = size;
    if(size>0){
      this->array      = new float[size];
      this->detectMap  = new short[imsize];
      if(this->par.getFlagATrous() || this->par.getFlagSmooth()){
	this->recon    = new float[size];
	this->reconAllocated = true;
      }
      if(this->par.getFlagBaseline()){
	this->baseline = new float[size];
	this->baselineAllocated = true;
      }
    }
    this->numDim  = 3;
    this->axisDim = new long[3];
    this->axisDim[0] = dimensions[lng];
    this->axisDim[1] = dimensions[lat];
    this->axisDim[2] = dimensions[spc];
    for(int i=0;i<imsize;i++) this->detectMap[i] = 0;
    this->reconExists = false;
  }
}
//--------------------------------------------------------------------

int Cube::getCube(){  
    /** 
     * A front-end to the Cube::getCube() function, that does 
     *  subsection checks.
     * Assumes the Param is set up properly.
     */
    string fname = par.getImageFile();
    if(par.getFlagSubsection()) fname+=par.getSubsection();
    return getCube(fname);
  };
//--------------------------------------------------------------------

int Cube::getopts(int argc, char ** argv)
{
  /**  
   *   A function that reads in the command-line options, in a manner 
   *    tailored for use with the main Duchamp program.
   *   Based on the options given, the appropriate Param set will be read
   *    in to the Cube class.
   *
   *   \param argc The number of command line arguments.
   *   \param argv The array of command line arguments.
   */

  int returnValue;
  if(argc==1){
    std::cout << ERR_USAGE_MSG;
    returnValue = FAILURE;
  }
  else {
    string file;
    Param *par = new Param;
    char c;
    while( ( c = getopt(argc,argv,"p:f:hv") )!=-1){
      switch(c) {
      case 'p':
	file = optarg;
	if(this->readParam(file)==FAILURE){
	  stringstream errmsg;
	  errmsg << "Could not open parameter file " << file << ".\n";
	  duchampError("Duchamp",errmsg.str());
	  returnValue = FAILURE;
	}
	else returnValue = SUCCESS;
	break;
      case 'f':
	file = optarg;
	par->setImageFile(file);
	this->saveParam(*par);
	returnValue = SUCCESS;
	break;
      case 'v':
	std::cout << PROGNAME << " version " << VERSION << std::endl;
	returnValue = FAILURE;
	break;
      case 'h':
      default :
	std::cout << ERR_USAGE_MSG;
	returnValue = FAILURE;
	break;
      }
    }
    delete par;
  }
  return returnValue;
}
//--------------------------------------------------------------------

void Cube::saveArray(float *input, long size){
  if(size != this->numPixels){
    stringstream errmsg;
    errmsg << "Input array different size to existing array ("
	   << size << " cf. " << this->numPixels << "). Cannot save.\n";
    duchampError("Cube::saveArray",errmsg.str());
  }
  else {
    if(this->numPixels>0) delete [] array;
    this->numPixels = size;
    this->array = new float[size];
    for(int i=0;i<size;i++) this->array[i] = input[i];
  }
}
//--------------------------------------------------------------------

void Cube::saveRecon(float *input, long size){
  /**
   * Saves the array in input to the reconstructed array Cube::recon
   * The size of the array given must be the same as the current number of
   * pixels, else an error message is returned and nothing is done.
   * If the recon array has already been allocated, it is deleted first, and 
   * then the space is allocated.
   * Afterwards, the appropriate flags are set.
   * \param input The array of values to be saved.
   * \param size The size of input.
   */
  if(size != this->numPixels){
    stringstream errmsg;
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

void Cube::getRecon(float *output){
  /**
   * The reconstructed array is written to output. The output array needs to 
   *  be defined beforehand: no checking is done on the memory.
   * \param output The array that is written to.
   */
  // Need check for change in number of pixels!
  for(int i=0;i<this->numPixels;i++){
    if(this->reconExists) output[i] = this->recon[i];
    else output[i] = 0.;
  }
}
//--------------------------------------------------------------------

void Cube::removeMW()
{
  /**
   * The channels corresponding to the Milky Way range (as given by the Param
   *  set) are all set to 0 in the pixel array.
   * Only done if the appropriate flag is set, and the pixels are not BLANK.
   * \deprecated
   */ 
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

void Cube::setCubeStatsOld()
{
  /**  
   *   \deprecated
   *
   *   Calculates the full statistics for the cube:  mean, rms, median, madfm.
   *   Only do this if the threshold has not been defined (ie. is still 0.,
   *    its default). 
   *   Also work out the threshold and store it in the Param set.
   *   
   *   For the stats calculations, we ignore BLANKs and MW channels.
   */

  if(!this->par.getFlagFDR() && this->par.getFlagUserThreshold() ){
    // if the user has defined a threshold, set this in the StatsContainer
    this->Stats.setThreshold( this->par.getThreshold() );
  }
  else{
    // only work out the mean etc if we need to.
    // the only reason we don't is if the user has specified a threshold.
    
    std::cout << "Calculating the cube statistics... " << std::flush;
    
    // get number of good pixels;
    int goodSize = 0;
    for(int p=0;p<this->axisDim[0]*this->axisDim[1];p++){
      for(int z=0;z<this->axisDim[2];z++){
	int vox = z * this->axisDim[0] * this->axisDim[1] + p;
	if(!this->isBlank(vox) && !this->par.isInMW(z)) goodSize++;
      }
    }

    float *tempArray = new float[goodSize];

    goodSize=0;
    for(int p=0;p<this->axisDim[0]*this->axisDim[1];p++){
      for(int z=0;z<this->axisDim[2];z++){
	int vox = z * this->axisDim[0] * this->axisDim[1] + p;
	if(!this->isBlank(vox) && !this->par.isInMW(z))
	  tempArray[goodSize++] = this->array[vox];
      }
    }
    if(!this->reconExists){
      // if there's no recon array, calculate everything from orig array
      this->Stats.calculate(tempArray,goodSize);
    }
    else{
      // just get mean & median from orig array, and rms & madfm from recon
      StatsContainer<float> origStats,reconStats;
      origStats.calculate(tempArray,goodSize);
      goodSize=0;
      for(int p=0;p<this->axisDim[0]*this->axisDim[1];p++){
	for(int z=0;z<this->axisDim[2];z++){
	  int vox = z * this->axisDim[0] * this->axisDim[1] + p;
	  if(!this->isBlank(vox) && !this->par.isInMW(z))
	    tempArray[goodSize++] = this->array[vox] - this->recon[vox];
	}
      }
      reconStats.calculate(tempArray,goodSize);

      // Get the "middle" estimators from the original array.
      this->Stats.setMean(origStats.getMean());
      this->Stats.setMedian(origStats.getMedian());
      // Get the "spread" estimators from the residual (orig-recon) array
      this->Stats.setStddev(reconStats.getStddev());
      this->Stats.setMadfm(reconStats.getMadfm());
    }

    delete [] tempArray;

    this->Stats.setUseFDR( this->par.getFlagFDR() );
    // If the FDR method has been requested
    if(this->par.getFlagFDR())  this->setupFDR();
    else{
      // otherwise, calculate one based on the requested SNR cut level, and 
      //   then set the threshold parameter in the Par set.
      this->Stats.setThresholdSNR( this->par.getCut() );
      this->par.setThreshold( this->Stats.getThreshold() );
    }
    
    
  }
  std::cout << "Using ";
  if(this->par.getFlagFDR()) std::cout << "effective ";
  std::cout << "flux threshold of: ";
  float thresh = this->Stats.getThreshold();
  if(this->par.getFlagNegative()) thresh *= -1.;
  std::cout << thresh << std::endl;

}
//--------------------------------------------------------------------

void Cube::setCubeStats()
{
  /**  
   *   Calculates the full statistics for the cube:
   *     mean, rms, median, madfm
   *   Only do this if the threshold has not been defined (ie. is still 0.,
   *    its default). 
   *   Also work out the threshold and store it in the par set.
   *   
   *   Different from Cube::setCubeStatsOld() as it doesn't use the 
   *    getStats functions but has own versions of them hardcoded to 
   *    ignore BLANKs and MW channels. This saves on memory usage -- necessary
   *    for dealing with very big files.
   */

  if(!this->par.getFlagFDR() && this->par.getFlagUserThreshold() ){
    // if the user has defined a threshold, set this in the StatsContainer
    this->Stats.setThreshold( this->par.getThreshold() );
  }
  else{
    // only work out the mean etc if we need to.
    // the only reason we don't is if the user has specified a threshold.
    
    std::cout << "Calculating the cube statistics... " << std::flush;
    
    long xysize = this->axisDim[0]*this->axisDim[1];

    // get number of good pixels;
    int vox,goodSize = 0;
    for(int p=0;p<xysize;p++){
      for(int z=0;z<this->axisDim[2];z++){
	vox = z*xysize+p;
	if(!this->isBlank(vox) && !this->par.isInMW(z)) goodSize++;
      }
    }

    float *tempArray = new float[goodSize];
    
    goodSize=0;
    for(int p=0;p<xysize;p++){
      for(int z=0;z<this->axisDim[2];z++){
	vox = z * xysize + p;
	if(!this->isBlank(vox) && !this->par.isInMW(z)){
	  tempArray[goodSize] = this->array[vox];
	  goodSize++;
	}
      }
    }

    float mean,median,stddev,madfm;
      mean = tempArray[0];
      for(int i=1;i<goodSize;i++) mean += tempArray[i];
      mean /= float(goodSize);
      mean = findMean(tempArray,goodSize);
      this->Stats.setMean(mean);

      std::sort(tempArray,tempArray+goodSize);
      if((goodSize%2)==0) 
	median = (tempArray[goodSize/2-1] + tempArray[goodSize/2])/2;
      else median = tempArray[goodSize/2];
      this->Stats.setMedian(median);

   
    if(!this->reconExists){
      // if there's no recon array, calculate everything from orig array
      stddev = (tempArray[0]-mean) * (tempArray[0]-mean);
      for(int i=1;i<goodSize;i++) 
	stddev += (tempArray[i]-mean)*(tempArray[i]-mean);
      stddev = sqrt(stddev/float(goodSize-1));
      this->Stats.setStddev(stddev);

      for(int i=0;i<goodSize;i++)// tempArray[i] = absval(tempArray[i]-median);
	{
	  if(tempArray[i]>median) tempArray[i] -= median;
	  else tempArray[i] = median - tempArray[i];
	}
      std::sort(tempArray,tempArray+goodSize);
      if((goodSize%2)==0) 
	madfm = (tempArray[goodSize/2-1]+tempArray[goodSize/2])/2;
      else madfm = tempArray[goodSize/2];
      this->Stats.setMadfm(madfm);
    }
    else{
      // just get mean & median from orig array, and rms & madfm from residual
      // recompute array values to be residuals & then find stddev & madfm
      goodSize = 0;
      for(int p=0;p<xysize;p++){
	for(int z=0;z<this->axisDim[2];z++){
	  vox = z * xysize + p;
	  if(!this->isBlank(vox) && !this->par.isInMW(z)){
	    tempArray[goodSize] = this->array[vox] - this->recon[vox];
	    goodSize++;
	  }
	}
      }
      mean = tempArray[0];
      for(int i=1;i<goodSize;i++) mean += tempArray[i];
      mean /= float(goodSize);
      stddev = (tempArray[0]-mean) * (tempArray[0]-mean);
      for(int i=1;i<goodSize;i++) 
	stddev += (tempArray[i]-mean)*(tempArray[i]-mean);
      stddev = sqrt(stddev/float(goodSize-1));
      this->Stats.setStddev(stddev);

      std::sort(tempArray,tempArray+goodSize);
      if((goodSize%2)==0) 
	median = (tempArray[goodSize/2-1] + tempArray[goodSize/2])/2;
      else median = tempArray[goodSize/2];
      for(int i=0;i<goodSize;i++){
	if(tempArray[i]>median) tempArray[i] = tempArray[i]-median;
	else tempArray[i] = median - tempArray[i];
      }
      std::sort(tempArray,tempArray+goodSize);
      if((goodSize%2)==0) 
	madfm = (tempArray[goodSize/2-1] + tempArray[goodSize/2])/2;
      else madfm = tempArray[goodSize/2];
      this->Stats.setMadfm(madfm);
    }

    delete [] tempArray;

    this->Stats.setUseFDR( this->par.getFlagFDR() );
    // If the FDR method has been requested
    if(this->par.getFlagFDR())  this->setupFDR();
    else{
      // otherwise, calculate threshold based on the requested SNR cut level,
      //  and then set the threshold parameter in the Par set.
      this->Stats.setThresholdSNR( this->par.getCut() );
      this->par.setThreshold( this->Stats.getThreshold() );
    }
    
  }

  std::cout << "Using ";
  if(this->par.getFlagFDR()) std::cout << "effective ";
  std::cout << "flux threshold of: ";
  float thresh = this->Stats.getThreshold();
  if(this->par.getFlagNegative()) thresh *= -1.;
  std::cout << thresh << std::endl;

}
//--------------------------------------------------------------------

int Cube::setupFDR()
{
  /**  
   *   Determines the critical Probability value for the False Discovery Rate
   *    detection routine. All pixels with Prob less than this value will
   *    be considered detections.
   *
   *   The Prob here is the probability, assuming a Normal distribution, of
   *    obtaining a value as high or higher than the pixel value (ie. only the
   *    positive tail of the PDF)
   */

  // first calculate p-value for each pixel -- assume Gaussian for now.

  float *orderedP = new float[this->numPixels];
  int count = 0;
  float zStat;
  for(int pix=0; pix<this->numPixels; pix++){

    if( !(this->par.isBlank(this->array[pix])) ){ 
      // only look at non-blank pixels
      zStat = (this->array[pix] - this->Stats.getMiddle()) / 
	this->Stats.getSpread();
      
      orderedP[count++] = 0.5 * erfc(zStat/M_SQRT2);
      // Need the factor of 0.5 here, as we are only considering the positive 
      //  tail of the distribution. Don't care about negative detections.
    }
  }

  // now order them 
  std::sort(orderedP,orderedP+count);
  
  // now find the maximum P value.
  int max = 0;
  float cN = 0.;
  int psfCtr;
  int numVox = int(this->par.getBeamSize()) * 2;
  // why beamSize*2? we are doing this in 3D, so spectrally assume just the
  //  neighbouring channels are correlated, but spatially all those within
  //  the beam, so total number of voxels is 2*beamSize
  for(psfCtr=1;psfCtr<=numVox;(psfCtr)++) 
    cN += 1./float(psfCtr);

  for(int loopCtr=0;loopCtr<count;loopCtr++) {
    if( orderedP[loopCtr] < 
	(double(loopCtr+1)*this->par.getAlpha()/(cN * double(count))) ) {
      max = loopCtr;
    }
  }

  this->Stats.setPThreshold( orderedP[max] );

  delete [] orderedP;

  // Find real value of the P threshold by finding the inverse of the 
  //  error function -- root finding with brute force technique 
  //  (relatively slow, but we only do it once).
  zStat = 0;
  float deltaZ = 0.1;
  float tolerance = 1.e-6;
  float zeroP = 0.5 * erfc(zStat/M_SQRT2) - this->Stats.getPThreshold();
  do{
    zStat+=deltaZ;
    if((zeroP * (erfc(zStat/M_SQRT2)-this->Stats.getPThreshold()))<0.){
      zStat-=deltaZ;
      deltaZ/=2.;
    }
  }while(deltaZ>tolerance);
  this->Stats.setThreshold( zStat*this->Stats.getSpread() + 
			    this->Stats.getMiddle() );

}
//--------------------------------------------------------------------

bool Cube::isDetection(long x, long y, long z)
{
  /** 
   * Is a given voxel at position (x,y,z) a detection, based on the statistics
   *  in the Cube's StatsContainer? 
   * If the pixel lies outside the valid range for the data array, return false.
   * \param x X-value of the Cube's voxel to be tested.
   * \param y Y-value of the Cube's voxel to be tested.
   * \param z Z-value of the Cube's voxel to be tested.
   */
    long voxel = z*axisDim[0]*axisDim[1] + y*axisDim[0] + x;
    return DataArray::isDetection(array[voxel]);
  };
//--------------------------------------------------------------------

void Cube::calcObjectWCSparams()
{
  /** 
   *  A function that calculates the WCS parameters for each object in the 
   *  Cube's list of detections.
   *  Each object gets an ID number assigned to it (which is simply its order 
   *   in the list), and if the WCS is good, the WCS paramters are calculated.
   */
  
  for(int i=0;i<this->objectList.size();i++){
    this->objectList[i].setID(i+1);
    this->objectList[i].calcWCSparams(this->head);
    this->objectList[i].setPeakSNR( (this->objectList[i].getPeakFlux() - this->Stats.getMiddle()) / this->Stats.getSpread() );
  }  

  if(!this->head.isWCS()){ 
    // if the WCS is bad, set the object names to Obj01 etc
    int numspaces = int(log10(this->objectList.size())) + 1;
    stringstream ss;
    for(int i=0;i<this->objectList.size();i++){
      ss.str("");
      ss << "Obj" << std::setfill('0') << std::setw(numspaces) << i+1;
      this->objectList[i].setName(ss.str());
    }
  }
  
}
//--------------------------------------------------------------------

void Cube::updateDetectMap()
{
  /**
   *  A function that, for each detected object in the cube's list, increments 
   *   the cube's detection map by the required amount at each pixel.
   */

  for(int obj=0;obj<this->objectList.size();obj++){
    for(int pix=0;pix<this->objectList[obj].getSize();pix++) {
      int spatialPos = this->objectList[obj].getX(pix)+
	this->objectList[obj].getY(pix)*this->axisDim[0];
      this->detectMap[spatialPos]++;
    }
  }
}
//--------------------------------------------------------------------

void Cube::updateDetectMap(Detection obj)
{
  /** 
   *  A function that, for the given object, increments the cube's
   *  detection map by the required amount at each pixel.
   * 
   *  \param obj A Detection object that is being incorporated into the map.
   */
  for(int pix=0;pix<obj.getSize();pix++) {
    int spatialPos = obj.getX(pix)+obj.getY(pix)*this->axisDim[0];
    this->detectMap[spatialPos]++;
  }
}
//--------------------------------------------------------------------

float Cube::enclosedFlux(Detection obj)
{
  /** 
   *   A function to calculate the flux enclosed by the range
   *    of pixels detected in the object obj (not necessarily all
   *    pixels will have been detected).
   *
   *   \param obj The Detection under consideration.
   */
  obj.calcParams();
  int xsize = obj.getXmax()-obj.getXmin()+1;
  int ysize = obj.getYmax()-obj.getYmin()+1;
  int zsize = obj.getZmax()-obj.getZmin()+1; 
  vector <float> fluxArray(xsize*ysize*zsize,0.);
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
  for(int i=0;i<fluxArray.size();i++) 
    if(!this->par.isBlank(fluxArray[i])) sum+=fluxArray[i];
  return sum;
}
//--------------------------------------------------------------------

void Cube::setupColumns()
{
  /** 
   *   A front-end to the two setup routines in columns.cc.
   *   This first calculates the WCS parameters for all objects, then
   *    sets up the columns (calculates their widths and precisions and so on).
   *   The precisions are also stored in each Detection object.
   */ 
  this->calcObjectWCSparams();  
  // need this as the colSet functions use vel, RA, Dec, etc...
  
  this->fullCols.clear();
  this->fullCols = getFullColSet(this->objectList, this->head);

  this->logCols.clear();
  this->logCols = getLogColSet(this->objectList, this->head);

  int vel,fpeak,fint,pos,xyz,temp,snr;
  vel = fullCols[VEL].getPrecision();
  fpeak = fullCols[FPEAK].getPrecision();
  snr = fullCols[SNRPEAK].getPrecision();
  xyz = fullCols[X].getPrecision();
  if(temp=fullCols[Y].getPrecision() > xyz) xyz = temp;
  if(temp=fullCols[Z].getPrecision() > xyz) xyz = temp;
  if(this->head.isWCS()) fint = fullCols[FINT].getPrecision();
  else fint = fullCols[FTOT].getPrecision();
  pos = fullCols[WRA].getPrecision();
  if(temp=fullCols[WDEC].getPrecision() > pos) pos = temp;
  
  for(int obj=0;obj<this->objectList.size();obj++){
    this->objectList[obj].setVelPrec(vel);
    this->objectList[obj].setFpeakPrec(fpeak);
    this->objectList[obj].setXYZPrec(xyz);
    this->objectList[obj].setPosPrec(pos);
    this->objectList[obj].setFintPrec(fint);
    this->objectList[obj].setSNRPrec(snr);
  }

}
//--------------------------------------------------------------------

bool Cube::objAtSpatialEdge(Detection obj)
{
  /** 
   *   A function to test whether the object obj
   *    lies at the edge of the cube's spatial field --
   *    either at the boundary, or next to BLANKs.
   *
   *   /param obj The Detection under consideration.
   */

  bool atEdge = false;

  int pix = 0;
  while(!atEdge && pix<obj.getSize()){
    // loop over each pixel in the object, until we find an edge pixel.
    Voxel vox = obj.getPixel(pix);
    for(int dx=-1;dx<=1;dx+=2){
      if(((vox.getX()+dx)<0) || ((vox.getX()+dx)>=this->axisDim[0])) 
	atEdge = true;
      else if(this->isBlank(vox.getX()+dx,vox.getY(),vox.getZ())) 
	atEdge = true;
    }
    for(int dy=-1;dy<=1;dy+=2){
      if(((vox.getY()+dy)<0) || ((vox.getY()+dy)>=this->axisDim[1])) 
	atEdge = true;
      else if(this->isBlank(vox.getX(),vox.getY()+dy,vox.getZ())) 
	atEdge = true;
    }
    pix++;
  }

  return atEdge;
}
//--------------------------------------------------------------------

bool Cube::objAtSpectralEdge(Detection obj)
{
  /**  
   *   A function to test whether the object obj
   *    lies at the edge of the cube's spectral extent --
   *    either at the boundary, or next to BLANKs.
   *
   *   /param obj The Detection under consideration.
   */

  bool atEdge = false;

  int pix = 0;
  while(!atEdge && pix<obj.getSize()){
    // loop over each pixel in the object, until we find an edge pixel.
    Voxel vox = obj.getPixel(pix);
    for(int dz=-1;dz<=1;dz+=2){
      if(((vox.getZ()+dz)<0) || ((vox.getZ()+dz)>=this->axisDim[2])) 
	atEdge = true;
      else if(this->isBlank(vox.getX(),vox.getY(),vox.getZ()+dz)) 
	atEdge = true;
    }
    pix++;
  }

  return atEdge;
}
//--------------------------------------------------------------------

void Cube::setObjectFlags()
{
  /**    
   *   A function to set any warning flags for all the detected objects
   *    associated with the cube.
   *   Flags to be looked for:
   *    <ul><li> Negative enclosed flux (N)
   *        <li> Detection at edge of field (spatially) (E)
   *        <li> Detection at edge of spectral region (S)
   *    </ul>
   */

  for(int i=0;i<this->objectList.size();i++){

    if( this->enclosedFlux(this->objectList[i]) < 0. )  
      this->objectList[i].addToFlagText("N");

    if( this->objAtSpatialEdge(this->objectList[i]) ) 
      this->objectList[i].addToFlagText("E");

    if( this->objAtSpectralEdge(this->objectList[i]) ) 
      this->objectList[i].addToFlagText("S");

  }

}
//--------------------------------------------------------------------



/****************************************************************/
/////////////////////////////////////////////////////////////
//// Functions for Image class
/////////////////////////////////////////////////////////////

Image::Image(long size){
  // need error handling in case size<0 !!!
  this->numPixels = this->numDim = 0;
  if(size<0)
    duchampError("Image(size)","Negative size -- could not define Image");
  else{
    if(size>0) this->array = new float[size];
    this->numPixels = size;
    this->axisDim = new long[2];
    this->numDim = 2;
  }
}
//--------------------------------------------------------------------

Image::Image(long *dimensions){
  this->numPixels = this->numDim = 0;
  int size = dimensions[0] * dimensions[1];
  if(size<0)
    duchampError("Image(dimArray)","Negative size -- could not define Image");
  else{
    this->numPixels = size;
    if(size>0) this->array = new float[size];
    this->numDim=2;
    this->axisDim = new long[2];
    for(int i=0;i<2;i++) this->axisDim[i] = dimensions[i];
  }
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------

void Image::saveArray(float *input, long size)
{
  /**
   * Saves the array in input to the pixel array Image::array.
   * The size of the array given must be the same as the current number of
   * pixels, else an error message is returned and nothing is done.
   * \param input The array of values to be saved.
   * \param size The size of input.
   */
  if(size != this->numPixels)
    duchampError("Image::saveArray",
		 "Input array different size to existing array. Cannot save.");
  else {
    if(this->numPixels>0) delete [] array;
    this->numPixels = size;
    if(this->numPixels>0) this->array = new float[size];
    for(int i=0;i<size;i++) this->array[i] = input[i];
  }
}
//--------------------------------------------------------------------

void Image::extractSpectrum(float *Array, long *dim, long pixel)
{
  /**
   *  A function to extract a 1-D spectrum from a 3-D array.
   *  The array is assumed to be 3-D with the third dimension the spectral one.
   *  The spectrum extracted is the one lying in the spatial pixel referenced
   *    by the third argument.
   *  The extracted spectrum is stored in the pixel array Image::array.
   * \param Array The array containing the pixel values, from which the spectrum is extracted.
   * \param dim The array of dimension values.
   * \param pixel The spatial pixel that contains the desired spectrum.
   */ 
  float *spec = new float[dim[2]];
  for(int z=0;z<dim[2];z++) spec[z] = Array[z*dim[0]*dim[1] + pixel];
  this->saveArray(spec,dim[2]);
  delete [] spec;
}
//--------------------------------------------------------------------

void Image::extractSpectrum(Cube &cube, long pixel)
{
  /**
   *  A function to extract a 1-D spectrum from a Cube class
   *  The spectrum extracted is the one lying in the spatial pixel referenced
   *    by the second argument.
   *  The extracted spectrum is stored in the pixel array Image::array.
   * \param cube The Cube containing the pixel values, from which the spectrum is extracted.
   * \param pixel The spatial pixel that contains the desired spectrum.
   */ 
  long zdim = cube.getDimZ();
  long spatSize = cube.getDimX()*cube.getDimY();
  float *spec = new float[zdim];
  for(int z=0;z<zdim;z++) spec[z] = cube.getPixValue(z*spatSize + pixel);
  this->saveArray(spec,zdim);
  delete [] spec;
}
//--------------------------------------------------------------------

void Image::extractImage(float *Array, long *dim, long channel)
{
  /**
   *  A function to extract a 2-D image from a 3-D array.
   *  The array is assumed to be 3-D with the third dimension the spectral one.
   *  The dimensions of the array are in the dim[] array.
   *  The image extracted is the one lying in the channel referenced
   *    by the third argument.
   *  The extracted image is stored in the pixel array Image::array.
   * \param Array The array containing the pixel values, from which the image is extracted.
   * \param dim The array of dimension values.
   * \param channel The spectral channel that contains the desired image.
   */ 
  float *image = new float[dim[0]*dim[1]];
  for(int npix=0; npix<dim[0]*dim[1]; npix++){ 
    image[npix] = Array[channel*dim[0]*dim[1] + npix];
  }
  this->saveArray(image,dim[0]*dim[1]);
  delete [] image;
}
//--------------------------------------------------------------------

void Image::extractImage(Cube &cube, long channel)
{
  /**
   *  A function to extract a 2-D image from Cube class.
   *  The image extracted is the one lying in the channel referenced
   *    by the second argument.
   *  The extracted image is stored in the pixel array Image::array.
   * \param cube The Cube containing the pixel values, from which the image is extracted.
   * \param channel The spectral channel that contains the desired image.
   */ 
  long spatSize = cube.getDimX()*cube.getDimY();
  float *image = new float[spatSize];
  for(int npix=0; npix<spatSize; npix++) 
    image[npix] = cube.getPixValue(channel*spatSize + npix);
  this->saveArray(image,spatSize);
  delete [] image;
}
//--------------------------------------------------------------------

void Image::removeMW()
{
  /**
   *  A function to remove the Milky Way range of channels from a 1-D spectrum.
   *  The array in this Image is assumed to be 1-D, with only the first axisDim
   *    equal to 1.
   *  The values of the MW channels are set to 0, unless they are BLANK.
   */ 
  if(this->par.getFlagMW() && (this->axisDim[1]==1) ){
    for(int z=0;z<this->axisDim[0];z++){
      if(!this->isBlank(z) && this->par.isInMW(z)) this->array[z]=0.;
    }
  }
}

