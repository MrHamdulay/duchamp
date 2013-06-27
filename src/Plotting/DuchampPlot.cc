// -----------------------------------------------------------------------
// DuchampPlot.cc: Definition of the base plotting class
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
#include <duchamp/Plotting/DuchampPlot.hh>
#include <duchamp/Utils/mycpgplot.hh>
#include <string>
#include <math.h>

namespace duchamp {

    namespace Plot {
	
	DuchampPlot::DuchampPlot()
	{
	    this->paperWidth=a4width/inchToCm - 2*psHoffset; 
	    this->aspectRatio = M_SQRT2;
	}

	DuchampPlot::DuchampPlot(const DuchampPlot& other)
	{
	    this->operator=(other);
	}

	DuchampPlot& DuchampPlot::operator= (const DuchampPlot& other)
	{
	    if(this == &other) return *this;
	    this->paperWidth  = other.paperWidth;  
	    this->paperHeight = other.paperHeight; 
	    this->aspectRatio = other.aspectRatio; 
	    this->identifier  = other.identifier;  
	    return *this;
	}

	int DuchampPlot::open(std::string pgDestination)
	{
	    this->identifier = mycpgplot::mycpgopen(pgDestination);
	    if(this->identifier>0) cpgpap(this->paperWidth, this->aspectRatio); 
	    return this->identifier;
	} 

	void DuchampPlot::close()
	{
	    cpgclos();
	}

	void  DuchampPlot::goToPlot()
	{
	    cpgslct(this->identifier);
	}             
	

    }

}
