// -----------------------------------------------------------------------
// SimpleSpectralPlot.hh: Definition of the class producing a single spectral plot
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
#ifndef SIMPLE_SPECTRALPLOT_H
#define SIMPLE_SPECTRALPLOT_H

#include <string>
#include <duchamp/Plotting/SpectralPlot.hh>
#include <duchamp/fitsHeader.hh>

namespace duchamp
{

    namespace Plot
    {


	class SimpleSpectralPlot : public SpectralPlot
	{
	public:
	    SimpleSpectralPlot();    ///< Constructor
	    virtual ~SimpleSpectralPlot(){}; ///< Destructor
	    SimpleSpectralPlot(const SimpleSpectralPlot& p);
	    SimpleSpectralPlot& operator=(const SimpleSpectralPlot& p);

	    /// @brief Set up PGPLOT output.
	    int setUpPlot(std::string pgDestination); 

	    /// @brief Write the labels
	    void label(std::string xlabel, std::string ylabel, std::string title);

	    /// @brief Set up main spectral plotting region.
	    void gotoMainSpectrum(float x1, float x2, float y1, float y2);

	    /// @brief Draw a line indicating the pixels that have been detected
	    void markDetectedPixels(short *detectMap, size_t size, FitsHeader &head);

	};

    }

}

#endif
