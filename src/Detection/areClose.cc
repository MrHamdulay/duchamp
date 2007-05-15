// -----------------------------------------------------------------------
// areClose.cc: Determine whether two Detections are close enough to
//              be merged.
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
#include <math.h>
#include <Detection/detection.hh>
#include <PixelMap/Scan.hh>
#include <PixelMap/Object3D.hh>
#include <param.hh>

using namespace PixelInfo;

bool areAdj(Object2D &obj1, Object2D &obj2);
bool areClose(Object2D &obj1, Object2D &obj2, float threshold);

bool areClose(Detection &obj1, Detection &obj2, Param &par)
{

  /**
   * A Function to test whether object1 and object2 are within the 
   * spatial and velocity thresholds specified in the parameter set par.
   * Returns true if at least one pixel of object1 is close to
   * at least one pixel of object2.
   */

  
  bool close = false;   // this will be the value returned

  // 
  // First, check to see if the objects are nearby.  We will only do
  // the pixel-by-pixel comparison if their pixel ranges overlap.
  // This saves a bit of time if the objects are big and are nowhere
  // near one another.
  // 

  bool flagAdj = par.getFlagAdjacent();
  float threshS = par.getThreshS();
  float threshV = par.getThreshV();

  long gap;
  if(flagAdj) gap = 1;
  else gap = long( ceil(threshS) );

  Scan test1,test2;

  // Test X ranges
  test1.define(0,obj1.getXmin()-gap,obj1.getXmax()-obj1.getXmin()+2*gap+1);
  test2.define(0,obj2.getXmin(),obj2.getXmax()-obj2.getXmin()+1);
  bool areNear = overlap(test1,test2);

  // Test Y ranges
  test1.define(0,obj1.getYmin()-gap,obj1.getYmax()-obj1.getYmin()+2*gap+1);
  test2.define(0,obj2.getYmin(),obj2.getYmax()-obj2.getYmin()+1);
  areNear = areNear && overlap(test1,test2);
  
  // Test Z ranges
  gap = long(ceil(threshV));
  test1.define(0,obj1.getZmin()-gap,obj1.getZmax()-obj1.getZmin()+2*gap+1);
  test2.define(0,obj2.getZmin(),obj2.getZmax()-obj2.getZmin()+1);
  areNear = areNear && overlap(test1,test2);
//   Scan commonZ = intersect(test1,test2);

  if(areNear){
    // 
    // If we get to here, the pixel ranges overlap -- so we do a
    // pixel-by-pixel comparison to make sure they are actually
    // "close" according to the thresholds.  Otherwise, close=false,
    // and so don't need to do anything else before returning.
    // 

    long nchan1 = obj1.getNumChannels(); 
    long nchan2 = obj2.getNumChannels(); 

    for(int chanct1=0; (!close && (chanct1<nchan1)); chanct1++){
      ChanMap map1=obj1.pixels().getChanMap(chanct1);
//       if(commonZ.isInScan(map1.getZ(),0)){
	
	for(int chanct2=0; (!close && (chanct2<nchan2)); chanct2++){
	  ChanMap map2=obj2.pixels().getChanMap(chanct2);
// 	  if(commonZ.isInScan(map2.getZ(),0)){
	
	    if(abs(map1.getZ()-map2.getZ())<=threshV){
	      
	      Object2D temp1 = map1.getObject();
	      Object2D temp2 = map2.getObject();
	      
	      if(flagAdj) gap = 1;
	      else gap = long( ceil(threshS) );
	      test1.define(0, temp1.getXmin()-gap,
			   temp1.getXmax()-temp1.getXmin()+2*gap+1);
	      test2.define(0, temp2.getXmin(),
			   temp2.getXmax()-temp2.getXmin()+1);
	      areNear = overlap(test1,test2);
	      test1.define(0, temp1.getYmin()-gap,
			   temp1.getYmax()-temp1.getYmin()+2*gap+1);
	      test2.define(0, temp2.getYmin(),
			   temp2.getYmax()-temp2.getYmin()+1);
	      areNear = areNear && overlap(test1,test2);
	      
	      if(areNear){
		if(flagAdj) close = close || areAdj(temp1,temp2);
		else close = close || areClose(temp1,temp2,threshS);
	      }
	    }
// 	  }

	}
//       }

    }

  }

  return close;

}


bool areClose(Object2D &obj1, Object2D &obj2, float threshold)
{
  bool close = false;

  long nscan1 = obj1.getNumScan();
  long nscan2 = obj2.getNumScan();

  Scan temp1(0, obj1.getYmin()-int(threshold),
	     obj1.getYmax()-obj1.getYmin()+1+2*int(threshold));
  Scan temp2(0, obj2.getYmin(),obj2.getYmax()-obj2.getYmin()+1);
  Scan overlap = intersect(temp1,temp2);

  if(overlap.getXlen()>0){
    overlap.growLeft();
    overlap.growRight();

    for(int scanct1=0; (!close && (scanct1<nscan1)); scanct1++){
      temp1 = obj1.getScan(scanct1);
      if(overlap.isInScan(temp1.getY(),0)){
	long y1 = temp1.getY();

	for(int scanct2=0; (!close && (scanct2<nscan2)); scanct2++){
	  temp2 = obj2.getScan(scanct2);
	  if(overlap.isInScan(temp2.getY(),0)){
	    long dy = abs(y1 - temp2.getY());

	    if(dy<=threshold){

	      int gap = int(sqrt(threshold*threshold - dy*dy));
	      Scan temp3(temp2.getY(),temp1.getX()-gap,temp1.getXlen()+2*gap);
	      if(touching(temp3,temp2)) close = true;

	    } // end of if(dy<thresh)

	  }// if overlap.isIn(temp2)
	} // end of scanct2 loop

      } // if overlap.isIn(temp1)

    } // end of scanct1 loop

  } //end of if(overlap.getXlen()>0)

  return close;
}

bool areAdj(Object2D &obj1, Object2D &obj2)
{
  bool close = false;

  long nscan1 = obj1.getNumScan();
  long nscan2 = obj2.getNumScan();

  Scan temp1(0, obj1.getYmin()-1,obj1.getYmax()-obj1.getYmin()+3);
  Scan temp2(0, obj2.getYmin(),obj2.getYmax()-obj2.getYmin()+1);
  Scan temp3;
  Scan commonY = intersect(temp1,temp2);
  if(commonY.getXlen()>0){
    commonY.growLeft();
    commonY.growRight();
    //    std::cerr << temp1 << " " << temp2 << " " << commonY << "\n";

    for(int scanct1=0;(!close && scanct1 < nscan1);scanct1++){
      temp1 = obj1.getScan(scanct1);
      if(commonY.isInScan(temp1.getY(),0)){
	long y1 = temp1.getY();

	for(int scanct2=0; (!close && scanct2 < nscan2); scanct2++){
	  temp2 = obj2.getScan(scanct2);
	  if(commonY.isInScan(temp2.getY(),0)){      
	    long dy = abs(y1 - temp2.getY());

	    if(dy<= 1){

	      temp3.define(temp2.getY(),temp1.getX(),temp1.getXlen());
	      if(touching(temp3,temp2)) close = true;

	    }
	  }
	} // end of for loop over scanct2
      
      }
     
    } // end of for loop over scanct1

  }
  return close;
}
