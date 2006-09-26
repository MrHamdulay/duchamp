/**
 *  LUTZ_DETECT.CC
 *
 * A detection algorithm based on that of Lutz (1980).
 *
 * INPUTS: 
 *    image     -- an Image object, containing a 2D image that has been 
 *                 processed such that its pValue array is defined.
 * OUTPUTS:
 *   The detection and mask arrays in image will be filled, according to 
 *   the location of the objects in the image.
 *
 */
#include <Cubes/cubes.hh>
#include <Detection/detection.hh>
#include <vector>

enum STATUS { NONOBJECT, OBJECT, COMPLETE, INCOMPLETE };
enum ROW { PRIOR=0, CURRENT};
enum NULLS { NULLSTART=-1, NULLMARKER=45}; //ASCII 45 = '-' --> eases printing in case of debugging

class Object{
public:
  Object(){start=NULLSTART; end=NULLSTART;};
  int start;
  int end;
  Detection info;
};

void Image::lutz_detect() 
{
  // Allocate necessary arrays.
  STATUS *status   = new STATUS[2];
  Detection *store = new Detection[this->axisDim[0]+1];
  char *marker     = new char[this->axisDim[0]+1];
  for(int i=0; i<(this->axisDim[0]+1); i++) marker[i] = NULLMARKER;
  vector <Object>  *oS    = new vector <Object>;
  vector <STATUS> *psS    = new vector <STATUS>;

  Pixel *pix = new Pixel;

  for(int posY=0;posY<(this->axisDim[1]+1);posY++){
    // Loop over each row -- consider rows one at a time
    
    status[PRIOR] = COMPLETE;
    status[CURRENT] = NONOBJECT;

    pix->setY(posY);

    for(int posX=0;posX<(this->axisDim[0]+1);posX++){
      // Now the loop for a given row, looking at each column individually.

      char currentMarker = marker[posX];
      marker[posX] = NULLMARKER;
      pix->setX(posX);

      bool isObject = false;
      if((posX<this->axisDim[0])&&(posY<this->axisDim[1])){ 
	// if we are in the original image
	isObject = this->isDetection(posX,posY);
	pix->setF( this->array[posY*this->axisDim[0] + posX] );
      }
      // else we're in the padding row/col and isObject=FALSE;

      /**
       * ------------------------------ START SEGMENT ------------------------
       * If the current pixel is object and the previous pixel is not, then
       * start a new segment.
       * If the pixel touches an object on the prior row, the marker is either
       * an S or an s, depending on whether the object has started yet.
       * If it doesn't touch a prior object, this is the start of a completly
       * new object on this row.
       */
      if ( (isObject) && (status[CURRENT] != OBJECT) ){

	status[CURRENT] = OBJECT;
	if(status[PRIOR] == OBJECT){
	  
	  if(oS->back().start==NULLSTART){
	    marker[posX] = 'S';
	    oS->back().start = posX;
	  }
	  else  marker[posX] = 's';
	}
	else{
	  psS->push_back(status[PRIOR]);  //PUSH PS onto PSSTACK;
	  marker[posX] = 'S';
	  oS->resize(oS->size()+1);        //PUSH OBSTACK;
	  oS->back().start = posX;

	  status[PRIOR] = COMPLETE;
	}
      }

      /**
       * ------------------------------ PROCESS MARKER -----------------------
       * If the current marker is not blank, then we need to deal with it. 
       * Four cases:
       *   S --> start of object on prior row. Push priorStatus onto PSSTACK 
       *         and set priorStatus to OBJECT
       *   s --> start of a secondary segment of object on prior row.
       *         If current object joined, pop PSSTACK and join the objects.
       *	 Set priorStatus to OBJECT.
       *   f --> end of a secondary segment of object on prior row.
       *         Set priorStatus to INCOMPLETE.
       *   F --> end of object on prior row. If no more of the object is to 
       *          come (priorStatus=COMPLETE), then finish it and output data.
       *         Add to list, but only if it has more than the minimum number
       *           of pixels.
       */
      if(currentMarker != NULLMARKER){

	if(currentMarker == 'S'){
	  psS->push_back(status[PRIOR]);      // PUSH PS onto PSSTACK
	  if(status[CURRENT] == NONOBJECT){
	    psS->push_back(COMPLETE);         // PUSH COMPLETE ONTO PSSTACK;
	    oS->resize(oS->size()+1);          // PUSH OBSTACK;
	    oS->back().info = store[posX];
	  }
	  else oS->back().info.addAnObject( store[posX] );
	  
	  status[PRIOR] = OBJECT;
	}

	/*---------*/
	if(currentMarker == 's'){

	  if( (status[CURRENT] == OBJECT) && (status[PRIOR] == COMPLETE) ){
	    status[PRIOR] = psS->back();
	    psS->pop_back();                   //POP PSSTACK ONTO PS

// 	    oS->at(oS->size()-2).info.addAnObject( oS->back().info );
// 	    if(oS->at(oS->size()-2).start == NULLSTART) oS->at(oS->size()-2).start = oS->back().start;
// 	    else marker[oS->back().start] = 's';
	    (*oS)[oS->size()-2].info.addAnObject( oS->back().info );
	    if((*oS)[oS->size()-2].start == NULLSTART) 
	      (*oS)[oS->size()-2].start = oS->back().start;
	    else marker[oS->back().start] = 's';

	    oS->pop_back();
	  }

	  status[PRIOR] = OBJECT;
	}

	/*---------*/
	if(currentMarker == 'f') status[PRIOR] = INCOMPLETE;

	/*---------*/
	if(currentMarker == 'F') {

	  status[PRIOR] = psS->back();
	  psS->pop_back();                    //POP PSSTACK ONTO PS

	  if( (status[CURRENT] == NONOBJECT) && (status[PRIOR] == COMPLETE) ){

	    if(oS->back().start == NULLSTART){ // object completed
	      if(oS->back().info.getSize() >= this->minSize){ 
		// is it big enough?
		oS->back().info.calcParams(); // work out midpoints, fluxes etc
		this->addObject(oS->back().info);
		this->maskObject(oS->back().info);
	      }
	    }
	    else{
	      marker[ oS->back().end ] = 'F';
	      store[ oS->back().start ] = oS->back().info;
	    }

	    oS->pop_back();

	    status[PRIOR] = psS->back();
	    psS->pop_back();
	  }
	}

      } // end of PROCESSMARKER section ( if(currentMarker!=NULLMARKER) )

      if (isObject){
	oS->back().info.addPixel(*pix);
      }
      else{
	/**
	 *------------------------------ END SEGMENT -------------------------
	 * If the current pixel is background and the previous pixel was an 
	 * object, then finish the segment.
	 * If the prior status is COMPLETE, it's the end of the final segment 
	 * of the object section.
	 * If not, it's end of the segment, but not necessarily the section.
	 */
	if ( status[CURRENT] == OBJECT) {

	  status[CURRENT] = NONOBJECT;

	  if(status[PRIOR] != COMPLETE){
	    marker[posX] = 'f';
	    oS->back().end = posX;
	  }
	  else{
	    status[PRIOR] = psS->back();
	    psS->pop_back();                   //POP PSSTACK onto PS;
	    marker[posX] = 'F';
	    store[ oS->back().start ] = oS->back().info;
	    oS->pop_back();
	  }
	}
      	
      } //-> end of END SEGMENT else{ clause

    }//-> end of loop over posX

  }//-> end of loop over posY

  // clean up and remove declared arrays
  delete [] marker;
  delete oS;
  delete psS;
  delete [] store;
  delete [] status;
  delete pix; 

}
