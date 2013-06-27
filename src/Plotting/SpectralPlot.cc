// -----------------------------------------------------------------------
// SpectralPlot.cc: Definition of the class producing a page of spectral plots
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
#include <duchamp/Plotting/SpectralPlot.hh>
#include <duchamp/Plotting/MultipleDuchampPlot.hh>
#include <duchamp/Plotting/DuchampPlot.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/Utils/Statistics.hh>
#include <duchamp/Utils/mycpgplot.hh>
#include <string>
#include <iostream>
#include <sstream>

using namespace mycpgplot;

namespace duchamp {

    namespace Plot {

	SpectralPlot::SpectralPlot():
	    MultipleDuchampPlot()
	{
	    this->numOnPage = 5;
	}
	
	SpectralPlot::SpectralPlot(const SpectralPlot& other)
	{
	    this->operator=(other);
	}
	
	SpectralPlot& SpectralPlot::operator=(const SpectralPlot& other)
	{
	    if(this == &other) return *this;
	    ((MultipleDuchampPlot &) *this) = other;
	    for(int i=0;i<4;i++) this->zoomCoords[i] = other.zoomCoords[i]; 
	    return *this;
	}

	void SpectralPlot::calcCoords()
	{
	    /// @details
	    /// Calculates the boundaries for the various boxes, in inches measured
	    ///  from the lower left corner.
	    /// Based on the fact that there are numOnPage spectra shown on each 
	    ///  page, going down the page in increasing number (given by 
	    ///  SpectralPlot::spectraCount).

	    int posOnPage = (this->numOnPage - (this->spectraCount%this->numOnPage)) % this->numOnPage;
	    this->mainCoords[0] = Plot::spMainX1/inchToCm;
	    this->mainCoords[1] = Plot::spMainX2/inchToCm;
	    this->zoomCoords[0] = Plot::spZoomX1/inchToCm;
	    this->zoomCoords[1] = Plot::spZoomX2/inchToCm;
	    this->mainCoords[2] = this->zoomCoords[2] = this->mapCoords[2] = 
		posOnPage*paperHeight/float(this->numOnPage) + Plot::spMainY1/inchToCm;
	    this->mainCoords[3] = this->zoomCoords[3] = this->mapCoords[3] = 
		posOnPage*paperHeight/float(this->numOnPage) + Plot::spMainY2/inchToCm;
	    this->mapCoords[0]  = Plot::spMapX1/inchToCm;
	    this->mapCoords[1]  = this->mapCoords[0] + (this->mapCoords[3]-this->mapCoords[2]);
	}

	void SpectralPlot::gotoHeader(std::string xlabel)
	{
	    this->MultipleDuchampPlot::gotoHeader();
	    cpgsch(Plot::plotLabelSize);
	    cpgmtxt("b",Plot::spXlabelOffset,0.5,0.5,xlabel.c_str());
	}

	void  SpectralPlot::firstHeaderLine(std::string line)
	{
	    cpgsch(Plot::plotTitleSize); 
	    cpgmtxt("t",Plot::spTitleOffset1*Plot::plotLabelSize/Plot::plotTitleSize,0.5,0.5,line.c_str());
	}
	void  SpectralPlot::secondHeaderLine(std::string line)
	{
	    cpgsch(Plot::plotLabelSize); 
	    cpgmtxt("t",Plot::spTitleOffset2,0.5,0.5,line.c_str());
	}
	void  SpectralPlot::thirdHeaderLine(std::string line)
	{
	    cpgsch(Plot::plotLabelSize);
	    cpgmtxt("t",Plot::spTitleOffset3,0.5,0.5,line.c_str());
	}
	void  SpectralPlot::fourthHeaderLine(std::string line)
	{
	    cpgsch(Plot::plotLabelSize); 
	    cpgmtxt("t",Plot::spTitleOffset4,0.5,0.5,line.c_str());
	}

	void SpectralPlot::gotoMainSpectrum(float x1, float x2, float y1, float y2, std::string ylabel)
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

	    cpgvsiz(this->mainCoords[0],this->mainCoords[1],
		    this->mainCoords[2],this->mainCoords[3]);
	    cpgsch(Plot::plotIndexSize);
	    cpgswin(x1,x2,y1,y2);
	    cpgbox("1bcnst",0.,0,"bcnst1v",0.,0);
	    cpgsch(Plot::plotLabelSize);
	    cpgmtxt("l",Plot::spYlabelOffset,0.5,0.5,ylabel.c_str());
	}
    
	void SpectralPlot::gotoZoomSpectrum(float x1, float x2, float y1, float y2)
	{
	    /// @details
	    ///   Defines the region for the zoomed-in part of the spectrum.
	    ///   Draws the box, with special tick marks on the bottom axis.
	    /// \param x1 Minimum X-coordinate of box.
	    /// \param x2 Maximum X-coordinate of box.
	    /// \param y1 Minimum Y-coordinate of box.
	    /// \param y2 Maximum Y-coordinate of box.

	    cpgvsiz(this->zoomCoords[0],this->zoomCoords[1],
		    this->zoomCoords[2],this->zoomCoords[3]);
	    cpgsch(Plot::plotIndexSize);
	    cpgswin(x1,x2,y1,y2);
	    cpgbox("bc",0.,0,"bcstn1v",0.,0);
	    float lengthL,lengthR,disp,tickpt,step;
	    std::stringstream label;
	    for(int i=1;i<10;i++){
		tickpt = x1+(x2-x1)*float(i)/10.;  // spectral coord of the tick
		switch(i)
		{
		case 2:
		case 8:
		    lengthL = lengthR = 0.5;
		    disp = 0.3 + float(i-2)/6.; // i==2 --> disp=0.3, i==8 --> disp=1.3
		    label.str("");
		    label << tickpt;
		    // do a labelled tick mark
		    cpgtick(x1,y1,x2,y1,float(i)/10.,lengthL,lengthR,
			    disp, 0., label.str().c_str());
		    break;
		default:
		    label.str("");
		    lengthL = 0.25;
		    lengthR = 0.;
		    disp = 0.;  // not used in this case, but set it anyway.
		    break;
		}
		// first the bottom axis, just the ticks
		if(fabs(tickpt)<(x2-x1)/1.e4) step = 2.*(x2-x1);
		else step = tickpt;
		cpgaxis("",
			tickpt-0.001*(x2-x1), y1,
			tickpt+0.001*(x2-x1), y1,
			tickpt-0.001*(x2-x1), tickpt+0.001*(x2-x1),
			step, -1, lengthL,lengthR, 0.5, disp, 0.);
		//and now the top -- no labels, just tick marks
		cpgtick(x1,y2,x2,y2,float(i)/10.,lengthL,lengthR,0.,0.,"");
	    }
	}
    
	void SpectralPlot::gotoMap()
	{
	    cpgvsiz(this->mapCoords[0],this->mapCoords[1],
		    this->mapCoords[2],this->mapCoords[3]);
	    cpgsch(Plot::plotIndexSize);
	}

	void SpectralPlot::drawVelRange(float v1, float v2)
	{
	    /// @details
	    /// Draws two vertical lines at the limits of velocity 
	    ///  given by the arguments.
	    /// \param v1 Minimum velocity. 
	    /// \param v2 Maximum velocity.

	    int ci,ls;
	    float dud,min,max;
	    cpgqwin(&dud,&dud,&min,&max);
	    cpgqci(&ci);
	    cpgqls(&ls);
	    cpgsci(DUCHAMP_OBJECT_OUTLINE_COLOUR);
	    cpgsls(DASHED);
	    cpgmove(v1,min);  cpgdraw(v1,max);
	    cpgmove(v2,min);  cpgdraw(v2,max);
	    cpgsci(ci);
	    cpgsls(ls);
	}

	void SpectralPlot::drawFlaggedChannelRange(float v1, float v2)
	{
	    ///  @details
	    /// Draws a box showing the extent of flagged channels. 
	    /// \param v1 Minimum world coordinate of the flagged channel range.
	    /// \param v2 Maximum world coordinate of the flagged channel range.

	    int ci,fs;
	    float dud,min,max,height;
	    cpgqwin(&dud,&dud,&min,&max);
	    height = max-min;
	    max += 0.01*height;
	    min -= 0.01*height;
	    cpgqci(&ci);
	    cpgqfs(&fs);
	    setDarkGreen();
	    cpgsci(DUCHAMP_MILKY_WAY_COLOUR);
	    cpgsfs(HATCHED);
	    cpgrect(v1,v2,min,max);
	    cpgsfs(OUTLINE);
	    cpgrect(v1,v2,min,max);
	    cpgsci(ci);
	    cpgsfs(fs);
	}
    
	void SpectralPlot::drawThresholds(Param &par, Statistics::StatsContainer<float> &stats)
	{

	    float dud,vmin,vmax;
	    cpgqwin(&vmin,&vmax,&dud,&dud);
	    cpgsci(BLUE);
	    cpgsls(DASHED);
	    float thresh = stats.getThreshold();
	    if(par.getFlagNegative()) thresh *= -1.;
	    cpgmove(vmin,thresh);
	    cpgdraw(vmax,thresh);
	    if(par.getFlagGrowth()){
		if(par.getFlagUserGrowthThreshold()) thresh= par.getGrowthThreshold();
		else thresh= stats.snrToValue(par.getGrowthCut());
		if(par.getFlagNegative()) thresh *= -1.;	
		cpgsls(DOTTED);
		cpgmove(vmin,thresh);
		cpgdraw(vmax,thresh);
	    }
	    cpgsci(FOREGND);
	    cpgsls(SOLID);
	}

    }

}

