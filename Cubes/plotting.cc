#include <iostream>
#include <iomanip>
#include <sstream>
#include <math.h>
#include <cpgplot.h>
#include <cpgsbox.h>
#include <pgwcsl.h>
#include <wcs.h>
#include <Utils/utils.hh>
#include <Cubes/cubes.hh>

/*********************************************************/
namespace Plot
{
  const float inchToCm=2.54;// Conversion factor from inches to centimetres.

  class ImagePlot
  {
    /** 
     *  ImagePlot class
     *    A class to hold the dimensions and set up for the plots used by the two 
     *     functions below.
     *    The physical dimensions (in inches) of the plot and the elements within
     *     it are stored, including maximum widths and heights (so that the plot will
     *     fit on an A4 page).
     *    Simple accessor functions are provided to enable access to quantities needed
     *     for pgplot routines.
     */
  public:
    ImagePlot(){
      paperWidth = 7.5; maxPaperHeight = 10.; marginWidth = 0.8; wedgeWidth = 0.7;
      imageWidth = paperWidth - 2*marginWidth - wedgeWidth;
    };
    ~ImagePlot(){};
  
    void setUpPlot(float x, float y){
      /**
       * setUpPlot(float x, float y)
       *  set the dimensions for the image, and calculate the required aspect ratios
       *   of the image and of the plot.
       *  If the resulting image is going to be tall enough to exceed the maximum height 
       *  (given the default width), then scale everything down by enough to make the 
       *  height equal to maxPaperHeight.
       */
      xdim = x;
      ydim = y;
      imageRatio= ydim / xdim; 
      aspectRatio =  (imageRatio*imageWidth + 2*marginWidth) / paperWidth;
      if((imageRatio*imageWidth + 2*marginWidth) > maxPaperHeight){
	float correction = maxPaperHeight / (imageRatio*imageWidth + 2*marginWidth);
	paperWidth *= correction;
	marginWidth *= correction;
	wedgeWidth *= correction;
	imageWidth = paperWidth - 2*marginWidth - wedgeWidth;
      }
    }
    float cmToCoord(float cm){return (cm/inchToCm) * ydim / (imageWidth*imageRatio);};
    float getMargin()     {return marginWidth;};
    float getPaperWidth() {return paperWidth;};
    float getImageWidth() {return imageWidth;};
    float getImageHeight(){return imageWidth*imageRatio;};
    float getAspectRatio(){return aspectRatio;};

  private:
    float paperWidth;       // Default (maximum) width of "paper" [inches]
    float maxPaperHeight;   // Maximum allowed height of paper [inches]
    float marginWidth;      // Width allowed for margins around main plot (ie. label & numbers) [inches]
    float wedgeWidth;       // Width allowed for placement of wedge on right hand side of plot. [inches]
    float imageWidth;       // Calculated total width of the image part of the plot [inches]
    float imageRatio;       // Aspect ratio of the image only (ie. y-value range / x-value range).
    float aspectRatio;      // Aspect ratio of whole plot.
    float xdim;             // Width of main plot, in display units.
    float ydim;             // Height of main plot, in display units.
  };
}

/*********************************************************/

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
  cpgopen(pgDestination.c_str());
  Plot::ImagePlot newplot;
  newplot.setUpPlot(float(xdim),float(ydim));
  cpgpap(newplot.getPaperWidth(),newplot.getAspectRatio());

  cpgvstd();
  cpgmtxt("t",2.7,0.5,0.5,this->pars().getImageFile().c_str());

  cpgvsiz(newplot.getMargin(),newplot.getMargin()+newplot.getImageWidth(),
	  newplot.getMargin(),newplot.getMargin()+newplot.getImageHeight());
  
  cpgslw(2);
  cpgswin(boxXmin+0.5,boxXmin+xdim+0.5,
	  boxYmin+0.5,boxYmin+ydim+0.5);
  cpgbox("bcst",0.,0,"bcst",0.,0);
  cpgslw(1);
  cpgbox("bcnst",0.,0,"bcnst",0.,0);
  cpglab("X pixel","Y pixel","");

  if(this->objectList.size()>0){ // if there are no detections, there will be nothing to plot here

    float *detectMap = new float[xdim*ydim];
    for(int pix=0;pix<xdim*ydim;pix++) detectMap[pix] = float(this->detectMap[pix]);  

    float tr[6] = {boxXmin,1.,0.,boxYmin,0.,1.};
    cpggray(detectMap,xdim,ydim,1,xdim,1,ydim,30,0,tr);  

    delete [] detectMap;
    cpgbox("bcnst",0.,0,"bcnst",0.,0);
    cpgsch(1.5);
    cpgwedg("rg",3.2,2,30,0,"Number of detected channels");
  }

  if(this->flagWCS) this->plotWCSaxes();
  
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

/*********************************************************/

void Cube::plotMomentMap(string pgDestination)
{
  /** 
   *  Cube::plotMomentMap(string)
   *    Creates a 1st moment map of the detections, which is written to the 
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

  cpgopen(pgDestination.c_str());
  Plot::ImagePlot newplot;
  newplot.setUpPlot(float(xdim),float(ydim));
  cpgpap(newplot.getPaperWidth(),newplot.getAspectRatio());

  cpgvstd();
  cpgmtxt("t",2.7,0.5,0.5,this->pars().getImageFile().c_str());

  cpgvsiz(newplot.getMargin(),newplot.getMargin()+newplot.getImageWidth(),
	  newplot.getMargin(),newplot.getMargin()+newplot.getImageHeight());
  
  cpgslw(2);
  cpgswin(boxXmin+0.5,boxXmin+xdim+0.5,
	  boxYmin+0.5,boxYmin+ydim+0.5);
  cpgbox("bcst",0.,0,"bcst",0.,0);
  cpgslw(1);
  cpgbox("bcnst",0.,0,"bcnst",0.,0);
  cpglab("X pixel","Y pixel","");

  if(this->objectList.size()>0){ // if there are no detections, there will be nothing to plot here

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

    float deltaVel;
    for(int pix=0; pix<xdim*ydim; pix++){ 
      // loop over each spatial pixel -- ie. each cell of momentMap
      int    *stat   = new    int[zdim];
      double *pixcrd = new double[zdim*3];
      double *imgcrd = new double[zdim*3];
      double *world  = new double[zdim*3];
      double *phi    = new double[zdim];
      double *theta  = new double[zdim];
      for(int i=0;i<zdim;i++){
	pixcrd[3*i]   =  pix%xdim;
	pixcrd[3*i+1] =  (pix/xdim);
	pixcrd[3*i+2] =  i;
      }
      wcsp2s(this->wcs, zdim, 3, pixcrd, imgcrd, phi, theta, world, stat);	
      for(int i=0;i<zdim;i++){
	// put velocity coords into km/s
	if(this->wcs->ctype[2]=="FREQ") 
	  world[3*i+2] = C_kms * (this->wcs->restfrq*this->wcs->restfrq - world[3*i+2]*world[3*i+2]) / 
	    (this->wcs->restfrq*this->wcs->restfrq + world[3*i+2]*world[3*i+2]);
	else world[3*i+2] /= 1000.;
      }
      delete [] stat;
      delete [] pixcrd;
      delete [] imgcrd;
      delete [] phi;
      delete [] theta;

      for(int z=0; z<zdim; z++){
	int pos =  z*xdim*ydim + pix;  // the voxel in the cube

	if(isObj[pos]){ // if it's an object pixel...
	  // delta-vel is half the distance between adjacent channels.
	  // if at end, then just use 0-1 or (zdim-1)-(zdim-2) distance
	  if(z==0){
	    if(zdim==1) deltaVel=1.; // pathological case -- if 2D image instead of cube.
	    else deltaVel = world[3*(z+1)+2] - world[ 3*z+2 ];
	  }
	  else if(z==(zdim-1)) deltaVel = world[3*(z-1)+2] - world[ 3*z+2 ];
	  deltaVel = (world[3*(z+1)+2] - world[ 3*(z-1)+2 ]) / 2.;
	  momentMap[pix] += this->array[pos] * fabsf(deltaVel);
	}
      }

      delete [] world;
    }
    
    float *temp = new float[xdim*ydim];
    int count=0;
    for(int i=0;i<xdim*ydim;i++) {
      bool addPixel = false;
      for(int z=0;z<zdim;z++) addPixel = addPixel || isObj[z*xdim*ydim+i];
      addPixel = addPixel && (momentMap[i]>0.);
      if(addPixel) temp[count++] = log10(momentMap[i]);
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

    float tr[6] = {boxXmin,1.,0.,boxYmin,0.,1.};
    cpggray(momentMap,xdim,ydim,1,xdim,1,ydim,z2,z1,tr);
    cpgbox("bcnst",0.,0,"bcnst",0.,0);
    cpgsch(1.5);
    cpgwedglog("rg",3.2,2,z2,z1,"Flux [Jy km/s]");

    delete [] momentMap;
    delete [] temp;
    delete [] isObj;

  }

  if(this->flagWCS) this->plotWCSaxes();
  
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

  float boxXmin = this->par.getXOffset() - 1;
  float boxYmin = this->par.getYOffset() - 1;

  char idents[3][80], opt[2], nlcprm[1];
  strcpy(idents[0], this->wcs->lngtyp);
  strcpy(idents[1], this->wcs->lattyp);
  strcpy(idents[2], "");
  if(strcmp(this->wcs->lngtyp,"RA")==0) opt[0] = 'G';
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
  // Draw the celestial grid letting PGSBOX choose the increments.
  // Set LABCTL=2100 to write 1st coord on top, and 2nd on right
  cpgslw(2);
  //Colour indices used by cpgsbox -- make text the background colour for thick line case
  ci[0] = 17; // grid lines, coord 1
  ci[1] = 17; // grid lines, coord 2
  ci[2] =  0; // numeric labels, coord 1
  ci[3] =  0; // numeric labels, coord 2
  ci[4] =  0; // axis annotation, coord 1
  ci[5] =  0; // axis annotation, coord 2
  ci[6] =  0; // title

  cpgsbox(blc, trc, idents, opt, 2100, 0, ci, gcode, 0.0, 0, grid1, 0, grid2,
	  0, pgwcsl_, 1, WCSLEN, 1, nlcprm, (int *)this->wcs, nldprm, 256, &ic,
	  cache, &ierr);
  
  //Colour indices used by cpgsbox -- make it all the same colour for thin line case.
  ci[0] = 17; // grid lines, coord 1
  ci[1] = 17; // grid lines, coord 2
  ci[2] = 17; // numeric labels, coord 1
  ci[3] = 17; // numeric labels, coord 2
  ci[4] = 17; // axis annotation, coord 1
  ci[5] = 17; // axis annotation, coord 2
  ci[6] = 17; // title
  cpgslw(1);

  cpgsbox(blc, trc, idents, opt, 2100, 0, ci, gcode, 0.0, 0, grid1, 0, grid2,
	  0, pgwcsl_, 1, WCSLEN, 1, nlcprm, (int *)this->wcs, nldprm, 256, &ic,
	  cache, &ierr);

  cpgsci(colour);
  cpgsch(size);
  cpgslw(lineWidth);
}


/*********************************************************/
/*********************************************************/







// void Cube::plotMomentMapOLD(string pgDestination)
// {
//   float boxXmin = this->par.getXOffset() - 1;
//   float boxYmin = this->par.getYOffset() - 1;

//   long xdim=this->axisDim[0];
//   long ydim=this->axisDim[1];

//   cpgopen(pgDestination.c_str());
//   cpgpap(7.5,(float)ydim/(float)xdim);
//   cpgsch(1.5);
//   cpgvstd();
//   cpgsch(1.);
//   cpgslw(2);
//   cpgswin(boxXmin+0.5,boxXmin+xdim+0.5,
// 	  boxYmin+0.5,boxYmin+ydim+0.5);
//   cpgbox("bcnst",0.,0,"bcnst",0.,0);
//   cpglab("X pixel","Y pixel","");
//   cpgmtxt("t",2.5,0.5,0.5,this->par.getImageFile().c_str());

//   if(this->objectList.size()>0){ // if there are no detections, there will be nothing to plot here

//     bool *isBlank = new bool[xdim*ydim];
//     for(int i=0;i<xdim*ydim;i++) 
//       isBlank[i] = this->par.isBlank(this->array[i]);

//     float *momentMap = new float[xdim*ydim];
//     bool *isObj = new bool[xdim*ydim];
//     for(int i=0;i<xdim*ydim;i++){
//       momentMap[i] = 0.;
//       isObj[i] = false;
//     }
//     for(int i=0;i<this->objectList.size();i++){
//       for(int p=0;p<this->objectList[i].getSize();p++){
// 	momentMap[ this->objectList[i].getX(p) + xdim*this->objectList[i].getY(p) ] += 
// 	  this->objectList[i].getF(p);
// 	isObj[ this->objectList[i].getX(p) + xdim*this->objectList[i].getY(p) ] = true;
//       }
//     }
//     for(int i=0;i<xdim*ydim;i++) if(isBlank[i]) momentMap[i] = this->par.getBlankPixVal();
 
//     float *temp = new float[xdim*ydim];
//     int count=0;
//     for(int i=0;i<xdim*ydim;i++) 
//       if(isObj[i] && !isBlank[i])  temp[count++] = momentMap[i];
//     float z1,z2;
//     zscale(count,temp,z1,z2);

//     float tr[6] = {boxXmin,1.,0.,boxYmin,0.,1.};
//     cpggray(momentMap,xdim,ydim,1,xdim,1,ydim,z2,z1,tr);
//     cpgbox("bcnst",0.,0,"bcnst",0.,0);
//     cpgsch(1.2);
//     cpgwedg("rg",3,2,z2,z1,"Flux");
//     cpgsch(1.0);
//     cpgsci(2);
//     count=0;
//     float xoffset=0.;
//     float yoffset=ydim/50.;
//     if(this->par.drawBorders()){
//       cpgsci(4);
//       for(int i=0;i<this->objectList.size();i++) this->objectList[i].drawBorders(0,0);
//       cpgsci(2);
//     }
//     std::stringstream label;
//     for(int i=0;i<this->objectList.size();i++){
//       cpgpt1(this->par.getXOffset()+this->objectList[i].getXcentre(), 
// 	     this->par.getYOffset()+this->objectList[i].getYcentre(),
// 	     5);
//       label.str("");
//       label << "#"<<setw(3)<<setfill('0')<<this->objectList[i].getID();
//       cpgptxt(this->par.getXOffset()+this->objectList[i].getXcentre()-xoffset, 
// 	      this->par.getYOffset()+this->objectList[i].getYcentre()-yoffset-1, 
// 	      0, 0.5, label.str().c_str());
//     }

//     delete [] momentMap;
//     delete [] temp;
//     delete [] isBlank;
//     delete [] isObj;

//   }

//   this->plotWCSaxes();
  
//   cpgclos();
  
// }

