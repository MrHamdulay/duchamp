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
#include <duchamp/Detection/finders.hh>
#include <duchamp/Utils/feedback.hh>

using namespace PixelInfo;

namespace duchamp
{

    void Cube::trimCube()
    {

	if(this->par.getFlagTrim()) {

	    size_t xdim = this->axisDim[0];
	    size_t ydim = this->axisDim[1];
	    size_t zdim = this->axisDim[2];
	    size_t spatsize=xdim*ydim;
	    
	    size_t left=0,right=xdim-1,top=ydim-1,bottom=0;

	    ProgressBar bar;
	    if(this->par.isVerbose()){
	      std::cout << "\n  Determining blank pixel borders: "<<std::flush;
	      bar.init(zdim);
	    }
	    std::vector<bool> rowblank(xdim,false);
	    std::vector<bool> colblank(ydim,false);
	    for (size_t z=0;z<zdim;z++){
	      if(this->par.isVerbose()) bar.update(z+1);
		std::vector<bool> currentBlank = this->par.makeBlankMask(this->array+z*spatsize,spatsize);
		for(size_t x=0;x<xdim;x++){
		  for(size_t y=0;y<ydim;y++){
		    rowblank[x] = rowblank[x] || currentBlank[x+y*xdim];
		    colblank[y] = colblank[y] || currentBlank[x+y*xdim];
		  }
		}
	    }

	    std::vector<Scan> rowscans = spectrumDetect(rowblank,xdim,1);
	    std::vector<Scan> colscans = spectrumDetect(colblank,ydim,1);

	    if(this->par.isVerbose()){
	      bar.remove();
	      std::cout << "Done."<<"\n";
	    }

	    if (rowscans.size()>0 && colscans.size()>0){

		// if these sizes are zero then there are either a row or column of blanks, and we can't trim

		left=rowscans.begin()->getX();
		right=xdim-1-rowscans.rbegin()->getXmax();
		bottom=colscans.begin()->getX();
		top=ydim-1-colscans.rbegin()->getXmax();

		if (left>0 || right>0 || bottom > 0 || top>0 ) {
		    // only trim if we need to - are the borders right at the edges?
		  
		    // Set the flag indicating trimming has taken place only if it has.
		    this->par.setFlagCubeTrimmed(true);
		    this->par.setBorderLeft(left);
		    this->par.setBorderRight(right);
		    this->par.setBorderTop(top);
		    this->par.setBorderBottom(bottom);

		    // Reset the size of the cube
		    this->axisDim[0] = xdim - left - right; 
		    this->axisDim[1] = ydim - bottom - top; 
		    this->axisDim[2] = zdim;
		    this->numPixels = this->axisDim[0]*this->axisDim[1]*this->axisDim[2];

		    size_t oldpos,newpos;

		    if(this->par.isVerbose()){
		      std::cout << "  Trimming data arrays: " << std::flush;
		      bar.init(4);
		    }
		    
		    if(this->par.isVerbose()) bar.update(1);
		    // Trim the array of pixel values
		    float *newarray  = new float[this->numPixels];
		    for(size_t x = 0; x < axisDim[0]; x++){
			for(size_t y = 0; y < axisDim[1]; y++){
			    for(size_t z = 0; z < axisDim[2]; z++){ 
				oldpos = (x+left) + (y+bottom)*xdim + z*xdim*ydim;
				newpos = x + y*this->axisDim[0] + 
				    z*this->axisDim[0]*this->axisDim[1];
				newarray[newpos]  = this->array[oldpos];
			    }
			}
		    }
		    delete [] this->array;
		    this->array = newarray;

		    if(this->par.isVerbose()) bar.update(2);
		    // Trim the array of baseline values
		    if(this->par.getFlagBaseline()){
			float *newarray  = new float[this->numPixels];
			for(size_t x = 0; x < axisDim[0]; x++){
			    for(size_t y = 0; y < axisDim[1]; y++){
				for(size_t z = 0; z < axisDim[2]; z++){ 
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

		    if(this->par.isVerbose()) bar.update(3);
		    // Trim the 2-D detection map
		    short *newdetect = new short[this->axisDim[0]*this->axisDim[1]];
		    for(size_t x = 0; x < axisDim[0]; x++){
			for(size_t y = 0; y < axisDim[1]; y++){
			    oldpos = (x+left) + (y+bottom)*xdim;
			    newpos = x + y*this->axisDim[0];
			    newdetect[newpos] = this->detectMap[oldpos];
			}
		    }
		    delete [] this->detectMap;
		    this->detectMap = newdetect;

		    if(this->par.isVerbose()) bar.update(4);
		    if(this->par.getFlagATrous() || this->par.getFlagSmooth()){
			// Trim the reconstructed array if we are going to do the
			// reconstruction or smooth the array
			float *newrecon  = new float[this->numPixels];
			for(size_t x = 0; x < axisDim[0]; x++){
			    for(size_t y = 0; y < axisDim[1]; y++){
				for(size_t z = 0; z < axisDim[2]; z++){ 
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

		}

	    }

	    if(!this->par.getFlagCubeTrimmed()){
	      if(this->par.isVerbose()) 
		std::cout << "  No trimming necessary."<<"\n";
	    }

	}

    }



  // void Cube::trimCubeOld()
  // {
  //   /// @details
  //   ///  If the blankPix flag has been set, this routine trims excess blank 
  //   ///    pixels from the edges of the spatial image.
  //   ///   It uses as its template the first channel, assuming that its non-BLANK 
  //   ///    size is representative of the rest of the channels.
  //   ///   The edges are moved in until the first non-BLANK pixel is encountered.
  //   ///   All other arrays are similarly edited, and the amount of trimming is 
  //   ///    recorded.

  //   //   if(this->par.getFlagBlankPix()) {
  //   if(this->par.getFlagTrim()) {

  //     size_t xdim = this->axisDim[0];
  //     size_t ydim = this->axisDim[1];
  //     size_t zdim = this->axisDim[2];
  //     const size_t ZCHAN = 0;

  //     size_t left,right,top,bottom;

  //     for(size_t z = 0; z < zdim; z++){

  // 	for(size_t row=0;row<ydim;row++){
  // 	  left=0;
  // 	  right=0;
  // 	  while((left<xdim)&&
  // 		(this->par.isBlank(this->array[row*xdim+left+ZCHAN*xdim*ydim])) ) {
  // 	    left++;
  // 	  }
  // 	  while((right<xdim)&&
  // 		(this->par.isBlank(this->array[row*xdim+(xdim-1-right)+ZCHAN*xdim*ydim]))){
  // 	    right++;
  // 	  }

  // 	  if( ((z==0)&&(row==0)) || (left < this->par.getBorderLeft()) ) 
  // 	    this->par.setBorderLeft(left);
  // 	  if( ((z==0)&&(row==0)) || (right < this->par.getBorderRight()) ) 
  // 	    this->par.setBorderRight(right);

  // 	}
    
  // 	for(size_t col=0;col<xdim;col++){
  // 	  bottom=0;
  // 	  top=0;
  // 	  while((bottom<ydim)&&
  // 		(this->par.isBlank(this->array[col+bottom*xdim+ZCHAN*xdim*ydim])) ) bottom++;

  // 	  while((top<ydim)&&
  // 		(this->par.isBlank(this->array[col+(ydim-1-top)*xdim+ZCHAN*xdim*ydim])) ) top++;

  // 	  if( ((z==0)&&(col==0)) || (bottom < this->par.getBorderBottom()) ) 
  // 	    this->par.setBorderBottom(bottom);
  // 	  if( ((z==0)&&(col==0)) || (top < this->par.getBorderTop()) ) 
  // 	    this->par.setBorderTop(top);

  // 	}

  //     }
    
  //     left = this->par.getBorderLeft();
  //     right = this->par.getBorderRight();
  //     top = this->par.getBorderTop();
  //     bottom = this->par.getBorderBottom();


  //     size_t oldpos,newpos;
    
  //     // Do the trimming, but only if we need to -- is there a border of Blanks?
  //     if(((left>0)||(right>0)||(bottom>0)||(top>0)) && (right>left) && (top>bottom)) {

  // 	  this->axisDim[0] = xdim - left - right; 
  // 	  this->axisDim[1] = ydim - bottom - top; 
  // 	  this->axisDim[2] = zdim;
  // 	  this->numPixels = this->axisDim[0]*this->axisDim[1]*this->axisDim[2];

  // 	// Trim the array of pixel values
  // 	float *newarray  = new float[this->numPixels];
  // 	for(size_t x = 0; x < axisDim[0]; x++){
  // 	  for(size_t y = 0; y < axisDim[1]; y++){
  // 	    for(size_t z = 0; z < axisDim[2]; z++){ 
  // 	      oldpos = (x+left) + (y+bottom)*xdim + z*xdim*ydim;
  // 	      newpos = x + y*this->axisDim[0] + 
  // 		z*this->axisDim[0]*this->axisDim[1];
  // 	      newarray[newpos]  = this->array[oldpos];
  // 	    }
  // 	  }
  // 	}
  // 	delete [] this->array;
  // 	this->array = newarray;

  // 	// Trim the array of baseline values
  // 	if(this->par.getFlagBaseline()){
  // 	  float *newarray  = new float[this->numPixels];
  // 	  for(size_t x = 0; x < axisDim[0]; x++){
  // 	    for(size_t y = 0; y < axisDim[1]; y++){
  // 	      for(size_t z = 0; z < axisDim[2]; z++){ 
  // 		oldpos = (x+left) + (y+bottom)*xdim + z*xdim*ydim;
  // 		newpos = x + y*this->axisDim[0] + 
  // 		  z*this->axisDim[0]*this->axisDim[1];
  // 		newarray[newpos]  = this->baseline[oldpos];
  // 	      }
  // 	    }
  // 	  }
  // 	  delete [] this->baseline;
  // 	  this->baseline = newarray;
  // 	}

  // 	// Trim the 2-D detection map
  // 	short *newdetect = new short[this->axisDim[0]*this->axisDim[1]];
  // 	for(size_t x = 0; x < axisDim[0]; x++){
  // 	  for(size_t y = 0; y < axisDim[1]; y++){
  // 	    oldpos = (x+left) + (y+bottom)*xdim;
  // 	    newpos = x + y*this->axisDim[0];
  // 	    newdetect[newpos] = this->detectMap[oldpos];
  // 	  }
  // 	}
  // 	delete [] this->detectMap;
  // 	this->detectMap = newdetect;

  // 	if(this->par.getFlagATrous() || this->par.getFlagSmooth()){
  // 	  // Trim the reconstructed array if we are going to do the
  // 	  // reconstruction or smooth the array
  // 	  float *newrecon  = new float[this->numPixels];
  // 	  for(size_t x = 0; x < axisDim[0]; x++){
  // 	    for(size_t y = 0; y < axisDim[1]; y++){
  // 	      for(size_t z = 0; z < axisDim[2]; z++){ 
  // 		oldpos = (x+left) + (y+bottom)*xdim + z*xdim*ydim;
  // 		newpos = x + y*this->axisDim[0] + 
  // 		  z*this->axisDim[0]*this->axisDim[1];
  // 		newrecon[newpos] = this->recon[oldpos];	  
  // 	      }
  // 	    }
  // 	  }
  // 	  delete [] this->recon;
  // 	  this->recon = newrecon;
  // 	}

  // 	// Set the flag indicating trimming has taken place only if it has.
  // 	this->par.setFlagCubeTrimmed(true);

  //     }

  //   }

  // }


  void Cube::unTrimCube()
  {
    /// @details
    ///  If the cube has been trimmed by trimCube(), this task adds back the 
    ///   BLANK pixels on the edges, so that it returns to its original size.
    ///   All arrays are similarly edited.

    if(this->par.getFlagCubeTrimmed()){

      size_t left = this->par.getBorderLeft();
      size_t right = this->par.getBorderRight();
      size_t top = this->par.getBorderTop();
      size_t bottom = this->par.getBorderBottom();

      size_t smallXDim = this->axisDim[0];
      size_t smallYDim = this->axisDim[1];
      size_t smallZDim = this->axisDim[2];

      // first correct the dimension sizes 
      this->axisDim[0] = smallXDim + left + right; 
      this->axisDim[1] = smallYDim + bottom + top; 
      this->axisDim[2] = smallZDim;
      this->numPixels = this->axisDim[0]*this->axisDim[1]*this->axisDim[2];

      size_t pos,smlpos;
      bool isDud;

      // Correct the array of pixel values
      float *newarray  = new float[this->numPixels];
      for(size_t x = 0; x < this->axisDim[0]; x++){
	for(size_t y = 0; y < this->axisDim[1]; y++){
	  isDud = (x<left) || (x>=smallXDim+left) || 
	    (y<bottom) || (y>=smallYDim+bottom);
	
	  for(size_t z = 0; z < this->axisDim[2]; z++){ 
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
	for(size_t x = 0; x < this->axisDim[0]; x++){
	  for(size_t y = 0; y < this->axisDim[1]; y++){
	    isDud = (x<left) || (x>=smallXDim+left) || 
	      (y<bottom) || (y>=smallYDim+bottom);
	  
	    for(size_t z = 0; z < this->axisDim[2]; z++){ 
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
	for(size_t x = 0; x < this->axisDim[0]; x++){
	  for(size_t y = 0; y < this->axisDim[1]; y++){
	    isDud = (x<left) || (x>=smallXDim+left) || 
	      (y<bottom) || (y>=smallYDim+bottom);
	
	    for(size_t z = 0; z < this->axisDim[2]; z++){ 
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
      for(size_t x = 0; x < this->axisDim[0]; x++){
	for(size_t y = 0; y < this->axisDim[1]; y++){
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
  
      std::vector<Detection>::iterator obj;
      for(obj=this->objectList->begin();obj<this->objectList->end();obj++){
	obj->addOffsets(left,bottom,0);
      }

    }

  }

}
