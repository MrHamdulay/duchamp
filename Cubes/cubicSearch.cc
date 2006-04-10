#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>

void Cube::SimpleSearch3D()
{
  /**
   * Cube::SimpleSearch3D()
   *  A front end to the cubic searching routine that does not
   *  involve any wavelet reconstruction. 
   *  Although if baseline-removal is required that is done prior to searching.
   *  Once searching is complete, the detection map is updated and
   *  the intermediate detections are logged in the log file.
   */

  this->objectList = cubicSearch(this->axisDim,this->array,this->par);
    
  this->updateDetectMap();
  if(this->par.getFlagLog()) this->logDetectionList();

}


vector <Detection> cubicSearch(long *dim, float *Array, Param &par)
{
  /**
   * cubicSearch
   *  Takes a dimension array and data array as input (and Parameter set)
   *  and searches for detections in a combination of 1D and 2D searches.
   *  Returns a vector list of Detections.
   *  No reconstruction is assumed to have taken place, so statistics are
   *  calculated (using robust methods) from the data array itself.
   */

  vector <Detection> outputList;
  int zdim = dim[2];
  int xySize = dim[0] * dim[1];
  int fullSize = zdim * xySize;
  int num = 0;

  float blankPixValue = par.getBlankPixVal();
  bool *isGood = new bool[fullSize];
  for(int pos=0;pos<fullSize;pos++) 
    isGood[pos] = !par.isBlank(Array[pos]);
  
  float dud;

  // FIRST SEARCH --  IN EACH SPECTRUM.
  // FIRST, GET STATS
  if(zdim>1){
    if(par.isVerbose()) std::cout << "  1D: |                    |" << std::flush;
//     if(par.isVerbose()) std::cout << "Done  0%" << "\b\b\b\b\b\b\b\b" << std::flush;
    float *specMedian = new float[xySize];
    float *specSigma = new float[xySize];

    for(int npix=0; npix<xySize; npix++){
      float *spec = new float[zdim];
      int goodSize=0;
      for(int z=0;z<zdim;z++) if(isGood[z*xySize+npix]) spec[goodSize++] = Array[z*xySize+npix];
      if(goodSize>0) findMedianStats(spec,goodSize,specMedian[npix],dud);
      else specMedian[npix] = blankPixValue;
      //     if(goodSize>0) findNormalStats(spec,goodSize,dud,specSigma[npix]);
      if(goodSize>0){
	findMedianStats(spec,goodSize,dud,specSigma[npix]);
	specSigma[npix] /= correctionFactor;
      }
      else specSigma[npix] = 1.;
      delete spec;
    }
    // NEXT, DO SOURCE FINDING
    int numSearches = xySize + zdim;
    for(int npix=0; npix<xySize; npix++){

      if( par.isVerbose() && ((100*(npix+1)/xySize)%5 == 0) ){
	std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b|";
	for(int i=0;i<(100*(npix+1)/xySize)/5;i++) std::cout << "#";
	for(int i=(100*(npix+1)/xySize)/5;i<20;i++) std::cout << " ";
	std::cout << "|" << std::flush;
      }

      float *spec = new float[zdim];
      for(int z=0;z<zdim;z++) spec[z] = Array[z*xySize + npix];
      long *specdim = new long[2];
      specdim[0] = zdim; specdim[1]=1;
      Image *spectrum = new Image(specdim);
      spectrum->saveParam(par);
      spectrum->pars().setBeamSize(2.); // for spectrum, only neighbouring channels correlated
      spectrum->saveArray(spec,zdim);
      spectrum->setStats(specMedian[npix],specSigma[npix],par.getCut());
      if(par.getFlagFDR()) spectrum->setupFDR();
      spectrum->setMinSize(par.getMinChannels());
      //      spectrum->lutz_detect();
      spectrum->spectrumDetect();
      for(int obj=0;obj<spectrum->getNumObj();obj++){
	Detection *object = new Detection;
	*object = spectrum->getObject(obj);
// 	if(par.getFlagGrowth()) growObject(*object,*spectrum);
	for(int pix=0;pix<object->getSize();pix++) {
	  // Fix up coordinates of each pixel to match original array
	  object->setZ(pix, object->getX(pix));
	  object->setX(pix, npix%dim[0]);
	  object->setY(pix, npix/dim[0]);
	}
	object->addOffsets(par);
	object->calcParams();
// 	outputList.push_back(*object);
	mergeIntoList(*object,outputList,par);
	delete object;
      }
      delete spectrum;
      delete spec;
      delete specdim;
    }

    delete [] specMedian;
    delete [] specSigma;

    num = outputList.size();
    if(par.isVerbose()) 
      std::cout <<"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bFound " << num <<";" << std::flush;

  }

  // SECOND SEARCH --  IN EACH CHANNEL
  // FIRST, GET STATS
  if(par.isVerbose()) std::cout << "  2D: |                    |" << std::flush;
//   if(par.isVerbose()) std::cout << "Done  0%" << "\b\b\b\b\b\b\b\b" << std::flush;
  float *imageMedian = new float[zdim];
  float *imageSigma = new float[zdim];
  for(int z=0; z<zdim; z++){
    float *image = new float[xySize];
    int goodSize=0;
    for(int npix=0; npix<xySize; npix++) 
      if(isGood[z*xySize + npix]) image[goodSize++] = Array[z*xySize + npix];
    if(goodSize>0) findMedianStats(image,goodSize,imageMedian[z],dud);
    else imageMedian[z] = blankPixValue;
    if(goodSize>0) findNormalStats(image,goodSize,dud,imageSigma[z]);
    else imageSigma[z] = 1.;
    delete image;
  }
  // NEXT, DO SOURCE FINDING
  bool *doChannel = new bool[zdim];
  for(int z=0;z<zdim;z++) 
    doChannel[z] = !( par.getFlagMW() && (z>=par.getMinMW()) && (z<=par.getMaxMW()) );

  for(int z=0; z<zdim; z++){

    if( par.isVerbose() && ((100*(z+1)/zdim)%5 == 0) ){
      std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b|";
      for(int i=0;i<(100*(z+1)/zdim)/5;i++) std::cout << "#";
      for(int i=(100*(z+1)/zdim)/5;i<20;i++) std::cout << " ";
      std::cout << "|" << std::flush;
    }

    if( doChannel[z] ){

      float *image = new float[xySize];
      for(int npix=0; npix<xySize; npix++) image[npix] = Array[z*xySize + npix];
      long *imdim = new long[2];
      imdim[0] = dim[0]; imdim[1] = dim[1];
      Image *channelImage = new Image(imdim);
      channelImage->saveParam(par);
      channelImage->saveArray(image,xySize);
      channelImage->setStats(imageMedian[z],imageSigma[z],par.getCut());
      if(par.getFlagFDR()) channelImage->setupFDR();
      channelImage->setMinSize(par.getMinPix());
      channelImage->lutz_detect();
      for(int obj=0;obj<channelImage->getNumObj();obj++){
	Detection *object = new Detection;
	*object = channelImage->getObject(obj);
// 	if(par.getFlagGrowth()) growObject(*object,*channelImage);
	// Fix up coordinates of each pixel to match original array
	for(int pix=0;pix<object->getSize();pix++) object->setZ(pix, z);
	object->addOffsets(par);
	object->calcParams();
// 	outputList.push_back(*object);
	mergeIntoList(*object,outputList,par);
	delete object;
      }
      delete image;
      delete channelImage;
      delete imdim;
    }

  }

  if(par.isVerbose())
    std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bFound " << outputList.size() - num 
	      << ".                                           " << std::endl << std::flush;

  delete [] imageMedian;
  delete [] imageSigma;
  delete [] isGood;
  delete [] doChannel;

  return outputList;
}
