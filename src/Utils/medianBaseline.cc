// -----------------------------------------------------------------------
// medianBaseline.cc: Obtaining spectral baselines using a median filter.
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
#include <map>

float medVal(std::map<float,int> &themap)
{
    std::map<float,int>::iterator it;
    it = themap.begin();
    if(themap.size()%2==0) {
	for(size_t i=0;i<themap.size()/2-1;i++) it++;
	float v1 = it->first;
	it++;
	float v2 = it->first;
	return (v1+v2)/2.;
    }
    else{
	for(size_t i=0;i<themap.size()/2;i++) it++;
	return it->first;
    }
}

void findMedianBaseline(int dim, float *inputArray, unsigned int boxWidth, float *baselineValues)
{

    if(boxWidth%2==0) std::cerr << "medianBaseline: boxWidth needs to be odd. Changing from " << boxWidth << " to " << boxWidth+1 <<"\n";
    boxWidth++;


    int halfWidth = boxWidth/2;

    if(halfWidth > dim) {
	std::cerr << "medianBaseline: Box half-width bigger than array dimension. Returning output array unchanged.\n";
    }

    std::map<float,int> pixMap;
  
//  float *baselineValues = new float[dim];

    // Initialise the map for the first point in the array
    int curpos = 0;
    for(int i=0; i<=halfWidth; i++) {
	pixMap.insert(std::pair<float,int>(inputArray[i],i));
    }
    baselineValues[curpos] = medVal(pixMap);
    // for(std::map<float,int>::iterator it=pixMap.begin();it!=pixMap.end();it++) std::cerr << it->first <<" -> " << it->second <<"\n";
    // std::cerr << baselineValues[curpos] <<"\n";

    // Do it for the rest of the points
    curpos++;
    for(;curpos<dim;curpos++){
    
	int newpix = curpos+halfWidth;
	if(newpix<dim) pixMap.insert(std::pair<float,int>(inputArray[newpix],newpix));

	if(pixMap.size()>boxWidth){
	    if(curpos>halfWidth) pixMap.erase(inputArray[curpos-halfWidth]);
	}

	baselineValues[curpos] = medVal(pixMap);

    }

}
