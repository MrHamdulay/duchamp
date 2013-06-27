// -----------------------------------------------------------------------
// MultipleDuchampPlot.cc: Definition of the base class for plots with
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
#include <duchamp/Plotting/MultipleDuchampPlot.hh>
#include <duchamp/Plotting/DuchampPlot.hh>

namespace duchamp {

    namespace Plot {

	MultipleDuchampPlot::MultipleDuchampPlot():
	    DuchampPlot()
	{
	    this->spectraCount=0;
	}
	
	MultipleDuchampPlot::MultipleDuchampPlot(const MultipleDuchampPlot& other)
	{
	    this->operator=(other);
	}

	MultipleDuchampPlot& MultipleDuchampPlot::operator= (const MultipleDuchampPlot& other)
	{
	    if(this==&other) return *this;
	    ((DuchampPlot &) *this) = other;
	    this->numOnPage = other.numOnPage;
	    this->spectraCount = other.spectraCount;  
	    for(int i=0;i<4;i++) this->mainCoords[i] = other.mainCoords[i]; 
	    for(int i=0;i<4;i++) this->mapCoords[i] = other.mapCoords[i]; 
	    return *this;
	}

	int MultipleDuchampPlot::setUpPlot(std::string pgDestination)
	{
	    this->paperHeight = this->paperWidth*this->aspectRatio; 
	    if(this->paperHeight+2*Plot::psVoffset > Plot::a4height){
		this->paperHeight = Plot::a4height - 2*Plot::psVoffset;
		this->paperWidth = this->paperHeight / this->aspectRatio;
	    }
	    return this->open(pgDestination);
	}

	void MultipleDuchampPlot::gotoHeader()
	{
	    if(spectraCount%numOnPage==0) cpgpage();
	    spectraCount++;
	    this->calcCoords();
	    cpgvsiz(0., this->paperWidth, this->mainCoords[2], this->mainCoords[3]);  
	}


    }

}
