// -----------------------------------------------------------------------
// ImagePlot.hh: Definition of the class for plotting 2D images
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
#ifndef IMAGEPLOT_H
#define IMAGEPLOT_H

#include <string>
#include <duchamp/Plotting/DuchampPlot.hh>

namespace duchamp
{

    /// @brief A namespace to control plotting of the spectral output and the
    /// spatial image output.

    namespace Plot
    {
	// These are the constants used for spacing out elements in ImagePlot.
	const float imTitleOffset = 2.7;  ///< Offset for title of map.

	///  @brief A class for plotting 2-dimensional maps.
	///  @details A class to hold the dimensions and set up for the
	///  plotting of maps in Duchamp.
	///    The physical dimensions (in inches) of the plot and the elements 
	///     within it are stored, including maximum widths and heights 
	///     (so that the plot will fit on an A4 page).
	///    Simple accessor functions are provided to enable access to quantities
	///     needed for pgplot routines.
	class ImagePlot : public DuchampPlot
	{
	public:
	    ImagePlot();  ///< Constructor
	    ImagePlot(long x, long y);
	    virtual ~ImagePlot(){}; ///< Destructor
	    ImagePlot(const ImagePlot& other);
	    ImagePlot& operator=(const ImagePlot& other);

	    int setUpPlot(std::string pgDestination); 

	    /// @brief Defines and draws box.
	    void  drawMapBox(float x1, float x2, float y1, float y2, 
			     std::string xlabel, std::string ylabel);

	    /// @brief Write plot title.
	    void  makeTitle(std::string title);

	    float cmToCoord(float cm);///< Converts distance in cm to coordinate distance on the plot.
	    float getMargin(){return marginWidth;};        ///< Returns width of margin.
	    float imageWidth();       ///< Returns the calculated total width of the image part of the plot [inches]
	    float getImageHeight(){return imageWidth()*imageRatio;};   ///< Returns height of image [inches]

	protected:
	    float maxPaperHeight; ///< Maximum allowed height of paper [inches]
	    float marginWidth;    ///< Width allowed for margins around main plot (ie. label & numbers) [inches]
	    float wedgeWidth;     ///< Width allowed for placement of wedge on right-hand side of plot. [inches]
	    float imageRatio;     ///< Aspect ratio of the image only (ie. y-value range / x-value range).
	    float xdim;           ///< Width of main plot, in display units.
	    float ydim;           ///< Height of main plot, in display units.
	};


    }

}

#endif
