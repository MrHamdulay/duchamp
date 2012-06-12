// -----------------------------------------------------------------------
// Merger.cc: Merging a list of Detections, and rejecting on the basis
//            of number of channels or pixels.
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
#include <fstream>
#include <iomanip>
#include <math.h>
#include <vector>
#include <duchamp/param.hh>
#include <duchamp/PixelMap/Object3D.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Cubes/cubeUtils.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Detection/ObjectGrower.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/feedback.hh>

using std::vector;
using namespace PixelInfo;

namespace duchamp
{

  void Cube::ObjectMerger()
  {
    /// @details
    /// A Function that takes a Cube's list of Detections and
    /// combines those that are close (according to the 
    /// thresholds specified in the parameter list par).
    /// It also excludes those that do not make the minimum
    /// number of channels requirement.
    /// A front end to simpler functions mergeList and finaliseList,
    ///  with code to cover the option of growing objects.

    int startSize = this->objectList->size();

    if(startSize > 0){

      // make a vector "currentList", which starts as a copy of the Cube's 
      //  objectList, but is the one worked on.
      vector <Detection> currentList(startSize);
      for(int i=0;i<startSize;i++) currentList[i] = this->objectList->at(i);
      this->objectList->clear();

      if(this->par.getFlagRejectBeforeMerge()) 
	finaliseList(currentList, this->par);

      mergeList(currentList, this->par);

      // Do growth stuff
      if(this->par.getFlagGrowth()) {
	ObjectGrower grower;
	grower.define(this);
	for(size_t i=0;i<currentList.size();i++){
	  if(this->par.isVerbose()){
	    std::cout.setf(std::ios::right);
	    std::cout << "Growing: " << std::setw(6) << i+1 << "/";	   
	    std::cout.unsetf(std::ios::right);
	    std::cout.setf(std::ios::left);
	    std::cout << std::setw(6) << currentList.size() << std::flush;
	    printBackSpace(22);
	    std::cout << std::flush;
	  }
	  grower.grow(&currentList[i]);
	}
	std::cout.unsetf(std::ios::left);

	// and do the merging again to pick up objects that have
	//  grown into each other.
	mergeList(currentList, this->par);
      }

      if(!this->par.getFlagRejectBeforeMerge()) 
	finaliseList(currentList, this->par);

      //     *this->objectList = currentList;
      this->objectList->resize(currentList.size());
      for(size_t i=0;i<currentList.size();i++)
	this->objectList->at(i) = currentList[i];
    
      currentList.clear();

    }
  }

  void ObjectMerger(vector<Detection> &objList, Param &par)
  {
    /// @details
    ///   A simple front-end to the mergeList() and finaliseList() functions,
    ///    so that if you want to merge a single list, it will
    ///    do both the merging and the cleaning up afterwards.

    mergeList(objList, par);
    finaliseList(objList, par);
  }

  void mergeList(vector<Detection> &objList, Param &par)
  {
    /// @details
    ///   A function that merges any objects in the list of 
    ///    Detections that are within stated threshold distances.
    ///   Determination of whether objects are close is done by
    ///    the function areClose. 

    if(objList.size() > 0){

      bool isVerb = par.isVerbose();
      vector <Detection>::iterator iter;
      
      std::vector<bool> isValid(objList.size(),true);
      int numRemoved=0;

      size_t counter=0, compCounter,goodCounter=0;

      while( counter < (objList.size()-1) ){
	if(isVerb){
	  std::cout.setf(std::ios::right);
	  std::cout << "Merging: " << std::setw(6) << goodCounter+1 << "/" ;
	  std::cout.unsetf(std::ios::right);
	  std::cout.setf(std::ios::left);
	  std::cout << std::setw(6) << objList.size()-numRemoved;
	  printBackSpace(22);
	  std::cout << std::flush;
	  std::cout.unsetf(std::ios::left);
	}

	if(isValid[counter]){

	  compCounter = counter + 1;

	  do {

	    if(isValid[compCounter]){

	      bool close = objList[counter].canMerge(objList[compCounter], par);

	      if(close){
		objList[counter].addDetection(objList[compCounter]);
		isValid[compCounter]=false;
		numRemoved++;

		if(isVerb){
		  std::cout.setf(std::ios::right);
		  std::cout << "Merging: "
			    << std::setw(6) << goodCounter+1 << "/";
		  std::cout.unsetf(std::ios::right);
		  std::cout.setf(std::ios::left);
		  std::cout << std::setw(6) << objList.size()-numRemoved;
		  printBackSpace(22);
		  std::cout << std::flush;
		  std::cout.unsetf(std::ios::left);
		}
		
		compCounter = counter + 1;
		
	      }
	      else compCounter++;
	    }
	    else compCounter++;

	  } while( (compCounter<objList.size()) );

	}
	counter++;
	if(isValid[counter]) goodCounter++;

      }  // end of while(counter<(objList.size()-1)) loop

      std::vector<Detection> newlist(objList.size()-numRemoved);
      size_t ct=0;
      for(size_t i=0;i<objList.size();i++){
	if(isValid[i]) newlist[ct++]=objList[i];
      }
      objList.clear();
      objList=newlist;

    }
  }


  void finaliseList(vector<Detection> &objList, Param &par)
  {
    /// @details
    ///  A function that looks at each object in the Detection vector
    ///    and determines whether is passes the requirements for the
    ///    minimum number of channels and spatial pixels, as provided by
    ///    the Param set par.
    ///   If it does not pass, it is removed from the list.
    ///   In the process, the offsets are set.

    if(par.isVerbose()){
      std::cout << "Rejecting:" << std::setw(6) << objList.size();
      printSpace(6);
      printBackSpace(22);
      std::cout << std::flush;
    }

    std::vector<Detection> newlist;
    
    std::vector<Detection>::iterator obj = objList.begin();
    int numRej=0;
    for(;obj<objList.end();obj++){
      obj->setOffsets(par);
      
      if( (obj->hasEnoughChannels(par.getMinChannels()))
	  && (obj->getSpatialSize() >= par.getMinPix())
	  && (obj->getSize() >= par.getMinVoxels() ) ){

	newlist.push_back(*obj);

      }      
      else{
	numRej++;
	if(par.isVerbose()){
	  std::cout << "Rejecting:" << std::setw(6) << objList.size()-numRej;
	  printSpace(6);
	  printBackSpace(22);
	  std::cout << std::flush;
	}

      }
    }

    objList.clear();
    objList = newlist;

  }


}
