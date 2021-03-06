// -----------------------------------------------------------------------
// B3SplineFilter.hh: Defining a B3Spline filter function for wavelet reconstruction.
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

#ifndef FILTER_B3SPLINE_H_
#define FILTER_B3SPLINE_H_
#include <duchamp/duchamp.hh>
#include <duchamp/ATrous/filter.hh>

namespace duchamp {

    class B3SplineFilter: public Filter
    {
    public:
	B3SplineFilter();
	B3SplineFilter(const B3SplineFilter& other);
	B3SplineFilter& operator= (const B3SplineFilter& other);
	virtual ~B3SplineFilter(){};

    };


}



#endif
