#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <wcs.h>
#include <Cubes/cubes.hh>
#include <Detection/detection.hh>
#include <Utils/utils.hh>
using std::endl;

/****************************************************************/
///////////////////////////////////////////////////
//// Functions for DataArray class:
///////////////////////////////////////////////////

DataArray::DataArray(short int nDim, long size){
  // need error handling in case size<0 !!!
  if(size>0) this->array = new float[size];
  this->numPixels = size;
  if(nDim>0) this->axisDim = new long[nDim];
  this->numDim = nDim;
}

DataArray::DataArray(short int nDim, long *dimensions){
  int size = dimensions[0];
  for(int i=1;i<nDim;i++) size *= dimensions[i];
  this->numPixels = size;
  if(size>0){
    this->array = new float[size];
  }
  this->numDim=nDim;
  if(nDim>0) this->axisDim = new long[nDim];
  for(int i=0;i<nDim;i++) this->axisDim[i] = dimensions[i];
}

void DataArray::getDimArray(long *output){
  for(int i=0;i<this->numDim;i++) output[i] = this->axisDim[i];
}

void DataArray::getArray(float *output){
  for(int i=0;i<this->numPixels;i++) output[i] = this->array[i];
}

void DataArray::saveArray(float *input, long size){
  delete [] this->array;
  // Need check for change in number of pixels!
  this->numPixels = size;
  this->array = new float[size];
  for(int i=0;i<size;i++) this->array[i] = input[i];
}

void DataArray::getDim(long &x, long &y, long &z){
  if(numDim>0) x=axisDim[0]; 
  else x=0;
  if(numDim>1) y=axisDim[1]; 
  else y=0;
  if(numDim>2) z=axisDim[2]; 
  else z=0;
}

void DataArray::addObject(Detection object){ 
  // adds a single detection to the object list
  // objectList is a vector, so just use push_back()
  this->objectList.push_back(object);
}

void DataArray::addObjectList(vector <Detection> newlist) {
  for(int i=0;i<newlist.size();i++) this->objectList.push_back(newlist[i]);
}

std::ostream& operator<< ( std::ostream& theStream, DataArray &array){

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
  if(size>0){
    this->array = new float[size];
    this->pValue = new float[size];
    this->mask = new short int[size];
  }
  this->numPixels = size;
  this->axisDim = new long[2];
  this->numDim = 2;
}

Image::Image(long *dimensions){
  int size = dimensions[0] * dimensions[1];
  this->numPixels = size;
  if(size>0){
    this->array = new float[size];
    this->pValue = new float[size];
    this->mask = new short int[size];
  }
  this->numDim=2;
  this->axisDim = new long[2];
  for(int i=0;i<2;i++) this->axisDim[i] = dimensions[i];
  for(int i=0;i<size;i++) this->mask[i] = 0;
}

void Image::saveArray(float *input, long size){
  // Need check for change in number of pixels!
  if(this->numPixels>0){
    delete [] array;
    delete [] pValue;
    delete [] mask;
  }
  this->numPixels = size;
  this->array = new float[size];
  this->pValue = new float[size];
  this->mask = new short int[size];
  for(int i=0;i<size;i++) this->array[i] = input[i];
  for(int i=0;i<size;i++) this->mask[i] = 0;
}

void Image::maskObject(Detection &object)
{
  /**
   * Image::maskObject(Detection &)
   *  A function that increments the mask for each pixel of the detection.
   */
  for(long i=0;i<object.getSize();i++){
    this->setMaskValue(object.getX(i),object.getY(i),1);
  }
}

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

void Image::removeMW()
{
  /**
   * Image::removeMW()
   *  A function to remove the Milky Way range of channels from a 1-D spectrum.
   *  The array in this Image is assumed to be 1-D, with only the first axisDim
   *    equal to 1.
   *  The values of the MW channels are set to 0, unless they are BLANK.
   */ 
  int maxMW = this->par.getMaxMW();
  int minMW = this->par.getMinMW();
  if(this->par.getFlagMW() && (this->axisDim[1]==1)){
    for(int z=0;z<this->axisDim[0];z++){
      if(!this->isBlank(z) && this->par.isInMW(z)) this->array[z]=0.;
    }
  }
}

void Image::findStats(int code)
{
  /**
   *  Image::findStats(int code)
   *    Front-end to function to find the stats (mean/median & sigma/madfm) and
   *     store them in the "mean" and "sigma" members of Image.
   *    The choice of normal(mean & sigma) or robust (median & madfm) is made via the 
   *      code parameter. This is stored as a decimal number, with 0s representing 
   *      normal stats, and 1s representing robust. 
   *      The 10s column is the mean, the 1s column the sigma.
   *      Eg: 00 -- mean&sigma; 01 -- mean&madfm; 10 -- median&sigma; 11 -- median&madfm
   *    If calculated, the madfm value is corrected to sigma units.
   *    The Image member "cut" is also assigned using the parameter in Image's par 
   *      (needs to be defined first -- also for the blank pixel determination).
   */
  float *tempArray = new float[this->numPixels];
  int goodSize=0;
  for(int i=0; i<this->numPixels; i++) 
    if(!this->isBlank(i)) tempArray[goodSize++] = this->array[i];
  float tempMean,tempSigma,tempMedian,tempMADFM;
  if(code != 0) findMedianStats(tempArray,goodSize,tempMedian,tempMADFM);
  if(code != 11) findNormalStats(tempArray,goodSize,tempMean,tempSigma);
  switch(code)
    {
    case 0:
      findNormalStats(tempArray,goodSize,tempMean,tempSigma);
      this->mean = tempMean;
      this->sigma = tempSigma;
      break;
    case 10:
      this->mean  = findMedian(tempArray,goodSize);;
      this->sigma = findStddev(tempArray,goodSize);
      break;
    case 1:
      this->mean  = findMean(tempArray,goodSize);
      this->sigma = findMADFM(tempArray,goodSize)/correctionFactor;
      break;
    case 11:
    default:
      if(code!=11) std::cerr << 
		     "Invalid code ("<<code<<") in findStats. Using robust method.\n";
      findMedianStats(tempArray,goodSize,tempMedian,tempMADFM);
      this->mean = tempMedian;
      this->sigma = tempMADFM/correctionFactor;
      break;
    }
  this->cutLevel = this->par.getCut();
  delete [] tempArray;
}

/****************************************************************/
/////////////////////////////////////////////////////////////
//// Functions for Cube class
/////////////////////////////////////////////////////////////

Cube::Cube(long size){
  // need error handling in case size<0 !!!
  if(size>0){
    this->array = new float[size];
    this->recon = new float[size];
  }
  this->numPixels = size;
  this->axisDim = new long[2];
  this->numDim = 3;
  this->reconExists = false;
//   flagWCS = false;
}

Cube::Cube(long *dimensions){
  int size   = dimensions[0] * dimensions[1] * dimensions[2];
  int imsize = dimensions[0] * dimensions[1];
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
//   this->wcs = new wcsprm;
//   this->wcs->flag=-1;
//   wcsini(true,3,this->wcs); 
//   flagWCS = false;
}

void Cube::initialiseCube(long *dimensions){
  int size   = dimensions[0] * dimensions[1] * dimensions[2];
  int imsize = dimensions[0] * dimensions[1];
  this->numPixels = size;
  if(size>0){
    this->array      = new float[size];
    this->detectMap  = new short[imsize];
    this->specMean   = new float[imsize];
    this->specSigma  = new float[imsize];
    this->chanMean   = new float[dimensions[2]];
    this->chanSigma  = new float[dimensions[2]];
    if(this->par.getFlagATrous())
      this->recon    = new float[size];
    if(this->par.getFlagBaseline())
      this->baseline = new float[size];
  }
  this->numDim  = 3;
  this->axisDim = new long[3];
  for(int i=0;i<3     ;i++) this->axisDim[i]   = dimensions[i];
  for(int i=0;i<imsize;i++) this->detectMap[i] = 0;
//   this->wcs = new wcsprm;
//   this->wcs->flag=-1;
//   wcsini(true,3,this->wcs); 
}

void Cube::saveArray(float *input, long size){
  // Need check for change in number of pixels!
  if(this->numPixels>0) delete [] array;
  this->numPixels = size;
  this->array = new float[size];
  for(int i=0;i<size;i++) this->array[i] = input[i];
}

void Cube::saveRecon(float *input, long size){
  // Need check for change in number of pixels!
  if(this->numPixels>0) delete [] this->recon;
  this->numPixels = size;
  this->recon = new float[size];
  for(int i=0;i<size;i++) this->recon[i] = input[i];
  this->reconExists = true;
}

void Cube::getRecon(float *output){
  // Need check for change in number of pixels!
  long size = this->numPixels;
  for(int i=0;i<size;i++){
    if(this->reconExists) output[i] = this->recon[i];
    else output[i] = 0.;
  }
}

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

void Cube::calcObjectWCSparams()
{
  /** 
   * Cube::calcObjectWCSparams()
   *  A function that calculates the WCS parameters for each object in the 
   *  cube's list of detections.
   *  Each object gets an ID number set (just the order in the list), and if the 
   *   WCS is good, the WCS paramters are calculated.
   */
  
  for(int i=0; i<this->objectList.size();i++){
    this->objectList[i].setID(i+1);
    this->objectList[i].calcWCSparams(this->head);
    //    this->objectList[i].setIntegFlux( this->objectList[i].getIntegFlux()/this->par.getBeamSize() );
    // this corrects the integrated flux for the beam size.
  }  

  
}

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

void Cube::updateDetectMap()
{
  /** 
   * Cube::updateDetectMap()
   *  A function that, for each detected object in the cube's list, increments the 
   *  cube's detection map by the required amount at each pixel.
   */

  for(int obj=0;obj<this->objectList.size();obj++)
    for(int pix=0;pix<this->objectList[obj].getSize();pix++) 
      this->detectMap[this->objectList[obj].getX(pix)+this->objectList[obj].getY(pix)*this->axisDim[0]]++;
}

void Cube::updateDetectMap(Detection obj)
{
  /** 
   * Cube::updateDetectMap(Detection)
   *  A function that, for the given object, increments the cube's
   *  detection map by the required amount at each pixel.
   */
  for(int pix=0;pix<obj.getSize();pix++) 
    this->detectMap[obj.getX(pix)+obj.getY(pix)*this->axisDim[0]]++;
}

void Cube::setCubeStats()
{
  // First set the stats for each spectrum (ie. each spatial pixel)
  long xySize = this->axisDim[0]*this->axisDim[1];
  float *spec = new float[this->axisDim[2]];
  for(int i=0;i<xySize;i++){
    for(int z=0;z<this->axisDim[2];z++){
      //Two cases: i) have reconstructed -- use residuals 
      //          ii) otherwise          -- use original array
      if(this->reconExists) spec[z] = this->array[z*xySize+i] - this->recon[z*xySize+1];
      else                  spec[z] = this->array[z*xySize+i];
    }
    findMedianStats(spec,this->axisDim[2],this->specMean[i],this->specSigma[i]);
  }
  delete spec;
  // Then set the stats for each channel map
  float *im = new float[xySize];
  for(int z=0;z<this->axisDim[2];z++){
    for(int i=0;i<xySize;i++){
      if(this->reconExists) im[i] = this->array[z*xySize+i] - this->recon[z*xySize+1];
      else                  im[i] = this->array[z*xySize+i];
     
    }
    findMedianStats(im,this->axisDim[2],this->chanMean[z],this->chanSigma[z]);
    this->chanSigma[z] /= correctionFactor;
  }
  delete im;

}

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
	  this->getPixValue(x+obj.getXmin(),y+obj.getYmin(),z+obj.getZmin());
	if(this->par.getFlagNegative()) fluxArray[x+y*xsize+z*ysize*xsize] *= -1.;
      }
    }
  }
  float sum = 0.;
  for(int i=0;i<fluxArray.size();i++) 
    if(!this->par.isBlank(fluxArray[i])) sum+=fluxArray[i];
  return sum;
}

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
      if(((vox.getX()+dx)<0) || ((vox.getX()+dx)>=this->axisDim[0])) atEdge = true;
      else if(this->isBlank(vox.getX()+dx,vox.getY(),vox.getZ())) atEdge = true;
    }
    for(int dy=-1;dy<=1;dy+=2){
      if(((vox.getY()+dy)<0) || ((vox.getY()+dy)>=this->axisDim[1])) atEdge = true;
      else if(this->isBlank(vox.getX(),vox.getY()+dy,vox.getZ())) atEdge = true;
    }
    for(int dz=-1;dz<=1;dz+=2){
      if(((vox.getZ()+dz)<0) || ((vox.getZ()+dz)>=this->axisDim[2])) atEdge = true;
      else if(this->isBlank(vox.getX(),vox.getY(),vox.getZ()+dz)) atEdge = true;
    }
    pix++;
  }

  return atEdge;
}

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

void Cube::plotBlankEdges()
{
  int colour;
  cpgqci(&colour);
  cpgsci(6);
  drawBlankEdges(this->array,this->axisDim[0],this->axisDim[1],this->par);
  cpgsci(colour);
}
