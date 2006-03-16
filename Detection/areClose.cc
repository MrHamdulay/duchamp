#include <math.h>
#include <Detection/detection.hh>
#include <param.hh>

/**
 * areClose(Detection &, Detection &, Param &):
 *   A Function to test whether object1 and object2 are 
 *   within the spatial and velocity thresholds specified
 *   in the parameter set par.
 *   Returns true if at least pixel of object1 is close to
 *   at least one pixel of object2.
 */

bool areClose(Detection &obj1, Detection &obj2, Param &par)
{
  
  // In each case, for two pixels to be close they need to satisfy *both*
  // thresholds, hence the && in the definition of "thisOneClose".
  // For an overall match between the objects, we only require one 
  // matching pair of pixels, hence the || in the "close" definition.
  
  bool close = false;

  /* */
  // Check to see if the objects are nearby.
  //  .. Only do the pixel-by-pixel comparison if their pixel ranges overlap.

  bool *flagAdj = new bool;
  *flagAdj = par.getFlagAdjacent();
  float *threshS = new float;
  *threshS = par.getThreshS();
  float *threshV = new float;
  *threshV = par.getThreshV();

  int *gap = new int;
  if(*flagAdj) *gap = 1;
  else *gap = int( ceilf(*threshS) );
  long *min1 = new long;
  long *min2 = new long;
  long *max1 = new long;
  long *max2 = new long;
  *min1 = obj1.getXmin();
  *min2 = obj2.getXmin();
  *max1 = obj1.getXmax();
  *max2 = obj2.getXmax();
  // Test X ranges
  bool *areNear = new bool;
  *areNear = 
    ((*min1-*min2+*gap)*(*min1-*max2-*gap) <= 0 ) ||
    ((*max1-*min2+*gap)*(*max1-*max2-*gap) <= 0 ) ||
    ((*min2-*min1+*gap)*(*min2-*max1-*gap) <= 0 ) ||
    ((*max2-*min1+*gap)*(*max2-*max1-*gap) <= 0 ) ;

  // Test Y ranges
  *min1 = obj1.getYmin();
  *min2 = obj2.getYmin();
  *max1 = obj1.getYmax();
  *max2 = obj2.getYmax();
  *areNear = *areNear &&
    ( ((*min1-*min2+*gap)*(*min1-*max2-*gap) <= 0 ) ||
      ((*max1-*min2+*gap)*(*max1-*max2-*gap) <= 0 ) ||
      ((*min2-*min1+*gap)*(*min2-*max1-*gap) <= 0 ) ||
      ((*max2-*min1+*gap)*(*max2-*max1-*gap) <= 0 ) );
  
  // Test Z ranges
  *min1 = obj1.getZmin();
  *min2 = obj2.getZmin();
  *max1 = obj1.getZmax();
  *max2 = obj2.getZmax();
  *areNear = *areNear && 
    ( ((*min1-*min2+*threshV)*(*min1-*max2-*threshV) <= 0 ) ||
      ((*max1-*min2+*threshV)*(*max1-*max2-*threshV) <= 0 ) ||
      ((*min2-*min1+*threshV)*(*min2-*max1-*threshV) <= 0 ) ||
      ((*max2-*min1+*threshV)*(*max2-*max1-*threshV) <= 0 ) );

  delete gap;
  delete min1;
  delete min2;
  delete max1;
  delete max2;

  if(*areNear){
    // pixel ranges overlap -- so do pixel-by-pixel comparison to make sure.
    // otherwise close=false, and so don't need to do anything before returning.

    float *first = new float[3];  //just store x,y,z positions of objects.
    float *second = new float[3];
    int *counter = new int;
    int *countermax = new int;
    int *size2 = new int;
    *counter = 0;
    *countermax = obj1.getSize()*obj2.getSize();
    *size2 = obj2.getSize();

    while(!close && *counter<*countermax ){
      // run this until we run out of pixels or we find a close pair.
    
      first[0]  = obj1.getX(*counter/(*size2));
      first[1]  = obj1.getY(*counter/(*size2));
      first[2]  = obj1.getZ(*counter/(*size2));
      second[0] = obj2.getX(*counter%(*size2));
      second[1] = obj2.getY(*counter%(*size2));
      second[2] = obj2.getZ(*counter%(*size2));

      if(*flagAdj){
	//This step just tests to see if there is a pair of *adjacent* pixels spatially,
	//   and if the velocity pixels are within the threshold.
	close = close || 
	  ( (fabsf(first[0]-second[0]) <= 1.)                     //X vals adjacent?
	    && (fabsf(first[1]-second[1]) <= 1.)                  //Y vals adjacent?
	    && (fabsf(first[2]-second[2]) <= *threshV)            //Z vals close?
	    );
      }
      else{
	close = close ||
	  ( (hypot(first[0]-second[0],first[1]-second[1])<=*threshS) 
	    && (fabsf(first[2]-second[2]) <= *threshV )
	    );
      }
    
      (*counter)++;
    }

    delete [] first;
    delete [] second;
    delete counter;
    delete countermax;
    delete size2;

  }

  delete areNear;
  delete flagAdj;
  delete threshS;
  delete threshV;

  return close;

}

  /* 
//////// OLD STUFF ////////////////////

  for(int i=0; i<obj1.getSize(); i++){
    Voxel *first = new Voxel;
    *first = obj1.getPixel(i);

    for(int j=0; j<obj2.getSize(); j++){      
      Voxel *second = new Voxel;
      *second = obj2.getPixel(j);

      if(par.getFlagAdjacent()){
	//This step just tests to see if there is a pair of *adjacent* pixels.
// 	thisOneClose = (abs(first->getX()-second->getX())<=1) && 
// 	  (abs(first->getY()-second->getY())<=1) && 
// 	  (abs(first->getZ()-second->getZ())<=1);
	//This step just tests to see if there is a pair of *adjacent* pixels spatially,
	//   and if the velocity pixels are within the threshold.

	close = close || 
	  ( (abs(first->getX()-second->getX()) <= 1)                     //X vals adjacent?
	    && (abs(first->getY()-second->getY()) <= 1)                  //Y vals adjacent?
	    && (abs(first->getZ()-second->getZ()) <= par.getThreshV())   //Z vals close?
	    );
      }
      else{
	// This tests to see if the pixels are within the spatial and velocity thresholds. 
// 	spatialSep = hypot( first->getX() - second->getX(),
// 			    first->getY() - second->getY());
	
// 	freqSep = abs( first->getZ() - second->getZ() );
	
// 	thisOneClose = ( spatialSep <= par.getThreshS() ) &&
// 	  ( freqSep <= par.getThreshV() );

	close = close ||
	  ( (hypot(first->getX()-second->getX(),first->getY()-second->getY())<=par.getThreshS()) 
	    && (abs(first->getZ()-second->getZ()) <= par.getThreshV() )
	    );
      }

      //      close = close || thisOneClose;

      delete second;
    }
    delete first;
  }

    */


