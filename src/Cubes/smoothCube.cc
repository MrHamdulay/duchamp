// -----------------------------------------------------------------------
// smoothCube: Smooth a Cube's array, and search for objects.
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
#include <vector>
#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/PixelMap/Object2D.hh>
#include <duchamp/Utils/feedback.hh>
#include <duchamp/Utils/Hanning.hh>
#include <duchamp/Utils/GaussSmooth2D.hh>
#include <duchamp/Utils/Statistics.hh> 
#include <duchamp/Utils/utils.hh>

namespace duchamp
{

void Cube::SmoothSearch()
{
  /// @details
  /// The Cube is first smoothed, using Cube::SmoothCube().
  /// It is then searched, using search3DArray()
  /// The resulting object list is stored in the Cube, and outputted
  ///  to the log file if the user so requests.

  if(!this->par.getFlagSmooth()){
    DUCHAMPWARN("SmoothSearch","FlagSmooth not set! Using basic CubicSearch.");
    this->CubicSearch();
  }
  else{    

    this->SmoothCube();
  
    if(this->par.isVerbose()) std::cout << "  ";

    this->setCubeStats();

    if(this->par.isVerbose()) std::cout << "  Searching... " << std::flush;
  
    *(this->objectList) = search3DArray(this->axisDim,this->recon,
					this->par,this->Stats); 
 
    if(this->par.isVerbose()) std::cout << "  Updating detection map... " 
					<< std::flush;
    this->updateDetectMap();
    if(this->par.isVerbose()) std::cout << "Done.\n";

    if(this->par.getFlagLog()){
      if(this->par.isVerbose()) 
	std::cout << "  Logging intermediate detections... " << std::flush;
      this->logDetectionList();
      if(this->par.isVerbose()) std::cout << "Done.\n";
    }
  
  }

}
//-----------------------------------------------------------

void Cube::SmoothCube()
{
  /// @details
  ///  Switching function that chooses the appropriate function with
  ///  which to smooth the cube, based on the Param::smoothType
  ///  parameter.

  if(this->par.getSmoothType()=="spectral"){
    
    this->SpectralSmooth();
    
  }
  else if(this->par.getSmoothType()=="spatial"){
    
    this->SpatialSmooth();
    
  }
}
//-----------------------------------------------------------

void Cube::SpectralSmooth()
{
  /// @details
  ///   A function that smoothes each spectrum in the cube using the 
  ///    Hanning smoothing function. The degree of smoothing is given
  ///    by the parameter Param::hanningWidth.

  size_t xySize = this->axisDim[0]*this->axisDim[1];
  size_t zdim = this->axisDim[2];
  ProgressBar bar;

  if(!this->reconExists && this->par.getSmoothType()=="spectral"){
    //    if(!this->head.isSpecOK())
    if(!this->head.canUseThirdAxis()){
      DUCHAMPWARN("SpectralSmooth","There is no spectral axis, so cannot do the spectral smoothing.");
    }
    else{

      Hanning hann(this->par.getHanningWidth());
  
      float *spectrum = new float[this->axisDim[2]];

      if(this->par.isVerbose()) {
	std::cout<<"  Smoothing spectrally... ";
	bar.init(xySize);
      }

      for(size_t pix=0;pix<xySize;pix++){

	if( this->par.isVerbose() ) bar.update(pix+1);
    
	for(size_t z=0;z<zdim;z++){
	  if(this->isBlank(z*xySize+pix)) spectrum[z]=0.;
	  else spectrum[z] = this->array[z*xySize+pix];
	}

	float *smoothed = hann.smooth(spectrum,zdim);

	for(size_t z=0;z<zdim;z++){
	  if(this->isBlank(z*xySize+pix)) 
	    this->recon[z*xySize+pix] = this->array[z*xySize+pix];
	  else 
	    this->recon[z*xySize+pix] = smoothed[z];
	}
	delete [] smoothed;
      }
      this->reconExists = true;
      if(this->par.isVerbose()) bar.fillSpace("All Done.\n");

      delete [] spectrum;

    }
  }
}
//-----------------------------------------------------------

void Cube::SpatialSmooth()
{

  if(!this->reconExists && this->par.getSmoothType()=="spatial"){

    if( this->head.getNumAxes() < 2 ){
      DUCHAMPWARN("SpatialSmooth","There are not enough axes to do the spatial smoothing.");
    }
    else{

      size_t xySize = this->axisDim[0]*this->axisDim[1];
      size_t xdim = this->axisDim[0];
      size_t ydim = this->axisDim[1];
      size_t zdim = this->axisDim[2];

      ProgressBar bar;
      bool useBar = this->par.isVerbose() && (zdim > 1);
      
      // if kernMin is negative (not defined), make it equal to kernMaj
      if(this->par.getKernMin() < 0) 
	this->par.setKernMin(this->par.getKernMaj());

      GaussSmooth2D<float> gauss(this->par.getKernMaj(),
			       this->par.getKernMin(),
			       this->par.getKernPA());

      if(this->par.isVerbose()) {
	std::cout<<"  Smoothing spatially... " << std::flush;
	if(useBar) bar.init(zdim);
      }

      // pointer used to point to start of a given channel's image
      //  Can do this because the spatial axes vary the quickest.
      float *image=0;

      for(size_t z=0;z<zdim;z++){

	if(useBar) bar.update(z+1);
      
	// update pointer to point to current channel
	image = this->array + z*xySize;
    
	bool *mask      = this->par.makeBlankMask(image,xySize);
    
	float *smoothed = gauss.smooth(image,xdim,ydim,mask);
    
	for(size_t pix=0;pix<xySize;pix++) 
	    this->recon[z*xySize+pix] = smoothed[pix];

	if(gauss.isAllocated()) delete [] smoothed;
	delete [] mask;
      }

      this->reconExists = true;
  
      if(par.isVerbose()){
	if(useBar) bar.fillSpace("All Done.");
	std::cout << "\n";
      }

    }
  }
}
//-----------------------------------------------------------

void Cube::SpatialSmoothNSearch()
{

  size_t xySize = this->axisDim[0]*this->axisDim[1];
  size_t xdim = this->axisDim[0];
  size_t ydim = this->axisDim[1];
  size_t zdim = this->axisDim[2];
  int numFound=0;
  ProgressBar bar;

  GaussSmooth2D<float> gauss(this->par.getKernMaj(),
			   this->par.getKernMin(),
			   this->par.getKernPA());

  this->Stats.scaleNoise(1./gauss.getStddevScale());
  if(this->par.getFlagFDR()) this->setupFDR();

  if(this->par.isVerbose()) {
    std::cout<<"  Smoothing spatially & searching... " << std::flush;
    bar.init(zdim);
  }

  std::vector <Detection> outputList;
  size_t *imdim = new size_t[2];
  imdim[0] = xdim; imdim[1] = ydim;
  Image *channelImage = new Image(imdim);
  delete [] imdim;
  channelImage->saveParam(this->par);
  channelImage->saveStats(this->Stats);
  channelImage->setMinSize(1);

  float *image = new float[xySize];
  float *smoothed=0;
  bool *mask=0;
  float median,madfm;//,threshold;
//  std::vector<bool> flaggedChans=this->par.getChannelFlags(zdim);
  for(size_t z=0;z<zdim;z++){

    if( this->par.isVerbose() ) bar.update(z+1);
      
    // if(!this->par.isInMW(z)){
//    if(!flaggedChans[z]){
    if(!this->par.isFlaggedChannel(z)){

      for(size_t pix=0;pix<xySize;pix++) image[pix] = this->array[z*xySize+pix];

      mask  = this->par.makeBlankMask(image+z*xySize,xySize);

      smoothed = gauss.smooth(image,xdim,ydim,mask);
      
      //     for(int pix=0;pix<xySize;pix++)
      //       this->recon[z*xySize+pix] = smoothed[pix];

      findMedianStats(smoothed,xySize,mask,median,madfm);
      //       threshold = median+this->par.getCut()*Statistics::madfmToSigma(madfm);
      //       for(int i=0;i<xySize;i++)
      // 	if(smoothed[i]<threshold) image[i] = this->Stats.getMiddle();
      //       channelImage->saveArray(image,xySize);


      //       channelImage->stats().setMadfm(madfm);
      //       channelImage->stats().setMedian(median);
      //       channelImage->stats().setThresholdSNR(this->par.getCut());
      channelImage->saveArray(smoothed,xySize);

      std::vector<PixelInfo::Object2D> objlist = channelImage->findSources2D();
      std::vector<PixelInfo::Object2D>::iterator obj;
      numFound += objlist.size();
      for(obj=objlist.begin();obj<objlist.end();obj++){
	Detection newObject;
	newObject.addChannel(z,*obj);
	newObject.setOffsets(this->par);
	mergeIntoList(newObject,outputList,this->par);
      }

    }
    
  }

  delete [] smoothed;
  delete [] mask;
  delete [] image;
  delete channelImage;
    
  //   this->reconExists = true;
  if(par.isVerbose()){
    bar.fillSpace("Found ");
    std::cout << numFound << ".\n";
  }
    
  *this->objectList = outputList;

}

}
