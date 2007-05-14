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
#include <Cubes/cubes.hh>
#include <Detection/detection.hh>

void Cube::invert()
{
  /**
   *  A function that multiplies all non-Blank pixels by -1. 
   *  This is used when searching for negative features.
   */
  for(int i=0; i<this->numPixels; i++)
    if(!this->isBlank(i)){
      this->array[i] *= -1.;
      if(this->reconExists) this->recon[i] *= -1.;
    }
}

void Cube::reInvert()
{
  /**
   *  A function that switches the array back to the original sign.
   *  This is used when searching for negative features.
   */
  for(int i=0; i<this->numPixels; i++){
    if(!this->isBlank(i)){
      this->array[i] *= -1.;
      if(this->reconExists) this->recon[i] *= -1.;
    }
  }

  for(int i=0; i<this->objectList->size(); i++){
    this->objectList->at(i).setNegative(true);
    this->objectList->at(i).setTotalFlux(-1. * this->objectList->at(i).getTotalFlux() );
    this->objectList->at(i).setIntegFlux(-1. * this->objectList->at(i).getIntegFlux() );
    this->objectList->at(i).setPeakFlux(-1. * this->objectList->at(i).getPeakFlux() );
  }

}
