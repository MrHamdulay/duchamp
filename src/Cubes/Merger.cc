#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <vector>
#include <PixelMap/Object3D.hh>
#include <Cubes/cubes.hh>
#include <Detection/detection.hh>
#include <Utils/utils.hh>
#include <Utils/feedback.hh>

using std::vector;
using namespace PixelInfo;

void Cube::ObjectMerger()
{
  /**
   * A Function that takes a Cube's list of Detections and
   * combines those that are close (according to the 
   * thresholds specified in the parameter list par).
   * It also excludes those that do not make the minimum
   * number of channels requirement.
   * A front end to simpler functions mergeList and finaliseList,
   *  with code to cover the option of growing objects.
   */

  int startSize = this->objectList.size();

  if(startSize > 0){

    // make a vector "currentList", which starts as a copy of the Cube's 
    //  objectList, but is the one worked on.
    vector <Detection> *currentList = new vector <Detection>(startSize);
    *currentList = this->objectList;
    this->objectList.clear();

    mergeList(*currentList, this->par);

    // Do growth stuff
    if(this->par.getFlagGrowth()) {
      std::cout << "Growing objects...     "<< std::flush;
      vector <Detection> *newList = new vector <Detection>;
      printBackSpace(23);
      std::cout << " Growing object #      "<< std::flush;
      std::cout.setf(std::ios::left);
      for(int i=0;i<currentList->size();i++){
	printBackSpace(6);
	std::cout << std::setw(6) << i+1 << std::flush;
	Detection *obj = new Detection;
	*obj = (*currentList)[i];
	growObject(*obj,*this);
	newList->push_back(*obj);
	delete obj;
      }
      delete currentList;
      currentList = newList;
      std::cout.unsetf(std::ios::left);
      printBackSpace(23);
      std::cout << std::flush;

      // and do the merging again to pick up objects that have
      //  grown into each other.
      mergeList(*currentList, this->par);
    }

    finaliseList(*currentList, this->par);

    this->objectList = *currentList;
    currentList->clear();
    delete currentList;

  }
}

void ObjectMerger(vector<Detection> &objList, Param &par)
{
  /**
   *   A simple front-end to the mergeList() and finaliseList() functions,
   *    so that if you want to merge a single list, it will
   *    do both the merging and the cleaning up afterwards.
   */
  mergeList(objList, par);
  finaliseList(objList, par);
}

void mergeList(vector<Detection> &objList, Param &par)
{
  /**
   *   A function that merges any objects in the list of 
   *    Detections that are within stated threshold distances.
   *   Determination of whether objects are close is done by
   *    the function areClose. 
   */

  if(objList.size() > 0){

    bool isVerb = par.isVerbose();
    vector <Detection>::iterator iter;

    int counter=0, compCounter;
    while( counter < (objList.size()-1) ){
      if(isVerb){
	std::cout.setf(std::ios::right);
	std::cout << "Progress: " << std::setw(6) << counter+1 << "/" ;
	std::cout.unsetf(std::ios::right);
	std::cout.setf(std::ios::left);
	std::cout << std::setw(6) << objList.size();
	printBackSpace(23);
	std::cout << std::flush;
	std::cout.unsetf(std::ios::left);
      }

      compCounter = counter + 1;

      do {

	Detection obj1 = objList[counter];
	Detection obj2 = objList[compCounter];

	bool close = areClose(obj1, obj2, par);

	if(close){
	  obj1 = obj1 + obj2 ;
	  iter = objList.begin() + compCounter;
	  objList.erase(iter);
	  iter = objList.begin() + counter;
	  objList.erase(iter);
	  objList.push_back( obj1 );

	  if(isVerb){
	    std::cout.setf(std::ios::right);
	    std::cout << "Progress: "
		      << std::setw(6) << counter << "/";
	    std::cout.unsetf(std::ios::right);
	    std::cout.setf(std::ios::left);
	    std::cout << std::setw(6) << objList.size();
	    printBackSpace(23);
	    std::cout << std::flush;
	    std::cout.unsetf(std::ios::left);
	  }

	  compCounter = counter + 1;

	}
	else compCounter++;

      } while( (compCounter<objList.size()) );

      counter++;

    }  // end of while(counter<(objList.size()-1)) loop
  }
}


void finaliseList(vector<Detection> &objList, Param &par)
{
  /**
   *  A function that looks at each object in the Detection vector
   *    and determines whether is passes the requirements for the
   *    minimum number of channels and spatial pixels, as provided by
   *    the Param set par.
   *   If it does not pass, it is removed from the list.
   *   In the process, the object parameters are calculated and offsets
   *    are added.
   */

  int listCounter = 0;
  while(listCounter < objList.size()){

    objList[listCounter].setOffsets(par);

    if( (objList[listCounter].hasEnoughChannels(par.getMinChannels()))
	&& (objList[listCounter].getSpatialSize() >= par.getMinPix()) ){

      listCounter++;
      if(par.isVerbose()){
	std::cout << "Final total:" << std::setw(5) << listCounter;
	printSpace(6);
	printBackSpace(23);
	std::cout << std::flush;
      }
    }      
    else objList.erase(objList.begin()+listCounter);

  }
}


