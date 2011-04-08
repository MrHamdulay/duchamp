// -----------------------------------------------------------------------
// ReconSearch.cc: Searching a wavelet-reconstructed cube.
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
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <duchamp/duchamp.hh>
#include <duchamp/PixelMap/Object3D.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/ATrous/atrous.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/feedback.hh>
#include <duchamp/Utils/Statistics.hh>

using namespace PixelInfo;
using namespace Statistics;

namespace duchamp
{

  void Cube::ReconSearch()
  {
    /// The Cube is first reconstructed, using Cube::ReconCube().
    /// The statistics of the cube are calculated next.
    /// It is then searched, using searchReconArray.
    /// The resulting object list is stored in the Cube, and outputted
    ///  to the log file if the user so requests.

    if(!this->par.getFlagATrous()){
      duchampWarning("ReconSearch",
		     "You've requested a reconSearch, but not allocated space for the reconstructed array.\nDoing the basic CubicSearch.\n");
      this->CubicSearch();
    }
    else {
  
      this->ReconCube();

      if(this->par.isVerbose()) std::cout << "  ";

      this->setCubeStats();
    
      if(this->par.isVerbose()) std::cout << "  Searching... " << std::flush;
  
      *this->objectList = searchReconArray(this->axisDim,this->array,
					   this->recon,this->par,this->Stats);

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

  /////////////////////////////////////////////////////////////////////////////
  void Cube::ReconCube()
  {
    /// A front-end to the various reconstruction functions, the choice of 
    ///  which is determined by the use of the reconDim parameter.
    /// Differs from ReconSearch only in that no searching is done.

    int dimRecon = this->par.getReconDim();
    // Test whether we have eg. an image, but have requested a 3-d 
    //  reconstruction.
    // If dimension of data array is less than dimRecon, change dimRecon 
    //  to the dimension of the array.
    int numGoodDim = this->head.getNumAxes();
    if(numGoodDim<dimRecon){
      dimRecon = numGoodDim;
      this->par.setReconDim(dimRecon);
      std::stringstream errmsg;
      errmsg << "You requested a " << dimRecon << "-dimensional reconstruction,"
	     << " but the FITS file is only " << numGoodDim << "-dimensional.\n"
	     << "Changing reconDim to " << numGoodDim << ".\n";
      duchampWarning("Reconstruction",errmsg.str());
    }

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
	  duchampWarning("Reconstruction", errmsg.str());
	  this->par.setReconDim(1);
	  this->ReconCube1D();
	}
	else if(dimRecon>3){ 
	  //this probably won't happen with new code above, but just in case...
	  std::stringstream errmsg;
	  errmsg << "reconDim (" << dimRecon
		 << ") is more than 3. Performing 3-D reconstruction.\n";
	  duchampWarning("Reconstruction", errmsg.str());
	  this->par.setReconDim(3);
	  this->ReconCube3D();
	}
	break;
      }
  }

  /////////////////////////////////////////////////////////////////////////////
  void Cube::ReconCube1D()
  {
    /// This reconstructs a cube by performing a 1D a trous reconstruction
    ///  in the spectrum of each spatial pixel.

    long xySize = this->axisDim[0] * this->axisDim[1];

    unsigned long zdim = this->axisDim[2];

    ProgressBar bar;
    if(!this->reconExists){
      if(this->par.isVerbose()){
	std::cout<<"  Reconstructing... ";
	bar.init(xySize);
      }
      for(int npix=0; npix<xySize; npix++){

	if( this->par.isVerbose() ) bar.update(npix+1);

	float *spec = new float[zdim];
	float *newSpec = new float[zdim];
	for(size_t z=0;z<zdim;z++) spec[z] = this->array[z*xySize + npix];
	bool verboseFlag = this->par.isVerbose();
	this->par.setVerbosity(false);
	atrous1DReconstruct(zdim,spec,newSpec,this->par);
	this->par.setVerbosity(verboseFlag);
	for(size_t z=0;z<zdim;z++) this->recon[z*xySize+npix] = newSpec[z];
	delete [] spec;
	delete [] newSpec;
      }
      this->reconExists = true;
      if(this->par.isVerbose()){
	bar.fillSpace(" All Done.");
	printSpace(22);
	std::cout << "\n";
      }
    }

  }

  /////////////////////////////////////////////////////////////////////////////
  void Cube::ReconCube2D()
  {
    /// This reconstructs a cube by performing a 2D a trous reconstruction
    ///  in each spatial image (ie. each channel map) of the cube.

    long xySize = this->axisDim[0] * this->axisDim[1];
    ProgressBar bar;
    bool useBar = (this->axisDim[2]>1);
    unsigned long xdim=this->axisDim[0],ydim=this->axisDim[1];

    if(!this->reconExists){
      if(this->par.isVerbose()) std::cout<<"  Reconstructing... ";
      if(useBar&&this->par.isVerbose()) bar.init(this->axisDim[2]);
      for(int z=0;z<this->axisDim[2];z++){

	if( this->par.isVerbose() && useBar ) bar.update((z+1));

	if(!this->par.isInMW(z)){
	  float *im = new float[xySize];
	  float *newIm = new float[xySize];
	  for(int npix=0; npix<xySize; npix++) 
	    im[npix] = this->array[z*xySize+npix];
	  bool verboseFlag = this->par.isVerbose();
	  this->par.setVerbosity(false);
	  atrous2DReconstruct(xdim,ydim,im,newIm,this->par);
	  this->par.setVerbosity(verboseFlag);
	  for(int npix=0; npix<xySize; npix++) 
	    this->recon[z*xySize+npix] = newIm[npix];
	  delete [] im;
	  delete [] newIm;
	}
	else {
	  for(int i=z*xySize; i<(z+1)*xySize; i++) 
	    this->recon[i] = this->array[i];
	}
      }
      this->reconExists = true;
      if(this->par.isVerbose()) {
	if(useBar) bar.fillSpace(" All Done.");
	printSpace(22);
	std::cout << "\n";
      }
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  void Cube::ReconCube3D()
  {
    /// This performs a full 3D a trous reconstruction of the cube

    if(this->axisDim[2]==1) this->ReconCube2D();
    else {
      unsigned long xdim=this->axisDim[0],ydim=this->axisDim[1],zdim=this->axisDim[2];
      if(!this->reconExists){
	if(this->par.isVerbose()) std::cout<<"  Reconstructing... "<<std::flush;
	atrous3DReconstruct(xdim,ydim,zdim,this->array,this->recon,this->par);
	this->reconExists = true;
	if(this->par.isVerbose()) {
	  std::cout << "  All Done.";
	  printSpace(22);
	  std::cout << "\n";
	}
      }

    }
  }

  /////////////////////////////////////////////////////////////////////////////
  std::vector <Detection> searchReconArray(long *dim, float *originalArray, 
					   float *reconArray, Param &par,
					   StatsContainer<float> &stats)
  {

  if(par.getSearchType()=="spectral")
    return searchReconArraySpectral(dim,originalArray,reconArray,par,stats);
  else if(par.getSearchType()=="spatial")
    return searchReconArraySpatial(dim,originalArray,reconArray,par,stats);
  else{
    std::stringstream ss;
    ss << "Unknown search type : " << par.getSearchType();
    duchampError("searchReconArray",ss.str());
    return std::vector<Detection>(0);
  }
  }
  /////////////////////////////////////////////////////////////////////////////
  std::vector <Detection> searchReconArraySpectral(long *dim, float *originalArray, 
						   float *reconArray, Param &par,
						   StatsContainer<float> &stats)
  {
    ///  This searches for objects in a cube that has been reconstructed.
    /// 
    ///  The search is conducted just in each spatial pixel (xdim*ydim 1D 
    ///   searches).
    ///  The searches are done on the reconstructed array, although the detected
    ///   objects have fluxes drawn from the corresponding pixels of the original
    ///   array.
    /// 
    ///  \param dim Array of dimension sizes
    ///  \param originalArray Original, un-reconstructed image array.
    ///  \param reconArray Reconstructed image array
    ///  \param par The Param set.
    ///  \param stats The StatsContainer that defines what a detection is.
    /// 
    ///  \return A vector of Detections resulting from the search.

    std::vector <Detection> outputList;
    long zdim = dim[2];
    long xySize = dim[0] * dim[1];
    int num=0;

    // First search --  in each spectrum.
    if(zdim > 1){

      ProgressBar bar;
      if(par.isVerbose()) bar.init(xySize);

      bool *doPixel = new bool[xySize];
      // doPixel is a bool array to say whether to look in a given spectrum
      for(int npix=0; npix<xySize; npix++){
	doPixel[npix] = false;
	for(int z=0;z<zdim;z++) {
	  doPixel[npix] = doPixel[npix] || 
	    (!par.isBlank(originalArray[npix]) && !par.isInMW(z));
	}
	// doPixel[i] is false only when there are no good pixels in spectrum
	//  of pixel #i.
      }

      long *specdim = new long[2];
      specdim[0] = zdim; specdim[1]=1;
      Image *spectrum = new Image(specdim);
      delete [] specdim;
      spectrum->saveParam(par);
      spectrum->saveStats(stats);
      spectrum->setMinSize(par.getMinChannels());
      // NB the beam is not used after this point
      // spectrum->pars().setBeamSize(2.); 
      // // beam size: for spectrum, only neighbouring channels correlated

      for(int y=0; y<dim[1]; y++){
	for(int x=0; x<dim[0]; x++){

	  int npix = y*dim[0] + x;
	  if( par.isVerbose() ) bar.update(npix+1);

	  if(doPixel[npix]){

	    spectrum->extractSpectrum(reconArray,dim,npix);
	    spectrum->removeMW(); // only works if flagMW is true
	    std::vector<Scan> objlist = spectrum->findSources1D();
	    std::vector<Scan>::iterator obj;
	    num += objlist.size();
	    for(obj=objlist.begin();obj!=objlist.end();obj++){
	      Detection newObject;
	      // Fix up coordinates of each pixel to match original array
	      for(int z=obj->getX();z<=obj->getXmax();z++) {
		newObject.addPixel(x,y,z);
	      }
	      newObject.setOffsets(par);
	      if(par.getFlagTwoStageMerging()) mergeIntoList(newObject,outputList,par);
	      else outputList.push_back(newObject);
	    }
	  }

	}
      }

      delete spectrum;
      delete [] doPixel;
      
      if(par.isVerbose()){
	bar.remove();
	std::cout << "Found " << num << ".\n";
      }

    }

    return outputList;
  }

  /////////////////////////////////////////////////////////////////////////////
  std::vector <Detection> searchReconArraySpatial(long *dim, float *originalArray,
						  float *reconArray, Param &par,
						  StatsContainer<float> &stats)
  {
    ///  This searches for objects in a cube that has been reconstructed.
    /// 
    ///  The search is conducted only in each channel image (zdim 2D
    ///  searches).
    /// 
    ///  The searches are done on the reconstructed array, although the detected
    ///   objects have fluxes drawn from the corresponding pixels of the original
    ///   array.
    /// 
    ///  \param dim Array of dimension sizes
    ///  \param originalArray Original, un-reconstructed image array.
    ///  \param reconArray Reconstructed image array
    ///  \param par The Param set.
    ///  \param stats The StatsContainer that defines what a detection is.
    /// 
    ///  \return A vector of Detections resulting from the search.

    std::vector <Detection> outputList;
    long zdim = dim[2];
    int num=0;
    ProgressBar bar;
    bool useBar = (zdim>1);
    if(useBar&&par.isVerbose()) bar.init(zdim);
  
    long *imdim = new long[2];
    imdim[0] = dim[0]; imdim[1] = dim[1];
    Image *channelImage = new Image(imdim);
    delete [] imdim;
    channelImage->saveParam(par);
    channelImage->saveStats(stats);
    channelImage->setMinSize(1);

    for(int z=0; z<zdim; z++){  // loop over all channels

      if( par.isVerbose() && useBar ) bar.update(z+1);

      if(!par.isInMW(z)){ 
	// purpose of this is to ignore the Milky Way channels 
	//  if we are flagging them

	channelImage->extractImage(reconArray,dim,z);
	std::vector<Object2D> objlist = channelImage->findSources2D();
	std::vector<Object2D>::iterator obj;
	num += objlist.size();
	for(obj=objlist.begin();obj!=objlist.end();obj++){
	  Detection newObject;
	  newObject.addChannel(z,*obj);
	  newObject.setOffsets(par);
	  if(par.getFlagTwoStageMerging()) mergeIntoList(newObject,outputList,par);
	  else outputList.push_back(newObject);
	}
      }
    
    }

    delete channelImage;

    if(par.isVerbose()){
      if(useBar) bar.remove();
      std::cout << "Found " << num << ".\n";
    }


    return outputList;
  }

}
