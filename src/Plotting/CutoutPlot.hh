// -----------------------------------------------------------------------
// CutoutPlot.hh: Definition of the class producing a page of cutout plots
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
#ifndef CUTOUTPLOT_H
#define CUTOUTPLOT_H

#include <string>
#include <duchamp/Plotting/MultipleDuchampPlot.hh>

namespace duchamp
{

    /// @brief A namespace to control plotting of the spectral output and the
    /// spatial image output.

    namespace Plot
    {

	// These are the constants used for spacing out elements in CutoutPlot.
	const float cuMainX1 = 1.0;
	const float cuMainX2 = 15.8;
	const float cuMainY1 = 1.0;
	const float cuMainY2 = 3.8;
	const float cuMapX1 = 16.0;

	class CutoutPlot: public MultipleDuchampPlot
	{
	public:
	    CutoutPlot();    ///< Constructor
	    virtual ~CutoutPlot(){}; ///< Destructor
	    CutoutPlot(const CutoutPlot& other);
	    CutoutPlot& operator=(const CutoutPlot& other);

	    /// @brief Calculate boundaries for boxes.
	    void calcCoords();
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
	    
	};

    }

}

#endif
