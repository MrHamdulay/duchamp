// -----------------------------------------------------------------------
// baseline.cc: Removing and replacing the spectral baseline of a Cube.
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
#include <iostream>
#include <iomanip>
#include <param.hh>
#include <ATrous/atrous.hh>
#include <Cubes/cubes.hh>
#include <Utils/feedback.hh>

namespace duchamp
{

  void Cube::removeBaseline()
  {
    /**
     *  A front-end to the getBaseline routine, specialised for the 
     *  Cube data structure. Calls getBaseline on each spectrum individually.
     *  Upon exit, the original array minus its spectral baseline is stored
     *   in this->array and the baseline is in this->baseline.
     *  If the reconstructed array exists, the baseline is subtracted from 
     *   it as well.
     */

    float *spec     = new float[this->axisDim[2]];
    float *thisBaseline = new float[this->axisDim[2]];
    int numSpec = this->axisDim[0]*this->axisDim[1];

    ProgressBar bar;
    if(this->par.isVerbose()) bar.init(numSpec);
    for(int pix=0; pix<numSpec; pix++){ // for each spatial pixel...

      if(this->par.isVerbose() ) bar.update(pix+1);

      for(int z=0; z<this->axisDim[2]; z++)  
	spec[z] = this->array[z*numSpec + pix];

      getBaseline(this->axisDim[2], spec, thisBaseline, this->par);

      for(int z=0; z<this->axisDim[2]; z++) {
	this->baseline[z*numSpec+pix] = thisBaseline[z];
	if(!par.isBlank(this->array[z*numSpec+pix])){
	  this->array[z*numSpec+pix] -= thisBaseline[z];
	  if(this->reconExists) this->recon[z*numSpec+pix] -= thisBaseline[z];
	}      
      }

    }  

    delete [] spec;
    delete [] thisBaseline;
  
    if(this->par.isVerbose()) bar.remove();
  }



  void Cube::replaceBaseline()
  {
    /**
     *  A routine to replace the baseline flux on the reconstructed array 
     *   (if it exists) and the fluxes of each of the detected objects (if any).
     */

    if(this->par.getFlagBaseline()){

      for(int i=0;i<this->numPixels;i++){
	if(!(this->par.isBlank(this->array[i])))
	  this->array[i] += this->baseline[i];
      }

      if(this->reconExists){ 
	// if we made a reconstruction, we need to add the baseline back in 
	//   for plotting purposes
	for(int i=0;i<this->numPixels;i++){
	  if(!(this->par.isBlank(this->array[i])))
	    this->recon[i] += this->baseline[i];
	}
      }
 
      for(int obj=0;obj<this->objectList->size();obj++){ 
	// for each detection, correct the flux calculations.
	this->objectList->at(obj).calcFluxes(this->array, this->axisDim);
      
      }
    
    }

  }

}
