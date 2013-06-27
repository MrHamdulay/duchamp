// -----------------------------------------------------------------------
// CutoutPlot.cc: Definition of the class producing a page of cutout plots
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
#include <duchamp/Plotting/CutoutPlot.hh>
#include <duchamp/Plotting/MultipleDuchampPlot.hh>
#include <duchamp/Plotting/DuchampPlot.hh>
#include <string>

namespace duchamp {

    namespace Plot {

	CutoutPlot::CutoutPlot():
	    MultipleDuchampPlot()
	{
	    this->numOnPage = 5;
	}

	CutoutPlot::CutoutPlot(const CutoutPlot& other)
	{
	    this->operator=(other);
	}

	CutoutPlot& CutoutPlot::operator=(const CutoutPlot& other)
	{
	    if(this==&other) return *this;
	    ((MultipleDuchampPlot &) *this) = other;
	    return *this;
	}
	
	void CutoutPlot::calcCoords()
	{
	    int posOnPage = (this->numOnPage - (this->spectraCount%this->numOnPage)) % this->numOnPage;
	    this->mainCoords[0] = Plot::cuMainX1/inchToCm;
	    this->mainCoords[1] = Plot::cuMainX2/inchToCm;
	    this->mainCoords[2] = this->mapCoords[2] = 
		posOnPage*paperHeight/float(this->numOnPage) + Plot::cuMainY1/inchToCm;
	    this->mainCoords[3] = this->mapCoords[3] = 
		posOnPage*paperHeight/float(this->numOnPage) + Plot::cuMainY2/inchToCm;
	    this->mapCoords[0]  = Plot::cuMapX1/inchToCm;
	    this->mapCoords[1]  = this->mapCoords[0] + (this->mapCoords[3]-this->mapCoords[2]);
	}

	/// @brief Write first line of header information (position/velocity info) in correct place.
	void CutoutPlot::firstHeaderLine(std::string line)
	{
	    cpgsch(Plot::plotTitleSize); 
	    cpgptxt(0.5,0.8,0.,0.5,line.c_str());
	}
    
	void  CutoutPlot::secondHeaderLine(std::string line)
	{
	    cpgsch(Plot::plotLabelSize); 
	    cpgptxt(0.5,0.6,0.,0.5,line.c_str());
	}
	void  CutoutPlot::thirdHeaderLine(std::string line)
	{
	    cpgsch(Plot::plotLabelSize);
	    cpgptxt(0.5,0.4,0.,0.5,line.c_str());
	}
	void  CutoutPlot::fourthHeaderLine(std::string line)
	{
	    cpgsch(Plot::plotLabelSize); 
	    cpgptxt(0.5,0.2,0.,0.5,line.c_str());
	}

	void CutoutPlot::gotoMap(){
	    cpgvsiz(this->mapCoords[0],this->mapCoords[1],
		    this->mapCoords[2],this->mapCoords[3]);
	    cpgsch(Plot::plotIndexSize);
	}
	    

    }

}
