#include <iostream>
//#include <Cubes/cubes.hh>
#include <Utils/utils.hh>
using namespace std;

void findSourcesAtrous1D(Image &image, Param &par) 
{
  float blankPixValue = par.getBlankPixVal();
  bool flagFDR = par.getFlagFDR();
  long size = image.getSize();
  float *arr = new float[size];
  float *newarr = new float[size];
  float *resid = new float[size];
  int goodSize=0;
  for(int i=0;i<size;i++){
    if(!par.isBlank(image.getPixValue(i)))
      arr[goodSize++] = image.getPixValue(i);
  }
  float originalMean,originalSigma,mean,sigma;
  if(goodSize>0){
    findMedianStats(arr,goodSize,originalMean,originalSigma);
    image.setStats(originalMean,originalSigma,par.getCut());
    image.getArray(arr);
    atrous1DReconstruct(size,arr,newarr,par);

    goodSize=0;
    for(int i=0;i<size;i++)
      if(!par.isBlank(image.getPixValue(i)))
	resid[goodSize++] = arr[i] - newarr[i];
    findNormalStats(resid,goodSize,mean,sigma);

    long *dim = new long[2]; dim[0] = size; dim[1]=1;
    Image *newimage = new Image(dim);
//     *newimage = image;
    newimage->saveArray(newarr,size);
//     newimage->setStats(mean,sigma,par.getCut());
    newimage->setStats(originalMean,sigma,par.getCut());
    if(flagFDR) setupFDR(*newimage,par);
    lutz_detect(*newimage,par);
    for(int obj=0;obj<newimage->getNumObj();obj++){
      Detection *object = new Detection;
      *object = newimage->getObject(obj);
      for(int pix=0;pix<object->getSize();pix++) {
	// set the flux of each pixel in each object to the flux
	// in the corresponding pixel in the original image.
	float flux = image.getPixValue(object->getX(pix),object->getY(pix));
	object->setF(pix,flux);
      }
      image.addObject(*object);
      maskObject(image,*object);
      delete object;
    }
    delete newimage;
  }
  delete [] arr;
}  


void findSourcesAtrous2D(Image &image, Param &par) 
{
  float blankPixValue = par.getBlankPixVal();
  bool flagFDR = par.getFlagFDR();

  long size = image.getSize();
  float *arr = new float[size];
  float *newarr = new float[size];
  float *resid = new float[size];
  int goodSize=0;
  for(int i=0;i<size;i++){
    if(!par.isBlank(image.getPixValue(i)))
      arr[goodSize++] = image.getPixValue(i);
  }
  if(goodSize>0){
    long xdim = image.getDimX();
    long ydim = image.getDimY();
    long *dim = new long[2];
    dim[0] = xdim; dim[1] = ydim;

    float originalMean,originalSigma,mean,sigma;
    //    findMedianStats(arr,goodSize,mean,sigma);
    findMedianStats(arr,goodSize,originalMean,originalSigma);
//     cerr<<"findSourcesAtrous2D: mean = "<<mean<<", sigma = "<<sigma<<endl;
    image.setStats(originalMean,originalSigma,par.getCut());
    image.getArray(arr);
    //    atrous2DReconstruct(xdim,ydim,arr,newarr,par);
    atrous2DReconstruct(xdim,ydim,arr,newarr,par);

    goodSize=0;
    for(int i=0;i<size;i++)
      if(!par.isBlank(image.getPixValue(i)))
	resid[goodSize++] = arr[i] - newarr[i];
    findNormalStats(resid,goodSize,mean,sigma);

    Image *newimage = new Image(dim);
    //    *newimage = image;
    newimage->saveArray(newarr,size);
    //    newimage->setStats(mean,sigma,par.getCut());
    newimage->setStats(originalMean,sigma,par.getCut());
    if(flagFDR) setupFDR(*newimage,par);
    lutz_detect(*newimage,par);
    for(int obj=0;obj<newimage->getNumObj();obj++){
      Detection *object = new Detection;
      *object = newimage->getObject(obj);
      for(int pix=0;pix<object->getSize();pix++) {
	// set the flux of each pixel in each object to the flux
	// in the corresponding pixel in the original image.
	float flux = image.getPixValue(object->getX(pix),object->getY(pix));
	object->setF(pix,flux);
      }
      image.addObject(*object);
      maskObject(image,*object);
      delete object;
    }
    delete newimage;
  }
  delete [] arr;
  delete [] newarr;
}  

