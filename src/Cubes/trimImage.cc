// -----------------------------------------------------------------------
// trimImage.cc: Trim a Cube of BLANKs around the spatial edge.
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
#include <duchamp/param.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/PixelMap/Object3D.hh>

using namespace PixelInfo;

namespace duchamp
{

  void Cube::trimCube()
  {
    /// @details
    ///  If the blankPix flag has been set, this routine trims excess blank 
    ///    pixels from the edges of the spatial image.
    ///   It uses as its template the first channel, assuming that its non-BLANK 
    ///    size is representative of the rest of the channels.
    ///   The edges are moved in until the first non-BLANK pixel is encountered.
    ///   All other arrays are similarly edited, and the amount of trimming is 
    ///    recorded.

    //   if(this->par.getFlagBlankPix()) {
    if(this->par.getFlagTrim()) {

      long xdim = this->axisDim[0];
      long ydim = this->axisDim[1];
      long zdim = this->axisDim[2];
      const long ZCHAN = 0;

      int left,right,top,bottom;

      for(int z = 0; z < zdim; z++){

	for(int row=0;row<ydim;row++){
	  left=0;
	  right=0;
	  while((left<xdim)&&
		(this->par.isBlank(this->array[row*xdim+left+ZCHAN*xdim*ydim])) ) {
	    left++;
	  }
	  while((right<xdim)&&
		(this->par.isBlank(this->array[row*xdim+(xdim-1-right)+ZCHAN*xdim*ydim]))){
	    right++;
	  }

	  if( ((z==0)&&(row==0)) || (left < this->par.getBorderLeft()) ) 
	    this->par.setBorderLeft(left);
	  if( ((z==0)&&(row==0)) || (right < this->par.getBorderRight()) ) 
	    this->par.setBorderRight(right);

	}
    
	for(int col=0;col<xdim;col++){
	  bottom=0;
	  top=0;
	  while((bottom<ydim)&&
		(this->par.isBlank(this->array[col+bottom*xdim+ZCHAN*xdim*ydim])) ) bottom++;

	  while((top<ydim)&&
		(this->par.isBlank(this->array[col+(ydim-1-top)*xdim+ZCHAN*xdim*ydim])) ) top++;

	  if( ((z==0)&&(col==0)) || (bottom < this->par.getBorderBottom()) ) 
	    this->par.setBorderBottom(bottom);
	  if( ((z==0)&&(col==0)) || (top < this->par.getBorderTop()) ) 
	    this->par.setBorderTop(top);

	}

      }
    
      left = this->par.getBorderLeft();
      right = this->par.getBorderRight();
      top = this->par.getBorderTop();
      bottom = this->par.getBorderBottom();

      this->axisDim[0] = xdim - left - right; 
      this->axisDim[1] = ydim - bottom - top; 
      this->axisDim[2] = zdim;
      this->numPixels = this->axisDim[0]*this->axisDim[1]*this->axisDim[2];

      long oldpos,newpos;
    
      // Do the trimming, but only if we need to -- is there a border of Blanks?
      if((left>0)||(right>0)||(bottom>0)||(top>0)) {

	// Trim the array of pixel values
	float *newarray  = new float[this->numPixels];
	for(int x = 0; x < axisDim[0]; x++){
	  for(int y = 0; y < axisDim[1]; y++){
	    for(int z = 0; z < axisDim[2]; z++){ 
	      oldpos = (x+left) + (y+bottom)*xdim + z*xdim*ydim;
	      newpos = x + y*this->axisDim[0] + 
		z*this->axisDim[0]*this->axisDim[1];
	      newarray[newpos]  = this->array[oldpos];
	    }
	  }
	}
	delete [] this->array;
	this->array = newarray;

	// Trim the array of baseline values
	if(this->par.getFlagBaseline()){
	  float *newarray  = new float[this->numPixels];
	  for(int x = 0; x < axisDim[0]; x++){
	    for(int y = 0; y < axisDim[1]; y++){
	      for(int z = 0; z < axisDim[2]; z++){ 
		oldpos = (x+left) + (y+bottom)*xdim + z*xdim*ydim;
		newpos = x + y*this->axisDim[0] + 
		  z*this->axisDim[0]*this->axisDim[1];
		newarray[newpos]  = this->baseline[oldpos];
	      }
	    }
	  }
	  delete [] this->baseline;
	  this->baseline = newarray;
	}

	// Trim the 2-D detection map
	short *newdetect = new short[this->axisDim[0]*this->axisDim[1]];
	for(int x = 0; x < axisDim[0]; x++){
	  for(int y = 0; y < axisDim[1]; y++){
	    oldpos = (x+left) + (y+bottom)*xdim;
	    newpos = x + y*this->axisDim[0];
	    newdetect[newpos] = this->detectMap[oldpos];
	  }
	}
	delete [] this->detectMap;
	this->detectMap = newdetect;

	if(this->par.getFlagATrous() || this->par.getFlagSmooth()){
	  // Trim the reconstructed array if we are going to do the
	  // reconstruction or smooth the array
	  float *newrecon  = new float[this->numPixels];
	  for(int x = 0; x < axisDim[0]; x++){
	    for(int y = 0; y < axisDim[1]; y++){
	      for(int z = 0; z < axisDim[2]; z++){ 
		oldpos = (x+left) + (y+bottom)*xdim + z*xdim*ydim;
		newpos = x + y*this->axisDim[0] + 
		  z*this->axisDim[0]*this->axisDim[1];
		newrecon[newpos] = this->recon[oldpos];	  
	      }
	    }
	  }
	  delete [] this->recon;
	  this->recon = newrecon;
	}

	// Set the flag indicating trimming has taken place only if it has.
	this->par.setFlagCubeTrimmed(true);

      }

    }

  }


  void Cube::unTrimCube()
  {
    /// @details
    ///  If the cube has been trimmed by trimCube(), this task adds back the 
    ///   BLANK pixels on the edges, so that it returns to its original size.
    ///   All arrays are similarly edited.

    if(this->par.getFlagCubeTrimmed()){

      long left = this->par.getBorderLeft();
      long right = this->par.getBorderRight();
      long top = this->par.getBorderTop();
      long bottom = this->par.getBorderBottom();

      long smallXDim = this->axisDim[0];
      long smallYDim = this->axisDim[1];
      long smallZDim = this->axisDim[2];

      // first correct the dimension sizes 
      this->axisDim[0] = smallXDim + left + right; 
      this->axisDim[1] = smallYDim + bottom + top; 
      this->axisDim[2] = smallZDim;
      this->numPixels = this->axisDim[0]*this->axisDim[1]*this->axisDim[2];

      long pos,smlpos;
      bool isDud;

      // Correct the array of pixel values
      float *newarray  = new float[this->numPixels];
      for(int x = 0; x < this->axisDim[0]; x++){
	for(int y = 0; y < this->axisDim[1]; y++){
	  isDud = (x<left) || (x>=smallXDim+left) || 
	    (y<bottom) || (y>=smallYDim+bottom);
	
	  for(int z = 0; z < this->axisDim[2]; z++){ 
	    pos = x + y*this->axisDim[0] + z*this->axisDim[0]*this->axisDim[1];
	    smlpos = (x-left) + (y-bottom)*smallXDim + z * smallXDim * smallYDim;
	    if(isDud) newarray[pos] = this->par.getBlankPixVal();
	    else      newarray[pos] = this->array[smlpos];
	  }
	}
      }
      delete [] this->array;
      this->array = newarray;

      if(this->reconExists){
	// Correct the reconstructed/smoothed array
	float *newrecon   = new float[this->numPixels];
	for(int x = 0; x < this->axisDim[0]; x++){
	  for(int y = 0; y < this->axisDim[1]; y++){
	    isDud = (x<left) || (x>=smallXDim+left) || 
	      (y<bottom) || (y>=smallYDim+bottom);
	  
	    for(int z = 0; z < this->axisDim[2]; z++){ 
	      pos = x + y*this->axisDim[0] + z*this->axisDim[0]*this->axisDim[1];
	      smlpos = (x-left) + (y-bottom)*smallXDim
		+ z * smallXDim * smallYDim;
	      if(isDud) newrecon[pos] = this->par.getBlankPixVal();
	      else      newrecon[pos] = this->recon[smlpos];
	    }
	  }
	}
	delete [] this->recon;
	this->recon = newrecon;
      }

      // Correct the array of baseline values
      if(this->par.getFlagBaseline()){
	float *newbase  = new float[this->numPixels];
	for(int x = 0; x < this->axisDim[0]; x++){
	  for(int y = 0; y < this->axisDim[1]; y++){
	    isDud = (x<left) || (x>=smallXDim+left) || 
	      (y<bottom) || (y>=smallYDim+bottom);
	
	    for(int z = 0; z < this->axisDim[2]; z++){ 
	      pos = x + y*this->axisDim[0] + z*this->axisDim[0]*this->axisDim[1];
	      smlpos = (x-left) + (y-bottom)*smallXDim + z*smallXDim*smallYDim;
	      if(isDud) newbase[pos] = this->par.getBlankPixVal();
	      else      newbase[pos] = this->baseline[smlpos];
	    }
	  }
	}
	delete [] this->baseline;
	this->baseline = newbase;
      }

      // Correct the 2-D detection map
      short *newdetect = new short[this->axisDim[0]*this->axisDim[1]];
      for(int x = 0; x < this->axisDim[0]; x++){
	for(int y = 0; y < this->axisDim[1]; y++){
	  pos = x + y*this->axisDim[0];
	  smlpos = (x-left) + (y-bottom)*smallXDim;
	  isDud = (x<left) || (x>=smallXDim+left) || 
	    (y<bottom) || (y>=smallYDim+bottom);
	  if(isDud) newdetect[pos]=0;
	  else newdetect[pos] = this->detectMap[smlpos];
	}
      }
      delete [] this->detectMap;
      this->detectMap = newdetect;    

  
      // Now update the positions for all the detections
  
      for(int i=0;i<this->objectList->size();i++){
	this->objectList->at(i).pixels().addOffsets(left,bottom,0);
	//      objectList[i].calcParams(this->array,this->axisDim);
      }

    }

  }

}
