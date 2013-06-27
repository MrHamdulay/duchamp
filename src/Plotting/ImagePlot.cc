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
#include <duchamp/Plotting/ImagePlot.hh>
#include <duchamp/Plotting/DuchampPlot.hh>
#include <string>
#include <cpgplot.h>

namespace duchamp
{
    namespace Plot {
	ImagePlot::ImagePlot():
	    DuchampPlot()
	{
	    this->paperWidth = 7.5; 
	    this->maxPaperHeight = 10.; 
	    this->marginWidth = 0.8; 
	    this->wedgeWidth = 0.7;
	}

	ImagePlot::ImagePlot(long x, long y):
	    DuchampPlot()
	{
	    this->paperWidth = 7.5; 
	    this->maxPaperHeight = 10.; 
	    this->marginWidth = 0.8; 
	    this->wedgeWidth = 0.7;
	    this->xdim = float(x);
	    this->ydim = float(y);
	    this->imageRatio= this->ydim / this->xdim; 
	    this->aspectRatio =  (this->imageRatio*this->imageWidth() + 2*this->marginWidth) / this->paperWidth;
	}

	ImagePlot::ImagePlot(const ImagePlot& other)
	{
	    this->operator=(other);
	}

	ImagePlot& ImagePlot::operator=(const ImagePlot& other)
	{
	    if(this==&other) return *this;
	    ((DuchampPlot &) *this) = other;
	    this->maxPaperHeight = other.maxPaperHeight; 
	    this->marginWidth = other.marginWidth;
	    this->wedgeWidth = other.wedgeWidth;
	    this->imageRatio = other.imageRatio;
	    this->xdim = other.xdim;
	    this->ydim = other.ydim;
	    return *this;
	}

	int ImagePlot::setUpPlot(std::string pgDestination)
	{
	    float correction;
	    if((this->imageRatio*this->imageWidth() + 2*this->marginWidth) > this->maxPaperHeight){
		correction = this->maxPaperHeight / (this->imageRatio*this->imageWidth()+2*this->marginWidth);
		this->paperWidth *= correction;
		this->marginWidth *= correction;
		this->wedgeWidth *= correction;
	    }
	    return this->open(pgDestination);
	}

	void  ImagePlot::drawMapBox(float x1, float x2, float y1, float y2, std::string xlabel, std::string ylabel)
	{
	    /// @details
	    ///  Defines the region that the box containing the map is to go in,
	    ///  and draws the box with limits given by the arguments. 
	    ///  Also writes labels on both X- and Y-axes.
	    /// \param x1 Minimum X-axis value.
	    /// \param x2 Maximum X-axis value.
	    /// \param y1 Minimum Y-axis value.
	    /// \param y2 Maximum Y-axis value.
	    /// \param xlabel The label to be put on the X-axis.
	    /// \param ylabel The label to be put on the Y-axis.

	    cpgvsiz(this->marginWidth, this->marginWidth + this->imageWidth(),
		    this->marginWidth, this->marginWidth + (this->imageWidth()*this->imageRatio) );
	    cpgslw(2);
	    cpgswin(x1,x2,y1,y2);
	    cpgbox("bcst",0.,0,"bcst",0.,0);
	    cpgslw(1);
	    cpgbox("bcnst",0.,0,"bcnst",0.,0);
	    cpglab(xlabel.c_str(), ylabel.c_str(), "");
	}

	void  ImagePlot::makeTitle(std::string title)
	{
	    /// @details
	    ///   Writes the title for the plot, making it centred for the entire 
	    ///    plot and not just the map.
	    ///  \param title String with title for plot.

	    cpgvstd();
	    cpgmtxt("t", Plot::imTitleOffset, 0.5, 0.5, title.c_str());
	}

	float ImagePlot::imageWidth()
	{
	    return this->paperWidth - 2*this->marginWidth - this->wedgeWidth;
	}

	float ImagePlot::cmToCoord(float cm){
	    /** \param cm Distance to be converted.*/
	    return (cm/Plot::inchToCm) * this->ydim / (this->imageWidth()*this->imageRatio);
	}

    }

}
