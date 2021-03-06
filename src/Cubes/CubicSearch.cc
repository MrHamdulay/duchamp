// -----------------------------------------------------------------------
// CubicSearch.cc: Searching a 3-dimensional Cube.
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <duchamp/param.hh>
#include <duchamp/PixelMap/Object3D.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/feedback.hh>
#include <duchamp/Utils/Statistics.hh>

using std::vector;
using namespace PixelInfo;
using namespace Statistics;

namespace duchamp
{

void Cube::CubicSearch()
{
  /// @details 
  ///  A front end to the cubic searching routine that does not
  ///  involve any wavelet reconstruction. 
  ///  The statistics of the cube are calculated first of all.
  ///  If baseline-removal is required that is done prior to searching.
  ///  Once searching is complete, the detection map is updated and
  ///  the intermediate detections are logged in the log file.

  if(this->par.isVerbose()) std::cout << "  ";

  this->setCubeStats();
    
  if(this->par.isVerbose()) std::cout << "  Searching... " << std::flush;
  
  *this->objectList = search3DArray(this->axisDim,this->array,
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
//---------------------------------------------------------------

std::vector <Detection> search3DArray(size_t *dim, float *Array, Param &par,
				      StatsContainer<float> &stats)
{

  if(par.getSearchType()=="spectral")
    return search3DArraySpectral(dim,Array,par,stats);
  else if(par.getSearchType()=="spatial")
    return search3DArraySpatial(dim,Array,par,stats);
  else{
    DUCHAMPERROR("search3DArray","Unknown search type : " << par.getSearchType());
    return std::vector<Detection>(0);
  }
}
//---------------------------------------------------------------


std::vector <Detection> search3DArraySpectral(size_t *dim, float *Array, Param &par,
					      StatsContainer<float> &stats)
{
  /// @details
  ///  Takes a dimension array and data array as input (and Parameter set)
  ///  and searches for detections in just the 1D spectra.
  ///  Returns a vector list of Detections.
  ///  No reconstruction is assumed to have taken place, so statistics are
  ///  calculated (using robust methods) from the data array itself.
  /// \param dim Array of dimension sizes for the data array.
  /// \param Array Array of data.
  /// \param par Param set defining how to do detection, and what a
  /// BLANK pixel is etc.
  /// \param stats The statistics that define what a detection is.
  /// \return Vector of detected objects.

  std::vector <Detection> outputList;
  size_t zdim = dim[2];
  size_t xySize = dim[0] * dim[1];
  int num = 0;

  if(zdim>1){
    
    ProgressBar bar;
    if(par.isVerbose()) bar.init(xySize);

    std::vector<bool> doPixel(xySize,false);
    for(size_t npix=0; npix<xySize; npix++){
      for(size_t z=0;z<zdim;z++){
	doPixel[npix] = doPixel[npix] || 
	    (!par.isBlank(Array[npix+xySize*z]) && !par.isFlaggedChannel(z));
      }
      // doPixel[i] is false only when there are no good pixels in spectrum
      //  of pixel #i.
    }

    size_t *specdim = new size_t[2];
    specdim[0] = zdim; specdim[1]=1;
    Image *spectrum = new Image(specdim);
    delete [] specdim;
    spectrum->saveParam(par);
    spectrum->saveStats(stats);
    //    spectrum->setMinSize(par.getMinChannels());
    spectrum->setMinSize(1);

    for(size_t y=0; y<dim[1]; y++){
      for(size_t x=0; x<dim[0]; x++){

	size_t npix = y*dim[0] + x;
	if( par.isVerbose() ) bar.update(npix+1);
	
	if(doPixel[npix]){
	  spectrum->extractSpectrum(Array,dim,npix);
	  spectrum->removeFlaggedChannels();
	  std::vector<Scan> objlist = spectrum->findSources1D();
	  std::vector<Scan>::iterator obj;
	  num += objlist.size();
	  for(obj=objlist.begin();obj<objlist.end();obj++){
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

    if(par.isVerbose()){
      bar.remove();
      std::cout << "Found " << num << ".\n";
    }

  }

  return outputList;
}
//---------------------------------------------------------------

std::vector <Detection> search3DArraySpatial(size_t *dim, float *Array, 
					     Param &par,
					     StatsContainer<float> &stats)
{
  /// @details
  ///  Takes a dimension array and data array as input (and Parameter
  ///  set) and searches for detections just in the channel maps -- no
  ///  1D searches are done.  
  ///  Returns a vector list of Detections.
  ///  No reconstruction is assumed to have taken place, so only the base
  ///  data array is searched.
  /// \param dim Array of dimension sizes for the data array.
  /// \param Array Array of data.
  /// \param par Param set defining how to do detection, and what a
  ///              BLANK pixel is etc.
  /// \param stats The statistics that define what a detection is.
  /// \return A std::vector of detected objects.

  std::vector <Detection> outputList;
  size_t zdim = dim[2];
  int num = 0;

  ProgressBar bar;
  bool useBar = (zdim>1);
  if(useBar && par.isVerbose()) bar.init(zdim);
  
  size_t *imdim = new size_t[2];
  imdim[0] = dim[0]; imdim[1] = dim[1];
  Image *channelImage = new Image(imdim);
  delete [] imdim;
  channelImage->saveParam(par);
  channelImage->saveStats(stats);
  channelImage->setMinSize(1);

  for(size_t z=0; z<zdim; z++){

    if( par.isVerbose() && useBar ) bar.update(z+1);

    if(!par.isFlaggedChannel(z)){

      channelImage->extractImage(Array,dim,z);
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
