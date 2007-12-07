// -----------------------------------------------------------------------
// spectrumDetect.cc: Search a 1D Image for objects.
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
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/PixelMap/Scan.hh>

using namespace PixelInfo;

enum STATUS { NONOBJECT, OBJECT };

namespace duchamp
{

  std::vector<Scan> Image::spectrumDetect() 
  {
    /**
     *  A detection algorithm that searches in a single 1-D spectrum.  It
     *  simply scans along the spectrum, storing connected sets of
     *  detected pixels as Scans, where "detected" means according to the
     *  Image::isDetection(long,long) function.
     *
     *  When finished a vector of the detected scans is returned.
     *
     */

    STATUS status;
    Scan obj;
    std::vector<Scan> outputlist;
    bool isObject;

    status = NONOBJECT;
    for(int pos=0;pos<(this->axisDim[0]+1);pos++){

      if(pos<this->axisDim[0]){
	isObject = this->isDetection(pos,0);
      }
      else isObject=false;

      if(isObject){
	if(status != OBJECT){
	  status = OBJECT;
	  obj.define(0, pos, 1);
	}
	else obj.growRight();
      }
      else{
	if(status == OBJECT){ // if we were on an object and have left
	  if(obj.getXlen() >= this->minSize){ // if it's big enough
	    outputlist.push_back(obj);  // add to list.
	  }
	  obj.clear();
	}
	status = NONOBJECT;
      }

    }

    return outputlist;
  
  }

}
