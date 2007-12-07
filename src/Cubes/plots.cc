// -----------------------------------------------------------------------
// plots.cc: Functions defining SpectralPlot and ImagePlot classes.
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
#include <sstream>
#include <string>
#include <math.h>
#include <cpgplot.h>
#include <duchamp/param.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/Utils/mycpgplot.hh>
#include <duchamp/Cubes/plots.hh>

using std::stringstream;
using namespace mycpgplot;

namespace duchamp
{

  namespace Plot
  {

    //----------------------------------------------------------
    // SpectralPlot functions
    //----------------------------------------------------------

    SpectralPlot::SpectralPlot(){
      this->paperWidth=a4width/inchToCm - 2*psHoffset; 
      this->spectraCount=0;
      this->numOnPage = 5;
    }

    SpectralPlot::~SpectralPlot(){}

    SpectralPlot::SpectralPlot(const SpectralPlot& p)
    {
      operator=(p);
    }

    SpectralPlot& SpectralPlot::operator=(const SpectralPlot& p)
    {
      if(this==&p) return *this;
      this->numOnPage = p.numOnPage;
      this->spectraCount = p.spectraCount;  
      for(int i=0;i<4;i++) this->mainCoords[i] = p.mainCoords[i]; 
      for(int i=0;i<4;i++) this->zoomCoords[i] = p.zoomCoords[i]; 
      for(int i=0;i<4;i++) this->mapCoords[i] = p.mapCoords[i]; 
      this->paperWidth = p.paperWidth;    
      this->paperHeight = p.paperHeight;   
      this->identifier = p.identifier;    
      return *this;
    }

    //----------------------------------------------------------
    int SpectralPlot::setUpPlot(std::string pgDestination){
      /** 
       * Opens the designated pgplot device.  Scales the paper so that
       * it fits on an A4 sheet (using known values of the default
       * pgplot offsets).  
       *
       * \param pgDestination The std::string indicating the PGPLOT device to
       * be written to.
       *
       * \return The value returned by mycpgopen. If <= 0, then an error
       * has occurred.
       */
      this->paperHeight = this->paperWidth*M_SQRT2; 
      if(this->paperHeight+2*Plot::psVoffset > Plot::a4height){
	this->paperHeight = Plot::a4height - 2*Plot::psVoffset;
	this->paperWidth = this->paperHeight / M_SQRT2;
      }
      this->identifier = mycpgopen(pgDestination);
      if(this->identifier>0) cpgpap(this->paperWidth, this->paperHeight/this->paperWidth); 
      // make paper size to fit on A4.
      return this->identifier;
    }
    //----------------------------------------------------------
    void SpectralPlot::calcCoords(){
      /**
       * Calculates the boundaries for the various boxes, in inches measured
       *  from the lower left corner.
       * Based on the fact that there are numOnPage spectra shown on each 
       *  page, going down the page in increasing number (given by 
       *  SpectralPlot::spectraCount).
       */
      int posOnPage = (this->numOnPage - 
		       (this->spectraCount%this->numOnPage))
	%this->numOnPage;
      this->mainCoords[0] = Plot::spMainX1/inchToCm;
      this->mainCoords[1] = Plot::spMainX2/inchToCm;
      this->zoomCoords[0] = Plot::spZoomX1/inchToCm;
      this->zoomCoords[1] = Plot::spZoomX2/inchToCm;
      this->mainCoords[2] = this->zoomCoords[2] = this->mapCoords[2] = 
	posOnPage*paperHeight/float(this->numOnPage) + Plot::spMainY1/inchToCm;
      this->mainCoords[3] = this->zoomCoords[3] = this->mapCoords[3] = 
	posOnPage*paperHeight/float(this->numOnPage) + Plot::spMainY2/inchToCm;
      this->mapCoords[0]  = Plot::spMapX1/inchToCm;
      this->mapCoords[1]  = this->mapCoords[0] + (this->mapCoords[3]-this->mapCoords[2]);
    }
    //----------------------------------------------------------
    void SpectralPlot::gotoHeader(std::string xlabel){
      /** 
       * Calls calcCoords, to calculate correct coordinates for this spectrum.
       * Defines the region for the header information, making it centred
       *  on the page.
       * Also writes the velocity (x axis) label, given by the string argument.
       * \param xlabel Label to go on the velocity/spectral axis.
       */
      if(spectraCount%numOnPage==0) cpgpage();
      spectraCount++;
      this->calcCoords();
      cpgvsiz(0., this->paperWidth, this->mainCoords[2], this->mainCoords[3]);  
      cpgsch(Plot::spLabelSize);
      cpgmtxt("b",Plot::spXlabelOffset,0.5,0.5,xlabel.c_str());
    }
    //----------------------------------------------------------
    void SpectralPlot::gotoMainSpectrum(float x1, float x2, float y1, float y2, std::string ylabel){
      /**
       *  Defines the region for the main spectrum.
       *  Draws the box, with tick marks, and 
       *   writes the flux (y axis) label, given by the string argument.
       * \param x1 Minimum X-coordinate of box.
       * \param x2 Maximum X-coordinate of box.
       * \param y1 Minimum Y-coordinate of box.
       * \param y2 Maximum Y-coordinate of box.
       * \param ylabel Label for the flux (Y) axis.
       */
      cpgvsiz(this->mainCoords[0],this->mainCoords[1],
	      this->mainCoords[2],this->mainCoords[3]);
      cpgsch(Plot::spIndexSize);
      cpgswin(x1,x2,y1,y2);
      cpgbox("1bcnst",0.,0,"bcnst1v",0.,0);
      cpgsch(Plot::spLabelSize);
      cpgmtxt("l",Plot::spYlabelOffset,0.5,0.5,ylabel.c_str());
    }
    //----------------------------------------------------------
    void SpectralPlot::gotoZoomSpectrum(float x1, float x2, float y1, float y2){
      /**
       *   Defines the region for the zoomed-in part of the spectrum.
       *   Draws the box, with special tick marks on the bottom axis.
       * \param x1 Minimum X-coordinate of box.
       * \param x2 Maximum X-coordinate of box.
       * \param y1 Minimum Y-coordinate of box.
       * \param y2 Maximum Y-coordinate of box.
       */
      cpgvsiz(this->zoomCoords[0],this->zoomCoords[1],
	      this->zoomCoords[2],this->zoomCoords[3]);
      cpgsch(Plot::spIndexSize);
      cpgswin(x1,x2,y1,y2);
      cpgbox("bc",0.,0,"bcstn1v",0.,0);
      float lengthL,lengthR,disp,tickpt,step;
      stringstream label;
      for(int i=1;i<10;i++){
	tickpt = x1+(x2-x1)*float(i)/10.;  // spectral coord of the tick
	switch(i)
	  {
	  case 2:
	  case 8:
	    lengthL = lengthR = 0.5;
	    disp = 0.3 + float(i-2)/6.; // i==2 --> disp=0.3, i==8 --> disp=1.3
	    label.str("");
	    label << tickpt;
	    // do a labelled tick mark
	    cpgtick(x1,y1,x2,y1,float(i)/10.,lengthL,lengthR,
		    disp, 0., label.str().c_str());
	    break;
	  default:
	    label.str("");
	    lengthL = 0.25;
	    lengthR = 0.;
	    disp = 0.;  // not used in this case, but set it anyway.
	    break;
	  }
	// first the bottom axis, just the ticks
	if(fabs(tickpt)<(x2-x1)/1.e4) step = 2.*(x2-x1);
	else step = tickpt;
	cpgaxis("",
		tickpt-0.001*(x2-x1), y1,
		tickpt+0.001*(x2-x1), y1,
		tickpt-0.001*(x2-x1), tickpt+0.001*(x2-x1),
		step, -1, lengthL,lengthR, 0.5, disp, 0.);
	//and now the top -- no labels, just tick marks
	cpgtick(x1,y2,x2,y2,float(i)/10.,lengthL,lengthR,0.,0.,"");
      }
    }
    //----------------------------------------------------------
    void SpectralPlot::gotoMap(){
      cpgvsiz(this->mapCoords[0],this->mapCoords[1],
	      this->mapCoords[2],this->mapCoords[3]);
      cpgsch(Plot::spIndexSize);
    }
    //----------------------------------------------------------
    void SpectralPlot::drawVelRange(float v1, float v2){
      /**
       * Draws two vertical lines at the limits of velocity 
       *  given by the arguments.
       * \param v1 Minimum velocity. 
       * \param v2 Maximum velocity.
       */
      int ci,ls;
      float dud,min,max;
      cpgqwin(&dud,&dud,&min,&max);
      cpgqci(&ci);
      cpgqls(&ls);
      cpgsci(DUCHAMP_OBJECT_OUTLINE_COLOUR);
      cpgsls(DASHED);
      cpgmove(v1,min);  cpgdraw(v1,max);
      cpgmove(v2,min);  cpgdraw(v2,max);
      cpgsci(ci);
      cpgsls(ls);
    }
    //----------------------------------------------------------
    void SpectralPlot::drawMWRange(float v1, float v2){
      /** 
       * Draws a box showing the extent of channels masked by the 
       *  Milky Way parameters
       * \param v1 Minimum velocity of the Milky Way range.
       * \param v2 Maximum velocity of the Milky Way range.
       */
      int ci,fs;
      float dud,min,max,height;
      cpgqwin(&dud,&dud,&min,&max);
      height = max-min;
      max += 0.01*height;
      min -= 0.01*height;
      cpgqci(&ci);
      cpgqfs(&fs);
      setDarkGreen();
      cpgsci(DUCHAMP_MILKY_WAY_COLOUR);
      cpgsfs(HATCHED);
      cpgrect(v1,v2,min,max);
      cpgsfs(OUTLINE);
      cpgrect(v1,v2,min,max);
      cpgsci(ci);
      cpgsfs(fs);
    }
    //----------------------------------------------------------
    // SpectralPlot functions...
    //----------------------------------------------------------
    void  SpectralPlot::firstHeaderLine(std::string line)
    {
      cpgsch(Plot::spTitleSize); 
      cpgmtxt("t",Plot::spTitleOffset1*Plot::spLabelSize/Plot::spTitleSize,
	      0.5,0.5,line.c_str());
    }
    void  SpectralPlot::secondHeaderLine(std::string line)
    {
      cpgsch(Plot::spLabelSize); 
      cpgmtxt("t",Plot::spTitleOffset2,0.5,0.5,line.c_str());
    }
    void  SpectralPlot::thirdHeaderLine(std::string line)
    {
      cpgsch(Plot::spLabelSize);
      cpgmtxt("t",Plot::spTitleOffset3,0.5,0.5,line.c_str());
    }
    void  SpectralPlot::fourthHeaderLine(std::string line)
    {
      cpgsch(Plot::spLabelSize); 
      cpgmtxt("t",Plot::spTitleOffset4,0.5,0.5,line.c_str());
    }
    void  SpectralPlot::goToPlot(){cpgslct(this->identifier);}

    //----------------------------------------------------------
    //----------------------------------------------------------
    // ImagePlot functions
    //----------------------------------------------------------

    ImagePlot::ImagePlot(){
      this->paperWidth = 7.5; 
      this->maxPaperHeight = 10.; 
      this->marginWidth = 0.8; 
      this->wedgeWidth = 0.7;
    };

    ImagePlot::~ImagePlot(){};

    ImagePlot::ImagePlot(const ImagePlot& p)
    {
      operator=(p);
    }

    ImagePlot& ImagePlot::operator=(const ImagePlot& p)
    {
      if(this==&p) return *this;
      this->paperWidth = p.paperWidth;    
      this->maxPaperHeight = p.maxPaperHeight;   
      this->marginWidth = p.marginWidth;    
      this->wedgeWidth = p.wedgeWidth; 
      this->imageRatio = p.imageRatio;
      this->aspectRatio = p.aspectRatio;
      this->xdim = p.xdim;
      this->ydim = p.ydim;
      this->identifier = p.identifier;    
      return *this;
    }

    //----------------------------------------------------------

    int ImagePlot::setUpPlot(std::string pgDestination, float x, float y){
      /**
       *  Opens a pgplot device and scales it to the correct shape.
       *  In doing so, the dimensions for the image are set, and the required 
       *   aspect ratios of the image and of the plot are calculated.
       *  If the resulting image is going to be tall enough to exceed the 
       *   maximum height (given the default width), then scale everything 
       *   down by enough to make the height equal to maxPaperHeight.
       * \param pgDestination  The string indicating the PGPLOT device to be 
       *   written to.
       * \param x The length of the X-axis.
       * \param y The length of the Y-axis.
       * \return The value returned by mycpgopen: if <= 0, then an error 
       *  has occurred.
       */
      this->xdim = x;
      this->ydim = y;
      this->imageRatio= this->ydim / this->xdim; 
      this->aspectRatio =  (this->imageRatio*this->imageWidth() + 2*this->marginWidth) 
	/ this->paperWidth;
      float correction;
      if((this->imageRatio*this->imageWidth() + 2*this->marginWidth) > 
	 this->maxPaperHeight){
	correction = this->maxPaperHeight / 
	  (this->imageRatio*this->imageWidth()+2*this->marginWidth);
	this->paperWidth *= correction;
	this->marginWidth *= correction;
	this->wedgeWidth *= correction;
      }
      this->identifier = mycpgopen(pgDestination);
      if(this->identifier>0) cpgpap(this->paperWidth, this->aspectRatio);
      return this->identifier;
    }
    //----------------------------------------------------------
    void ImagePlot::drawMapBox(float x1, float x2, float y1, float y2, 
			       std::string xlabel, std::string ylabel){
      /**
       *  Defines the region that the box containing the map is to go in,
       *  and draws the box with limits given by the arguments. 
       *  Also writes labels on both X- and Y-axes.
       * \param x1 Minimum X-axis value.
       * \param x2 Maximum X-axis value.
       * \param y1 Minimum Y-axis value.
       * \param y2 Maximum Y-axis value.
       * \param xlabel The label to be put on the X-axis.
       * \param ylabel The label to be put on the Y-axis.
       */
      cpgvsiz(this->marginWidth, this->marginWidth + this->imageWidth(),
	      this->marginWidth, this->marginWidth + (this->imageWidth()*this->imageRatio));
      cpgslw(2);
      cpgswin(x1,x2,y1,y2);
      cpgbox("bcst",0.,0,"bcst",0.,0);
      cpgslw(1);
      cpgbox("bcnst",0.,0,"bcnst",0.,0);
      cpglab(xlabel.c_str(), ylabel.c_str(), "");
    }
    //----------------------------------------------------------
    void ImagePlot::makeTitle(std::string title){
      /**
       *    Writes the title for the plot, making it centred for the entire 
       *     plot and not just the map.
       *   \param title String with title for plot.
       */ 
      cpgvstd();
      cpgmtxt("t", Plot::imTitleOffset, 0.5, 0.5, title.c_str());
    }

    void  ImagePlot::goToPlot(){cpgslct(this->identifier);}

    float ImagePlot::imageWidth(){ 
      return this->paperWidth - 2*this->marginWidth - this->wedgeWidth;
    }

    float ImagePlot::cmToCoord(float cm){
      /** \param cm Distance to be converted.*/
      return (cm/Plot::inchToCm) * this->ydim / (this->imageWidth()*this->imageRatio);
    }


    //----------------------------------------------------------
    //----------------------------------------------------------
    // CutoutPlot functions
    //----------------------------------------------------------
    //----------------------------------------------------------

    CutoutPlot::CutoutPlot(){
      this->paperWidth=a4width/inchToCm - 2*psHoffset; 
      this->sourceCount=0;
      this->numOnPage = 7;
    }

    CutoutPlot::~CutoutPlot(){}

    CutoutPlot::CutoutPlot(const CutoutPlot& p)
    {
      operator=(p);
    }

    CutoutPlot& CutoutPlot::operator=(const CutoutPlot& p)
    {
      if(this==&p) return *this;
      this->numOnPage = p.numOnPage;
      this->sourceCount = p.sourceCount;  
      for(int i=0;i<4;i++) this->mainCoords[i] = p.mainCoords[i]; 
      for(int i=0;i<4;i++) this->mapCoords[i] = p.mapCoords[i]; 
      this->paperWidth = p.paperWidth;    
      this->paperHeight = p.paperHeight;   
      this->identifier = p.identifier;    
      return *this;
    }

    //----------------------------------------------------------
    int CutoutPlot::setUpPlot(std::string pgDestination){
      /** 
       * Opens the designated pgplot device.  Scales the paper so that
       * it fits on an A4 sheet (using known values of the default
       * pgplot offsets).  
       *
       * \param pgDestination The std::string indicating the PGPLOT device to
       * be written to.
       *
       * \return The value returned by mycpgopen. If <= 0, then an error
       * has occurred.
       */
      this->paperHeight = this->paperWidth*M_SQRT2; 
      if(this->paperHeight+2*Plot::psVoffset > Plot::a4height){
	this->paperHeight = Plot::a4height - 2*Plot::psVoffset;
	this->paperWidth = this->paperHeight / M_SQRT2;
      }
      this->identifier = mycpgopen(pgDestination);
      if(this->identifier>0) cpgpap(this->paperWidth, this->paperHeight/this->paperWidth); 
      // make paper size to fit on A4.
      return this->identifier;
    }
    //----------------------------------------------------------
    void CutoutPlot::calcCoords(){
      /**
       * Calculates the boundaries for the various boxes, in inches measured
       *  from the lower left corner.
       * Based on the fact that there are numOnPage spectra shown on each 
       *  page, going down the page in increasing number (given by 
       *  CutoutPlot::spectraCount).
       */
      int posOnPage = (this->numOnPage - 
		       (this->sourceCount%this->numOnPage))
	%this->numOnPage;
      this->mainCoords[0] = Plot::cuMainX1/inchToCm;
      this->mainCoords[1] = Plot::cuMainX2/inchToCm;
      this->mainCoords[2] = this->mapCoords[2] = 
	posOnPage*paperHeight/float(this->numOnPage) + Plot::cuMainY1/inchToCm;
      this->mainCoords[3] = this->mapCoords[3] = 
	posOnPage*paperHeight/float(this->numOnPage) + Plot::cuMainY2/inchToCm;
      this->mapCoords[0]  = Plot::cuMapX1/inchToCm;
      this->mapCoords[1]  = this->mapCoords[0] + (this->mapCoords[3]-this->mapCoords[2]);
    }
    //----------------------------------------------------------
    void CutoutPlot::gotoHeader(){
      /** 
       * Calls calcCoords, to calculate correct coordinates for this spectrum.
       * Defines the region for the header information, making it centred
       *  on the page.
       * Also writes the velocity (x axis) label, given by the string argument.
       * \param xlabel Label to go on the velocity/spectral axis.
       */
      if(sourceCount%numOnPage==0) cpgpage();
      sourceCount++;
      this->calcCoords();
      //     cpgvsiz(0., this->paperWidth, this->mainCoords[2], this->mainCoords[3]);  
      cpgvsiz(this->mainCoords[0], this->mainCoords[1], 
	      this->mainCoords[2], this->mainCoords[3]);  
      cpgsch(Plot::spLabelSize);
      //   cpgmtxt("b",Plot::spXlabelOffset,0.5,0.5,xlabel.c_str());
      cpgswin(0.,1.,0.,1.);
      //    cpgbox("bc",0,0,"bc",0,0);
    }
    //----------------------------------------------------------

    void CutoutPlot::gotoMap(){
      cpgvsiz(this->mapCoords[0],this->mapCoords[1],
	      this->mapCoords[2],this->mapCoords[3]);
      cpgsch(Plot::spIndexSize);
    }
    //----------------------------------------------------------
    // CutoutPlot functions...
    //----------------------------------------------------------
    void  CutoutPlot::firstHeaderLine(std::string line)
    {
      cpgsch(Plot::spTitleSize); 
      //     cpgmtxt("t",Plot::spTitleOffset1*Plot::spLabelSize/Plot::spTitleSize,
      // 	    0.5,0.5,line.c_str());
      cpgptxt(0.5,0.8,0.,0.5,line.c_str());
    }
    void  CutoutPlot::secondHeaderLine(std::string line)
    {
      cpgsch(Plot::spLabelSize); 
      //    cpgmtxt("t",Plot::spTitleOffset2,0.5,0.5,line.c_str());
      cpgptxt(0.5,0.6,0.,0.5,line.c_str());
    }
    void  CutoutPlot::thirdHeaderLine(std::string line)
    {
      cpgsch(Plot::spLabelSize);
      //    cpgmtxt("t",Plot::spTitleOffset3,0.5,0.5,line.c_str());
      cpgptxt(0.5,0.4,0.,0.5,line.c_str());
    }
    void  CutoutPlot::fourthHeaderLine(std::string line)
    {
      cpgsch(Plot::spLabelSize); 
      //    cpgmtxt("t",Plot::spTitleOffset4,0.5,0.5,line.c_str());
      cpgptxt(0.5,0.2,0.,0.5,line.c_str());
    }
    void  CutoutPlot::goToPlot(){cpgslct(this->identifier);}

  }

}
