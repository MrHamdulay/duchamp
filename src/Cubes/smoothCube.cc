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
#include <duchamp.hh>
#include <Cubes/cubes.hh>
#include <Detection/detection.hh>
#include <PixelMap/Object2D.hh>
#include <Utils/feedback.hh>
#include <Utils/Hanning.hh>
#include <Utils/GaussSmooth.hh>
#include <Utils/Statistics.hh> 
#include <Utils/utils.hh>

void Cube::SmoothSearch()
{
  /**
   * The Cube is first smoothed, using Cube::SmoothCube().
   * It is then searched, using search3DArray()
   * The resulting object list is stored in the Cube, and outputted
   *  to the log file if the user so requests.
   */

  if(!this->par.getFlagSmooth()){
    duchampWarning("SmoothSearch",
		   "FlagSmooth not set! Using basic CubicSearch.\n");
    this->CubicSearch();
  }
  else{    

    this->SmoothCube();
  
    if(this->par.isVerbose()) std::cout << "  ";

    this->setCubeStats();

//       this->Stats.scaleNoise(1./gauss.getStddevScale());

    if(this->par.isVerbose()) std::cout << "  Searching... " << std::flush;
  
    *(this->objectList) = search3DArraySimple(this->axisDim,this->recon,
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
  /**
   *  Switching function that chooses the appropriate function with
   *  which to smooth the cube, based on the Param::smoothType
   *  parameter.
   *
   */
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
  /**
   *   A function that smoothes each spectrum in the cube using the 
   *    Hanning smoothing function. The degree of smoothing is given
   *    by the parameter Param::hanningWidth.
   */

  long xySize = this->axisDim[0]*this->axisDim[1];
  long zdim = this->axisDim[2];
  ProgressBar bar;

  if(!this->reconExists && this->par.getSmoothType()=="spectral"){
    if(this->head.getNumAxes() <= 2)
      duchampWarning("SpectralSmooth",
		     "There is no spectral axis, so cannot do the spectal smoothing.\n");
    else{

      Hanning hann(this->par.getHanningWidth());
  
      float *spectrum = new float[this->axisDim[2]];

      if(this->par.isVerbose()) {
	std::cout<<"  Smoothing spectrally... ";
	bar.init(xySize);
      }

      for(int pix=0;pix<xySize;pix++){

	if( this->par.isVerbose() ) bar.update(pix+1);
    
	for(int z=0;z<zdim;z++){
	  if(this->isBlank(z*xySize+pix)) spectrum[z]=0.;
	  else spectrum[z] = this->array[z*xySize+pix];
	}

	float *smoothed = hann.smooth(spectrum,zdim);

	for(int z=0;z<zdim;z++){
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

    if( this->head.getNumAxes() < 2 )
      duchampWarning("SpatialSmooth",
		     "There are not enough axes to do the spatial smoothing.\n");
    else{

      long xySize = this->axisDim[0]*this->axisDim[1];
      long xdim = this->axisDim[0];
      long ydim = this->axisDim[1];
      long zdim = this->axisDim[2];

      ProgressBar bar;

      // if kernMin is negative (not defined), make it equal to kernMaj
      if(this->par.getKernMin() < 0) 
	this->par.setKernMin(this->par.getKernMaj());

      GaussSmooth gauss(this->par.getKernMaj(),
			this->par.getKernMin(),
			this->par.getKernPA());

      if(this->par.isVerbose()) {
	std::cout<<"  Smoothing spatially... " << std::flush;
	bar.init(zdim);
      }

      float *image = new float[xySize];

      for(int z=0;z<zdim;z++){

	if( this->par.isVerbose() ) bar.update(z+1);
      
	for(int pix=0;pix<xySize;pix++) image[pix] = this->array[z*xySize+pix];
    
	bool *mask      = this->par.makeBlankMask(image,xySize);
    
	float *smoothed = gauss.smooth(image,xdim,ydim,mask);
    
	for(int pix=0;pix<xySize;pix++){
	  if(mask[pix])
	    this->recon[z*xySize+pix] = this->array[z*xySize+pix];
	  else
	    this->recon[z*xySize+pix] = smoothed[pix];
	}

	delete [] smoothed;
	delete [] mask;
      }

      delete [] image;
 
      this->reconExists = true;
  
      if(par.isVerbose()) bar.fillSpace("All Done.\n");
    } 
  }
}
//-----------------------------------------------------------

void Cube::SpatialSmoothNSearch()
{

  long xySize = this->axisDim[0]*this->axisDim[1];
  long xdim = this->axisDim[0];
  long ydim = this->axisDim[1];
  long zdim = this->axisDim[2];
  int numFound=0;
  ProgressBar bar;

  GaussSmooth gauss(this->par.getKernMaj(),
		    this->par.getKernMin(),
		    this->par.getKernPA());

  this->Stats.scaleNoise(1./gauss.getStddevScale());
  if(this->par.getFlagFDR()) this->setupFDR();

  if(this->par.isVerbose()) {
    std::cout<<"  Smoothing spatially & searching... " << std::flush;
    bar.init(zdim);
  }

  std::vector <Detection> outputList;
  long *imdim = new long[2];
  imdim[0] = xdim; imdim[1] = ydim;
  Image *channelImage = new Image(imdim);
  delete [] imdim;
  channelImage->saveParam(this->par);
  channelImage->saveStats(this->Stats);
  channelImage->setMinSize(1);

  float *image = new float[xySize];
  float *smoothed;
  bool *mask;
  float median,madfm;//,threshold;
  for(int z=0;z<zdim;z++){

    if( this->par.isVerbose() ) bar.update(z+1);
      
    if(!this->par.isInMW(z)){

      for(int pix=0;pix<xySize;pix++) image[pix] = this->array[z*xySize+pix];

      mask  = this->par.makeBlankMask(image,xySize);

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

      std::vector<PixelInfo::Object2D> objlist = channelImage->lutz_detect();
      numFound += objlist.size();
      for(int obj=0;obj<objlist.size();obj++){
	Detection newObject;
	newObject.pixels().addChannel(z,objlist[obj]);
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
