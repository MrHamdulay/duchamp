#include <iostream>
#include <sstream>
#include <cpgplot.h>
#include <math.h>
#include <wcs.h>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>

void Cube::drawMomentCutout(Detection &object)
{
  /** 
   *  Cube::drawMomentCutout(object)
   *
   *   A routine to draw the 0th moment for the given detection
   *    using the flux given by the pixel array in the Cube.
   *   The 0th moment is constructed by adding the flux of each
   *    pixel within the full extent of the object (this may be more
   *    pixels than were actually detected in the object)
   *   A tick mark is also drawn to indicate angular scale (but only
   *    if the WCS for the Cube is valid).
   */

  float x1,x2,y1,y2;
  cpgqwin(&x1,&x2,&y1,&y2);

  long size = (object.getXmax()-object.getXmin()+1);
  if(size<(object.getYmax()-object.getYmin()+1)) 
    size = object.getYmax()-object.getYmin()+1;
  size += 20;

  float blankVal = this->par.getBlankPixVal();

  long xmin = (object.getXmax()+object.getXmin())/2 - size/2 + 1;
  long xmax = (object.getXmax()+object.getXmin())/2 + size/2;
  long ymin = (object.getYmax()+object.getYmin())/2 - size/2 + 1;
  long ymax = (object.getYmax()+object.getYmin())/2 + size/2;
  long zmin = object.getZmin();
  long zmax = object.getZmax();

  float *image = new float[size*size];
  for(int i=0;i<size*size;i++) image[i]=blankVal;

  int imPos,cubePos;
  float val;
  for(int z=zmin; z<=zmax; z++){
    for(int x=xmin; x<=xmax; x++){
      for(int y=ymin; y<=ymax; y++){
	imPos = (y-ymin) * size + (x-xmin);
	cubePos = (z)*this->axisDim[0]*this->axisDim[1]+(y)*this->axisDim[0]+(x);
	if((x<0)||(x>=this->axisDim[0])||(y<0)||(y>=this->axisDim[1])) // if outside the boundaries
	  image[imPos] = blankVal;
	else{
	  val = this->array[cubePos];
	  if (!this->par.isBlank(val)) // if pixel's not blank
	    image[imPos] += val;
	}
      }
    }
  }

  for(int i=0;i<size*size;i++){
    if(image[i]!=blankVal){ // if there is some signal on this pixel
      image[i]-=blankVal;   // remove the starting value so we just have the signal
      image[i] /= float(zmax-zmin+1); // normalise by the velocity width
    }
  }

  // now work out the greyscale display limits, excluding blank pixels where necessary.
  float z1,z2,median,madfm;
  int ct=0;
  while(image[ct]==this->par.getBlankPixVal()) ct++;
  z1 = z2 = image[ct];
  for(int i=1;i<size*size;i++){
    if(image[i]!=blankVal){
      if(image[i]<z1) z1=image[i];
      if(image[i]>z2) z2=image[i];
    }
  }

  float tr[6] = {xmin-1,1.,0.,ymin-1,0.,1.};

  cpgswin(xmin-0.5,xmax-0.5,ymin-0.5,ymax-0.5);
  cpggray(image, size, size, 1, size, 1, size, z1, z2, tr);

  delete [] image;

  // Draw the borders around the object
  int ci;
  cpgqci(&ci);
  cpgsci(4);
  cpgsfs(2);
  if(this->par.drawBorders()) 
    object.drawBorders(xmin,ymin);
  else 
    cpgrect(object.getXmin()-xmin+0.5,object.getXmax()-xmin+1.5,
	    object.getYmin()-ymin+0.5,object.getYmax()-ymin+1.5);
  /*
    To get the borders localised correctly, we need to subtract (xmin-1) from the 
    X values. We then subtract 0.5 for the left hand border (to place it on the
    pixel border), and add 0.5 for the right hand border. Similarly for y.
  */
  cpgsci(ci);

  if(this->head.isWCS()){
    // Now draw a tick mark to indicate size -- 15 arcmin in length
    this->drawScale(xmin+2.,ymin+2.,object.getZcentre(),0.25);
  }

  cpgsci(1);
  cpgswin(x1,x2,y1,y2);

}

void Cube::drawScale(float xstart, float ystart, float channel, float scaleLength)
{
  /** 
   *  Cube::drawScale(xstart, ystart, channel, scaleLength)
   *
   *   A routine to draw a scale bar on a (pre-existing) PGPlot image.
   *   It uses an iterative technique to move from the given start position 
   *    (xstart,ystart) along the positive x-direction so that the length is
   *    within 1% of the requested value scaleLength.
   *   The parameter "channel" is required for the wcslib calculations, as the 
   *    positions could theoretically change with channel.
   */

  double *pix1   = new double[3];
  double *pix2   = new double[3];
  double *world1 = new double[3];
  double *world2 = new double[3];
  pix1[0] = pix2[0] = xstart + this->par.getXOffset();
  pix1[1] = pix2[1] = ystart + this->par.getYOffset();
  pix1[2] = pix2[2] = channel;
  this->head.pixToWCS(pix1,world1);

  double angSep=0.;
  bool keepGoing=false;
  float step = 1.;
  do{
    if(angSep>scaleLength){
      pix2[0] -= step;
      step /= 2.;
    }
    pix2[0] += step;
    this->head.pixToWCS(pix2,world2);
    angSep = angularSeparation(world1[0],world1[1],world2[0],world2[1]);
  }while((fabs(angSep-scaleLength)/scaleLength)>0.01); // look for 1% change

  float tickpt1 = pix1[0] - this->par.getXOffset();
  float tickpt2 = pix2[0] - this->par.getXOffset();
  float tickpt3 = ystart;
  int colour;
  cpgqci(&colour);
  cpgsci(2);
  int thick;
  cpgqlw(&thick);
  cpgslw(3);
  cpgerrx(1,&tickpt1,&tickpt2,&tickpt3,2.);
  cpgslw(thick);

  std::stringstream text;
  text << scaleLength*60 << "'";
  float size,xch,ych;
  cpgqch(&size);
  cpgsch(0.4);
  cpgqcs(4,&xch,&ych); // get the character size in world coords
  cpgptxt((tickpt1+tickpt2)/2., ystart+ych, 0, 0.5, text.str().c_str());
  cpgsch(size);
  cpgsci(colour);

  delete [] pix1,pix2;
  delete [] world1,world2;


}


void Detection::drawBorders(int xoffset, int yoffset)
{

  float x1,x2,y1,y2;
  cpgqwin(&x1,&x2,&y1,&y2);
  int xsize = int(x2 - x1) + 1;
  int ysize = int(y2 - y1) + 1;

  bool *isObj = new bool[xsize*ysize];
  for(int i=0;i<xsize*ysize;i++) isObj[i]=false;
  for(int i=0;i<this->pix.size();i++)
    isObj[ (this->pix[i].getY()-yoffset)*xsize + (this->pix[i].getX()-xoffset) ] = true;
  

  cpgswin(0,xsize-1,0,ysize-1);
  for(int x=this->xmin; x<=this->xmax; x++){
    // for each column...
    for(int y=1;y<ysize;y++){
      int current = y*xsize + (x-xoffset);
      int previous = (y-1)*xsize + (x-xoffset);
      if((isObj[current]&&!isObj[previous])||(!isObj[current]&&isObj[previous])){
	cpgmove(x-xoffset+0, y+0);
	cpgdraw(x-xoffset+1, y+0);
      }
    }
  }
  for(int y=this->ymin; y<=this->ymax; y++){
    // now for each row...
    for(int x=1;x<xsize;x++){
      int current = (y-yoffset)*xsize + x;
      int previous = (y-yoffset)*xsize + x - 1;
      if((isObj[current]&&!isObj[previous])||(!isObj[current]&&isObj[previous])){
	cpgmove(x+0, y-yoffset+0);
	cpgdraw(x+0, y-yoffset+1);
      }
    }
  }
  cpgswin(x1,x2,y1,y2);
  
  delete [] isObj;

}    
