#include <iostream>
#include <iomanip>
#include <math.h>
#include <vector>
#include <Cubes/cubes.hh>
#include <Detection/detection.hh>
#include <Utils/utils.hh>

/**
 * Cube::ObjectMerger():
 *   A Function that takes a Cube's list of Detections and
 *   combines those that are close (according to the 
 *   thresholds specified in the parameter list par).
 *   It also excludes those that do not make the minimum
 *   number of channels requirement.
 */
void Cube::ObjectMerger()
{

  if(this->objectList.size() > 0){

    // make a vector "currentList", which starts as a copy of the Cube's objectList,
    //  but is the one worked on.
    vector <Detection> *currentList = new vector <Detection>(this->objectList.size());
    *currentList = this->objectList;
    vector <Detection>::iterator iter;
//     for(int i=0;i<this->objectList.size();i++) currentList->push_back( this->objectList[i] );

    // The Cube's objectList is cleared, and then the final objects are added to it at the end.
    this->objectList.clear();
  
    int counter, compCounter,numLoops;
    bool areNear,isMatch;
    long gap;

    bool isVerb = this->par.isVerbose();

    if(this->par.getFlagGrowth()) numLoops=2;
    else numLoops = 1;

    for(int nloop = 0; nloop < numLoops; nloop++){

      counter=0;
      while( counter < (currentList->size()-1) ){
	if(isVerb){
	  std::cout.setf(std::ios::right);
	  if(nloop>0) std::cout << "Progress#2: " << std::setw(6) << counter << "/" ;
	  else std::cout << "Progress: " << std::setw(6) << counter << "/" ;
	  std::cout.unsetf(std::ios::right);
	  std::cout.setf(std::ios::left);
	  std::cout << std::setw(6) << currentList->size() 
		    << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << std::flush;
	  if(nloop>0) std::cout << "\b\b" << std::flush; ;
	  std::cout.unsetf(std::ios::left);
	}

	compCounter = counter + 1;
	do {

	  Detection *obj1 = new Detection;
	  *obj1 = currentList->at(counter);
	  Detection *obj2 = new Detection;
	  *obj2 = currentList->at(compCounter);

	  if(areClose(*obj1, *obj2, this->par)){
	    obj1->addAnObject( *obj2 );
	    iter = currentList->begin() + compCounter;
	    currentList->erase(iter);
	    iter = currentList->begin() + counter;
	    currentList->erase(iter);
	    currentList->push_back( *obj1 );
	    //   	  currentList->insert(iter, *obj1 );
	    if(isVerb){
	      std::cout.setf(std::ios::right);
	      std::cout << "Progress: "
			<< std::setw(6) << counter << "/";
	      std::cout.unsetf(std::ios::right);
	      std::cout.setf(std::ios::left);
	      std::cout << std::setw(6) << currentList->size() 
			<< "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << std::flush;
	      std::cout.unsetf(std::ios::left);
	    }
	    compCounter = counter + 1;

	  }
	  else compCounter++;

	  delete obj2;
	  delete obj1;

	} while( (compCounter<currentList->size()) );

	counter++;

      }  // end of while(counter<(currentList->size()-1)) loop

      if( (nloop==0) && (this->par.getFlagGrowth()) ) {
	std::cout << "Growing objects...     "<< std::flush;
	this->setCubeStats();
	vector <Detection> *newList = new vector <Detection>;
	std::cout<< "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"<<std::flush;
	std::cout << " Growing object #      "<< std::flush;
	std::cout.setf(std::ios::left);
	for(int i=0;i<currentList->size();i++){
	  std::cout<< "\b\b\b\b\b\b"<<std::setw(6)<<i+1<<std::flush;
	  Detection *obj = new Detection;
	  *obj = currentList->at(i);
	  growObject(*obj,*this);
	  newList->push_back(*obj);
	  delete obj;
	}
	delete currentList;
	currentList = newList;
	std::cout.unsetf(std::ios::left);
	std::cout<< "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"<<std::flush;
      }

    } 
    // when you get here, every detection in the input list has been 
    // examined and compared to every other detection.

    int listCounter = 0;
    while(listCounter < currentList->size()){

      (*currentList)[listCounter].addOffsets(this->par);
      (*currentList)[listCounter].calcParams();

      // Now, for each object in the newly modified list, we need to check if 
      // they pass the minimum no. of channels requirement
      // If not, remove from list.

      if( (*currentList)[listCounter].hasEnoughChannels(this->par.getMinChannels()) ){
	listCounter++;
	if(isVerb){
	  std::cout << "Final total:"<<std::setw(5)<<listCounter<<"      "
		    << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"<<std::flush;
	}
      }      
      else currentList->erase(currentList->begin()+listCounter);

    }

    this->objectList = *currentList;
    currentList->clear();
    delete currentList;

  }



//     vector<Detection> *finalList = new vector<Detection>;
//     for(int listCounter = 0; listCounter < currentList->size(); listCounter++){
//       Detection *currentObject = new Detection;
//       *currentObject = currentList->at(listCounter);
//       currentObject->addOffsets(this->par);
//       currentObject->calcParams();
//       // Now, for each object in the newly modified list, we need to check if 
//       // they pass the minimum no. of channels requirement            
//       int numChannels = currentObject->getZmax() - currentObject->getZmin() + 1;
//       if( numChannels >= this->par.getMinChannels() ){

// 	/*
// 	// Now remove any multiply-appearing pixels in each good object
// 	// and add to the output list      
// 	Detection *newObject = new Detection;
// 	// the first pixel gets included -- then compare all others...
// 	newObject->addPixel(currentObject->getPixel(0)); 
// 	for(int pix = 1; pix < currentObject->getSize(); pix++){
// 	bool addThisOne = true;
// 	for(int newpix = 0; newpix < newObject->getSize(); newpix++){
// 	bool newOneMatches = ( newObject->getX(newpix) == currentObject->getX(pix) )
// 	&& ( newObject->getY(newpix) == currentObject->getY(pix) )
// 	&& ( newObject->getZ(newpix) == currentObject->getZ(pix) );
// 	addThisOne = addThisOne && !newOneMatches;
// 	}
// 	if(addThisOne) newObject->addPixel(currentObject->getPixel(pix));
// 	}
// 	newObject->addOffsets(this->par);
// 	newObject->calcParams();
// 	this->objectList.push_back(*newObject);
// 	*/
// 	finalList->push_back(*currentObject);
// 	if(isVerb){
// 	  std::cout << "Final total:"<<std::setw(5)<<finalList->size()
// 		    << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"<<std::flush;
// 	}
// 	//	delete newObject;
//       }

//       delete currentObject;
//     }

//     currentList->clear();
//     delete currentList;

//     this->objectList = *finalList;
//     finalList->clear();
//     delete finalList;

//   }

}

