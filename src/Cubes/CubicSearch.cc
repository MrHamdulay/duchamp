#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <param.hh>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>
#include <Utils/feedback.hh>
#include <Utils/Statistics.hh>

void Cube::CubicSearch()
{
  /**
   *  A front end to the cubic searching routine that does not
   *  involve any wavelet reconstruction. 
   *  The statistics of the cube are calculated first of all.
   *  If baseline-removal is required that is done prior to searching.
   *  Once searching is complete, the detection map is updated and
   *  the intermediate detections are logged in the log file.
   */

  if(this->par.isVerbose()) std::cout << "  ";
  this->setCubeStats();
    
  this->objectList = search3DArray(this->axisDim,this->array,
				   this->par,this->Stats);

  this->updateDetectMap();
  if(this->par.getFlagLog()) this->logDetectionList();

}


vector <Detection> search3DArray(long *dim, float *Array, Param &par,
				 StatsContainer<float> &stats)
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
  int num = 0;

  bool *doPixel = new bool[xySize];
  int goodSize=0;
  for(int npix=0; npix<xySize; npix++){
    doPixel[npix] = false;
    for(int z=0;z<zdim;z++){
      doPixel[npix] = doPixel[npix] || 
	(!par.isBlank(Array[npix]) && !par.isInMW(z));
    }
    // doPixel[i] is false only when there are no good pixels in spectrum
    //  of pixel #i.
  }

  ProgressBar bar;
  // FIRST SEARCH --  IN EACH SPECTRUM.
  if(zdim>1){
    if(par.isVerbose()) {
      std::cout << "  1D: ";
      bar.init(xySize);
    }

    for(int npix=0; npix<xySize; npix++){

      if( par.isVerbose() ) bar.update(npix+1);

      if(doPixel[npix]){

	long *specdim = new long[2];
	specdim[0] = zdim; specdim[1]=1;
	Image *spectrum = new Image(specdim);
	delete [] specdim;
	spectrum->saveParam(par);
	spectrum->saveStats(stats);
	spectrum->pars().setBeamSize(2.); 
	// beam size: for spectrum, only neighbouring channels correlated
	spectrum->extractSpectrum(Array,dim,npix);
	spectrum->removeMW(); // only works if flagMW is true
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
	  object->setOffsets(par);
	  object->calcParams();
	  mergeIntoList(*object,outputList,par);
	  delete object;
	}
	delete spectrum;
      }
    }

    //    num = outputList.size();
    if(par.isVerbose()) {
      bar.fillSpace("Found ");
      std::cout << num <<";" << std::flush;
    }

  }

  // SECOND SEARCH --  IN EACH CHANNEL
  if(par.isVerbose()){
    std::cout << "  2D: ";
    bar.init(zdim);
  }
  
  num = 0;

  for(int z=0; z<zdim; z++){

    if( par.isVerbose() ) bar.update(z+1);

    if(!par.isInMW(z)){

      long *imdim = new long[2];
      imdim[0] = dim[0]; imdim[1] = dim[1];
      Image *channelImage = new Image(imdim);
      delete [] imdim;
      channelImage->saveParam(par);
      channelImage->saveStats(stats);
      channelImage->extractImage(Array,dim,z);
      channelImage->setMinSize(par.getMinPix());
      channelImage->lutz_detect();
      num += channelImage->getNumObj();
      for(int obj=0;obj<channelImage->getNumObj();obj++){
	Detection *object = new Detection;
	*object = channelImage->getObject(obj);
	// Fix up coordinates of each pixel to match original array
	for(int pix=0;pix<object->getSize();pix++) object->setZ(pix, z);
	object->setOffsets(par);
	object->calcParams();
	mergeIntoList(*object,outputList,par);
	delete object;
      }
      delete channelImage;
    }

  }

  if(par.isVerbose()){
    bar.fillSpace("Found ");
    std::cout << num << ".\n";
  }

  delete [] doPixel;
  
  return outputList;
}
