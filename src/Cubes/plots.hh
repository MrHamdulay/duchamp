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
  const float inchToCm=2.54;        ///< Conversion factor from inches to centimetres.
  const float a4width=21.0;         ///< A4 width in cm
  const float a4height=29.7;        ///< A4 height in cm
  const float psHoffset=0.35;       ///< The default pgplot offset applied to ps files
  const float psVoffset=0.25;       ///< The default pgplot offset applied to ps files

  // These are the constants used for spacing out elements in SpectralPlot.
  const float spMainX1 =  2.0;      ///< min X-value of main box [cm]
  const float spMainX2 = 13.7;      ///< max X-value of main box [cm]
  const float spZoomX1 = 15.0;      ///< min X-value of zoom box [cm]
  const float spZoomX2 = 16.8;      ///< max X-value of zoom box [cm]
  const float spMapX1  = 17.0;      ///< min X-value of map box [cm]
  const float spMainY1 =  1.8;      ///< min Y-value of box wrt base of current spectrum [cm]
  const float spMainY2 =  3.8;      ///< max Y-value of box wrt base of current spectrum [cm]
  const float spXlabelOffset = 3.0; ///< Offset for X-axis label.
  const float spYlabelOffset = 4.0; ///< Offset for Y-axis label.
  const float spTitleOffset1 = 3.8; ///< Offset for first title line.
  const float spTitleOffset2 = 2.3; ///< Offset for second title line.
  const float spTitleOffset3 = 0.8; ///< Offset for third title line.

  // These are the constants used for spacing out elements in ImagePlot.
  const float imTitleOffset = 2.7;  ///< Offset for title of map.

  //***************************************************************************

  /** 
   *  A class for plotting spectra of detections.
   *  This class is designed to hold the dimensions and set up for the plotting 
   *   of the spectra (including the full spectra, the zoomed in part, and the 
   *   moment map).
   *  The physical dimensions (in inches) of the plot and the elements 
   *   within it are stored, on the assumption that the plot will go on 
   *   an A4 page.
   *  Simple accessor functions are provided to enable access to quantities
   *   needed for pgplot routines.
   */
  class SpectralPlot
  {
  public:
    SpectralPlot();    ///< Constructor
    virtual ~SpectralPlot(); ///< Destructor

    int setUpPlot(string pgDestination);   ///< Set up PGPLOT output.
    void calcCoords();                     ///< Calculate boundaries for boxes.
    void gotoHeader(string xlabel);        ///< Set up the header & write the X-label.
    void firstHeaderLine(string line);   
    ///< Write first line of header information (position/velocity info) in correct place.
    void secondHeaderLine(string line);  
    ///< Write second line of header information (widths and fluxes) in correct place.
    void thirdHeaderLine(string line);   
    ///< Write third line of header information (pixel coordinates) in correct place.
    void gotoMainSpectrum(float x1, float x2, float y1, float y2,string ylabel);
    ///< Set up main spectral plotting region.
    void gotoZoomSpectrum(float x1, float x2, float y1, float y2);    
    ///< Set up zoomed-in spectral plotting region.
    void gotoMap();                        ///< Defines the region for the moment map.
    void drawVelRange(float v1, float v2); ///< Draw lines indicating velocity range.
    void drawMWRange(float v1, float v2);///< Draw box showing excluded range due to Milky Way.
    
    int   getNumOnPage();          ///< Return number of spectra that go on a page.
    void  setNumOnPage(int i);     ///< Set number of spectra that go on a page.
    float getPaperWidth();         ///< Return width of plottable region.
    void  setPaperWidth(float f);  ///< Set width of plottable region.
    float getPaperHeight();        ///< Return height of plottable region.
    void  setPaperHeight(float f); ///< Set height of plottable region.
    void  goToPlot()      ;        ///< Goes to the plot when more than one are open.

  private:
    int numOnPage;       ///< Number of spectra to put on one page.
    int spectraCount;    ///< Number of spectra done so far: where on the page?
    float mainCoords[4]; ///< Boundaries for the main spectrum [inches]
    float zoomCoords[4]; ///< Boundaries for the zoomed-in spectrum [inches]
    float mapCoords[4];  ///< Boundaries for the map box [inches]
    float paperWidth;    ///< Width of plottable region of the paper [inches]
    float paperHeight;   ///< Height of plottable region of the paper [inches]
    float indexSize;     ///< PGPlot character height for tick mark labels
    float labelSize;     ///< PGPlot character height for axis labels.
    int   identifier;    ///< The identifier code used by cpgslct.
    
  };

  //----------------------------------------------------------
  // Inline SpectralPlot functions...
  //----------------------------------------------------------
  inline void  SpectralPlot::firstHeaderLine(string line){
    cpgmtxt("t",Plot::spTitleOffset1,0.5,0.5,line.c_str());}
  inline void  SpectralPlot::secondHeaderLine(string line){
    cpgmtxt("t",Plot::spTitleOffset2,0.5,0.5,line.c_str());}
  inline void  SpectralPlot::thirdHeaderLine(string line){
    cpgmtxt("t",Plot::spTitleOffset3,0.5,0.5,line.c_str());}
  inline int   SpectralPlot::getNumOnPage(){return numOnPage;}
  inline void  SpectralPlot::setNumOnPage(int i){numOnPage=i;}
  inline float SpectralPlot::getPaperWidth(){return paperWidth;}
  inline void  SpectralPlot::setPaperWidth(float f){paperWidth=f;}
  inline float SpectralPlot::getPaperHeight(){return paperHeight;}
  inline void  SpectralPlot::setPaperHeight(float f){paperHeight=f;}
  inline void  SpectralPlot::goToPlot(){cpgslct(identifier);}

  //***************************************************************************
  //***************************************************************************

  /** 
   *  A class for plotting 2-dimensional maps.
   *    A class to hold the dimensions and set up for the plots used by the 
   *     two functions below.
   *    The physical dimensions (in inches) of the plot and the elements 
   *     within it are stored, including maximum widths and heights 
   *     (so that the plot will fit on an A4 page).
   *    Simple accessor functions are provided to enable access to quantities
   *     needed for pgplot routines.
   */
  class ImagePlot
  {
  public:
    ImagePlot();  ///< Constructor
    virtual ~ImagePlot(); ///< Destructor
  
    int   setUpPlot(string pgDestination, float x, float y); ///< Set up PGPLOT output.
    void  drawMapBox(float x1, float x2, float y1, float y2, 
		     string xlabel, string ylabel);          ///< Defines and draws box.
    void  makeTitle(string title);                           ///< Write plot title.

    float cmToCoord(float cm);///< Converts distance in cm to coordinate distance on the plot.
    float getMargin();        ///< Returns width of margin.
    float getPaperWidth();    ///< Returns width of paper.
    float imageWidth();       ///<  Returns the calculated total width of the image part of the plot [inches]   
    float getImageHeight();   ///< Returns height of image [inches]
    float getAspectRatio();   ///< Returns the aspect ratio of the image.
    void  goToPlot();         ///< Goes to the plot when more than one are open

  private:
    float paperWidth;     ///< Default (maximum) width of "paper" [inches]
    float maxPaperHeight; ///< Maximum allowed height of paper [inches]
    float marginWidth;    ///< Width allowed for margins around main plot (ie. label & numbers) [inches]
    float wedgeWidth;     ///< Width allowed for placement of wedge on right-hand side of plot. [inches]
    float imageRatio;     ///< Aspect ratio of the image only (ie. y-value range / x-value range).
    float aspectRatio;    ///< Aspect ratio of whole plot.
    float xdim;           ///< Width of main plot, in display units.
    float ydim;           ///< Height of main plot, in display units.
    int   identifier;     ///< The identifier code used by cpgslct.
  };
  //----------------------------------------------------------
  // Inline ImagePlot functions...
  //----------------------------------------------------------

  inline float ImagePlot::imageWidth(){ return paperWidth - 2*marginWidth - wedgeWidth;}
  inline float ImagePlot::cmToCoord(float cm){/** \param cm Distance to be converted.*/
    return (cm/inchToCm) * ydim / (imageWidth()*imageRatio);}
  inline float ImagePlot::getMargin()     {return marginWidth;}
  inline float ImagePlot::getPaperWidth() {return paperWidth;}
  inline float ImagePlot::getImageHeight(){return imageWidth()*imageRatio;}
  inline float ImagePlot::getAspectRatio(){return aspectRatio;}
  inline void  ImagePlot::goToPlot(){cpgslct(identifier);}
  
}


#endif

