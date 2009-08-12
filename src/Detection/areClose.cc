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
#include <duchamp/Detection/detection.hh>
#include <duchamp/PixelMap/Scan.hh>
#include <duchamp/PixelMap/Object3D.hh>
#include <duchamp/param.hh>

using namespace PixelInfo;

namespace duchamp
{

  bool areAdj(Object2D &obj1, Object2D &obj2);
  bool areClose(Object2D &obj1, Object2D &obj2, float threshold);

  bool areClose(Detection &obj1, Detection &obj2, Param &par)
  {

    /// A Function to test whether object1 and object2 are within the 
    /// spatial and velocity thresholds specified in the parameter set par.
    /// Returns true if at least one pixel of object1 is close to
    /// at least one pixel of object2.
  
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

      std::vector<long> zlist1 = obj1.getChannelList();
      std::vector<long> zlist2 = obj2.getChannelList();

      for(size_t ct1=0; (!close && (ct1<zlist1.size())); ct1++){
	
	for(size_t ct2=0; (!close && (ct2<zlist2.size())); ct2++){
	
	  if(abs(zlist1[ct1]-zlist2[ct2])<=threshV){
	      
	    Object2D temp1 = obj1.getChanMap(zlist1[ct1]);
	    Object2D temp2 = obj2.getChanMap(zlist2[ct2]);

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

	}

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
    Scan Yoverlap = intersect(temp1,temp2);
//       std::cerr << "\n"<<temp1 <<"\t" << temp2 << "\t" << Yoverlap<<"\n";
      
      // Xoverlap has the scans that overlap with a padding of width threshold
      // If two pixels are separated in Y by threshold, but have the same X, then they match.
      // This is the furthest apart they can be in Y.


    if(Yoverlap.getXlen()>0){
      // Grow by a pixel in each direction, to take into account the possibility of fractional thresholds (eg. 7.5)
      Yoverlap.growLeft();
      Yoverlap.growRight();
    
      // Now look at just these scans pixel by pixel

      // Get the scans from object 2 that lie in the overlap region (these haven't been affected by the threshold gap)
       for(int scanct2=0; (!close && (scanct2<nscan2)); scanct2++){
	temp2 = obj2.getScan(scanct2);
// 	std::cerr << temp2.getY() << ":\n";
	if(Yoverlap.isInScan(temp2.getY(),0)){ // if the scan is in the allowed Y range

	  for(int scanct1=0; (!close && (scanct1<nscan1)); scanct1++){
	    temp1 = obj1.getScan(scanct1);
	    long y = temp1.getY();
// 	    std::cerr << temp1.getY() << " ";
	    if(abs(y-temp2.getY())<threshold){
// 	      std::cerr << "("<<minSep(temp1,temp2)<<")";

	      close = (minSep(temp1,temp2) < threshold);

	    }

	  }
// 	  std::cerr << "\n";

	}

       }
      
    }
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

}
