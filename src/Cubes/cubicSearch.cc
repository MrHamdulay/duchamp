#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>

void Cube::CubicSearch()
{
  /**
   * Cube::SimpleSearch3D()
   *  A front end to the cubic searching routine that does not
   *  involve any wavelet reconstruction. 
   *  If baseline-removal is required that is done prior to searching.
   *  Once searching is complete, the detection map is updated and
   *  the intermediate detections are logged in the log file.
   */

  this->objectList = search3DArray(this->axisDim,this->array,this->par);
    
  this->updateDetectMap();
  if(this->par.getFlagLog()) this->logDetectionList();

}


vector <Detection> search3DArray(long *dim, float *Array, Param &par)
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
  long zdim = dim[2];
  long xySize = dim[0] * dim[1];
  long fullSize = zdim * xySize;
  int num = 0;

  float blankPixValue = par.getBlankPixVal();
  bool *isGood = new bool[fullSize];
  for(int pos=0;pos<fullSize;pos++){ 
    isGood[pos] = !par.isBlank(Array[pos]) && !par.isInMW(pos/xySize);
  }
  bool *doChannel = new bool[xySize];
  int goodSize=0;
  for(int npix=0; npix<xySize; npix++){
    for(int z=0;z<zdim;z++) if(isGood[z*xySize+npix]) goodSize++;
    if(goodSize==0) doChannel[npix] = false;
    else doChannel[npix] = true;
  }
  
  // FIRST SEARCH --  IN EACH SPECTRUM.
  if(zdim>1){
    if(par.isVerbose()) std::cout << "  1D: |                    |" 
				  << std::flush;

    for(int npix=0; npix<xySize; npix++){

      if( par.isVerbose() && ((100*(npix+1)/xySize)%5 == 0) ){
	std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b|";
	for(int i=0;i<(100*(npix+1)/xySize)/5;i++) std::cout << "#";
	for(int i=(100*(npix+1)/xySize)/5;i<20;i++) std::cout << " ";
	std::cout << "|" << std::flush;
      }

      if(doChannel[npix]){

	float *spec = new float[zdim];
	float specMedian,specSigma;
	goodSize=0;
	for(int z=0;z<zdim;z++) 
	  if(isGood[z*xySize+npix]) spec[goodSize++] = Array[z*xySize+npix];
	findMedianStats(spec,goodSize,specMedian,specSigma);
	specSigma /= correctionFactor;


	long *specdim = new long[2];
	specdim[0] = zdim; specdim[1]=1;
	Image *spectrum = new Image(specdim);
	delete specdim;
	spectrum->saveParam(par);
	spectrum->pars().setBeamSize(2.); 
	// beam size: for spectrum, only neighbouring channels correlated
	spectrum->extractSpectrum(Array,dim,npix);
	spectrum->removeMW(); // only works if flagMW is true
	spectrum->setStats(specMedian,specSigma,par.getCut());
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
	  }
	  object->addOffsets(par);
	  object->calcParams();
	  mergeIntoList(*object,outputList,par);
	  delete object;
	}
	delete spectrum;
      }
    }

    //    num = outputList.size();
    if(par.isVerbose()) 
      std::cout <<"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bFound " 
		<< num <<";" << std::flush;

  }

  // SECOND SEARCH --  IN EACH CHANNEL
  if(par.isVerbose()) std::cout << "  2D: |                    |" 
				<< std::flush;
  num = 0;

  for(int z=0; z<zdim; z++){

    if( par.isVerbose() && ((100*(z+1)/zdim)%5 == 0) ){
      std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b|";
      for(int i=0;i<(100*(z+1)/zdim)/5;i++) std::cout << "#";
      for(int i=(100*(z+1)/zdim)/5;i<20;i++) std::cout << " ";
      std::cout << "|" << std::flush;
    }

    if(!par.isInMW(z)){

      float *image = new float[xySize];
      float imageMedian, imageSigma;
      goodSize=0;
      for(int npix=0; npix<xySize; npix++) {
	if(isGood[z*xySize + npix]) 
	  image[goodSize++] = Array[z*xySize + npix];
      }
      findMedianStats(image,goodSize,imageMedian,imageSigma);
      imageSigma /= correctionFactor;

      long *imdim = new long[2];
      imdim[0] = dim[0]; imdim[1] = dim[1];
      Image *channelImage = new Image(imdim);
      delete imdim;
      channelImage->saveParam(par);
      channelImage->extractImage(Array,dim,z);
      channelImage->setStats(imageMedian,imageSigma,par.getCut());
      if(par.getFlagFDR()) channelImage->setupFDR();
      channelImage->setMinSize(par.getMinPix());
      channelImage->lutz_detect();
      num += channelImage->getNumObj();
      for(int obj=0;obj<channelImage->getNumObj();obj++){
	Detection *object = new Detection;
	*object = channelImage->getObject(obj);
	// Fix up coordinates of each pixel to match original array
	for(int pix=0;pix<object->getSize();pix++) object->setZ(pix, z);
	object->addOffsets(par);
	object->calcParams();
	mergeIntoList(*object,outputList,par);
	delete object;
      }
      delete channelImage;
    }

  }

  if(par.isVerbose())
    std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bFound " 
	      << num 
	      << ".                                           " 
	      << std::endl << std::flush;

  delete [] isGood;
  delete [] doChannel;
  
  return outputList;
}
