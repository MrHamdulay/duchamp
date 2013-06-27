// -----------------------------------------------------------------------
// SpectralPlot.hh: Definition of the class producing a page of spectral plots
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
#ifndef SPECTRALPLOT_H
#define SPECTRALPLOT_H

#include <string>
#include <duchamp/Plotting/MultipleDuchampPlot.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/Utils/Statistics.hh>

namespace duchamp
{

    namespace Plot
    {

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

	class SpectralPlot: public MultipleDuchampPlot
	{
	public:
	    SpectralPlot();    ///< Constructor
	    virtual ~SpectralPlot(){}; ///< Destructor
	    SpectralPlot(const SpectralPlot& other);
	    SpectralPlot& operator=(const SpectralPlot& other);

	    /// @brief Calculate boundaries for boxes.
	    void calcCoords();

	    /// @brief Set up the header and write the X label
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
	    void gotoMainSpectrum(float x1, float x2, float y1, float y2, std::string ylabel);
    
	    /// @brief Set up zoomed-in spectral plotting region.
	    void gotoZoomSpectrum(float x1, float x2, float y1, float y2);    
    
	    /// @brief Defines the region for the moment map.
	    void gotoMap();

	    /// @brief Draw lines indicating velocity range.
	    void drawVelRange(float v1, float v2); 

	    /// @brief Draw box showing excluded range due flagged channels.
	    void drawFlaggedChannelRange(float v1, float v2);
    
	    /// @brief Draw thresholds
	    void drawThresholds(Param &par, Statistics::StatsContainer<float> &stats);


	private:
	    float zoomCoords[4]; ///< Boundaries for the zoomed-in spectrum [inches]
    
	};

    }

}

#endif
