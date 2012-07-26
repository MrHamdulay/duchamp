// -----------------------------------------------------------------------
// sorting.cc: Sort the list of Detections by channel or velocity.
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
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/Detection/detection.hh>



namespace duchamp
{

  void SortByZ(std::vector <Detection> &inputList)
  {
    /// A Function that takes a list of Detections and sorts them in
    /// order of increasing z-pixel value.  Upon return, the inputList
    /// is sorted.
    /// 
    /// We use the std::stable_sort function, so that the order of
    /// objects with the same z-value is preserved.
    /// \param inputList List of Detections to be sorted.
    /// \return The inputList is returned with the elements sorted.

    std::multimap<float, size_t> complist;
    std::vector<Detection> sorted;
    std::multimap<float, size_t>::iterator comp;
    std::vector<Detection>::iterator det;
    size_t ct=0;
    for (det=inputList.begin();det<inputList.end();det++){
      complist.insert(std::pair<float, size_t>(det->getZcentre(), ct++));
    }

    for (comp = complist.begin(); comp != complist.end(); comp++) 
      sorted.push_back(inputList[comp->second]);

    inputList.clear();
    for (det=sorted.begin();det<sorted.end();det++) inputList.push_back( *det );
    sorted.clear();
	  
  }

  //======================================================================

  void SortByVel(std::vector <Detection> &inputList)
  {
    /// @details
    /// A Function that takes a list of Detections and sorts them in 
    ///  order of increasing velocity.
    /// Every member of the vector needs to have WCS defined, (and if so,
    ///   then vel is assumed to be defined for all), otherwise no sorting
    ///   is done.
    /// 
    /// We use the std::stable_sort function, so that the order of
    /// objects with the same z-value is preserved.
    /// 
    /// \param inputList List of Detections to be sorted.
    /// \return The inputList is returned with the elements sorted,
    /// unless the WCS is not good for at least one element, in which
    /// case it is returned unaltered.

    bool isGood = true;
    for(size_t i=0;i<inputList.size();i++) isGood = isGood && inputList[i].isWCS();

    if(isGood){

      std::multimap<double, size_t> complist;
      std::vector<Detection> sorted;
      std::multimap<double, size_t>::iterator comp;
      std::vector<Detection>::iterator det;
      size_t ct=0;
      for (det=inputList.begin();det<inputList.end();det++){
	complist.insert(std::pair<double, size_t>(det->getVel(), ct++));
      }

      for (comp = complist.begin(); comp != complist.end(); comp++) 
	sorted.push_back(inputList[comp->second]);

      inputList.clear();
      for (det=sorted.begin();det<sorted.end();det++) inputList.push_back( *det );
      sorted.clear();

 
    }

  }  

  //======================================================================

  void SortDetections(std::vector <Detection> &inputList, std::string parameter)
  {
    /// @details
    /// A Function that takes a list of Detections and sorts them in 
    ///  order of increasing value of the parameter given.
    ///
    /// For parameters that need the WCS (iflux, vel, ra, dec, w50), a
    /// check is made that the WCS is valid, using
    /// Detection::isWCS(). If it is not, the list is returned
    /// unsorted.
    /// 
    /// \param inputList List of Detections to be sorted.
    /// \param parameter The name of the parameter to be sorted
    ///        on. Options are listed in the param.hh file
    /// \return The inputList is returned with the elements sorted,
    ///         unless the WCS is not good for at least one element,
    ///         in which case it is returned unaltered.

    bool OK = false, reverseSort=(parameter[0]=='-');
    std::string checkParam;
    if(reverseSort) checkParam = parameter.substr(1);
    else checkParam = parameter;
    for(int i=0;i<numSortingParamOptions;i++) 
      OK = OK || (checkParam == sortingParamOptions[i]);
    if(!OK){
      DUCHAMPERROR("SortDetections", "Invalid sorting parameter: " << parameter << " -- Not doing any sorting.");
      return;
    }

    bool isGood = true;
    if(checkParam!="zvalue" && checkParam!="pflux" && checkParam!="snr" && checkParam!="xvalue" && checkParam!="yvalue"){
      for(size_t i=0;i<inputList.size();i++) isGood = isGood && inputList[i].isWCS();
    }

    if(isGood){

      std::vector<Detection> sorted;
      std::vector<Detection>::iterator det;

      if(checkParam=="xvalue" || checkParam=="yvalue" || checkParam=="zvalue" || checkParam=="iflux" || checkParam=="pflux" || checkParam=="snr"){

	std::multimap<float, size_t> complist;
	std::multimap<float, size_t>::iterator comp;
	size_t ct=0;
	float reverse = reverseSort ? -1. : 1.;
	for (det=inputList.begin();det<inputList.end();det++){
	  if(checkParam=="xvalue")      complist.insert(std::pair<float, size_t>(reverse*det->getXcentre(),   ct++));
	  else if(checkParam=="yvalue") complist.insert(std::pair<float, size_t>(reverse*det->getYcentre(),   ct++));
	  else if(checkParam=="zvalue") complist.insert(std::pair<float, size_t>(reverse*det->getZcentre(),   ct++));
	  else if(checkParam=="iflux")  complist.insert(std::pair<float, size_t>(reverse*det->getIntegFlux(), ct++));
	  else if(checkParam=="pflux")  complist.insert(std::pair<float, size_t>(reverse*det->getPeakFlux(),  ct++));
	  else if(checkParam=="snr")    complist.insert(std::pair<float, size_t>(reverse*det->getPeakSNR(),   ct++));
	}
	
	for (comp = complist.begin(); comp != complist.end(); comp++) 
	  sorted.push_back(inputList[comp->second]);
	
      }
      else if(checkParam=="ra" || checkParam=="dec" || checkParam=="vel" || checkParam=="w50"){

	std::multimap<double, size_t> complist;
	std::multimap<double, size_t>::iterator comp;
	size_t ct=0;
	double reverse = reverseSort ? -1. : 1.;
	for (det=inputList.begin();det<inputList.end();det++){
	  if(checkParam=="ra")       complist.insert(std::pair<double, size_t>(reverse*det->getRA(),  ct++));
	  else if(checkParam=="dec") complist.insert(std::pair<double, size_t>(reverse*det->getDec(), ct++));
	  else if(checkParam=="vel") complist.insert(std::pair<double, size_t>(reverse*det->getVel(), ct++));
	  else if(checkParam=="w50") complist.insert(std::pair<double, size_t>(reverse*det->getW50(), ct++));
	}
	
	for (comp = complist.begin(); comp != complist.end(); comp++) 
	  sorted.push_back(inputList[comp->second]);
	
      }

      inputList.clear();
      for (det=sorted.begin();det<sorted.end();det++) inputList.push_back( *det );
      sorted.clear();
 
    }

  }  

}
