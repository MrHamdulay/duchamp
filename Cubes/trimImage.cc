#include <iostream>
#include <Cubes/cubes.hh>

void Cube::trimCube()
{
  /**
   *  Cube::trimCube()
   *
   *   If the blankPix flag has been set, this routine trims excess blank pixels 
   *    from the edges of the spatial image.
   *   It uses as its template the first channel, assuming that its non-BLANK size
   *    is representative of the rest of the channels.
   *   The edges are moved in until the first non-BLANK pixel is encountered.
   *   All other arrays are similarly edited, and the amount of trimming is recorded.
   *
   */

  if(this->par.getFlagBlankPix()) {

    long xdim = this->axisDim[0];
    long ydim = this->axisDim[1];
    long zdim = this->axisDim[2];
    const long ZCHAN = 0;

    for(int z = 0; z < zdim; z++){

      for(int row=0;row<ydim;row++){
	int borderLeft=0;
	int borderRight=0;
	while((borderLeft<xdim)&&
	      (this->par.isBlank(this->array[row*xdim+borderLeft+ZCHAN*xdim*ydim])) ) {
	  borderLeft++;
	}
	while((borderRight<xdim)&&
	      (this->par.isBlank(this->array[row*xdim+(xdim-1-borderRight)+ZCHAN*xdim*ydim]))){
	  borderRight++;
	}

	if( ((z==0)&&(row==0)) || (borderLeft < this->par.getBorderLeft()) ) 
	  this->par.setBorderLeft(borderLeft);
	if( ((z==0)&&(row==0)) || (borderRight < this->par.getBorderRight()) ) 
	  this->par.setBorderRight(borderRight);

      }
    
      for(int col=0;col<xdim;col++){
	int borderBottom=0;
	int borderTop=0;
	while((borderBottom<ydim)&&
	      (this->par.isBlank(this->array[col+borderBottom*xdim+ZCHAN*xdim*ydim])) ) borderBottom++;

	while((borderTop<ydim)&&
	      (this->par.isBlank(this->array[col+(ydim-1-borderTop)*xdim+ZCHAN*xdim*ydim])) ) borderTop++;

	if( ((z==0)&&(col==0)) || (borderBottom < this->par.getBorderBottom()) ) 
	  this->par.setBorderBottom(borderBottom);
	if( ((z==0)&&(col==0)) || (borderTop < this->par.getBorderTop()) ) 
	  this->par.setBorderTop(borderTop);

      }

    }
    
    this->axisDim[0] = xdim - this->par.getBorderLeft() - this->par.getBorderRight(); 
    this->axisDim[1] = ydim - this->par.getBorderBottom() - this->par.getBorderTop(); 
    this->axisDim[2] = zdim;
    this->numPixels = this->axisDim[0]*this->axisDim[1]*this->axisDim[2];

    long oldpos,newpos;
    
    // Do the trimming, but only if we need to -- is there a border of Blanks?
    if((this->par.getBorderLeft()>0)||(this->par.getBorderRight()>0)||
       (this->par.getBorderBottom()>0)||(this->par.getBorderTop()>0)) {

      // Trim the array of pixel values
      float *newarray  = new float[this->numPixels];
      for(int x = 0; x < axisDim[0]; x++){
	for(int y = 0; y < axisDim[1]; y++){
	  for(int z = 0; z < axisDim[2]; z++){ 
	    oldpos = (x+this->par.getBorderLeft()) + (y+this->par.getBorderBottom())*xdim + z*xdim*ydim;
	    newpos = x + y*axisDim[0] + z*axisDim[0]*axisDim[1];
	    newarray[newpos]  = this->array[oldpos];
	  }
	}
      }
      delete [] this->array;
      this->array = newarray;

      // Trim the 2-D detection map
      short *newdetect = new short[this->axisDim[0]*this->axisDim[1]];
      for(int x = 0; x < axisDim[0]; x++){
	for(int y = 0; y < axisDim[1]; y++){
	  oldpos = (x+this->par.getBorderLeft()) + (y+this->par.getBorderBottom())*xdim;
	  newpos = x + y*axisDim[0];
	  newdetect[newpos] = this->detectMap[oldpos];
	}
      }
      delete [] this->detectMap;
      this->detectMap = newdetect;

      if(this->par.getFlagATrous()){
	// Trim the reconstructed array if we are going to do the reconstruction
	float *newrecon  = new float[this->numPixels];
	for(int x = 0; x < axisDim[0]; x++){
	  for(int y = 0; y < axisDim[1]; y++){
	    for(int z = 0; z < axisDim[2]; z++){ 
	      oldpos = (x+this->par.getBorderLeft()) + (y+this->par.getBorderBottom())*xdim + z*xdim*ydim;
	      newpos = x + y*axisDim[0] + z*axisDim[0]*axisDim[1];
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
  /**
   *  Cube::unTrimCube()
   *
   *   If the cube has been trimmed by trimCube(), this task adds back the BLANK pixels on
   *    the edges, so that it returns to its original size.
   *   All arrays are similarly edited.
   */

  if(this->par.getFlagCubeTrimmed()){

    long smallXDim = this->axisDim[0];
    long smallYDim = this->axisDim[1];
    long smallZDim = this->axisDim[2];

    // first correct the dimension sizes 
    this->axisDim[0] = smallXDim + this->par.getBorderLeft() + this->par.getBorderRight(); 
    this->axisDim[1] = smallYDim + this->par.getBorderBottom() + this->par.getBorderTop(); 
    this->axisDim[2] = smallZDim;
    this->numPixels = this->axisDim[0]*this->axisDim[1]*this->axisDim[2];

    long pos,smlpos;
    bool isDud;

    // Correct the array of pixel values
    float *newarray  = new float[this->numPixels];
    for(int x = 0; x < this->axisDim[0]; x++){
      for(int y = 0; y < this->axisDim[1]; y++){
	isDud = (x<this->par.getBorderLeft()) || (x>=smallXDim+this->par.getBorderLeft()) || 
	  (y<this->par.getBorderBottom()) || (y>=smallYDim+this->par.getBorderBottom());
	
	for(int z = 0; z < this->axisDim[2]; z++){ 
	  pos = x + y*this->axisDim[0] + z*this->axisDim[0]*this->axisDim[1];
	  smlpos = (x-this->par.getBorderLeft()) + (y-this->par.getBorderBottom())*smallXDim
	    + z * smallXDim * smallYDim;
	  if(isDud) newarray[pos] = this->par.getBlankPixVal();
	  else      newarray[pos] = this->array[smlpos];
	}
      }
    }
    delete [] this->array;
    this->array = newarray;

    if(this->reconExists){
      // Correct the reconstructed array
      float *newrecon   = new float[this->numPixels];
      for(int x = 0; x < this->axisDim[0]; x++){
	for(int y = 0; y < this->axisDim[1]; y++){
	  isDud = (x<this->par.getBorderLeft()) || (x>=smallXDim+this->par.getBorderLeft()) || 
	    (y<this->par.getBorderBottom()) || (y>=smallYDim+this->par.getBorderBottom());
	  
	  for(int z = 0; z < this->axisDim[2]; z++){ 
	    pos = x + y*this->axisDim[0] + z*this->axisDim[0]*this->axisDim[1];
	    smlpos = (x-this->par.getBorderLeft()) + (y-this->par.getBorderBottom())*smallXDim
	      + z * smallXDim * smallYDim;
	    if(isDud) newrecon[pos] = this->par.getBlankPixVal();
	    else      newrecon[pos] = this->recon[smlpos];
	  }
	}
      }
      delete [] this->recon;
      this->recon = newrecon;
    }

    // Correct the 2-D detection map
    short *newdetect = new short[this->axisDim[0]*this->axisDim[1]];
    for(int x = 0; x < this->axisDim[0]; x++){
      for(int y = 0; y < this->axisDim[1]; y++){
	pos = x + y*this->axisDim[0];
	smlpos = (x-this->par.getBorderLeft()) + (y-this->par.getBorderBottom())*smallXDim;
	isDud = (x<this->par.getBorderLeft()) || (x>=smallXDim+this->par.getBorderLeft()) || 
	  (y<this->par.getBorderBottom()) || (y>=smallYDim+this->par.getBorderBottom());
	if(isDud) newdetect[pos]=0;
	else newdetect[pos] = this->detectMap[smlpos];
      }
    }
    delete [] this->detectMap;
    this->detectMap = newdetect;    

  
    // Now update the positions for all the detections
  
    for(int i=0;i<this->objectList.size();i++){
      for(int pix=0;pix<objectList[i].getSize();pix++){
	long x = objectList[i].getX(pix);
	long y = objectList[i].getY(pix);
	objectList[i].setX(pix,x+this->par.getBorderLeft());
	objectList[i].setY(pix,y+this->par.getBorderBottom());
      }
      objectList[i].calcParams();
    }

  }

}
