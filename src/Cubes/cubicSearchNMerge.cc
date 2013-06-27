// -----------------------------------------------------------------------
// cubicSearchNMerge.cc: Combining both the searching and the merging
//                       functions.
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
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Utils/utils.hh>
using std::endl;
using std::setw;

vector <Detection> cubicSearchNMerge(long *dim, float *Array, Param &par)
{
  /// @details
  ///  Takes a dimension array and data array as input (and Parameter set)
  ///  and searches for detections in a combination of 1D and 2D searches.
  ///  Returns a vector list of Detections.
  ///  No reconstruction is assumed to have taken place, so statistics are
  ///  calculated (using robust methods) from the data array itself.

  vector <Detection> outputList;
  int zdim = dim[2];
  size_t xySize = dim[0] * dim[1];
  size_t fullSize = zdim * xySize;
  int num=0;

  //  bool flagBlank=par.getFlagBlankPix();
  float blankPixValue = par.getBlankPixVal();
  bool *isGood = new bool[fullSize];
  for(size_t pos=0;pos<fullSize;pos++) 
    isGood[pos] = !par.isBlank(Array[pos]);
    //    isGood[pos] = (!flagBlank) || (Array[pos]!=blankPixValue);
  
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

//       if(par.isVerbose() && ((1000*npix/xySize)%10==0) )
// 	std::cout << "Done " << setw(2) << 100*npix/xySize << "%\b\b\b\b\b\b\b\b" << std::flush;
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
      spectrum->findSources2D();
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
	//	outputList.push_back(*object);
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
      doChannel[z] = !par.getChannelFlag(z);

  for(int z=0; z<zdim; z++){

//     if(par.isVerbose() && ((1000*z/zdim)%10==0) )
//       std::cout << "Done " << setw(2) << 100*z/zdim << "%\b\b\b\b\b\b\b\b" << std::flush;
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
      channelImage->findSources2D();
      for(int obj=0;obj<channelImage->getNumObj();obj++){
	Detection *object = new Detection;
	*object = channelImage->getObject(obj);
// 	if(par.getFlagGrowth()) growObject(*object,*channelImage);
	// Fix up coordinates of each pixel to match original array
	for(int pix=0;pix<object->getSize();pix++) object->setZ(pix, z);
	object->addOffsets(par);
	object->calcParams();
	mergeIntoList(*object,outputList,par);
	//	outputList.push_back(*object);
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

