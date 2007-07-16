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
#include <param.hh>
#include <Utils/mycpgplot.hh>
#include <Cubes/plots.hh>

using std::stringstream;
using namespace mycpgplot;

namespace Plot
{

  //----------------------------------------------------------
  // SpectralPlot functions
  //----------------------------------------------------------

  SpectralPlot::SpectralPlot(){
    paperWidth=a4width/inchToCm - 2*psHoffset; 
    spectraCount=0;
    numOnPage = 5;
  };

  SpectralPlot::~SpectralPlot(){};
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
    paperHeight = paperWidth*M_SQRT2; 
    if(paperHeight+2*psVoffset > a4height){
      paperHeight = a4height - 2*psVoffset;
      paperWidth = paperHeight / M_SQRT2;
    }
    identifier = mycpgopen(pgDestination);
    if(identifier>0) cpgpap(paperWidth, paperHeight/paperWidth); 
    // make paper size to fit on A4.
    return identifier;
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
    int posOnPage = (numOnPage - (spectraCount%numOnPage))%numOnPage;
    mainCoords[0] = Plot::spMainX1/inchToCm;
    mainCoords[1] = Plot::spMainX2/inchToCm;
    zoomCoords[0] = Plot::spZoomX1/inchToCm;
    zoomCoords[1] = Plot::spZoomX2/inchToCm;
    mainCoords[2] = zoomCoords[2] = mapCoords[2] = 
      posOnPage*paperHeight/float(numOnPage) + Plot::spMainY1/inchToCm;
    mainCoords[3] = zoomCoords[3] = mapCoords[3] = 
      posOnPage*paperHeight/float(numOnPage) + Plot::spMainY2/inchToCm;
    mapCoords[0]  = Plot::spMapX1/inchToCm;
    mapCoords[1]  = mapCoords[0] + (mapCoords[3]-mapCoords[2]);
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
    calcCoords();
    cpgvsiz(0., paperWidth, mainCoords[2], mainCoords[3]);  
    cpgsch(spLabelSize);
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
    cpgvsiz(mainCoords[0],mainCoords[1],mainCoords[2],mainCoords[3]);
    cpgsch(spIndexSize);
    cpgswin(x1,x2,y1,y2);
    cpgbox("1bcnst",0.,0,"bcnst1v",0.,0);
    cpgsch(spLabelSize);
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
    cpgvsiz(zoomCoords[0],zoomCoords[1],zoomCoords[2],zoomCoords[3]);
    cpgsch(spIndexSize);
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
    cpgvsiz(mapCoords[0],mapCoords[1],mapCoords[2],mapCoords[3]);
    cpgsch(spIndexSize);
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
  //----------------------------------------------------------
  // ImagePlot functions
  //----------------------------------------------------------

  ImagePlot::ImagePlot(){
    paperWidth = 7.5; 
    maxPaperHeight = 10.; 
    marginWidth = 0.8; 
    wedgeWidth = 0.7;
  };

  ImagePlot::~ImagePlot(){};
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
    xdim = x;
    ydim = y;
    imageRatio= ydim / xdim; 
    aspectRatio =  (imageRatio*imageWidth() + 2*marginWidth) / paperWidth;
    float correction;
    if((imageRatio*imageWidth() + 2*marginWidth) > maxPaperHeight){
      correction = maxPaperHeight / (imageRatio*imageWidth()+2*marginWidth);
      paperWidth *= correction;
      marginWidth *= correction;
      wedgeWidth *= correction;
    }
    identifier = mycpgopen(pgDestination);
    if(identifier>0) cpgpap(paperWidth, aspectRatio);
    return identifier;
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
    cpgvsiz(marginWidth, marginWidth + imageWidth(),
	    marginWidth, marginWidth + (imageWidth()*imageRatio));
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


}
