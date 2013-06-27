// -----------------------------------------------------------------------
// plots.hh: Definition of the SpectralPlot and ImagePlot classes, for
//           plotting spectra and maps of detected objects.
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
#ifndef PLOTS_H
#define PLOTS_H

#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/pgheader.hh>
#include <duchamp/Utils/Statistics.hh>

namespace duchamp
{

  /// @brief A function to handle plotting of WCS axes, with duchamp-suitable defaults.
  void plotWCSaxes(struct wcsprm *wcs, size_t *axes, int textColour=DUCHAMP_ID_TEXT_COLOUR, int axisColour=DUCHAMP_WCS_AXIS_COLOUR);

  /// @brief A namespace to control plotting of the spectral output and the
  /// spatial image output.

  namespace Plot
  {
    const float inchToCm=2.54;        ///< Conversion factor from inches to centimetres.
    const float a4width=21.0;         ///< A4 width in cm
    const float a4height=29.7;        ///< A4 height in cm
    const float psHoffset=0.35;       ///< The default horizontal pgplot offset applied to ps files
    const float psVoffset=0.25;       ///< The default vertical pgplot offset applied to ps files

    // These are the constants used for spacing out elements in SpectralPlot.
    const float spMainX1 =  2.0;      ///< min X-value of main box [cm]
    const float spMainX2 = 13.7;      ///< max X-value of main box [cm]
    const float spZoomX1 = 15.0;      ///< min X-value of zoom box [cm]
    const float spZoomX2 = 16.8;      ///< max X-value of zoom box [cm]
    const float spMapX1  = 17.0;      ///< min X-value of map box [cm]
    const float spMainY1 =  1.5;      ///< min Y-value of box wrt base of current spectrum [cm]
    const float spMainY2 =  3.4;      ///< max Y-value of box wrt base of current spectrum [cm]
    const float spXlabelOffset = 3.0; ///< Offset for X-axis label.
    const float spYlabelOffset = 4.0; ///< Offset for Y-axis label.
    const float spTitleOffset1 = 5.1; ///< Offset for first title line.
    const float spTitleOffset2 = 3.6; ///< Offset for second title line.
    const float spTitleOffset3 = 2.1; ///< Offset for third title line.
    const float spTitleOffset4 = 0.6; ///< Offset for fourth title line.

    const float spIndexSize = 0.6;    ///< PGPlot character height for tick mark labels
    const float spLabelSize = 0.7;    ///< PGPlot character height for axis labels
    const float spTitleSize = 0.8;    ///< PGPlot character height for plot title line

    // These are the constants used for spacing out elements in ImagePlot.
    const float imTitleOffset = 2.7;  ///< Offset for title of map.

    // These are the constants used for spacing out elements in CutoutPlot.
    const float cuMainX1 = 1.0;
    const float cuMainX2 = 15.8;
    const float cuMainY1 = 1.0;
    const float cuMainY2 = 3.8;
    const float cuMapX1 = 16.0;

    //***************************************************************************
    //***************************************************************************

    ///  @brief A class for plotting spectra of detections.
    ///  @details This class is designed to hold the dimensions and
    ///  set up for the plotting of the spectra (including the full
    ///  spectra, the zoomed in part, and the moment map).  The
    ///  physical dimensions (in inches) of the plot and the elements
    ///  within it are stored, on the assumption that the plot will go
    ///  on an A4 page.  Simple accessor functions are provided to
    ///  enable access to quantities needed for pgplot routines.

    class SpectralPlot
    {
    public:
      SpectralPlot();    ///< Constructor
      virtual ~SpectralPlot(); ///< Destructor
      SpectralPlot(const SpectralPlot& p);
      SpectralPlot& operator=(const SpectralPlot& p);

      /// @brief Set up PGPLOT output.
      int setUpPlot(std::string pgDestination); 

      /// @brief Close the PGPLOT device
      void close();

      /// @brief Calculate boundaries for boxes.
      void calcCoords();
      /// @brief Set up the header & write the X-label.
      void gotoHeader(std::string xlabel);

      /// @brief Write first line of header information (position/velocity info) in correct place.
      void firstHeaderLine(std::string line);   
    
      /// @brief Write second line of header information (fluxes) in correct place.
      void secondHeaderLine(std::string line);  

      /// @brief Write third line of header information (WCS widths) in correct place. 
      void thirdHeaderLine(std::string line);   

      /// @brief Write fourth line of header information (pixel coordinates) in correct place. 
      void fourthHeaderLine(std::string line);   

      /// @brief Set up main spectral plotting region.
      void gotoMainSpectrum(float x1, float x2, float y1, float y2,
			    std::string ylabel);
    
      /// @brief Set up zoomed-in spectral plotting region.
      void gotoZoomSpectrum(float x1, float x2, float y1, float y2);    
    
      /// @brief Defines the region for the moment map.
      void gotoMap();

      /// @brief Draw lines indicating velocity range.
      void drawVelRange(float v1, float v2); 

      // /// @brief Draw box showing excluded range due to Milky Way.
      // void drawMWRange(float v1, float v2);
      /// @brief Draw box showing excluded range due flagged channels.
      void drawFlaggedChannelRange(float v1, float v2);
    
      /// @brief Draw thresholds
      void drawThresholds(Param &par, Statistics::StatsContainer<float> &stats);

      /// @brief Return number of spectra that go on a page.
      int   getNumOnPage();

      /// @brief Set number of spectra that go on a page.
      void  setNumOnPage(int i);
      float getPaperWidth();         ///< Return width of plottable region.
      void  setPaperWidth(float f);  ///< Set width of plottable region.
      float getPaperHeight();        ///< Return height of plottable region.
      void  setPaperHeight(float f); ///< Set height of plottable region.
      float getAspectRatio();        ///< Return aspect ratio
      void  setAspectRatio(float f); ///< Set aspect ratio
      void  goToPlot();              ///< Goes to the plot when more than one are open.

    private:
      int numOnPage;       ///< Number of spectra to put on one page.
      int spectraCount;    ///< Number of spectra done so far: where on the page?
      float mainCoords[4]; ///< Boundaries for the main spectrum [inches]
      float zoomCoords[4]; ///< Boundaries for the zoomed-in spectrum [inches]
      float mapCoords[4];  ///< Boundaries for the map box [inches]
      float paperWidth;    ///< Width of plottable region of the paper [inches]
      float paperHeight;   ///< Height of plottable region of the paper [inches]
      float aspectRatio;   ///< Aspect ratio height/width (as used by PGPlot calls)
      int   identifier;    ///< The identifier code used by cpgslct.
    
    };

    inline int   SpectralPlot::getNumOnPage(){return numOnPage;}
    inline void  SpectralPlot::setNumOnPage(int i){numOnPage=i;}
    inline float SpectralPlot::getPaperWidth(){return paperWidth;}
    inline void  SpectralPlot::setPaperWidth(float f){paperWidth=f;}
    inline float SpectralPlot::getPaperHeight(){return paperHeight;}
    inline void  SpectralPlot::setPaperHeight(float f){paperHeight=f;}
    inline float SpectralPlot::getAspectRatio(){return aspectRatio;}
    inline void  SpectralPlot::setAspectRatio(float f){aspectRatio=f;}

    //***************************************************************************
    //***************************************************************************

    ///  @brief A class for plotting a single spectrum.
    ///  @details This class is designed to hold the dimensions and
    ///  set up for the plotting of the spectra (including the full
    ///  spectra, the zoomed in part, and the moment map).  The
    ///  physical dimensions (in inches) of the plot and the elements
    ///  within it are stored, on the assumption that the plot will go
    ///  on an A4 page.  Simple accessor functions are provided to
    ///  enable access to quantities needed for pgplot routines.

    class SimpleSpectralPlot
    {
    public:
      SimpleSpectralPlot();    ///< Constructor
      virtual ~SimpleSpectralPlot(); ///< Destructor
      SimpleSpectralPlot(const SimpleSpectralPlot& p);
      SimpleSpectralPlot& operator=(const SimpleSpectralPlot& p);

      /// @brief Set up PGPLOT output.
      int setUpPlot(std::string pgDestination); 

      /// @brief Close the PGPLOT device
      void close();

      /// @brief Write the labels
      void label(std::string xlabel, std::string ylabel, std::string title);

      /// @brief Set up main spectral plotting region.
      void gotoMainSpectrum(float x1, float x2, float y1, float y2);

      /// @brief Draw a line indicating the pixels that have been detected
      void markDetectedPixels(short *detectMap, size_t size, FitsHeader &head);

      /// @brief Draw lines indicating velocity range.
      void drawVelRange(float v1, float v2); 

      /// @brief Draw box showing excluded range due to Milky Way.
      void drawMWRange(float v1, float v2);
    
      float getPaperWidth();         ///< Return width of plottable region.
      void  setPaperWidth(float f);  ///< Set width of plottable region.
      float getPaperHeight();        ///< Return height of plottable region.
      void  setPaperHeight(float f); ///< Set height of plottable region.
      float getAspectRatio();        ///< Return aspect ratio
      void  setAspectRatio(float f); ///< Set aspect ratio
      void  goToPlot();              ///< Goes to the plot when more than one are open.

    private:
      float mainCoords[4]; ///< Boundaries for the main spectrum [inches]
      float paperWidth;    ///< Width of plottable region of the paper [inches]
      float paperHeight;   ///< Height of plottable region of the paper [inches]
      float aspectRatio;   ///< Aspect ratio height/width (as used by PGPlot calls)
      int   identifier;    ///< The identifier code used by cpgslct.
  
    };
  
    inline float SimpleSpectralPlot::getPaperWidth(){return paperWidth;}
    inline void  SimpleSpectralPlot::setPaperWidth(float f){paperWidth=f;}
    inline float SimpleSpectralPlot::getPaperHeight(){return paperHeight;}
    inline void  SimpleSpectralPlot::setPaperHeight(float f){paperHeight=f;}
    inline float SimpleSpectralPlot::getAspectRatio(){return aspectRatio;}
    inline void  SimpleSpectralPlot::setAspectRatio(float f){aspectRatio=f;}


    //***************************************************************************
    //***************************************************************************

    ///  @brief A class for plotting 2-dimensional maps.
    ///  @details A class to hold the dimensions and set up for the
    ///  plotting of maps in Duchamp.
    ///    The physical dimensions (in inches) of the plot and the elements 
    ///     within it are stored, including maximum widths and heights 
    ///     (so that the plot will fit on an A4 page).
    ///    Simple accessor functions are provided to enable access to quantities
    ///     needed for pgplot routines.

    class ImagePlot
    {
    public:
      ImagePlot();  ///< Constructor
      virtual ~ImagePlot(); ///< Destructor
      ImagePlot(const ImagePlot& p);
      ImagePlot& operator=(const ImagePlot& p);
  
      /// @brief Set up PGPLOT output.
      int   setUpPlot(std::string pgDestination, float x, float y); 

      /// @brief Close the PGPLOT device
      void close();

      /// @brief Defines and draws box.
      void  drawMapBox(float x1, float x2, float y1, float y2, 
		       std::string xlabel, std::string ylabel);

      /// @brief Write plot title.
      void  makeTitle(std::string title);

      float cmToCoord(float cm);///< Converts distance in cm to coordinate distance on the plot.
      float getMargin();        ///< Returns width of margin.
      float getPaperWidth();    ///< Returns width of paper.
      float imageWidth();       ///< Returns the calculated total width of the image part of the plot [inches]
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

    inline float ImagePlot::getMargin()     {return marginWidth;}
    inline float ImagePlot::getPaperWidth() {return paperWidth;}
    inline float ImagePlot::getImageHeight(){return imageWidth()*imageRatio;}
    inline float ImagePlot::getAspectRatio(){return aspectRatio;}
  
    //***************************************************************************
    //***************************************************************************

    ///  @brief A class for plotting just the image cutouts of individual sources.
    ///  @details This class is designed to hold the dimensions and set up for the
    ///  plotting of the cutouts, plus the textual information for each source.
    ///  The physical dimensions (in inches) of the plot and the elements 
    ///   within it are stored, on the assumption that the plot will go on 
    ///   an A4 page.
    ///  Simple accessor functions are provided to enable access to quantities
    ///   needed for pgplot routines.

    class CutoutPlot
    {
    public:
      CutoutPlot();    ///< Constructor
      virtual ~CutoutPlot(); ///< Destructor
      CutoutPlot(const CutoutPlot& p);
      CutoutPlot& operator=(const CutoutPlot& p);

      /// @brief Set up PGPLOT output.
      int setUpPlot(std::string pgDestination); 

      /// @brief Calculate boundaries for boxes.
      void calcCoords();
      /// @brief Set up the header box 
      void gotoHeader();

      /// @brief Write first line of header information (position/velocity info) in correct place.
      void firstHeaderLine(std::string line);   
    
      /// @brief Write second line of header information (fluxes) in correct place.
      void secondHeaderLine(std::string line);  

      /// @brief Write third line of header information (WCS widths) in correct place. 
      void thirdHeaderLine(std::string line);   

      /// @brief Write fourth line of header information (pixel coordinates) in correct place. 
      void fourthHeaderLine(std::string line);   

      /// @brief Defines the region for the moment map.
      void gotoMap();

      /// @brief Goes to the plot when more than one are open. 
      void  goToPlot();   

      /// @brief Return the number of objects that go on a page.
      int   getNumOnPage(){return numOnPage;}
      /// @brief Set number of objects that go on a page.
      void  setNumOnPage(int i){numOnPage=i;}

      /// @brief Return width of plottable region.
      float getPaperWidth(){return paperWidth;}
      /// @brief Set width of plottable region.
      void  setPaperWidth(float f){paperWidth=f;}
      /// @brief Return height of plottable region.
      float getPaperHeight(){return paperHeight;}
      /// @brief Set height of plottable region.
      void  setPaperHeight(float f){paperHeight=f;}

    private:
      int numOnPage;       ///< Number of sources to put on one page.
      int sourceCount;     ///< Number of sources done so far: where on the page?
      float mainCoords[4]; ///< Boundaries for the main spectrum [inches]
      float mapCoords[4];  ///< Boundaries for the map box [inches]
      float paperWidth;    ///< Width of plottable region of the paper [inches]
      float paperHeight;   ///< Height of plottable region of the paper [inches]
      int   identifier;    ///< The identifier code used by cpgslct.
    
    };


  }

}

#endif

