#include <math.h>
#include <Detection/detection.hh>
#include <param.hh>

bool areClose(Detection &obj1, Detection &obj2, Param &par)
{

  /**
   * A Function to test whether object1 and object2 are within the 
   * spatial and velocity thresholds specified in the parameter set par.
   * Returns true if at least one pixel of object1 is close to
   * at least one pixel of object2.
   */

  
  bool close = false;   // this will be the value returned

  /*
   * First, check to see if the objects are nearby.  We will only do
   * the pixel-by-pixel comparison if their pixel ranges overlap.
   * This saves a bit of time if the objects are big and are nowhere
   * near one another.
   */

  bool flagAdj = par.getFlagAdjacent();
  float threshS = par.getThreshS();
  float threshV = par.getThreshV();

  int gap;
  if(flagAdj) gap = 1;
  else gap = int( ceil(threshS) );
  long min1 = obj1.getXmin();
  long min2 = obj2.getXmin();
  long max1 = obj1.getXmax();
  long max2 = obj2.getXmax();
  // Test X ranges
  bool areNear = 
    ((min1-min2+gap)*(min1-max2-gap) <= 0 ) ||
    ((max1-min2+gap)*(max1-max2-gap) <= 0 ) ||
    ((min2-min1+gap)*(min2-max1-gap) <= 0 ) ||
    ((max2-min1+gap)*(max2-max1-gap) <= 0 ) ;

  // Test Y ranges
  min1 = obj1.getYmin();
  min2 = obj2.getYmin();
  max1 = obj1.getYmax();
  max2 = obj2.getYmax();
  areNear = areNear &&
    ( ((min1-min2+gap)*(min1-max2-gap) <= 0 ) ||
      ((max1-min2+gap)*(max1-max2-gap) <= 0 ) ||
      ((min2-min1+gap)*(min2-max1-gap) <= 0 ) ||
      ((max2-min1+gap)*(max2-max1-gap) <= 0 ) );
  
  // Test Z ranges
  min1 = obj1.getZmin();
  min2 = obj2.getZmin();
  max1 = obj1.getZmax();
  max2 = obj2.getZmax();
  areNear = areNear && 
    ( ((min1-min2+threshV)*(min1-max2-threshV) <= 0 ) ||
      ((max1-min2+threshV)*(max1-max2-threshV) <= 0 ) ||
      ((min2-min1+threshV)*(min2-max1-threshV) <= 0 ) ||
      ((max2-min1+threshV)*(max2-max1-threshV) <= 0 ) );

  if(areNear){
    /*
     * If we get to here, the pixel ranges overlap -- so we do a
     * pixel-by-pixel comparison to make sure they are actually
     * "close" according to the thresholds.  Otherwise, close=false,
     * and so don't need to do anything else before returning.
     */

    float *first = new float[3];  //just store x,y,z positions of objects.
    float *second = new float[3];
    int counter = 0;
    int countermax = obj1.getSize()*obj2.getSize();
    int size2 = obj2.getSize();

    while(!close && counter<countermax ){
      // run this until we run out of pixels or we find a close pair.
    
      first[0]  = obj1.getX(counter/size2);
      first[1]  = obj1.getY(counter/size2);
      first[2]  = obj1.getZ(counter/size2);
      second[0] = obj2.getX(counter%size2);
      second[1] = obj2.getY(counter%size2);
      second[2] = obj2.getZ(counter%size2);

      if(flagAdj){
	//This step just tests to see if there is a pair of *adjacent*
	//pixels spatially, and if the velocity pixels are within the
	//threshold.  For an overall match between the objects, we
	//only require one matching pair of pixels, hence the || in
	//the "close" definition.
	close = close || 
	  ( (fabs(first[0]-second[0]) <= 1.5)         //X vals adjacent?
	    && (fabs(first[1]-second[1]) <= 1.5)      //Y vals adjacent?
	    && (fabs(first[2]-second[2]) <= threshV) //Z vals close?
	    );
      }
      else{
	close = close ||
	  ( (hypot(first[0]-second[0],first[1]-second[1])<=threshS) 
	    && (fabs(first[2]-second[2]) <= threshV )
	    );
      }
    
      counter++;
    }

    delete [] first;
    delete [] second;

  }

  return close;

}
