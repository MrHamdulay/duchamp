// -----------------------------------------------------------------------
// plottingUtilities.cc: Functions to simplify basic PGPLOT commands,
//                       plus one to draw a contour map of a set 
//                       of points.
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
#include <stdlib.h>
#include <cpgplot.h>
#include <duchamp/Utils/utils.hh>

void plotLine(const float slope, const float intercept)
{
  float x1, x2, y1, y2;
  cpgqwin(&x1,&x2,&y1,&y2);
  cpgmove(x1,slope*x1+intercept);
  cpgdraw(x2,slope*x2+intercept);
}

void lineOfEquality()
{
  plotLine(1.,0.);
}

void lineOfBestFit(int size, float *x, float *y)
{
  float a,b,r,erra,errb;
  linear_regression(size,x,y,0,size-1,a,erra,b,errb,r);
  plotLine(a,b);
}

void plotVertLine(const float xval, const int colour, const int style)
{
  float x1, x2, y1, y2;
  cpgqwin(&x1,&x2,&y1,&y2);
  int currentColour,currentStyle;
  cpgqci(&currentColour);
  cpgqls(&currentStyle);
  if(currentColour!=colour) cpgsci(colour);
  if(currentStyle !=style)  cpgsls(style);
  cpgmove(xval,y1);
  cpgdraw(xval,y2);
  if(currentColour!=colour) cpgsci(currentColour);
  if(currentStyle !=style)  cpgsls(currentStyle);
}

void plotVertLine(const float xval)
{
  int colour,style;
  cpgqci(&colour);
  cpgqls(&style);
  plotVertLine(xval,colour,style);
}

void plotVertLine(const float xval, const int colour)
{
  int style;
  cpgqls(&style);
  plotVertLine(xval,colour,style);
}

void plotHorizLine(const float yval, const int colour, const int style)
{
  float x1, x2, y1, y2;
  cpgqwin(&x1,&x2,&y1,&y2);
  int currentColour,currentStyle;
  cpgqci(&currentColour);
  cpgqls(&currentStyle);
  if(currentColour!=colour) cpgsci(colour);
  if(currentStyle !=style)  cpgsls(style);
  cpgmove(x1,yval);
  cpgdraw(x2,yval);
  if(currentColour!=colour) cpgsci(currentColour);
  if(currentStyle !=style)  cpgsls(currentStyle);
}

void plotHorizLine(const float yval)
{
  int colour,style;
  cpgqci(&colour);
  cpgqls(&style);
  plotHorizLine(yval,colour,style);
}

void plotHorizLine(const float yval, const int colour)
{
  int style;
  cpgqls(&style);
  plotHorizLine(yval,colour,style);
}

void lineOfBestFitPB(const int size, const float *x, const float *y)
{
  int numSim = int(size * log(float(size)*log(float(size))) );
  float slope=0, intercept=0;
  float *xboot = new float[size];
  float *yboot = new float[size];
  for(int sim=0;sim<numSim;sim++){
    for(int i=0;i<size;i++){
      int point = int((1.*rand())/(RAND_MAX+1.0) * size);
      xboot[i] = x[point];
      yboot[i] = y[point];
    }
    float a,b,r,erra,errb;
    linear_regression(size,xboot,yboot,0,size-1,a,erra,b,errb,r);
    slope += a;
    intercept += b;
  }
  delete [] xboot;
  delete [] yboot;
  slope /= float(numSim);
  intercept /= float(numSim);
  
  plotLine(slope,intercept);
}

void drawContours(const int size, const float *x, const float *y)
{
  float x1, x2, y1, y2;
  cpgqwin(&x1,&x2,&y1,&y2);
  const int nbin = 30;
  // widths of bins in x and y directions: have one bin either side of
  // extrema
  float binWidthX = (x2 - x1) / float(nbin-2); 
  float binWidthY = (y2 - y1) / float(nbin-2);
  float *binnedarray = new float[nbin*nbin];
  for(int i=0;i<nbin*nbin;i++) binnedarray[i] = 0.;
  for(int i=0;i<size;i++){
    int xbin = int( (x[i] - (x1-binWidthX)) / binWidthX );
    int ybin = int( (y[i] - (y1-binWidthY)) / binWidthY );
    int binNum = ybin*nbin + xbin;
    if((binNum>=0)&&(binNum<nbin*nbin)) binnedarray[binNum] += 1.;
  }  
  float maxct = binnedarray[0];
  for(int i=1;i<nbin*nbin;i++) if(binnedarray[i]>maxct) maxct=binnedarray[i];
  float tr[6]={x1-3*binWidthX/2,binWidthX,0.,y1-3*binWidthY/2,0.,binWidthY};
  const int ncont = 10;
  float *cont = new float[ncont];
  for(int i=0;i<ncont;i++) cont[i] = maxct / pow(2,float(i)/2.);
  cpgcont(binnedarray,nbin,nbin,1,nbin,1,nbin,cont,ncont,tr);
  delete [] binnedarray;
  delete [] cont;
}
