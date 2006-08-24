#include <iostream>
#include <iomanip>
#include <sstream>
#include <math.h>
#include <cpgplot.h>
#include <cpgsbox.h>
#include <pgwcsl.h>
#include <wcs.h>
#include <duchamp.hh>
#include <param.hh>
#include <Cubes/cubes.hh>
#include <Cubes/plots.hh>
#include <Utils/utils.hh>


void Cube::plotDetectionMap(string pgDestination)
{
  /** 
   *  Cube::plotDetectionMap(string)
   *    Creates a map of the spatial locations of the detections, which is written to the 
   *     PGPlot device given by pgDestination.
   *    The map is done in greyscale, where the scale indicates the number of velocity channels
   *     that each spatial pixel is detected in.
   *    The boundaries of each detection are drawn, and each object is numbered (to match
   *     the output list and spectra).
   *    The primary grid scale is pixel coordinate, and if the WCS is valid, the correct
   *     WCS gridlines are also drawn.
   */

  // These are the minimum values for the X and Y ranges of the box drawn by pgplot 
  //   (without the half-pixel difference).
  // The -1 is necessary because the arrays we are dealing with start at 0 index, while
  // the ranges given in the subsection start at 1... 
  float boxXmin = this->par.getXOffset() - 1;
  float boxYmin = this->par.getYOffset() - 1;

  long xdim=this->axisDim[0];
  long ydim=this->axisDim[1];
  Plot::ImagePlot newplot;
  int flag = newplot.setUpPlot(pgDestination.c_str(),float(xdim),float(ydim));

  if(flag<=0){
    duchampError("plotDetectionMap", "Could not open PGPlot device "+pgDestination+".\n");
  }
  else{

    newplot.makeTitle(this->pars().getImageFile());

    newplot.drawMapBox(boxXmin+0.5,boxXmin+xdim+0.5,boxYmin+0.5,boxYmin+ydim+0.5,"X pixel","Y pixel");

    if(this->objectList.size()>0){ // if there are no detections, there will be nothing to plot here

      float *detectMap = new float[xdim*ydim];
      int maxNum;
      for(int pix=0;pix<xdim*ydim;pix++){
	detectMap[pix] = float(this->detectMap[pix]);  
	if((pix==0)||(this->detectMap[pix]>maxNum)) maxNum = this->detectMap[pix];
      }

      maxNum = 5 * ((maxNum-1)%5 + 1);  // move to next multiple of 5

      float tr[6] = {boxXmin,1.,0.,boxYmin,0.,1.};
      cpggray(detectMap,xdim,ydim,1,xdim,1,ydim,maxNum,0,tr);  

      delete [] detectMap;
      cpgbox("bcnst",0.,0,"bcnst",0.,0);
      cpgsch(1.5);
      cpgwedg("rg",3.2,2,maxNum,0,"Number of detected channels");
    }

    if(this->head.isWCS()) this->plotWCSaxes();
  
    if(this->objectList.size()>0){ // now show and label each detection, drawing over the WCS lines.

      cpgsch(1.0);
      cpgsci(2);
      cpgslw(2);    
      float xoffset=0.;
      float yoffset=newplot.cmToCoord(0.5);
      if(this->par.drawBorders()){
	cpgsci(4);
	for(int i=0;i<this->objectList.size();i++) this->objectList[i].drawBorders(0,0);
	cpgsci(2);
      }
      std::stringstream label;
      cpgslw(1);
      for(int i=0;i<this->objectList.size();i++){
	cpgpt1(this->par.getXOffset()+this->objectList[i].getXcentre(), 
	       this->par.getYOffset()+this->objectList[i].getYcentre(), 
	       5);
	label.str("");
	label << this->objectList[i].getID();
	cpgptxt(this->par.getXOffset()+this->objectList[i].getXcentre()-xoffset, 
		this->par.getYOffset()+this->objectList[i].getYcentre()-yoffset, 
		0, 0.5, label.str().c_str());
      }

    }

    cpgclos();
  }
}

/*********************************************************/

void Cube::plotMomentMap(string pgDestination)
{
  /** 
   *  Cube::plotMomentMap(string)
   *    Creates a 0th moment map of the detections, which is written to the 
   *     PGPlot device given by pgDestination.
   *    The map is done in greyscale, where the scale indicates the integrated flux at each
   *     spatial pixel.
   *    The boundaries of each detection are drawn, and each object is numbered (to match
   *     the output list and spectra).
   *    The primary grid scale is pixel coordinate, and if the WCS is valid, the correct
   *     WCS gridlines are also drawn.
   */

  float boxXmin = this->par.getXOffset() - 1;
  float boxYmin = this->par.getYOffset() - 1;

  long xdim=this->axisDim[0];
  long ydim=this->axisDim[1];
  long zdim=this->axisDim[2];

  Plot::ImagePlot newplot;

  int flag = newplot.setUpPlot(pgDestination.c_str(),float(xdim),float(ydim));
    
  if(flag<=0){
    duchampError("plotMomentMap", "Could not open PGPlot device "+pgDestination+".\n");
  }
  else{

    if(this->objectList.size()==0){ // if there are no detections, we plot an empty field.

      newplot.makeTitle(this->pars().getImageFile());
    
      newplot.drawMapBox(boxXmin+0.5,boxXmin+xdim+0.5,boxYmin+0.5,boxYmin+ydim+0.5,"X pixel","Y pixel");

      this->plotBlankEdges();

      if(this->head.isWCS()) this->plotWCSaxes();

    }
    else {  // if there are some detections, do the calculations first before plotting anything.
  
      newplot.makeTitle(this->pars().getImageFile());
    
      newplot.drawMapBox(boxXmin+0.5,boxXmin+xdim+0.5,boxYmin+0.5,boxYmin+ydim+0.5,"X pixel","Y pixel");

      if(pgDestination=="/xs") 
	cpgptxt(boxXmin+0.5+xdim/2.,boxYmin+0.5+ydim/2.,0,0.5,"Calculating map...");

      bool *isObj = new bool[xdim*ydim*zdim];
      for(int i=0;i<xdim*ydim*zdim;i++) isObj[i] = false;
      for(int i=0;i<this->objectList.size();i++){
	for(int p=0;p<this->objectList[i].getSize();p++){
	  int pixelpos = this->objectList[i].getX(p) + xdim*this->objectList[i].getY(p) 
	    + xdim*ydim*this->objectList[i].getZ(p);
	  isObj[pixelpos] = true;
	}
      }

      float *momentMap = new float[xdim*ydim];
      // Initialise to zero
      for(int i=0;i<xdim*ydim;i++) momentMap[i] = 0.;

      // if we are looking for negative features, we need to invert the detected pixels for the moment map
      float sign = 1.;
      if(this->pars().getFlagNegative()) sign = -1.;

      float deltaVel;
      double x,y;

      double *zArray  = new double[zdim];
      for(int z=0; z<zdim; z++) zArray[z] = double(z);
    
      double *world  = new double[zdim];

      for(int pix=0; pix<xdim*ydim; pix++){ 

	x = double(pix%xdim);
	y = double(pix/xdim);

	//       for(int z=0; z<zdim; z++){
	// 	double zpos = double(z);
	// 	world[z] = this->head.pixToVel(x,y,zpos);
	//       }

	delete [] world;
	world = this->head.pixToVel(x,y,zArray,zdim);
      
	for(int z=0; z<zdim; z++){      
	  int pos =  z*xdim*ydim + pix;  // the voxel in the cube
	  if(isObj[pos]){ // if it's an object pixel...
	    // delta-vel is half the distance between adjacent channels.
	    // if at end, then just use 0-1 or (zdim-1)-(zdim-2) distance
	    if(z==0){
	      if(zdim==1) deltaVel=1.; // pathological case -- if 2D image instead of cube.
	      else deltaVel = world[z+1] - world[z];
	    }
	    else if(z==(zdim-1)) deltaVel = world[z-1] - world[z];
	    else deltaVel = (world[z+1] - world[z-1]) / 2.;

	    momentMap[pix] += sign * this->array[pos] * fabs(deltaVel);

	  }
	}

      }
    
      delete [] world;
      delete [] zArray;

      float *temp = new float[xdim*ydim];
      int count=0;
      for(int i=0;i<xdim*ydim;i++) {
	if(momentMap[i]>0.){
	  bool addPixel = false;
	  for(int z=0;z<zdim;z++) addPixel = addPixel || isObj[z*xdim*ydim+i];
	  if(addPixel) temp[count++] = log10(momentMap[i]);
	}
      }
      float z1,z2;
      z1 = z2 = temp[0];
      for(int i=1;i<count;i++){
	if(temp[i]<z1) z1 = temp[i];
	if(temp[i]>z2) z2 = temp[i];
      }

      for(int i=0;i<xdim*ydim;i++) {
	bool addPixel = false;
	for(int z=0;z<zdim;z++) addPixel = addPixel || isObj[z*xdim*ydim+i];
	addPixel = addPixel && (momentMap[i]>0.);
	if(!addPixel) momentMap[i] = z1-1.;
	else momentMap[i] = log10(momentMap[i]);
      }

      // Have now done all necessary calculations for moment map.
      // Now produce the plot

      float tr[6] = {boxXmin,1.,0.,boxYmin,0.,1.};
      cpggray(momentMap,xdim,ydim,1,xdim,1,ydim,z2,z1,tr);
      cpgbox("bcnst",0.,0,"bcnst",0.,0);
      cpgsch(1.5);
      string wedgeLabel = "Flux ";
      if(this->pars().getFlagNegative()) wedgeLabel = "-1. * " + wedgeLabel;
      if(this->head.isWCS()) wedgeLabel += "[" + this->head.getIntFluxUnits() + "]";
      cpgwedglog("rg",3.2,2,z2,z1,wedgeLabel.c_str());

      delete [] momentMap;
      delete [] temp;
      delete [] isObj;

      this->plotBlankEdges();

      if(this->head.isWCS()) this->plotWCSaxes();
  
      // now show and label each detection, drawing over the WCS lines.
      cpgsch(1.0);
      cpgsci(2);
      cpgslw(2);
      float xoffset=0.;
      float yoffset=newplot.cmToCoord(0.5);
      if(this->par.drawBorders()){
	cpgsci(4);
	for(int i=0;i<this->objectList.size();i++) this->objectList[i].drawBorders(0,0);
	cpgsci(2);
      }
      std::stringstream label;
      cpgslw(1);
      for(int i=0;i<this->objectList.size();i++){
	cpgpt1(this->par.getXOffset()+this->objectList[i].getXcentre(), 
	       this->par.getYOffset()+this->objectList[i].getYcentre(),
	       5);
	label.str("");
	label << this->objectList[i].getID();
	cpgptxt(this->par.getXOffset()+this->objectList[i].getXcentre()-xoffset, 
		this->par.getYOffset()+this->objectList[i].getYcentre()-yoffset, 
		0, 0.5, label.str().c_str());
      }

    }

  
    cpgclos();
  }
  
}

/*********************************************************/


void Cube::plotWCSaxes()
{
  /**
   *  Cube::plotWCSaxes()
   *    A front-end to the cpgsbox command, to draw the gridlines for the WCS over the 
   *      current plot.
   *    Lines are drawn in dark green over the full plot area, and the axis labels are
   *      written on the top and on the right hand sides, so as not to conflict with
   *      other labels.
   */

  float boxXmin,boxYmin;
//   if(!this->par.getFlagSubsection()){
//     float boxXmin = this->par.getXOffset() - 1;
//     float boxYmin = this->par.getYOffset() - 1;
//   }
//   else
    boxXmin = boxYmin = 0.;

  char idents[3][80], opt[2], nlcprm[1];

  wcsprm *tempwcs;
  tempwcs = this->head.getWCS();

  strcpy(idents[0], tempwcs->lngtyp);
  strcpy(idents[1], tempwcs->lattyp);
  strcpy(idents[2], "");
  if(strcmp(tempwcs->lngtyp,"RA")==0) opt[0] = 'G';
  else opt[0] = 'D';
  opt[1] = 'E';

  float  blc[2], scl, trc[2];
  blc[0] = boxXmin + 0.5;
  blc[1] = boxYmin + 0.5;
  trc[0] = boxXmin + this->axisDim[0]+0.5;
  trc[1] = boxYmin + this->axisDim[1]+0.5;
  
  int lineWidth;
  cpgqlw(&lineWidth);
  int colour;
  cpgqci(&colour);
  float size;
  cpgqch(&size);
  cpgsci(3);
  cpgsch(0.8);
  int    c0[7], ci[7], gcode[2], ic, ierr;
  for(int i=0;i<7;i++) c0[i] = -1;
   /* define a Dark Green colour. */
  cpgscr(17, 0.3, 0.5, 0.3);

  gcode[0] = 2;  // type of grid to draw: 0=none, 1=ticks only, 2=full grid
  gcode[1] = 2;

  double cache[257][4], grid1[9], grid2[9], nldprm[8];
  grid1[0] = 0.0;  
  grid2[0] = 0.0;

//   nlfunc_t pgwcsl_;
  // Draw the celestial grid with no intermediate tick marks.
  // Set LABCTL=2100 to write 1st coord on top, and 2nd on right
  //Colour indices used by cpgsbox -- make it all the same colour for thin line case.
  ci[0] = 17; // grid lines, coord 1
  ci[1] = 17; // grid lines, coord 2
  ci[2] = 17; // numeric labels, coord 1
  ci[3] = 17; // numeric labels, coord 2
  ci[4] = 17; // axis annotation, coord 1
  ci[5] = 17; // axis annotation, coord 2
  ci[6] = 17; // title

  cpgsbox(blc, trc, idents, opt, 2100, 0, ci, gcode, 0.0, 0, grid1, 0, grid2,
	  0, pgwcsl_, 1, WCSLEN, 1, nlcprm, (int *)tempwcs, 
	  nldprm, 256, &ic, cache, &ierr);

  wcsfree(tempwcs);

  cpgsci(colour);
  cpgsch(size);
  cpgslw(lineWidth);
}

