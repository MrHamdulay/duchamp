#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <Cubes/cubes.hh>
#include <ATrous/atrous.hh>
#include <Utils/utils.hh>

using std::setw;

//////////////////////////////////////////////////////////////////////////////////////
/**
 * Cube::ReconSearch1D()
 *   This reconstructs a cube by performing a 1D a trous reconstruction
 *   in the spectrum of each spatial pixel.
 *   It then searches the cube using reconSearch (below).
 *
 *   The resulting object list is stored in the Cube.
 */
void Cube::ReconSearch1D()
{
  long xySize = axisDim[0] * axisDim[1];
  long zdim = axisDim[2];

  // Reconstruct the cube by 1d atrous transform in each spatial pixel
  std::cout<<"Reconstructing... ";
  for(int npix=0; npix<xySize; npix++){
    if((100*npix/xySize)%5==0)
      std::cout<<setw(3)<<100*npix/xySize<<"% done"<<"\b\b\b\b\b\b\b\b\b"<<std::flush;
    float *spec = new float[zdim];
    float *newSpec = new float[zdim];
    for(int z=0;z<zdim;z++){
      int cubepos = z*xySize + npix;
      spec[z] = this->array[cubepos];
    }
    atrous1DReconstruct(axisDim[2],spec,newSpec,this->par);
    for(int z=0;z<zdim;z++){
      int cubepos = z*xySize + npix;
      this->recon[cubepos] = newSpec[z];
    }
    delete spec;
    delete newSpec;
  }
  this->reconExists = true;
  std::cout<<"All Done. Searching... ";
    
  this->objectList = reconSearch(this->axisDim,this->array,this->recon,this->par);
  this->updateDetectMap();
  if(this->par.getFlagLog()) this->logDetectionList();

}

//////////////////////////////////////////////////////////////////////////////////////
/**
 * Cube::ReconSearch2D()
 *   This reconstructs a cube by performing a 2D a trous reconstruction
 *   in each spatial image of the cube.
 *   It then searches the cube using reconSearch (below).
 *
 *   The resulting object list is stored in the Cube.
 */
void Cube::ReconSearch2D()
{
  long xySize = axisDim[0] * axisDim[1];

  // RECONSTRUCT THE CUBE BY 2D ATROUS TRANSFORM IN EACH CHANNEL  
  bool *doChannel = new bool[axisDim[2]];
  for(int z=0;z<axisDim[2];z++) 
    doChannel[z] = !( this->par.getFlagMW() && (z>=this->par.getMinMW()) && (z<=this->par.getMaxMW()) );
  std::cout<<"Reconstructing... ";
  for(int z=0;z<axisDim[2];z++){
    if((100*z/axisDim[2])%5==0)
      std::cout<<setw(3)<<100*z/axisDim[2]<<"% done"<<"\b\b\b\b\b\b\b\b\b"<<std::flush;
    if(doChannel[z]){
      float *im = new float[xySize];
      float *newIm = new float[xySize];
      for(int npix=0; npix<xySize; npix++){
	int cubepos = z*xySize + npix;
	im[npix] = this->array[cubepos];
      }
      atrous2DReconstruct(axisDim[0],axisDim[1],im,newIm,this->par);
      for(int npix=0; npix<xySize; npix++){
	int cubepos = z*xySize + npix;
	this->recon[cubepos] = newIm[npix];
      }
      delete im;
      delete newIm;
    }
    else 
      for(int i=0; i<xySize; i++) this->recon[z*xySize+i] = this->array[z*xySize+i];
  }
  this->reconExists = true;
  std::cout<<"All Done.                      \nSearching... ";

  this->objectList = reconSearch(this->axisDim,this->array,this->recon,this->par);
  
  this->updateDetectMap();
  if(this->par.getFlagLog()) this->logDetectionList();

  delete [] doChannel;

}

//////////////////////////////////////////////////////////////////////////////////////
/**
 * Cube::ReconSearch3D()
 *   This performs a full 3D a trous reconstruction of the cube
 *   It then searches the cube using reconSearch (below).
 *
 *   The resulting object list is stored in the Cube.
 */
void Cube::ReconSearch3D()
{
  if(this->axisDim[2]==1) this->ReconSearch2D();
  else {

    if(!this->reconExists){
      std::cout<<"  Reconstructing... "<<std::flush;
      atrous3DReconstruct(this->axisDim[0],this->axisDim[1],this->axisDim[2],
			  this->array,this->recon,this->par);
      this->reconExists = true;
      std::cout<<"All Done.                      \n  Searching... "<<std::flush;
    }

    this->objectList = reconSearch(this->axisDim,this->array,this->recon,this->par);

    this->updateDetectMap();
    if(this->par.getFlagLog()) this->logDetectionList();

  }

}


//////////////////////////////////////////////////////////////////////////////////////
/**
 * reconSearch(long *dim, float *originalArray, float *reconArray, Param &par)
 *   This searches for objects in a cube that has been reconstructed.
 *
 *   Inputs:   - dimension array
 *             - original, un-reconstructed image array
 *             - reconstructed image array
 *             - parameters
 *
 *   Searches first in each spatial pixel (1D search), 
 *   then in each channel image (2D search).
 *
 *   Returns: vector of Detections resulting from the search.
 */
vector <Detection> reconSearch(long *dim, float *originalArray, float *reconArray, Param &par)
{
  vector <Detection> outputList;
  int zdim = dim[2];
  int xySize = dim[0] * dim[1];
  int fullSize = zdim * xySize;
  int num=0, goodSize;

  float blankPixValue = par.getBlankPixVal();
  bool *isGood = new bool[fullSize];
  for(int pos=0;pos<fullSize;pos++) 
    isGood[pos] = !par.isBlank(originalArray[pos]);
 
  float dud;

  // First search --  in each spectrum.
  // First, get stats
  if(zdim > 1){
    if(par.isVerbose()) std::cout << "1D: |                    |" << std::flush;

    float *specMedian = new float[xySize];
    float *specSigma = new float[xySize];
    float *spec = new float[zdim];
   
    for(int npix=0; npix<xySize; npix++){
      goodSize=0;
      for(int z=0;z<zdim;z++) 
	if(isGood[z*xySize+npix]) spec[goodSize++] = originalArray[z*xySize+npix];
      if(goodSize>0) specMedian[npix] = findMedian(spec,goodSize);
      else specMedian[npix] = blankPixValue;
      goodSize=0;
      for(int z=0;z<zdim;z++) 
	if(isGood[z*xySize+npix]) 
	  spec[goodSize++] = originalArray[z*xySize+npix]-reconArray[z*xySize+npix];
      if(goodSize>0) specSigma[npix] = findStddev(spec,goodSize);
      else specSigma[npix] = 1.;
    }

    // Next, do source finding.
    long *specdim = new long[2];
    specdim[0] = zdim; specdim[1]=1;
    Image *spectrum = new Image(specdim);
    delete [] specdim;
    spectrum->saveParam(par);
    spectrum->pars().setBeamSize(2.); // for spectrum, only neighbouring channels correlated
    for(int npix=0; npix<xySize; npix++){

      if( par.isVerbose() && ((100*(npix+1)/xySize)%5 == 0) ){
	std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b|";
	for(int i=0;i<(100*(npix+1)/xySize)/5;i++) std::cout << "#";
	for(int i=(100*(npix+1)/xySize)/5;i<20;i++) std::cout << " ";
	std::cout << "|" << std::flush;
      }

      spectrum->extractSpectrum(reconArray,dim,npix);
      spectrum->setStats(specMedian[npix],specSigma[npix],par.getCut());
      if(par.getFlagFDR()) spectrum->setupFDR();
      spectrum->setMinSize(par.getMinChannels());
      spectrum->spectrumDetect();
      for(int obj=0;obj<spectrum->getNumObj();obj++){
	Detection *object = new Detection;
	*object = spectrum->getObject(obj);
	for(int pix=0;pix<object->getSize();pix++) {
	  // Fix up coordinates of each pixel to match original array
	  object->setZ(pix, object->getX(pix));
	  object->setX(pix, npix%dim[0]);
	  object->setY(pix, npix/dim[0]);
	  object->setF(pix, originalArray[object->getX(pix)+object->getY(pix)*dim[0]+object->getZ(pix)*xySize]); 
	  // NB: set F to the original value, not the recon value.
	}
	object->addOffsets(par);
	object->calcParams();
	mergeIntoList(*object,outputList,par);
	delete object;
      }
      spectrum->clearDetectionList();
    }
    delete spectrum;
    delete [] specMedian;
    delete [] specSigma;

    num = outputList.size();
    std::cout <<"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bFound " << num <<"; " << std::flush;

  }

  // Second search --  in each channel
  if(par.isVerbose()) std::cout << "2D: |                    |" << std::flush;

  float *imageMedian = new float[zdim];
  float *imageSigma = new float[zdim];
  float *image = new float[xySize];
  //  First, get stats
  for(int z=0; z<zdim; z++){
    goodSize=0;
    for(int npix=0; npix<xySize; npix++) 
      if(isGood[z*xySize + npix]) image[goodSize++] = originalArray[z*xySize + npix];
    if(goodSize>0) imageMedian[z] = findMedian(image,goodSize);
    else imageMedian[z] = blankPixValue;
    goodSize=0;
    for(int npix=0; npix<xySize; npix++) 
      if(isGood[z*xySize+npix]) 
	image[goodSize++]=originalArray[z*xySize+npix]-reconArray[z*xySize+npix];
    if(goodSize>0) imageSigma[z] = findStddev(image,goodSize);
    else imageSigma[z] = 1.;
  }
  delete [] image;
  // Next, do source finding.
  long *imdim = new long[2];
  imdim[0] = dim[0]; imdim[1] = dim[1];
  Image *channelImage = new Image(imdim);
  channelImage->saveParam(par);
  delete [] imdim;
  
  for(int z=0; z<zdim; z++){  // loop over all channels

    if( par.isVerbose() && ((100*(z+1)/zdim)%5 == 0) ){
      std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b|";
      for(int i=0;i<(100*(z+1)/zdim)/5;i++)  std::cout << "#";
      for(int i=(100*(z+1)/zdim)/5;i<20;i++) std::cout << " ";
      std::cout << "|" << std::flush;
    }

    if( !par.getFlagMW() || (z<par.getMinMW()) || (z>par.getMaxMW()) ){
      // purpose of this is to ignore the Milky Way channels, but only if we are flagging them

      channelImage->extractImage(reconArray,dim,z);
      channelImage->setStats(imageMedian[z],imageSigma[z],par.getCut());
      if(par.getFlagFDR()) channelImage->setupFDR();
      channelImage->setMinSize(par.getMinPix());
      channelImage->lutz_detect();
      for(int obj=0;obj<channelImage->getNumObj();obj++){
	Detection *object = new Detection;
	*object = channelImage->getObject(obj);
	// Fix up z coordinates of each pixel to match original array (x & y are fine)
	for(int pix=0;pix<object->getSize();pix++){
	  object->setZ(pix, z);
	  object->setF(pix, originalArray[object->getX(pix)+object->getY(pix)*dim[0]+z*xySize]); 
	  // NB: set F to the original value, not the recon value.
	}
	object->addOffsets(par);
	object->calcParams();
	mergeIntoList(*object,outputList,par);
	delete object;
      }
      channelImage->clearDetectionList();

    }

  }
  delete channelImage;
  delete [] imageMedian;
  delete [] imageSigma;


  std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bFound " << outputList.size() - num 
	    << ".                                           "  << std::endl << std::flush;

  delete [] isGood;

  return outputList;
}
