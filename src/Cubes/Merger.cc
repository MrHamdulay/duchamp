#include <iostream>
#include <iomanip>
#include <math.h>
#include <vector>
#include <Cubes/cubes.hh>
#include <Detection/detection.hh>
#include <Utils/utils.hh>

void Cube::ObjectMerger()
{
  /**
   * Cube::ObjectMerger():
   *   A Function that takes a Cube's list of Detections and
   *   combines those that are close (according to the 
   *   thresholds specified in the parameter list par).
   *   It also excludes those that do not make the minimum
   *   number of channels requirement.
   *   A front end to simpler functions mergeList and finaliseList,
   *    with code to cover the option of growing objects.
   */

  if(this->objectList.size() > 0){

    // make a vector "currentList", which starts as a copy of the Cube's objectList,
    //  but is the one worked on.
    vector <Detection> *currentList = new vector <Detection>(this->objectList.size());
    *currentList = this->objectList;
    this->objectList.clear();

    mergeList(*currentList, this->par);

    // Do growth stuff
    if(this->par.getFlagGrowth()) {
      std::cout << "Growing objects...     "<< std::flush;
      this->setCubeStats();
      vector <Detection> *newList = new vector <Detection>;
      std::cout<< "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"<<std::flush;
      std::cout << " Growing object #      "<< std::flush;
      std::cout.setf(std::ios::left);
      for(int i=0;i<currentList->size();i++){
	std::cout<< "\b\b\b\b\b\b"<<std::setw(6)<<i+1<<std::flush;
	Detection *obj = new Detection;
	*obj = (*currentList)[i];
	growObject(*obj,*this);
	newList->push_back(*obj);
	delete obj;
      }
      delete currentList;
      currentList = newList;
      std::cout.unsetf(std::ios::left);
      std::cout<< "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"<<std::flush;

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
   *  ObjectMerger(objList, par)
   *   A simple front-end to the two following functions,
   *    so that if you want to merge a single list, it will
   *    do both the merging and the cleaning up afterwards.
   */
  mergeList(objList, par);
  finaliseList(objList, par);
}

void mergeList(vector<Detection> &objList, Param &par)
{
  /**
   *  mergeList(objList, par)
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
// if(nloop>0) std::cout << "Progress#2: " << std::setw(6) << counter << "/" ;
// 	else 
	std::cout << "Progress: " << std::setw(6) << counter << "/" ;
	std::cout.unsetf(std::ios::right);
	std::cout.setf(std::ios::left);
	std::cout << std::setw(6) << objList.size() 
		  << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << std::flush;
// 	if(nloop>0) std::cout << "\b\b" << std::flush; ;
	std::cout.unsetf(std::ios::left);
      }

      compCounter = counter + 1;

      do {

	Detection *obj1 = new Detection;
	*obj1 = objList[counter];
	Detection *obj2 = new Detection;
	*obj2 = objList[compCounter];

	if(areClose(*obj1, *obj2, par)){
	  obj1->addAnObject( *obj2 );
	  iter = objList.begin() + compCounter;
	  objList.erase(iter);
	  iter = objList.begin() + counter;
	  objList.erase(iter);
	  objList.push_back( *obj1 );

	  if(isVerb){
	    std::cout.setf(std::ios::right);
	    std::cout << "Progress: "
		      << std::setw(6) << counter << "/";
	    std::cout.unsetf(std::ios::right);
	    std::cout.setf(std::ios::left);
	    std::cout << std::setw(6) << objList.size() 
		      << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" 
		      << std::flush;
	    std::cout.unsetf(std::ios::left);
	  }

	  compCounter = counter + 1;

	}
	else compCounter++;

	delete obj2;
	delete obj1;

      } while( (compCounter<objList.size()) );

      counter++;

    }  // end of while(counter<(objList.size()-1)) loop
  }
}


void finaliseList(vector<Detection> &objList, Param &par)
{
  /**
   *  finaliseList(objList, par)
   *   A function that looks at each object in the Detection vector
   *    and determines whether is passes the requirements for the
   *    minimum number of channels and spatial pixels, as provided by
   *    the Param set par.
   *   In the process, the object parameters are calculated and offsets
   *    are added.
   */

  int listCounter = 0;
  while(listCounter < objList.size()){

    objList[listCounter].addOffsets(par);
    objList[listCounter].calcParams();

    // Now, for each object in the newly modified list, we need to check if 
    // they pass the requirements on the minimum no. of channels and minimum 
    // number of spatial pixels
    // If not, remove from list.

    if( objList[listCounter].hasEnoughChannels(par.getMinChannels())
	&& (objList[listCounter].getSpatialSize() >= par.getMinPix()) ){
      listCounter++;
      if(par.isVerbose()){
	std::cout << "Final total:"<<std::setw(5)<<listCounter<<"      "
		  << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"<<std::flush;
      }
    }      
    else objList.erase(objList.begin()+listCounter);

  }
}


