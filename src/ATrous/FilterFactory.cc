// -----------------------------------------------------------------------
// FilterFactory.hh: Factory method for obtaining a Filter
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
#include <duchamp/duchamp.hh>
#include <duchamp/ATrous/FilterFactory.hh>
#include <duchamp/ATrous/Filter.hh>
#include <duchamp/ATrous/B3SplineFilter.hh>
#include <duchamp/ATrous/TriangleFilter.hh>
#include <duchamp/ATrous/HaarFilter.hh>

namespace duchamp {

    Filter *FilterFactory::getFilter(unsigned int code)
    {
	Filter *filt=0;
	if (code == 2){
	    TriangleFilter *tri = new TriangleFilter;
	    filt = &(*tri);
	}
	else if(code == 3){
	    HaarFilter *haar = new HaarFilter;
	    filt = &(*haar);
	}
	else{
	    if(code!=1)
		DUCHAMPWARN("Wavelet Filter", "Filter code " << code << " undefined. Using B3 spline.");
	    B3SplineFilter *b3 = new B3SplineFilter;
	    filt = &(*b3);
	}
	return filt;


    }



}
