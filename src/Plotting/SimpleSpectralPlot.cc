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
#include <duchamp/Plotting/SimpleSpectralPlot.hh>
#include <duchamp/Plotting/SpectralPlot.hh>
#include <duchamp/FitsHeader.hh>
#include <duchamp/PixelMap/Scan.hh>
#include <duchamp/PixelMap/Object2D.hh>
#include <string>

namespace duchamp {

    namespace Plot {

	SimpleSpectralPlot::SimpleSpectralPlot():
	    SpectralPlot()
	{
	    this->numOnPage = 1;
	}
	 
	SimpleSpectralPlot::SimpleSpectralPlot(const SimpleSpectralPlot& other)
	{
	    this->operator=(other);
	}

	SimpleSpectralPlot& SimpleSpectralPlot::operator=(const SimpleSpectralPlot& other)
	{
	    if(this == &other) return *this;
	    ((SpectralPlot &) *this) = other;
	    return *this;
	}

	int SimpleSpectralPlot::setUpPlot(std::string pgDestination)
	{
	    if(pgDestination == "/xs") this->paperWidth=12.;
	    this->paperHeight = this->paperWidth*this->aspectRatio; 
	    this->open(pgDestination);
	    float scaling = this->paperWidth*inchToCm / a4width;
	    this->mainCoords[0] = Plot::spMainX1/inchToCm * scaling;
	    this->mainCoords[1] = (Plot::spMapX1+Plot::spMainY2-Plot::spMainY1)/inchToCm * scaling;
	    this->mainCoords[2] = Plot::spMainY1/inchToCm * scaling;
	    this->mainCoords[3] = Plot::spMainY2/inchToCm * scaling;
	    return this->identifier;

	}

	void SimpleSpectralPlot::label(std::string xlabel, std::string ylabel, std::string title)
	{
	    /// @details 
	    /// Calls calcCoords, to calculate correct coordinates for this spectrum.
	    /// Defines the region for the header information, making it centred
	    ///  on the page.
	    /// Also writes the velocity (x axis) label, given by the string argument.
	    /// \param xlabel Label to go on the velocity/spectral axis.

	    cpgvsiz(this->mainCoords[0],this->mainCoords[1],this->mainCoords[2],this->mainCoords[3]);
	    cpgsch(2.);
	    cpgmtxt("B",3.,0.5,0.5,xlabel.c_str());
	    cpgmtxt("L",4.,0.5,0.5,ylabel.c_str());
	    cpgmtxt("T",2.,0.5,0.5,title.c_str());
	}

	void SimpleSpectralPlot::gotoMainSpectrum(float x1, float x2, float y1, float y2)
	{
	    /// @details
	    ///  Defines the region for the main spectrum.
	    ///  Draws the box, with tick marks, and 
	    ///   writes the flux (y axis) label, given by the string argument.
	    /// \param x1 Minimum X-coordinate of box.
	    /// \param x2 Maximum X-coordinate of box.
	    /// \param y1 Minimum Y-coordinate of box.
	    /// \param y2 Maximum Y-coordinate of box.
	    /// \param ylabel Label for the flux (Y) axis.

	    cpgvsiz(this->mainCoords[0],this->mainCoords[1],this->mainCoords[2],this->mainCoords[3]);
	    cpgsch(2.);
	    cpgswin(x1,x2,y1,y2);
	    cpgbox("1bcnst",0.,0,"bcnst1v",0.,0);
	}

	void SimpleSpectralPlot::markDetectedPixels(short *detectMap, size_t size, FitsHeader &head)
	{
	    PixelInfo::Object2D detectionLine;
	    for(size_t z=0;z<size;z++) 
		if(detectMap[z]>0) detectionLine.addPixel(z,0);
	    std::vector<PixelInfo::Scan> detlist = detectionLine.getScanlist();	    
	    // size_t dim[2]; dim[0]=size; dim[1]=1;
	    // Image detIm(dim);
	    // detIm.setMinSize(1);
	    // for(size_t z=0;z<size;z++) detIm.setPixValue(z,float(detectMap[z]));
	    // detIm.stats().setThreshold(0.5);
	    // std::vector<PixelInfo::Scan> detlist=detIm.findSources1D();
	    for(std::vector<PixelInfo::Scan>::iterator sc=detlist.begin();sc<detlist.end();sc++){
		float v1,v2;
		double zero=0.;
		if(head.isWCS()){
		    double zpt=double(sc->getX()-0.5);
		    v1=head.pixToVel(zero,zero,zpt);
		    zpt=double(sc->getXmax()+0.5);
		    v2=head.pixToVel(zero,zero,zpt);
		}
		else{
		    v1=float(sc->getX()-0.5);
		    v2=float(sc->getXmax()+0.5);
		}
		float x1,x2,y1,y2; 
		cpgqwin(&x1,&x2,&y1,&y2);
		float ymax=y2-0.04*(y2-y1);
		float ymin=y2-0.06*(y2-y1);
		int lw,fs;
		cpgqlw(&lw);
		cpgqfs(&fs);
		cpgslw(3);
		cpgsfs(1);
		cpgrect(v1,v2,ymin,ymax);
		cpgslw(lw);
		cpgsfs(fs);
	    }
	}

    }

}
