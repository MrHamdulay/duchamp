#ifndef PLOTS_H
#define PLOTS_H

#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
#include <cpgplot.h>
#include <Utils/mycpgplot.hh>

using std::string;
using std::stringstream;
using namespace mycpgplot;

namespace Plot
{
  const float inchToCm=2.54; // Conversion factor from inches to centimetres.
  const float a4width=21.0;  // A4 width in cm
  const float a4height=29.7; // A4 height in cm
  const float psHoffset=0.35; // The default pgplot offset applied to ps files
  const float psVoffset=0.25; // The default pgplot offset applied to ps files


  //***************************************************************************

  class SpectralPlot
  {
    /** 
     *  SpectralPlot class
     *    A class to hold the dimensions and set up for the plotting of the 
     *     spectra (including the full spectra, the zoomed in part, and the 
     *     moment map).
     *    The physical dimensions (in inches) of the plot and the elements 
     *     within it are stored, on the assumption that the plot will go on 
     *     an A4 page.
     *    Simple accessor functions are provided to enable access to quantities
     *     needed for pgplot routines.
     */
  public:
    SpectralPlot(){
      paperWidth=a4width/inchToCm - 2*psHoffset; 
      spectraCount=0;
      numOnPage = 5;
      indexSize = 0.6;
      labelSize = 0.7;
    };
    ~SpectralPlot(){};

    int setUpPlot(string pgDestination){
      /** SpectralPlot::setUpPlot
       *    Opens the designated pgplot device.
       *    Scales the paper so that it fits on an A4 sheet (using known 
       *     values of the default pgplot offsets).
       *    Returns the value returned by cpgopen -- 
       *                      if <= 0, then an error has occurred.
       */
      paperHeight = paperWidth*M_SQRT2; 
      if(paperHeight+2*psVoffset > a4height){
	paperHeight = a4height - 2*psVoffset;
	paperWidth = paperHeight / M_SQRT2;
      }
      int flag = cpgopen(pgDestination.c_str());
      if(flag>0) cpgpap(paperWidth, paperHeight/paperWidth); 
                 // make paper size to fit on A4.
      return flag;
    }

    void calcCoords(){
      /** SpectralPlot::calcCoords()
       *    Calculates the boundaries for the various boxes, in inches measured
       *     from the lower left corner.
       *    Based on the fact that there are numOnPage spectra shown on each 
       *     page, going down the page in increasing number 
       *           -- given by spectraCount.
       */
      int posOnPage = (numOnPage - (spectraCount%numOnPage))%numOnPage;
      mainCoords[0] = 2.0/inchToCm;
      mainCoords[1] = 13.7/inchToCm;
      zoomCoords[0] = 15.0/inchToCm;
      zoomCoords[1] = 16.8/inchToCm;
      mainCoords[2] = zoomCoords[2] = mapCoords[2] = 
	posOnPage*paperHeight/float(numOnPage) + 1.8/inchToCm;
      mainCoords[3] = zoomCoords[3] = mapCoords[3] = 
	posOnPage*paperHeight/float(numOnPage) + 3.8/inchToCm;
      mapCoords[0]  = 17.0/inchToCm;
      mapCoords[1]  = mapCoords[0] + (mapCoords[3]-mapCoords[2]);
   }

    void gotoHeader(string xlabel){
      /** SpectralPlot::gotoHeader(string)
       *   Calls calcCoords, to calculate correct coordinates for this 
       *    spectrum.
       *   Defines the region for the header information, making it centred
       *    on the page.
       *   Also writes the velocity (x axis) label, given by the string 
       *    argument.
       */
      if(spectraCount%numOnPage==0) cpgpage();
      spectraCount++;
      calcCoords();
      cpgvsiz(0., paperWidth, mainCoords[2], mainCoords[3]);  
      cpgsch(labelSize);
      cpgmtxt("b",3.,0.5,0.5,xlabel.c_str());
    }

    /**
     * Header line functions
     *  Functions to write the header information above the boxes.
     *  One for each line (position/velocity, widths & fluxes, pixel coords).
     */
    void firstHeaderLine(string line){cpgmtxt("t",3.8,0.5,0.5,line.c_str());};
    void secondHeaderLine(string line){cpgmtxt("t",2.3,0.5,0.5,line.c_str());};
    void thirdHeaderLine(string line){cpgmtxt("t",0.8,0.5,0.5,line.c_str());};

    void gotoMainSpectrum(float x1, float x2, float y1, float y2, 
			  string ylabel){
      /** SpectralPlot::gotoMainSpectrum()
       *   Defines the region for the main spectrum.
       *   Draws the box, with tick marks, and 
       *    writes the flux (y axis) label, given by the string argument.
       */
      cpgvsiz(mainCoords[0],mainCoords[1],mainCoords[2],mainCoords[3]);
      cpgsch(indexSize);
      cpgswin(x1,x2,y1,y2);
      cpgbox("1bcnst",0.,0,"bcnst1v",0.,0);
      cpgsch(labelSize);
      cpgmtxt("l",4.,0.5,0.5,ylabel.c_str());
    }

    void gotoZoomSpectrum(float x1, float x2, float y1, float y2){
      /** SpectralPlot::gotoZoomSpectrum()
       *   Defines the region for the zoomed-in part of the spectrum.
       *   Draws the box, with special tick marks on the bottom axis.
       */
      cpgvsiz(zoomCoords[0],zoomCoords[1],zoomCoords[2],zoomCoords[3]);
      cpgsch(indexSize);
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
    
    void gotoMap(){
      /** SpectralPlot::gotoMap()
       *   Defines the region for the moment map.
       */
      cpgvsiz(mapCoords[0],mapCoords[1],mapCoords[2],mapCoords[3]);
      cpgsch(indexSize);
    }

    void drawVelRange(float v1, float v2){
      /** SpectralPlot::drawVelRange(float v1, float v2)
       *   Draws two vertical lines at the limits of velocity 
       *    given by the arguments.
       */
      int ci,ls;
      float dud,min,max;
      cpgqwin(&dud,&dud,&min,&max);
      cpgqci(&ci);
      cpgqls(&ls);
      cpgsci(BLUE);
      cpgsls(DASHED);
      cpgmove(v1,min);  cpgdraw(v1,max);
      cpgmove(v2,min);  cpgdraw(v2,max);
      cpgsci(ci);
      cpgsls(ls);
    }
    
    void drawMWRange(float v1, float v2){
      /** SpectralPlot::drawMWRange(float v1, float v2)
       *   Draws a box showing the extent of channels masked by the 
       *     Milky Way parameters
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
      cpgsci(DARKGREEN);
      cpgsfs(HATCHED);
      cpgrect(v1,v2,min,max);
      cpgsfs(OUTLINE);
      cpgrect(v1,v2,min,max);
      cpgsci(ci);
      cpgsfs(fs);
    }
    
    int   getNumOnPage(){return numOnPage;};
    void  setNumOnPage(int i){numOnPage=i;};
    float getPaperWidth(){return paperWidth;};
    void  setPaperWidth(float f){paperWidth=f;};
    float getPaperHeight(){return paperHeight;};
    void  setPaperHeight(float f){paperHeight=f;};

  private:
    int numOnPage;       // Number of spectra to put on one page.
    int spectraCount;    // Number of spectra done so far: where on the page?
    float mainCoords[4]; // Boundaries for the main spectrum [inches]
    float zoomCoords[4]; // Boundaries for the zoomed-in spectrum [inches]
    float mapCoords[4];  // Boundaries for the map box [inches]
    float paperWidth;    // Width of plottable region of the paper [inches]
    float paperHeight;   // Height of plottable region of the paper [inches]
    float indexSize;     // PGPlot character height for tick mark labels
    float labelSize;     // PGPlot character height for axis labels.
    
  };

  //***************************************************************************

  class ImagePlot
  {
    /** 
     *  ImagePlot class
     *    A class to hold the dimensions and set up for the plots used by the 
     *     two functions below.
     *    The physical dimensions (in inches) of the plot and the elements 
     *     within it are stored, including maximum widths and heights 
     *     (so that the plot will fit on an A4 page).
     *    Simple accessor functions are provided to enable access to quantities
     *     needed for pgplot routines.
     */
  public:
    ImagePlot(){
      paperWidth = 7.5; maxPaperHeight = 10.; 
      marginWidth = 0.8; wedgeWidth = 0.7;
    };
    ~ImagePlot(){};
  
    int  setUpPlot(string pgDestination, float x, float y){
      /**
       * setUpPlot(string pgDestination, float x, float y)
       *  Opens a pgplot device and scales it to the correct shape.
       *  In doing so, the dimensions for the image are set, and the required 
       *   aspect ratios of the image and of the plot are calculated.
       *  If the resulting image is going to be tall enough to exceed the 
       *   maximum height (given the default width), then scale everything 
       *   down by enough to make the height equal to maxPaperHeight.
       *  Returns the value returned by cpgopen -- 
       *             if <= 0, then an error has occurred.
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
      int flag = cpgopen(pgDestination.c_str());
      if(flag>0) cpgpap(paperWidth, aspectRatio);
      return flag;
    }

    void  drawMapBox(float x1, float x2, float y1, float y2, 
		     string xlabel, string ylabel){
      /**
       * drawMapBox() 
       *  Defines the region that the box containing the map is to go in,
       *  and draws the box with limits given by the arguments.
       *  The labels for the x and y axes are also given as arguments.
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
    void  makeTitle(string title){
      /**
       *  makeTitle(string)
       *    Writes the title for the plot, making it centred for the entire 
       *     plot and not just the map.
       */ 
      cpgvstd();
      cpgmtxt("t", 2.7, 0.5, 0.5, title.c_str());
    }

    float imageWidth(){
      /**
       * imageWidth()
       *   Returns the calculated total width of the image part of the 
       *    plot [inches]
       */
      return paperWidth - 2*marginWidth - wedgeWidth;
    };       

    float cmToCoord(float cm){return (cm/inchToCm) * ydim / 
				(imageWidth()*imageRatio);};
    float getMargin()     {return marginWidth;};
    float getPaperWidth() {return paperWidth;};
    float getImageHeight(){return imageWidth()*imageRatio;};
    float getAspectRatio(){return aspectRatio;};


  private:
    float paperWidth;     // Default (maximum) width of "paper" [inches]
    float maxPaperHeight; // Maximum allowed height of paper [inches]
    float marginWidth;    // Width allowed for margins around main plot 
                          //   (ie. label & numbers) [inches]
    float wedgeWidth;     // Width allowed for placement of wedge on 
                          //   right-hand side of plot. [inches]
    float imageRatio;     // Aspect ratio of the image only 
                          //   (ie. y-value range / x-value range).
    float aspectRatio;    // Aspect ratio of whole plot.
    float xdim;           // Width of main plot, in display units.
    float ydim;           // Height of main plot, in display units.
  };

}

#endif

