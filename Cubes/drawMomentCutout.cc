#include <iostream>
#include <cpgplot.h>
#include <math.h>
#include <wcs.h>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>

void drawMomentCutout(Cube &cube, Detection &object)
{
  float x1,x2,y1,y2;
  cpgqwin(&x1,&x2,&y1,&y2);

  long size = (object.getXmax()-object.getXmin()+1);
  if(size<(object.getYmax()-object.getYmin()+1)) 
    size = object.getYmax()-object.getYmin()+1;
  size += 20;

  float blankVal = cube.pars().getBlankPixVal();

  long xmin = (object.getXmax()+object.getXmin())/2 - size/2 + 1;
  long xmax = (object.getXmax()+object.getXmin())/2 + size/2;
  long ymin = (object.getYmax()+object.getYmin())/2 - size/2 + 1;
  long ymax = (object.getYmax()+object.getYmin())/2 + size/2;
  long zmin = object.getZmin();
  long zmax = object.getZmax();

  float *image = new float[size*size];
  for(int i=0;i<size*size;i++) image[i]=blankVal;
  long *dim = new long[3];
  cube.getDimArray(dim);

  int imPos,cubePos;
  float val;
  for(int z=zmin; z<=zmax; z++){
    for(int x=xmin; x<=xmax; x++){
      for(int y=ymin; y<=ymax; y++){
	imPos = (y-ymin) * size + (x-xmin);
	cubePos = (z)*dim[0]*dim[1]+(y)*dim[0]+(x);
	if((x<0)||(x>=dim[0])||(y<0)||(y>=dim[1])) // if outside the boundaries
	  image[imPos] = blankVal;
	else{
	  val = cube.getPixValue(cubePos);
	  if (!cube.pars().isBlank(val)) // if pixel's not blank
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
  while(image[ct]==cube.pars().getBlankPixVal()) ct++;
  z1 = z2 = image[ct];
  for(int i=1;i<size*size;i++){
    if(// (!cube.pars().getFlagBlankPix())||
       (image[i]!=blankVal)){
      if(image[i]<z1) z1=image[i];
      if(image[i]>z2) z2=image[i];
    }
  }

  float tr[6] = {xmin-1,1.,0.,ymin-1,0.,1.};

  cpgswin(xmin-0.5,xmax-0.5,ymin-0.5,ymax-0.5);
  cpggray(image, size, size, 1, size, 1, size, z1, z2, tr);

  delete [] image;
  delete [] dim;

  // Draw the borders around the object
  int ci;
  cpgqci(&ci);
  cpgsci(4);
  cpgsfs(2);
  if(cube.pars().drawBorders()) 
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

  if(cube.isWCS()){
    // Now draw a tick mark to indicate size -- 30 arcmin in length
    wcsprm *wcs = new wcsprm;
    wcs = cube.getWCS();

    double *pix   = new double[3];
    double *world = new double[3];
    pix[0] = double(xmin) + double(object.getXOffset()) + 2.;
    pix[1] = double(ymin) + double(object.getYOffset()) + 2.;
    pix[2] = object.getZcentre();
    pixToWCSSingle(wcs,pix,world);
    world[0] -= 0.25;
    wcsToPixSingle(wcs,world,pix);
    wcsfree(wcs);

    float tickpt1 = xmin + 2.;
    float tickpt2 = pix[0] - object.getXOffset();
    float tickpt3 = ymin + 2.;
    cpgsci(2);
    int thick;
    cpgqlw(&thick);
    cpgslw(3);
    cpgerrx(1,&tickpt1,&tickpt2,&tickpt3,2.);
    //   cpgtext(0.5*(tickpt1+tickpt2)-1,tickpt3+2.,"30\'");
    cpgslw(thick);
    cpgsci(1);

    delete [] pix;
    delete [] world;
  }

  cpgsci(1);
  cpgswin(x1,x2,y1,y2);

}

// void Detection::drawBorders(Detection &object, int xoffset, int yoffset)
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
