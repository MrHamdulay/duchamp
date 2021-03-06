// -----------------------------------------------------------------------
// drawMomentCutout.cc: Drawing a 0th-moment map and related functions.
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
#include <sstream>
#include <cpgplot.h>
#include <math.h>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Cubes/cubeUtils.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/mycpgplot.hh>
#include <duchamp/PixelMap/Voxel.hh>
#include <duchamp/PixelMap/Object3D.hh>
#include <vector>

const int MIN_WIDTH=20;
using namespace mycpgplot;
using namespace PixelInfo;

namespace duchamp
{

  void Cube::drawMomentCutout(Detection &object)
  {
    ///  @details
    ///   A routine to draw the 0th moment for the given detection
    ///    using the flux given by the pixel array in the Cube.
    ///   The 0th moment is constructed by adding the flux of each
    ///    pixel within the full extent of the object (this may be more
    ///    pixels than were actually detected in the object)
    ///   A tick mark is also drawn to indicate angular scale (but only
    ///    if the WCS for the Cube is valid).
    /// \param object The Detection to be drawn.

    if(!cpgtest()){
      DUCHAMPERROR("Draw Cutout","There is no PGPlot device open.");
    }
    else{

      size_t size = (object.getXmax()-object.getXmin()+1);
      if(size<size_t(object.getYmax()-object.getYmin()+1)) 
	size = object.getYmax()-object.getYmin()+1;
      size += MIN_WIDTH;

      long xmin = (object.getXmax()+object.getXmin())/2 - size/2 + 1;
      long xmax = (object.getXmax()+object.getXmin())/2 + size/2;
      long ymin = (object.getYmax()+object.getYmin())/2 - size/2 + 1;
      long ymax = (object.getYmax()+object.getYmin())/2 + size/2;
      long zmin = object.getZmin();
      long zmax = object.getZmax();

      std::vector<bool> isGood(size*size,true);
      for(int z=zmin; z<=zmax; z++){
	for(int x=xmin; x<=xmax; x++){
	  for(int y=ymin; y<=ymax; y++){
	    isGood[(y-ymin) * size + (x-xmin)] = 
	      ((x>=0)&&(x<int(this->axisDim[0])))    // if inside the boundaries
	      && ((y>=0)&&(y<int(this->axisDim[1]))) // if inside the boundaries
	      && !this->isBlank(x,y,z);         // if not blank
	  }
	}
      }

      float *image = new float[size*size];
      for(size_t i=0;i<size*size;i++) image[i]=0.;

      size_t imPos,cubePos;
      for(int z=zmin; z<=zmax; z++){
	for(int x=xmin; x<=xmax; x++){
	  for(int y=ymin; y<=ymax; y++){

	    imPos = (y-ymin) * size + (x-xmin);
	    cubePos = z*this->axisDim[0]*this->axisDim[1] + 
	      y*this->axisDim[0] + x;

	    if(isGood[imPos]) image[imPos] += this->array[cubePos];

	  }
	}
      }

      for(size_t i=0;i<size*size;i++){
	// if there is some signal on this pixel, normalise by the velocity width
	if(isGood[i]) image[i] /= float(zmax-zmin+1); 
      }

      // now work out the greyscale display limits, 
      //   excluding blank pixels where necessary.
      float z1,z2;
      int ct=0;
      while(!isGood[ct]) ct++; // move to first non-blank pixel
      z1 = z2 = image[ct];
      for(size_t i=1;i<size*size;i++){
	if(isGood[i]){
	  if(image[i]<z1) z1=image[i];
	  if(image[i]>z2) z2=image[i];
	}
      }

      // adjust the values of the blank and extra-image pixels
      for(size_t i=0;i<size*size;i++)
	if(!isGood[i]) image[i] = z1 - 1.;


      float tr[6] = {xmin-1,1.,0.,ymin-1,0.,1.};

      cpgswin(xmin-0.5,xmax-0.5,ymin-0.5,ymax-0.5);
      //     cpggray(image, size, size, 1, size, 1, size, z1, z2, tr);
      cpggray(image, size, size, 1, size, 1, size, z2, z1, tr);
      cpgbox("bc",0,0,"bc",0,0);

      delete [] image;

      int ci;
      cpgqci(&ci);

      // Draw the border of the BLANK region, if there is one...
      drawBlankEdges(this->array,this->axisDim[0],this->axisDim[1],this->par);

      // Draw the border of cube's pixels
      this->drawFieldEdge();

      // Draw the fitted ellipse
      cpgsci(RED);
      float scale=this->head.getShapeScale();
      cpgellipse(object.getXcentre(), object.getYcentre(), 
		 object.getMajorAxis()/this->head.getAvPixScale()/2./scale, object.getMinorAxis()/this->head.getAvPixScale()/2./scale, 
		 90.+object.getPositionAngle());

      // Draw the beam
      if(this->head.beam().isDefined()){
	  setDarkGreen();
	  cpgsci(DARKGREEN);
	  cpgellipse(xmax-0.5-this->head.beam().maj(), ymin-0.5+this->head.beam().maj(),
		     this->head.beam().maj()/2., this->head.beam().min()/2., this->head.beam().pa()+90.);
      }

      // Draw the borders around the object
      cpgsci(DUCHAMP_OBJECT_OUTLINE_COLOUR);
      cpgsfs(OUTLINE);
      if(this->par.drawBorders()) 
	object.drawBorders(xmin,ymin);
      else 
	cpgrect(object.getXmin()-xmin+0.5,object.getXmax()-xmin+1.5,
		object.getYmin()-ymin+0.5,object.getYmax()-ymin+1.5);
      /*
	To get the borders localised correctly, we need to subtract (xmin-1) 
	from the X values. We then subtract 0.5 for the left hand border 
	(to place it on the pixel border), and add 0.5 for the right hand 
	border. Similarly for y.
      */

      if(this->head.isWCS()){
	// Now draw a tick mark to indicate size -- 15 arcmin in length
	//     this->drawScale(xmin+2.,ymin+2.,object.getZcentre(),0.25);
	this->drawScale(xmin+2.,ymin+2.,object.getZcentre());
      }

      cpgsci(ci);


    }

  }

  void Cube::drawScale(float xstart, float ystart, float channel)
  {
    ///  @details
    ///   A routine to draw a scale bar on a (pre-existing) PGPlot image.
    ///   It uses an iterative technique to move from the given start position 
    ///    (xstart,ystart) along the positive x-direction so that the length is
    ///    within 1% of the scaleLength (length in degrees), calculated 
    ///    according to the pixel scale of the cube.
    ///  \param xstart X-coordinate of the start position (left-hand edge
    ///  of tick mark typically).
    ///  \param ystart Y-coordinate of the start position
    ///  \param channel Which channel to base WCS calculations on: needed
    ///  as the positions could theoretically change with channel.

    if(!cpgtest()){
      DUCHAMPERROR("Draw Cutout","There is no PGPlot device open.");
    }
    else{

      if(this->head.isWCS()){  // can only do this if the WCS is good!

	enum ANGLE {ARCSEC, ARCMIN, DEGREE};
	const std::string symbol[3] = {"\"", "'", mycpgplot::degrees };
	const float angleScale[3] = {3600., 60., 1.};
	//  degree, arcmin, arcsec symbols
    
	const int numLengths = 17;
	const double lengths[numLengths] = 
	  {0.001/3600., 0.005/3600., 0.01/3600., 0.05/3600., 
	   0.1/3600., 0.5/3600., 
	   1./3600., 5./3600., 15./3600., 30./3600.,
	   1./60., 5./60., 15./60., 30./60.,
	   1., 5., 15.};
	const ANGLE angleType[numLengths] = 
	  {ARCSEC, ARCSEC, ARCSEC, ARCSEC,
	   ARCSEC, ARCSEC, ARCSEC, ARCSEC,
	   ARCSEC, ARCSEC,
	   ARCMIN, ARCMIN, ARCMIN, ARCMIN,
	   DEGREE, DEGREE, DEGREE};
	const float desiredRatio = 0.2;

	// first, work out what is the optimum length of the scale bar,
	//   based on the pixel scale and size of the image.
	float pixscale = this->head.getAvPixScale();
	double *fraction = new double[numLengths];
	int best=0;
	float x1,x2,y1,y2;
	cpgqwin(&x1,&x2,&y1,&y2);
	for(int i=0;i<numLengths;i++){
	  fraction[i] = (lengths[i]/pixscale) / (x2-x1);
	  if(i==0) best=0;
	  else if(fabs(fraction[i] - desiredRatio) < 
		  fabs(fraction[best] - desiredRatio)) best=i;
	}
	delete [] fraction;

	// Now work out actual pixel locations for the ends of the scale bar
	double *pix1   = new double[3];
	double *pix2   = new double[3];
	double *world1 = new double[3];
	double *world2 = new double[3];
	pix1[0] = pix2[0] = xstart;
	pix1[1] = pix2[1] = ystart;
	pix1[2] = pix2[2] = channel;
	this->head.pixToWCS(pix1,world1);

	double angSep=0.;
	double error;
	double step=1.; // this is in pixels
	double scaleLength = lengths[best];  // this is in degrees
	pix2[0] = pix1[0] + scaleLength/(2.*pixscale);
	do{
	  this->head.pixToWCS(pix2,world2);
	  angSep = angularSeparation(world1[0],world1[1],world2[0],world2[1]);
	  error = (angSep-scaleLength)/scaleLength;
	  if(error<0) error = 0 - error;
	  if(angSep>scaleLength){
	    pix2[0] -= step;
	    step /= 2.;
	  }
	  pix2[0] += step;
	}while(error>0.05); // look for 1% change

	float tickpt1 = pix1[0];
	float tickpt2 = pix2[0];
	float tickpt3 = ystart;
	int colour;
	cpgqci(&colour);
	cpgsci(DUCHAMP_TICKMARK_COLOUR);
	int thickness;
	cpgqlw(&thickness);
	cpgslw(3);
	cpgerrx(1,&tickpt1,&tickpt2,&tickpt3,2.);
	cpgslw(thickness);

	std::stringstream text;
	text << scaleLength * angleScale[angleType[best]] 
	     << symbol[angleType[best]];
	float size,xch,ych;
	cpgqch(&size);
	cpgsch(0.4);
	cpgqcs(4,&xch,&ych); // get the character size in world coords
	cpgptxt((tickpt1+tickpt2)/2., ystart+ych, 0, 0.5, text.str().c_str());
	cpgsch(size);
	cpgsci(colour);

	delete [] pix1;
	delete [] pix2;
	delete [] world1;
	delete [] world2;

      }
    }

  }
  
  void Cube::drawFieldEdge()
  {
    /// @details
    /// Draw a border around the spatial edge of the data. Lines are
    /// drawn in yellow at 0 and the values of xdim & ydim.  There must
    /// be a PGPLOT window open, else an error message is returned.

    if(!cpgtest()){
      DUCHAMPERROR("Draw Cutout","There is no PGPlot device open.");
    }
    else{
      int ci;
      cpgqci(&ci);
      cpgsci(DUCHAMP_CUBE_EDGE_COLOUR);
  
      cpgmove(-0.5,-0.5);
      cpgdraw(-0.5,this->axisDim[1]-0.5);
      cpgdraw(this->axisDim[0]-0.5,this->axisDim[1]-0.5);
      cpgdraw(this->axisDim[0]-0.5,-0.5);
      cpgdraw(-0.5,-0.5);

      cpgsci(ci);
    }
  }

}
