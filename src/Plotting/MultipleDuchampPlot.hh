// -----------------------------------------------------------------------
// MultipleDuchampPlot.hh: Definition of the base class for plots with
//                         multiple instances per page
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
#ifndef MULTIPLE_DUCHAMPPLOT_H
#define MULTIPLE_DUCHAMPPLOT_H

#include <string>
#include <duchamp/duchamp.hh>
#include <duchamp/Plotting/DuchampPlot.hh>

namespace duchamp
{

    /// @brief A namespace to control plotting of the spectral output and the
    /// spatial image output.

    namespace Plot
    {

	// constants for labels
	const float plotIndexSize = 0.6;    ///< PGPlot character height for tick mark labels
	const float plotLabelSize = 0.7;    ///< PGPlot character height for axis labels
	const float plotTitleSize = 0.8;    ///< PGPlot character height for plot title line

	class MultipleDuchampPlot : public DuchampPlot
	{
	public:
	    MultipleDuchampPlot();
	    MultipleDuchampPlot(const MultipleDuchampPlot& other);
	    MultipleDuchampPlot& operator= (const MultipleDuchampPlot& other);
	    virtual ~MultipleDuchampPlot(){};

	    /// @brief Calculate boundaries for boxes.
	    virtual void calcCoords()=0;
	    /// @brief Set up the header 
	    virtual void gotoHeader();

	    /// @brief Set up PGPLOT output.
	    virtual int setUpPlot(std::string pgDestination); 

	    /// @brief Return number of spectra that go on a page.
	    int   getNumOnPage(){return numOnPage;};

	    /// @brief Set number of spectra that go on a page.
	    void  setNumOnPage(int i){numOnPage = i;};

	    /// @brief Write first line of header information (position/velocity info) in correct place.
	    virtual void firstHeaderLine(std::string line)=0;   
    
	    /// @brief Write second line of header information (fluxes) in correct place.
	    virtual void secondHeaderLine(std::string line)=0;  

	    /// @brief Write third line of header information (WCS widths) in correct place. 
	    virtual void thirdHeaderLine(std::string line)=0;   

	    /// @brief Write fourth line of header information (pixel coordinates) in correct place. 
	    virtual void fourthHeaderLine(std::string line)=0;   

 


	protected:
	    int numOnPage;       ///< Number of spectra to put on one page.
	    int spectraCount;    ///< Number of spectra done so far: where on the page?
	    float mainCoords[4]; ///< Boundaries for the main spectrum [inches]
	    float mapCoords[4];  ///< Boundaries for the map box [inches]

	};

    }

}

#endif
