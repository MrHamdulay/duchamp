// -----------------------------------------------------------------------
// lutz_detect.cc: Search a 2D Image for objects.
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
#include <duchamp/Detection/finders.hh>
#include <duchamp/PixelMap/Voxel.hh>
#include <duchamp/PixelMap/Object2D.hh>
#include <vector>

using namespace PixelInfo;

/// @brief Enumeration to describe status of a pixel or a detected object
enum STATUS { NONOBJECT, ///< Pixel not above the threshold.
	      OBJECT,    ///< Pixel above the threshold.
	      COMPLETE,  ///< Object is complete
	      INCOMPLETE ///< Object not yet complete
};

/// @brief Simple enumeration to enable obvious reference to current or prior row.
enum ROW { PRIOR=0, CURRENT};

/// @brief A couple of null values: the default starting value for markers, and one used for debugging.
enum NULLS { NULLSTART=-1, ///< Default start/end value, obviously
			   ///   outside valid range.
	     NULLMARKER=45 ///< ASCII 45 = '-', which eases printing
			   ///   for debugging purposes
}; 

//---------------------------
/// @brief
/// A simple class local to @file to help manage detected
/// objects.
/// 
/// @details Keeps a track of a detection, as well as the start and finish
/// locations of the detection on the current row.
/// 
class FoundObject{
public:
  /// @brief Basic constructor, setting the start & end to NULL values. 
  FoundObject(){start=NULLSTART; end=NULLSTART;};
  int start; ///< Pixel on the current row where the detection starts.
  int end;   ///< Pixel on the current row where the detection finishes.
  Object2D info; ///< Collection of detected pixels.
};
//---------------------------

namespace duchamp
{

  std::vector<Object2D> lutz_detect(std::vector<bool> &array, long xdim, long ydim, unsigned int minSize) 
  {
    /// @details
    ///  A detection algorithm for 2-dimensional images based on that of
    ///  Lutz (1980).
    ///  
    ///  The image is raster-scanned, and searched row-by-row. Objects
    ///  detected in each row are compared to objects in subsequent rows,
    ///  and combined if they are connected (in an 8-fold sense).
    /// 

    // Allocate necessary arrays.
    std::vector<Object2D> outputlist;
    STATUS *status  = new STATUS[2];
    Object2D *store = new Object2D[xdim+1];
    char *marker    = new char[xdim+1];
    for(int i=0; i<(xdim+1); i++) marker[i] = NULLMARKER;
    std::vector<FoundObject> oS;
    std::vector<STATUS>      psS;

    Pixel pix;
    size_t loc=0;

    for(long posY=0;posY<(ydim+1);posY++){
      // Loop over each row -- consider rows one at a time
    
      status[PRIOR] = COMPLETE;
      status[CURRENT] = NONOBJECT;

      for(long posX=0;posX<(xdim+1);posX++){
	// Now the loop for a given row, looking at each column individually.

	char currentMarker = marker[posX];
	marker[posX] = NULLMARKER;

	bool isObject;
	if((posX<xdim)&&(posY<ydim)){ 
	  // if we are in the original image
	  isObject = array[loc++];
	}
	else isObject = false;
	// else we're in the padding row/col and isObject=FALSE;

	// 
	// ------------------------------ START SEGMENT ------------------------
	// If the current pixel is object and the previous pixel is not, then
	// start a new segment.
	// If the pixel touches an object on the prior row, the marker is either
	// an S or an s, depending on whether the object has started yet.
	// If it doesn't touch a prior object, this is the start of a completly
	// new object on this row.
	// 
	if ( (isObject) && (status[CURRENT] != OBJECT) ){

	  status[CURRENT] = OBJECT;
	  if(status[PRIOR] == OBJECT){
	  
	    if(oS.back().start==NULLSTART){
	      marker[posX] = 'S';
	      oS.back().start = posX;
	    }
	    else  marker[posX] = 's';
	  }
	  else{
	    psS.push_back(status[PRIOR]);  //PUSH PS onto PSSTACK;
	    marker[posX] = 'S';
	    oS.resize(oS.size()+1);        //PUSH OBSTACK;
	    oS.back().start = posX;

	    status[PRIOR] = COMPLETE;
	  }
	}

	// 
	// ------------------------------ PROCESS MARKER -----------------------
	// If the current marker is not blank, then we need to deal with it. 
	// Four cases:
	//   S --> start of object on prior row. Push priorStatus onto PSSTACK 
	//         and set priorStatus to OBJECT
	//   s --> start of a secondary segment of object on prior row.
	//         If current object joined, pop PSSTACK and join the objects.
	// 	 Set priorStatus to OBJECT.
	//   f --> end of a secondary segment of object on prior row.
	//         Set priorStatus to INCOMPLETE.
	//   F --> end of object on prior row. If no more of the object is to 
	//          come (priorStatus=COMPLETE), then finish it and output data.
	//         Add to list, but only if it has more than the minimum number
	//           of pixels.
	// 
	if(currentMarker != NULLMARKER){

	  if(currentMarker == 'S'){
	    psS.push_back(status[PRIOR]);      // PUSH PS onto PSSTACK
	    if(status[CURRENT] == NONOBJECT){
	      psS.push_back(COMPLETE);         // PUSH COMPLETE ONTO PSSTACK;
	      oS.resize(oS.size()+1);          // PUSH OBSTACK;
	      oS.back().info = store[posX];
	    }
	    else oS.back().info = oS.back().info + store[posX];
	  
	    status[PRIOR] = OBJECT;
	  }

	  /*---------*/
	  if(currentMarker == 's'){

	    if( (status[CURRENT] == OBJECT) && (status[PRIOR] == COMPLETE) ){
	      status[PRIOR] = psS.back();
	      psS.pop_back();                   //POP PSSTACK ONTO PS

	      // 	    oS.at(oS.size()-2).info.addAnObject( oS.back().info );
	      //  	    if(oS.at(oS.size()-2).start == NULLSTART) 
	      // 	      oS.at(oS.size()-2).start = oS.back().start;
	      // 	    else marker[oS.back().start] = 's';

	      oS[oS.size()-2].info = oS[oS.size()-2].info + oS.back().info;
	      if(oS[oS.size()-2].start == NULLSTART) 
		oS[oS.size()-2].start = oS.back().start;
	      else marker[oS.back().start] = 's';

	      oS.pop_back();
	    }

	    status[PRIOR] = OBJECT;
	  }

	  /*---------*/
	  if(currentMarker == 'f') status[PRIOR] = INCOMPLETE;

	  /*---------*/
	  if(currentMarker == 'F') {

	    status[PRIOR] = psS.back();
	    psS.pop_back();                    //POP PSSTACK ONTO PS

	    if( (status[CURRENT] == NONOBJECT) && (status[PRIOR] == COMPLETE) ){

	      if(oS.back().start == NULLSTART){ 
		// The object is completed. If it is big enough, add to
		// the end of the output list.	      
		if(oS.back().info.getSize() >= minSize){ 
		  //oS.back().info.calcParams(); // work out midpoints, fluxes etc
		  outputlist.push_back(oS.back().info);
		}
	      }
	      else{
		marker[ oS.back().end ] = 'F';
		store[ oS.back().start ] = oS.back().info;
	      }

	      oS.pop_back();

	      status[PRIOR] = psS.back();
	      psS.pop_back();
	    }
	  }

	} // end of PROCESSMARKER section ( if(currentMarker!=NULLMARKER) )

	if (isObject){
	  oS.back().info.addPixel(posX,posY);
	}
	else{
	  // 
	  // ----------------------------- END SEGMENT -------------------------
	  // If the current pixel is background and the previous pixel was an 
	  // object, then finish the segment.
	  // If the prior status is COMPLETE, it's the end of the final segment 
	  // of the object section.
	  // If not, it's end of the segment, but not necessarily the section.
	  // 
	  if ( status[CURRENT] == OBJECT) {

	    status[CURRENT] = NONOBJECT;

	    if(status[PRIOR] != COMPLETE){
	      marker[posX] = 'f';
	      oS.back().end = posX;
	    }
	    else{
	      status[PRIOR] = psS.back();
	      psS.pop_back();                   //POP PSSTACK onto PS;
	      marker[posX] = 'F';
	      store[ oS.back().start ] = oS.back().info;
	      oS.pop_back();
	    }
	  }
      	
	} //-> end of END SEGMENT else{ clause

      }//-> end of loop over posX

    }//-> end of loop over posY

    // clean up and remove declared arrays
    delete [] marker;
    delete [] store;
    delete [] status;

    return outputlist;

  }

}
