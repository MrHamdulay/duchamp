// -----------------------------------------------------------------------
// plotImage.cc: Functions to plot 2D images
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
#include <iostream>
#include <math.h>
#include <cpgplot.h>
#include <duchamp/param.hh>
#include <duchamp/Utils/utils.hh>

void plotImage(float *array, int xdim, int ydim)
{
  const float tr[6] = {0.,1.,0.,0.,0.,1.};

  cpgenv(0.5,xdim+0.5,0.5,ydim+0.5,0,0);
  float z1,z2;
  zscale(xdim*ydim,array,z1,z2);
  cpggray(array,xdim,ydim,1,xdim,1,ydim,z1,z2,tr);
  cpgbox("bcstn",0,0,"bc",0,0);
  cpgwedg("rg",1,2,z1,z2,"");
}

void plotImage(float *array, int xdim, int ydim, duchamp::Param par)
{
  const float tr[6] = {0.,1.,0.,0.,0.,1.};

  cpgenv(0.5,xdim+0.5,0.5,ydim+0.5,0,0);
  float z1,z2;
  zscale(xdim*ydim,array,z1,z2,par.getBlankPixVal());
  cpggray(array,xdim,ydim,1,xdim,1,ydim,z1,z2,tr);
  cpgbox("bcstn",0,0,"bc",0,0);
  cpgwedg("rg",1,2,z1,z2,"");
}

void plotImage(float *array, int xdim, int ydim, float z1, float z2)
{
  const float tr[6] = {0.,1.,0.,0.,0.,1.};

  cpgenv(0.5,xdim+0.5,0.5,ydim+0.5,0,0);
  cpggray(array,xdim,ydim,1,xdim,1,ydim,z1,z2,tr);
  cpgbox("bc",0,0,"bc",0,0);
  cpgwedg("rg",1,2,z1,z2,"");
}

