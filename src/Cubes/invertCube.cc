// -----------------------------------------------------------------------
// invertCube.cc: Multiply the flux of a Cube by -1.
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
#include <duchamp/Detection/detection.hh>

namespace duchamp
{

  void Cube::invert()
  {
    /// @details A function that multiplies all non-Blank pixels by
    ///  -1.  This is used when searching for negative features. This
    ///  only has an effect when the main array has been allocated.

    if(this->arrayAllocated){
      for(size_t i=0; i<this->numPixels; i++)
	if(!this->isBlank(i)){
	  this->array[i] *= -1.;
	  if(this->reconExists) this->recon[i] *= -1.;
	}
    }

  }

  void Cube::reInvert()
  {
    /// @details A function that switches the array back to the
    ///  original sign by calling Cube::invert(). It also inverts the
    ///  fluxes in all the detected objects.  This is used when
    ///  searching for negative features.
    
    this->invert();

    std::vector<Detection>::iterator obj;
    for(obj=this->objectList->begin(); obj<this->objectList->end(); obj++){
      obj->setNegative(true);
      obj->setTotalFlux(-1. * obj->getTotalFlux() );
      obj->setIntegFlux(-1. * obj->getIntegFlux() );
      obj->setPeakFlux(-1. * obj->getPeakFlux() );
    }

  }

}
