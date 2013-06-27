// -----------------------------------------------------------------------
// DuchampPlot.hh: Definition of the base plotting class
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
#ifndef DUCHAMPPLOT_H
#define DUCHAMPPLOT_H

#include <string>

namespace duchamp
{

    /// @brief A namespace to control plotting of the spectral output and the
    /// spatial image output.

    namespace Plot
    {
	const float inchToCm=2.54;        ///< Conversion factor from inches to centimetres.
	const float a4width=21.0;         ///< A4 width in cm
	const float a4height=29.7;        ///< A4 height in cm
	const float psHoffset=0.35;       ///< The default horizontal pgplot offset applied to ps files
	const float psVoffset=0.25;       ///< The default vertical pgplot offset applied to ps files

	class DuchampPlot
	{
	public:
	    DuchampPlot();
	    DuchampPlot(const DuchampPlot& other);
	    DuchampPlot& operator= (const DuchampPlot& other);
	    virtual ~DuchampPlot(){};

	    /// @brief Set up PGPLOT output.
	    virtual int setUpPlot(std::string pgDestination)=0; 

	    /// @brief Open the PGPLOT device
	    int open(std::string pgDestination);

	    /// @brief Close the PGPLOT device
	    void close();

	    float getPaperWidth(){return paperWidth;};    ///< Return width of plottable region.
	    void  setPaperWidth(float f){paperWidth=f;};  ///< Set width of plottable region.
	    float getPaperHeight(){return paperHeight;};  ///< Return height of plottable region.
	    void  setPaperHeight(float f){paperHeight=f;};///< Set height of plottable region.
	    float getAspectRatio(){return aspectRatio;};  ///< Return aspect ratio
	    void  setAspectRatio(float f){aspectRatio=f;};///< Set aspect ratio

	    void  goToPlot();              ///< Goes to the plot when more than one are open.

	protected:
	    float paperWidth;    ///< Width of plottable region of the paper [inches]
	    float paperHeight;   ///< Height of plottable region of the paper [inches]
	    float aspectRatio;   ///< Aspect ratio height/width (as used by PGPlot calls)
	    int   identifier;    ///< The identifier code used by cpgslct.



	};

    }
}

#endif

