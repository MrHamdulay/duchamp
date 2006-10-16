#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#include <wcs.h>

#include <duchamp.hh>
#include <param.hh>
#include <Cubes/cubes.hh>
#include <Detection/detection.hh>
#include <Detection/columns.hh>
#include <Utils/utils.hh>
#include <Utils/mycpgplot.hh>
#include <Utils/Statistics.hh>
using std::endl;
using namespace Column;
using namespace mycpgplot;
using namespace Statistics;

/****************************************************************/
///////////////////////////////////////////////////
//// Functions for DataArray class:
///////////////////////////////////////////////////

DataArray::DataArray(short int nDim, long size){

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

DataArray::DataArray(short int nDim, long *dimensions){
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
      }
      this->numDim=nDim;
      if(nDim>0) this->axisDim = new long[nDim];
      for(int i=0;i<nDim;i++) this->axisDim[i] = dimensions[i];
    }
  }
}
//--------------------------------------------------------------------

DataArray::~DataArray()
{
  delete [] array;
  delete [] axisDim;
  objectList.clear();
}
//--------------------------------------------------------------------

void DataArray::getDimArray(long *output){
  for(int i=0;i<this->numDim;i++) output[i] = this->axisDim[i];
}
//--------------------------------------------------------------------

void DataArray::getArray(float *output){
  for(int i=0;i<this->numPixels;i++) output[i] = this->array[i];
}
//--------------------------------------------------------------------

void DataArray::saveArray(float *input, long size){
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

void DataArray::getDim(long &x, long &y, long &z){
  if(numDim>0) x=axisDim[0]; 
  else x=0;
  if(numDim>1) y=axisDim[1]; 
  else y=0;
  if(numDim>2) z=axisDim[2]; 
  else z=0;
}
//--------------------------------------------------------------------

void DataArray::addObject(Detection object){ 
  // adds a single detection to the object list
  // objectList is a vector, so just use push_back()
  this->objectList.push_back(object);
}
//--------------------------------------------------------------------

void DataArray::addObjectList(vector <Detection> newlist) {
  for(int i=0;i<newlist.size();i++) this->objectList.push_back(newlist[i]);
}
//--------------------------------------------------------------------

void DataArray::addObjectOffsets(){
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

std::ostream& operator<< ( std::ostream& theStream, DataArray &array)
{
  for(int i=0;i<array.numDim;i++){
    if(i>0) theStream<<"x";
    theStream<<array.axisDim[i];
  }
  theStream<<endl;
  theStream<<array.objectList.size()<<" detections:"<<endl<<"--------------\n";
  for(int i=0;i<array.objectList.size();i++){
    theStream << "Detection #" << array.objectList[i].getID()<<endl;
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
//// Functions for Image class
/////////////////////////////////////////////////////////////

Image::Image(long size){
  // need error handling in case size<0 !!!
  if(size<0)
    duchampError("Image(size)","Negative size -- could not define Image");
  else{
    if(size>0){
      this->array = new float[size];
//       this->pValue = new float[size];
//       this->mask = new short int[size];
    }
    this->numPixels = size;
    this->axisDim = new long[2];
    this->numDim = 2;
  }
}
//--------------------------------------------------------------------

Image::Image(long *dimensions){
  int size = dimensions[0] * dimensions[1];
  if(size<0)
    duchampError("Image(dimArray)","Negative size -- could not define Image");
  else{
    this->numPixels = size;
    if(size>0){
      this->array = new float[size];
//       this->pValue = new float[size];
//       this->mask = new short int[size];
    }
    this->numDim=2;
    this->axisDim = new long[2];
    for(int i=0;i<2;i++) this->axisDim[i] = dimensions[i];
//     for(int i=0;i<size;i++) this->mask[i] = 0;
  }
}
//--------------------------------------------------------------------

Image::~Image(){
  if(this->numPixels > 0){
//     delete [] this->pValue;
//     delete [] this->mask;
  }
}
//--------------------------------------------------------------------

void Image::saveArray(float *input, long size){
  if(size != this->numPixels)
    duchampError("Image::saveArray",
		 "Input array different size to existing array. Cannot save.");
  else {
    if(this->numPixels>0){
      delete [] array;
//       delete [] pValue;
//       delete [] mask;
    }
    this->numPixels = size;
    if(this->numPixels>0){
      this->array = new float[size];
//       this->pValue = new float[size];
//       this->mask = new short int[size];
    }
    for(int i=0;i<size;i++) this->array[i] = input[i];
//     for(int i=0;i<size;i++) this->mask[i] = 0;
  }
}
//--------------------------------------------------------------------

void Image::extractSpectrum(float *Array, long *dim, long pixel)
{
  /**
   * Image::extractSpectrum(float *, long *, int)
   *  A function to extract a 1-D spectrum from a 3-D array.
   *  The array is assumed to be 3-D with the third dimension the spectral one.
   *  The dimensions of the array are in the dim[] array.
   *  The spectrum extracted is the one lying in the spatial pixel referenced
   *    by the third argument.
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
   * Image::extractSpectrum(Cube &, int)
   *  A function to extract a 1-D spectrum from a Cube class
   *  The spectrum extracted is the one lying in the spatial pixel referenced
   *    by the second argument.
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
   * Image::extractImage(float *, long *, int)
   *  A function to extract a 2-D image from a 3-D array.
   *  The array is assumed to be 3-D with the third dimension the spectral one.
   *  The dimensions of the array are in the dim[] array.
   *  The image extracted is the one lying in the channel referenced
   *    by the third argument.
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
   * Image::extractImage(Cube &, int)
   *  A function to extract a 2-D image from Cube class.
   *  The image extracted is the one lying in the channel referenced
   *    by the second argument.
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
   * Image::removeMW()
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

/****************************************************************/
/////////////////////////////////////////////////////////////
//// Functions for Cube class
/////////////////////////////////////////////////////////////

Cube::Cube(long size){
  // need error handling in case size<0 !!!
  if(size<0)
    duchampError("Cube(size)","Negative size -- could not define Cube");
  else{
    if(size>0){
      this->array = new float[size];
      this->recon = new float[size];
    }
    this->numPixels = size;
    this->axisDim = new long[2];
    this->numDim = 3;
    this->reconExists = false;
  }
}
//--------------------------------------------------------------------

Cube::Cube(long *dimensions){
  int size   = dimensions[0] * dimensions[1] * dimensions[2];
  int imsize = dimensions[0] * dimensions[1];
  if((size<0) || (imsize<0) )
    duchampError("Cube(dimArray)","Negative size -- could not define Cube");
  else{
    this->numPixels = size;
    if(size>0){
      this->array      = new float[size];
      this->detectMap  = new short[imsize];
      if(this->par.getFlagATrous())
	this->recon    = new float[size];
      if(this->par.getFlagBaseline())
	this->baseline = new float[size];
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
  delete [] detectMap;
  if(this->par.getFlagATrous())   delete [] recon;
  if(this->par.getFlagBaseline()) delete [] baseline;
}
//--------------------------------------------------------------------

void Cube::initialiseCube(long *dimensions)
{
  /**
   *  Cube::initialiseCube(long *dim)
   *   A function that defines the sizes of all the necessary
   *    arrays in the Cube class.
   *   It also defines the values of the axis dimensions.
   *   This is done with the WCS in the FitsHeader class, so the 
   *    WCS needs to be good and have three axes.
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


  if((size<0) || (imsize<0) )
    duchampError("Cube::initialiseCube(dimArray)",
		 "Negative size -- could not define Cube.\n");
  else{
    this->numPixels = size;
    if(size>0){
      this->array      = new float[size];
      this->detectMap  = new short[imsize];
      if(this->par.getFlagATrous())
	this->recon    = new float[size];
      if(this->par.getFlagBaseline())
	this->baseline = new float[size];
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

int Cube::getopts(int argc, char ** argv)
{
  /**
   *   Cube::getopt
   *    A front-end to read in the command-line options,
   *     and then read in the correct parameters to the cube->par
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
  if(size != this->numPixels){
    stringstream errmsg;
    errmsg << "Input array different size to existing array ("
	   << size << " cf. " << this->numPixels << "). Cannot save.\n";
    duchampError("Cube::saveRecon",errmsg.str());
  }
  else {
    if(this->numPixels>0) delete [] this->recon;
    this->numPixels = size;
    this->recon = new float[size];
    for(int i=0;i<size;i++) this->recon[i] = input[i];
    this->reconExists = true;
  }
}
//--------------------------------------------------------------------

void Cube::getRecon(float *output){
  // Need check for change in number of pixels!
  long size = this->numPixels;
  for(int i=0;i<size;i++){
    if(this->reconExists) output[i] = this->recon[i];
    else output[i] = 0.;
  }
}
//--------------------------------------------------------------------

void Cube::removeMW()
{
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

void Cube::calcObjectWCSparams()
{
  /** 
   * Cube::calcObjectWCSparams()
   *  A function that calculates the WCS parameters for each object in the 
   *  cube's list of detections.
   *  Each object gets an ID number set (just the order in the list), and if 
   *   the WCS is good, the WCS paramters are calculated.
   */
  
  for(int i=0; i<this->objectList.size();i++){
    this->objectList[i].setID(i+1);
    this->objectList[i].calcWCSparams(this->head);
  }  

  
}
//--------------------------------------------------------------------

void Cube::sortDetections()
{
  /** 
   * Cube::sortDetections()
   *  A front end to the sort-by functions.
   *  If there is a good WCS, the detection list is sorted by velocity.
   *  Otherwise, it is sorted by increasing z-pixel value.
   *  The ID numbers are then re-calculated.
   */
  
  if(this->head.isWCS()) SortByVel(this->objectList);
  else SortByZ(this->objectList);
  for(int i=0; i<this->objectList.size();i++) this->objectList[i].setID(i+1);

}
//--------------------------------------------------------------------

void Cube::updateDetectMap()
{
  /** 
   * Cube::updateDetectMap()
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
   * Cube::updateDetectMap(Detection)
   *  A function that, for the given object, increments the cube's
   *  detection map by the required amount at each pixel.
   */
  for(int pix=0;pix<obj.getSize();pix++) {
    int spatialPos = obj.getX(pix)+obj.getY(pix)*this->axisDim[0];
    this->detectMap[spatialPos]++;
  }
}
//--------------------------------------------------------------------

void Cube::setCubeStats()
{
  /**
   *  Cube::setCubeStats()
   *   Calculates the full statistics for the cube:
   *     mean, rms, median, madfm
   *   Only do this if the threshold has not been defined (ie. is still 0.,
   *    its default). 
   *   Also work out the threshold and store it in the par set.
   *   
   *   For stats calculations, ignore BLANKs and MW channels.
   */

  // get number of good pixels;
  int goodSize = 0;
  for(int x=0;x<this->axisDim[0];x++){
    for(int y=0;y<this->axisDim[1];y++){
      for(int z=0;z<this->axisDim[2];z++){
	int vox = z * this->axisDim[0] * this->axisDim[1] + 
	  this->axisDim[0] * y + x;
	if(!this->isBlank(vox) && !this->par.isInMW(z)) goodSize++;
      }
    }
  }
  float *tempArray = new float[goodSize];

  goodSize=0;
  for(int x=0;x<this->axisDim[0];x++){
    for(int y=0;y<this->axisDim[1];y++){
      for(int z=0;z<this->axisDim[2];z++){
	int pix = x + this->axisDim[0] * y;
	int vox = z * this->axisDim[0] * this->axisDim[1] + pix;
	if(!this->isBlank(vox) && !this->par.isInMW(z))
	  tempArray[goodSize++] = this->array[vox];
      }
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
    for(int x=0;x<this->axisDim[0];x++){
      for(int y=0;y<this->axisDim[1];y++){
	for(int z=0;z<this->axisDim[2];z++){
	  int pix = x + this->axisDim[0] * y;
	  int vox = z * this->axisDim[0] * this->axisDim[1] + pix;
	  if(!this->isBlank(vox) && !this->par.isInMW(z))
	    tempArray[goodSize++] = this->array[vox] - this->recon[vox];
	}
      }
    }
    reconStats.calculate(tempArray,goodSize);

    // Get the "middle" estimators from the original array.
    // Get the "spread" estimators from the residual (orig-recon) array
    this->Stats.setMean(origStats.getMean());
    this->Stats.setMedian(origStats.getMedian());
    this->Stats.setStddev(reconStats.getStddev());
    this->Stats.setMadfm(reconStats.getMadfm());
  }

  this->Stats.setUseFDR( this->par.getFlagFDR() );
  // If the FDR method has been requested
  if(this->par.getFlagFDR())  this->setupFDR();
  else{
    if(this->par.getFlagUserThreshold()){
      // if the user has defined a threshold, set this in the StatsContainer
      this->Stats.setThreshold( this->par.getThreshold() );
    }
    else{
      // otherwise, calculate one based on the requested SNR cut level, and 
      //   then set the threshold parameter in the Par set.
      this->Stats.setThresholdSNR( this->par.getCut() );
      this->par.setThreshold( this->Stats.getThreshold() );
      std::cout << "Using flux threshold of: " << this->Stats.getThreshold()
		<< std::endl;
      std::cout << "Median = " << this->Stats.getMedian()
		<< ", MADFM = " << this->Stats.getMadfm()
		<< ", Robust Threshold = " 
		<< this->Stats.getMedian() + 
	this->par.getCut()*madfmToSigma(this->Stats.getMadfm()) << std::endl;
      std::cout << "Mean = " << this->Stats.getMean()
		<< ", Sigma = " << this->Stats.getStddev()
		<< ", Threshold = " 
		<< this->Stats.getMean() + this->par.getCut()*this->Stats.getStddev() << std::endl;
    }
  }

}
//--------------------------------------------------------------------

int Cube::setupFDR()
{
  /**
   *  Cube::setupFDR()
   *   Determines the critical Prob value for the False Discovery Rate
   *    detection routine. All pixels with Prob less than this value will
   *    be considered detections.
   *   The Prob here is the probability, assuming a Normal distribution, of
   *    obtaining a value as high or higher than the pixel value (ie. only the
   *    positive tail of the PDF)
   */

  // first calculate p-value for each pixel -- assume Gaussian for now.

  float *orderedP = new float[this->numPixels];
  int count = 0;
  for(int pix=0; pix<this->numPixels; pix++){

    if( !(this->par.isBlank(this->array[pix])) ){ 
      // only look at non-blank pixels
      float zStat = (this->array[pix] - this->Stats.getMiddle()) / 
	this->Stats.getSpread();
      
      orderedP[count++] = 0.5 * erfc(zStat/M_SQRT2);
      // Need the factor of 0.5 here, as we are only considering the positive 
      //  tail of the distribution. Don't care about negative detections.
    }
  }

  // now order them 
  sort(orderedP,0,count);
  
  // now find the maximum P value.
  int max = 0;
  float cN = 0.;
  int psfCtr;
  int numPix = int(this->par.getBeamSize());
  for(psfCtr=1;psfCtr<=numPix;(psfCtr)++) 
    cN += 1./float(psfCtr);

  for(int loopCtr=0;loopCtr<count;loopCtr++) {
    if( orderedP[loopCtr] < 
	(double(loopCtr+1)*this->par.getAlpha()/(cN * double(count))) ) {
      max = loopCtr;
    }
  }

  this->Stats.setPThreshold( orderedP[max] );

  delete [] orderedP;

}
//--------------------------------------------------------------------

float Cube::enclosedFlux(Detection obj)
{
  /** 
   *  float Cube::enclosedFlux(Detection obj)
   *   A function to calculate the flux enclosed by the range
   *    of pixels detected in the object obj (not necessarily all
   *    pixels will have been detected).
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
   *  Cube::setupColumns()
   *   A front-end to the two setup routines in columns.cc.
   */ 
  this->calcObjectWCSparams();  
  // need this as the colSet functions use vel, RA, Dec, etc...
  
  this->fullCols.clear();
  this->fullCols = getFullColSet(this->objectList, this->head);

  this->logCols.clear();
  this->logCols = getLogColSet(this->objectList, this->head);

  int vel,fpeak,fint,pos,xyz,temp;
  vel = fullCols[VEL].getPrecision();
  fpeak = fullCols[FPEAK].getPrecision();
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
  }

}
//--------------------------------------------------------------------

bool Cube::objAtEdge(Detection obj)
{
  /**
   *  bool Cube::objAtEdge()
   *   A function to test whether the object obj
   *    lies at the edge of the cube's field --
   *    either at the boundary, or next to BLANKs
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
   *  void Cube::setObjectFlags()
   *   A function to set any warning flags for all the detected objects
   *    associated with the cube.
   *   Flags to be looked for:
   *       * Negative enclosed flux (N)
   *       * Object at edge of field (E)
   */

  for(int i=0;i<this->objectList.size();i++){

    if( this->enclosedFlux(this->objectList[i]) < 0. )  
      this->objectList[i].addToFlagText("N");

    if( this->objAtEdge(this->objectList[i]) ) 
      this->objectList[i].addToFlagText("E");

  }

}
//--------------------------------------------------------------------

void Cube::plotBlankEdges()
{
  if(this->par.drawBlankEdge()){
    int colour;
    cpgqci(&colour);
    cpgsci(MAGENTA);
    drawBlankEdges(this->array,this->axisDim[0],this->axisDim[1],this->par);
    cpgsci(colour);
  }
}
