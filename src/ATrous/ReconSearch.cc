#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <duchamp.hh>
#include <Cubes/cubes.hh>
#include <Detection/detection.hh>
#include <ATrous/atrous.hh>
#include <Utils/utils.hh>

void Cube::ReconSearch()
{
  /**
   * Cube::ReconSearch()
   *   The Cube is first reconstructed, using Cube::ReconCube().
   *   It is then searched, using searchReconArray.
   *   The resulting object list is stored in the Cube, and outputted
   *    to the log file if the user so requests.
   */
  
  this->ReconCube();

  this->setCubeStats();
    
  std::cout << "  Searching... " << std::flush;
  
  this->objectList = searchReconArray(this->axisDim,this->array,
				      this->recon,this->par);

  this->updateDetectMap();
  if(this->par.getFlagLog()) this->logDetectionList();

}

/////////////////////////////////////////////////////////////////////////////
void Cube::ReconCube()
{
  /**
   * Cube::ReconSearch()
   *   A front-end to the various reconstruction functions, the choice of 
   *    which is determined by the use of the reconDim parameter.
   *   Differs from ReconSearch only in that no searching is done.
   */
  int dimRecon = this->par.getReconDim();
  // Test whether we have eg. an image, but have requested a 3-d 
  //  reconstruction.
  // If dimension of data array is less than dimRecon, change dimRecon 
  //  to the dimension of the array.
  int numGoodDim = 0;
  for(int i=0;i<this->numDim;i++) if(this->axisDim[i]>1) numGoodDim++;
  if(numGoodDim<dimRecon) dimRecon = numGoodDim;
  this->par.setReconDim(dimRecon);

  switch(dimRecon)
    {
    case 1: this->ReconCube1D(); break;
    case 2: this->ReconCube2D(); break;
    case 3: this->ReconCube3D(); break;
    default:
      if(dimRecon<=0){
	std::stringstream errmsg;
	errmsg << "reconDim (" << dimRecon
	       << ") is less than 1. Performing 1-D reconstruction.\n";
	duchampWarning("ReconCube", errmsg.str());
	this->par.setReconDim(1);
	this->ReconCube1D();
      }
      else if(dimRecon>3){ 
	//this probably won't happen with new code above, but just in case...
	std::stringstream errmsg;
	errmsg << "reconDim (" << dimRecon
	       << ") is more than 3. Performing 3-D reconstruction.\n";
	duchampWarning("ReconCube", errmsg.str());
	this->par.setReconDim(3);
	this->ReconCube3D();
      }
      break;
    }
}

/////////////////////////////////////////////////////////////////////////////
void Cube::ReconCube1D()
{
  /**
   * Cube::ReconCube1D()
   *   This reconstructs a cube by performing a 1D a trous reconstruction
   *   in the spectrum of each spatial pixel.
   */
  long xySize = this->axisDim[0] * this->axisDim[1];

  long zdim = this->axisDim[2];

  ProgressBar bar;
  if(!this->reconExists){
    std::cout<<"  Reconstructing... ";
    if(par.isVerbose()) bar.init(xySize);
    for(int npix=0; npix<xySize; npix++){

      if( par.isVerbose() ) bar.update(npix+1);

      float *spec = new float[zdim];
      float *newSpec = new float[zdim];
      for(int z=0;z<zdim;z++) spec[z] = this->array[z*xySize + npix];
      bool verboseFlag = this->par.isVerbose();
      this->par.setVerbosity(false);
      atrous1DReconstruct(this->axisDim[2],spec,newSpec,this->par);
      this->par.setVerbosity(verboseFlag);
      for(int z=0;z<zdim;z++) this->recon[z*xySize+npix] = newSpec[z];
      delete [] spec,newSpec;
    }
    this->reconExists = true;
    bar.rewind();
    std::cout << "  All Done.";
    printSpace(22);
    std::cout << "\n";
  }

}

/////////////////////////////////////////////////////////////////////////////
void Cube::ReconCube2D()
{
  /**
   * Cube::ReconCube2D()
   *   This reconstructs a cube by performing a 2D a trous reconstruction
   *   in each spatial image (ie. each channel map) of the cube.
   */
  long xySize = this->axisDim[0] * this->axisDim[1];
  ProgressBar bar;

  if(!this->reconExists){
    std::cout<<"  Reconstructing... ";
    if(par.isVerbose()) bar.init(this->axisDim[2]);
    for(int z=0;z<this->axisDim[2];z++){

      if( par.isVerbose() ) bar.update((z+1));

      if(!this->par.isInMW(z)){
	float *im = new float[xySize];
	float *newIm = new float[xySize];
	for(int npix=0; npix<xySize; npix++) 
	  im[npix] = this->array[z*xySize+npix];
	bool verboseFlag = this->par.isVerbose();
	this->par.setVerbosity(false);
	atrous2DReconstruct(this->axisDim[0],this->axisDim[1],
			    im,newIm,this->par);
	this->par.setVerbosity(verboseFlag);
	for(int npix=0; npix<xySize; npix++) 
	  this->recon[z*xySize+npix] = newIm[npix];
	delete [] im, newIm;
      }
      else {
	for(int i=z*xySize; i<(z+1)*xySize; i++) 
	  this->recon[i] = this->array[i];
      }
    }
    this->reconExists = true;
    bar.rewind();
    std::cout << "  All Done.";
    printSpace(22);
    std::cout << "\n";
  }
}

/////////////////////////////////////////////////////////////////////////////
void Cube::ReconCube3D()
{
  /**
   * Cube::ReconCube3D()
   *   This performs a full 3D a trous reconstruction of the cube
   */
  if(this->axisDim[2]==1) this->ReconCube2D();
  else {

    if(!this->reconExists){
      std::cout<<"  Reconstructing... "<<std::flush;
      atrous3DReconstruct(this->axisDim[0],this->axisDim[1],this->axisDim[2],
			  this->array,this->recon,this->par);
      this->reconExists = true;
      std::cout << "  All Done.";
      printSpace(22);
      std::cout << "\n";
    }

  }
}

/////////////////////////////////////////////////////////////////////////////
vector <Detection> searchReconArray(long *dim, float *originalArray, 
				    float *reconArray, Param &par)
{
  /**
   * searchReconArray(long *dim, float *originalArray, 
   *                  float *reconArray, Param &par)
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
  vector <Detection> outputList;
  long zdim = dim[2];
  long xySize = dim[0] * dim[1];
//   long fullSize = zdim * xySize;
  int num=0// , goodSize
    ;

//   bool *isGood = new bool[fullSize];
//   for(int pos=0;pos<fullSize;pos++){
//     isGood[pos] = ((!par.isBlank(originalArray[pos])) && 
//       (!par.isInMW(pos/xySize)));
//   }
  bool *doPixel = new bool[xySize];
  for(int npix=0; npix<xySize; npix++){
    doPixel[npix] = false;
    for(int z=0;z<zdim;z++) {
      doPixel[npix] = doPixel[npix] || 
	(!par.isBlank(originalArray[npix]) && !par.isInMW(z));
    }
    // doPixel[i] is false only when there are no good pixels in spectrum
    //  of pixel #i.
  }
 
  ProgressBar bar;
  // First search --  in each spectrum.
  if(zdim > 1){
    if(par.isVerbose()){
      std::cout << "1D: ";
      bar.init(xySize);
    }

    for(int npix=0; npix<xySize; npix++){

      if( par.isVerbose() ) bar.update(npix+1);

      if(doPixel[npix]){
	
	// First, get stats
// 	float *spec = new float[zdim];
// 	float specMedian,specSigma;
// 	goodSize=0;
// 	for(int z=0;z<zdim;z++) {
// 	  if(isGood[z*xySize+npix]) 
// 	    spec[goodSize++] = originalArray[z*xySize+npix];
// 	}
// 	specMedian = findMedian(spec,goodSize);
// 	goodSize=0;
// 	for(int z=0;z<zdim;z++) {
// 	  if(isGood[z*xySize+npix]) 
// 	    spec[goodSize++] = originalArray[z*xySize+npix] -
// 	      reconArray[z*xySize+npix];
// 	}
// 	specSigma = findMADFM(spec,goodSize) / correctionFactor;
// 	delete [] spec;
	
	// Next, do source finding.
	long *specdim = new long[2];
	specdim[0] = zdim; specdim[1]=1;
	Image *spectrum = new Image(specdim);
	delete [] specdim;
	spectrum->saveParam(par);
	spectrum->pars().setBeamSize(2.); 
	// beam size: for spectrum, only neighbouring channels correlated
	spectrum->extractSpectrum(reconArray,dim,npix);
	spectrum->removeMW(); // only works if flagMW is true
// 	spectrum->setStats(specMedian,specSigma,par.getCut());
	if(par.getFlagFDR()) spectrum->setupFDR();
	spectrum->setMinSize(par.getMinChannels());
	spectrum->spectrumDetect();
	num += spectrum->getNumObj();
	for(int obj=0;obj<spectrum->getNumObj();obj++){
	  Detection *object = new Detection;
	  *object = spectrum->getObject(obj);
	  for(int pix=0;pix<object->getSize();pix++) {
	    // Fix up coordinates of each pixel to match original array
	    object->setZ(pix, object->getX(pix));
	    object->setX(pix, npix%dim[0]);
	    object->setY(pix, npix/dim[0]);
	    object->setF(pix, originalArray[object->getX(pix)+
					    object->getY(pix)*dim[0]+
					    object->getZ(pix)*xySize]); 
	    // NB: set F to the original value, not the recon value.
	  }
	  object->addOffsets(par);
	  object->calcParams();
	  mergeIntoList(*object,outputList,par);
	  delete object;
	}
	delete spectrum;
      }
    }

    num = outputList.size();
    if(par.isVerbose()) {
      bar.rewind();
      std::cout <<"Found " << num <<"; " << std::flush;
    }
  }

  // Second search --  in each channel
  if(par.isVerbose()){
    std::cout << "2D: ";
    bar.init(zdim);
  }

  num = 0;

  for(int z=0; z<zdim; z++){  // loop over all channels

    if( par.isVerbose() ) bar.update(z+1);

    if(!par.isInMW(z)){ 

      // purpose of this is to ignore the Milky Way channels 
      //  if we are flagging them

      //  First, get stats
//       float *image = new float[xySize];
//       float imageMedian,imageSigma;
//       goodSize=0;
//       for(int npix=0; npix<xySize; npix++) {
// 	if(isGood[z*xySize + npix]) 
// 	  image[goodSize++] = originalArray[z*xySize + npix];
//       }
//       imageMedian = findMedian(image,goodSize);
//       goodSize=0;
//       for(int npix=0; npix<xySize; npix++) 
// 	if(isGood[z*xySize+npix]) 
// 	  image[goodSize++]=originalArray[z*xySize+npix]-
// 	    reconArray[z*xySize+npix];
//       imageSigma = findMADFM(image,goodSize) / correctionFactor;
//       delete [] image;

      // Next, do source finding.
      long *imdim = new long[2];
      imdim[0] = dim[0]; imdim[1] = dim[1];
      Image *channelImage = new Image(imdim);
      channelImage->saveParam(par);
      delete [] imdim;
      channelImage->extractImage(reconArray,dim,z);
//       channelImage->setStats(imageMedian,imageSigma,par.getCut());
      if(par.getFlagFDR()) channelImage->setupFDR();
      channelImage->setMinSize(par.getMinPix());
      channelImage->lutz_detect();
      num += channelImage->getNumObj();
      for(int obj=0;obj<channelImage->getNumObj();obj++){
	Detection *object = new Detection;
	*object = channelImage->getObject(obj);
	// Fix up z coordinates of each pixel to match original array 
	//   (x & y are fine)
	for(int pix=0;pix<object->getSize();pix++){
	  object->setZ(pix, z);
	  object->setF(pix, originalArray[object->getX(pix)+
					  object->getY(pix)*dim[0]+
					  z*xySize]); 
	  // NB: set F to the original value, not the recon value.
	}
	object->addOffsets(par);
	object->calcParams();
	mergeIntoList(*object,outputList,par);
	delete object;
      }
      delete channelImage;
   }

  }

  bar.rewind();
  std::cout << "Found " << num << ".";
  printSpace(22);  
  std::cout << std::endl << std::flush;

//   delete [] isGood;
  delete [] doPixel;

  return outputList;
}
