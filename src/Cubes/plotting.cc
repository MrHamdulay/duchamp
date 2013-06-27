// -----------------------------------------------------------------------
// plotting.cc: Plot the moment map and detection maps, showing the
//              location of the detected objects.
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
#include <iomanip>
#include <sstream>
#include <math.h>
#include <string.h>
#include <cpgplot.h>
#include <wcslib/cpgsbox.h>
#include <wcslib/pgwcsl.h>
#include <wcslib/wcs.h>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/PixelMap/Object3D.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Cubes/cubeUtils.hh>
//#include <duchamp/Cubes/plots.hh>
#include <duchamp/Plotting/SpectralPlot.hh>
#include <duchamp/Plotting/SimpleSpectralPlot.hh>
#include <duchamp/Plotting/ImagePlot.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/mycpgplot.hh>

using namespace mycpgplot;
using namespace PixelInfo;

namespace duchamp
{

  void Cube::plotDetectionMap(std::string pgDestination)
  {
    ///  @details
    ///  Creates a map of the spatial locations of the detections, which is 
    ///   written to the PGPlot device given by pgDestination.
    ///  The map is done in greyscale, where the scale indicates the number of 
    ///   velocity channels that each spatial pixel is detected in.
    ///  The boundaries of each detection are drawn, and each object is numbered
    ///   (to match the output list and spectra).
    ///  The primary grid scale is pixel coordinate, and if the WCS is valid, 
    ///   the correct WCS gridlines are also drawn.
    /// \param pgDestination The PGPLOT device to be opened, in the typical PGPLOT format.

    // These are the minimum values for the X and Y ranges of the box drawn by 
    //   pgplot (without the half-pixel difference).
    // The -1 is necessary because the arrays we are dealing with start at 0 
    //  index, while the ranges given in the subsection start at 1... 
    float boxXmin = this->par.getXOffset() - 1;
    float boxYmin = this->par.getYOffset() - 1;

    long xdim=this->axisDim[0];
    long ydim=this->axisDim[1];
    long zdim=this->axisDim[2];

    if( this->numNondegDim == 1){
      
      float *specx  = new float[zdim];
      float *specy  = new float[zdim];
      float *specy2 = new float[zdim];
      float *base   = new float[zdim];
      Plot::SimpleSpectralPlot spPlot;
      int flag = spPlot.setUpPlot(pgDestination.c_str());
//       int flag=cpgopen(pgDestination.c_str());
//       cpgpap(spPlot.getPaperWidth(),spPlot.getAspectRatio());
//       cpgsch(Plot::spLabelSize);
      if(flag <= 0){
	DUCHAMPERROR("Plot Detection Map", "Could not open PGPlot device " << pgDestination);
      }
      else{
	
//	std::vector<bool> flaggedChans=this->par.getChannelFlags(zdim);
	this->getSpectralArrays(-1,specx,specy,specy2,base);
	float vmax,vmin,width;
	vmax = vmin = specx[0];
	for(int i=1;i<zdim;i++){
	  if(specx[i]>vmax) vmax=specx[i];
	  if(specx[i]<vmin) vmin=specx[i];
	}
      
	// Find the maximum & minimum values of the spectrum, ignoring flagged channels.
	float max,min;
	bool haveStarted=false;
	for(int z=0;z<zdim;z++){
//	    if(!flaggedChans[z]){
	    if(!this->par.isFlaggedChannel(z)){
		if(specy[z]>max && !haveStarted) max=specy[z];
		if(specy[z]<min && !haveStarted) min=specy[z];
		haveStarted=true;
	    }
	}

	// widen the ranges slightly so that the top & bottom & edges don't 
	// lie on the axes.
	width = max - min;
	max += width * 0.15;
	min -= width * 0.05;
	width = vmax - vmin;
	vmax += width * 0.01;
	vmin -= width * 0.01;
	std::string label,fluxLabel = "Flux ["+this->head.getFluxUnits()+"]";
	if(this->head.isWCS()){
	  label = this->head.getSpectralDescription() + " [" + 
	    this->head.getSpectralUnits() + "]";
	}
	else label="Spectral pixel";
	std::string filename=this->pars().getImageFile();
	filename = filename.substr(filename.rfind('/')+1);
	spPlot.label(label,fluxLabel,"Detection summary : " + filename);
	spPlot.gotoMainSpectrum(vmin,vmax,min,max);
	cpgline(zdim,specx,specy);
	if(this->par.getFlagBaseline()){
	  cpgsci(DUCHAMP_BASELINE_SPECTRA_COLOUR);
	  cpgline(zdim,specx,base);
	  cpgsci(FOREGND);
	}
	if(this->reconExists){
	  cpgsci(DUCHAMP_RECON_SPECTRA_COLOUR);
	  cpgline(zdim,specx,specy2);    
	  cpgsci(FOREGND);
	}
	this->drawFlaggedChannels(spPlot,0.,0.);

	spPlot.markDetectedPixels(this->detectMap,zdim,this->head);

	for(size_t i=0;i<this->getNumObj();i++){
	  drawSpectralRange(spPlot,this->objectList->at(i),this->head);
	}

	cpgsci(RED);
	cpgsls(DASHED);
	float thresh = this->Stats.getThreshold();
	if(this->par.getFlagNegative()) thresh *= -1.;
	cpgmove(vmin,thresh);
	cpgdraw(vmax,thresh);
	if(this->par.getFlagGrowth()){
	  if(this->par.getFlagUserGrowthThreshold()) thresh= this->par.getGrowthThreshold();
	  else thresh= this->Stats.snrToValue(this->par.getGrowthCut());
	  if(this->par.getFlagNegative()) thresh *= -1.;	
	  cpgsls(DOTTED);
	  cpgmove(vmin,thresh);
	  cpgdraw(vmax,thresh);
	}
      }

      spPlot.close();
    }
    else { // num dim > 1

      Plot::ImagePlot newplot(xdim,ydim);
      int flag = newplot.setUpPlot(pgDestination);

      if(flag<=0){
	DUCHAMPERROR("Plot Detection Map", "Could not open PGPlot device " << pgDestination);
      }
      else{

	// get the list of objects that should be plotted. Only applies to outlines and labels.
	std::vector<bool> objectChoice = this->par.getObjectChoices(this->objectList->size());

	std::string filename=this->pars().getImageFile();
	newplot.makeTitle(filename.substr(filename.rfind('/')+1,filename.size()));

	newplot.drawMapBox(boxXmin+0.5,boxXmin+xdim+0.5,
			   boxYmin+0.5,boxYmin+ydim+0.5,
			   "X pixel","Y pixel");

	//     if(this->objectList.size()>0){ 
	// if there are no detections, there will be nothing to plot here

	// Define a float equivalent of this->detectMap that can be plotted by cpggray.
	// Also find the maximum value, so that we can get the greyscale right and plot a colour wedge.
	float *detectionMap = new float[xdim*ydim];
	int maxNum = this->detectMap[0];
	detectionMap[0] = float(maxNum);
	for(int pix=1;pix<xdim*ydim;pix++){
	  detectionMap[pix] = float(this->detectMap[pix]);  
	  if(this->detectMap[pix] > maxNum)  maxNum = this->detectMap[pix];
	}

	if(maxNum>0){ // if there are no detections, it will be 0.

	  maxNum = 5 * ((maxNum-1)/5 + 1);  // move to next multiple of 5

	  float tr[6] = {boxXmin,1.,0.,boxYmin,0.,1.};
	  cpggray(detectionMap,xdim,ydim,1,xdim,1,ydim,maxNum,0,tr);  
	  cpgbox("bcnst",0.,0,"bcnst",0.,0);
	  cpgsch(1.5);
	  cpgwedg("rg",3.2,2,maxNum,0,"Number of detected channels");
	}

	delete [] detectionMap;
      
	drawBlankEdges(this->array,this->axisDim[0],this->axisDim[1],this->par);
      
	if(this->head.isWCS()) this->plotWCSaxes();
  
	if(this->objectList->size()>0){ 
	  // now show and label each detection, drawing over the WCS lines.

	  cpgsch(1.0);
	  cpgslw(2);    
	  float xoff=0.;
	  float yoff=newplot.cmToCoord(0.5);
	  if(this->par.drawBorders()){
	    cpgsci(DUCHAMP_OBJECT_OUTLINE_COLOUR);
	    for(size_t i=0;i<this->objectList->size();i++) 
	      if(objectChoice[i]) this->objectList->at(i).drawBorders(0,0);
	  }
	  cpgsci(DUCHAMP_ID_TEXT_COLOUR);
	  std::stringstream label;
	  cpgslw(1);
	  for(size_t i=0;i<this->objectList->size();i++){
	    if(objectChoice[i]) {
	      cpgpt1(this->par.getXOffset()+this->objectList->at(i).getXPeak(), 
		     this->par.getYOffset()+this->objectList->at(i).getYPeak(), 
		     CROSS);
	      label.str("");
	      label << this->objectList->at(i).getID();
	      cpgptxt(this->par.getXOffset()+this->objectList->at(i).getXPeak()-xoff, 
		      this->par.getYOffset()+this->objectList->at(i).getYPeak()-yoff, 
		      0, 0.5, label.str().c_str());
	    }
	  }

	}

	newplot.close();
      }
    }
  }

  /*********************************************************/

  void Cube::plotMomentMap(std::string pgDestination)
  {
    ///  @details
    ///  Uses the other function
    ///  Cube::plotMomentMap(std::vector<std::string>) to plot the moment
    ///  map.
    /// \param pgDestination The PGPLOT device that the map is to be written to.

    std::vector<std::string> devicelist;
    devicelist.push_back(pgDestination);
    this->plotMomentMap(devicelist);
  }

  /*********************************************************/

  void Cube::plotMomentMap(std::vector<std::string> pgDestination)
  {
    ///  @details
    ///  Creates a 0th moment map of the detections, which is written to each
    ///   of the PGPlot devices mentioned in pgDestination.
    ///  The advantage of this function is that the map is only calculated once,
    ///   even if multiple maps are required.
    ///  The map is done in greyscale, where the scale indicates the integrated 
    ///   flux at each spatial pixel.
    ///  The boundaries of each detection are drawn, and each object is numbered
    ///   (to match the output list and spectra).
    ///  The primary grid scale is pixel coordinate, and if the WCS is valid, 
    ///   the correct WCS gridlines are also drawn.
    /// \param pgDestination A set of PGPLOT devices that are to be
    /// opened, each in the typical PGPLOT format.

    float boxXmin = this->par.getXOffset() - 1;
    float boxYmin = this->par.getYOffset() - 1;

    long xdim=this->axisDim[0];
    long ydim=this->axisDim[1];

    int numPlots = pgDestination.size();
    std::vector<Plot::ImagePlot> plotList(numPlots,Plot::ImagePlot(xdim,ydim));
    std::vector<int> plotFlag(numPlots,0);
    std::vector<bool> doPlot(numPlots,false);
    bool plotNeeded = false;

    for(int i=0;i<numPlots;i++){
    
      plotFlag[i] = plotList[i].setUpPlot(pgDestination[i]);
       
      if(plotFlag[i]<=0){
	DUCHAMPERROR("Plot Moment Map", "Could not open PGPlot device " << pgDestination[i]);
      }
      else{
	doPlot[i] = true;
	plotNeeded = true;
      }

    }

    if(plotNeeded){

      if(this->objectList->size()==0){ 
	// if there are no detections, we plot an empty field.

	for(int iplot=0; iplot<numPlots; iplot++){
	  plotList[iplot].goToPlot();
	  std::string filename=this->pars().getImageFile();
	  plotList[iplot].makeTitle(filename.substr(filename.rfind('/')+1,filename.size()));
	
	  plotList[iplot].drawMapBox(boxXmin+0.5,boxXmin+xdim+0.5,
				     boxYmin+0.5,boxYmin+ydim+0.5,
				     "X pixel","Y pixel");
	
	  drawBlankEdges(this->array,this->axisDim[0],this->axisDim[1],this->par);
	
	  if(this->head.isWCS()) this->plotWCSaxes();
	}

      }
      else {  
	// if there are some detections, do the calculations first before
	//  plotting anything.
  
	// get the list of objects that should be plotted. Only applies to outlines and labels.
	std::vector<bool> objectChoice = this->par.getObjectChoices(this->objectList->size());

	for(int iplot=0; iplot<numPlots; iplot++){
	  // Although plot the axes so that the user knows something is 
	  //  being done (at least, they will if there is an /xs plot)
	  plotList[iplot].goToPlot();
	  std::string filename=this->pars().getImageFile();
	  plotList[iplot].makeTitle(filename.substr(filename.rfind('/')+1,filename.size()));
    
	  plotList[iplot].drawMapBox(boxXmin+0.5,boxXmin+xdim+0.5,
				     boxYmin+0.5,boxYmin+ydim+0.5,
				     "X pixel","Y pixel");
	
	  if(pgDestination[iplot]=="/xs") 
	    cpgptxt(boxXmin+0.5+xdim/2., boxYmin+0.5+ydim/2., 0, 0.5,
		    "Calculating map...");
	}

	float *momentMap = new float[xdim*ydim];
	float z1=0.,z2=0.;
	this->getMomentMapForPlot(momentMap,z1,z2);


	// Have now done all necessary calculations for moment map.
	// Now produce the plot

	for(int iplot=0; iplot<numPlots; iplot++){
	  plotList[iplot].goToPlot();
      
	  float tr[6] = {boxXmin,1.,0.,boxYmin,0.,1.};
	  cpggray(momentMap,xdim,ydim,1,xdim,1,ydim,z2,z1,tr);
	  cpgbox("bcnst",0.,0,"bcnst",0.,0);
	  cpgsch(1.5);
	  std::string wedgeLabel = "Integrated Flux ";
	  if(this->par.getFlagNegative()) 
	    wedgeLabel = "-1. " + times + " " + wedgeLabel;
	  if(this->head.isWCS())
	    wedgeLabel += "[" + this->head.getIntFluxUnits() + "]";
	  else wedgeLabel += "[" + this->head.getFluxUnits() + "]";
	  cpgwedglog("rg",3.2,2,z2,z1,wedgeLabel.c_str());


	  drawBlankEdges(this->array,this->axisDim[0],this->axisDim[1],this->par);

	  if(this->head.isWCS()) this->plotWCSaxes();
  
	  // now show and label each detection, drawing over the WCS lines.
	  cpgsch(1.0);
	  cpgslw(2);
	  float xoff=0.;
	  float yoff=plotList[iplot].cmToCoord(0.5);
	  if(this->par.drawBorders()){
	    cpgsci(DUCHAMP_OBJECT_OUTLINE_COLOUR);
	    for(size_t i=0;i<this->objectList->size();i++) 
	      if(objectChoice[i]) this->objectList->at(i).drawBorders(0,0);
	  }
	  cpgsci(DUCHAMP_ID_TEXT_COLOUR);
	  std::stringstream label;
	  cpgslw(1);
	  for(size_t i=0;i<this->objectList->size();i++){
	    if(objectChoice[i]) {
	      cpgpt1(this->par.getXOffset()+this->objectList->at(i).getXPeak(), 
		     this->par.getYOffset()+this->objectList->at(i).getYPeak(),
		     CROSS);
	      label.str("");
	      label << this->objectList->at(i).getID();
	      cpgptxt(this->par.getXOffset()+this->objectList->at(i).getXPeak()-xoff,
		      this->par.getYOffset()+this->objectList->at(i).getYPeak()-yoff,
		      0, 0.5, label.str().c_str());
	    }
	  }

	} // end of iplot loop over number of devices

	delete [] momentMap;

      } // end of else (from if(numdetections==0) )

  
      for(int iplot=0; iplot<numPlots; iplot++){
	plotList[iplot].goToPlot();
	plotList[iplot].close();
      }
    
    }
  
  }

  /*********************************************************/

//   void Cube::plotWCSaxes(int textColour, int axisColour)
//   {
//     duchamp::plotWCSaxes(this->head.getWCS(), this->axisDim, textColour, axisColour);
//   }
  
  
  void plotWCSaxes(struct wcsprm *wcs, size_t *axes, int textColour, int axisColour)
  {

    /// @details
    ///  A front-end to the cpgsbox command, to draw the gridlines for the WCS 
    ///    over the current plot.
    ///  Lines are drawn in dark green over the full plot area, and the axis 
    ///    labels are written on the top and on the right hand sides, so as not 
    ///    to conflict with other labels.
    ///  \param textColour The colour index to use for the text labels --
    ///  defaults to duchamp::DUCHAMP_ID_TEXT_COLOUR 
    ///  \param axisColour The colour index to use for the axes --
    ///  defaults to duchamp::DUCHAMP_WCS_AXIS_COLOUR

    float boxXmin=0,boxYmin=0;

    char idents[3][80], opt[2], nlcprm[1];

    strcpy(idents[0], wcs->lngtyp);
    strcpy(idents[1], wcs->lattyp);
    strcpy(idents[2], "");
    if(strcmp(wcs->lngtyp,"RA")==0) opt[0] = 'G';
    else opt[0] = 'D';
    opt[1] = 'E';

    float  blc[2], trc[2];
    //   float  scl; // --> unused here.
    blc[0] = boxXmin + 0.5;
    blc[1] = boxYmin + 0.5;
    trc[0] = boxXmin + axes[0]+0.5;
    trc[1] = boxYmin + axes[1]+0.5;
  
    int existingLineWidth;
    cpgqlw(&existingLineWidth);
    int existingColour;
    cpgqci(&existingColour);
    float existingSize;
    cpgqch(&existingSize);
    cpgsci(textColour);
    cpgsch(0.8);
    int    c0[7], ci[7], gcode[2], ic, ierr;
    for(int i=0;i<7;i++) c0[i] = -1;
    /* define the WCS axes colour */
    setWCSGreen();

    gcode[0] = 2;  // type of grid to draw: 0=none, 1=ticks only, 2=full grid
    gcode[1] = 2;

    double cache[257][4], grid1[9], grid2[9], nldprm[8];
    grid1[0] = 0.0;  
    grid2[0] = 0.0;

    // Draw the celestial grid with no intermediate tick marks.
    // Set LABCTL=2100 to write 1st coord on top, and 2nd on right

    //Colour indices used by cpgsbox: make it all the same colour for thin 
    // line case.
    ci[0] = axisColour; // grid lines, coord 1
    ci[1] = axisColour; // grid lines, coord 2
    ci[2] = axisColour; // numeric labels, coord 1
    ci[3] = axisColour; // numeric labels, coord 2
    ci[4] = axisColour; // axis annotation, coord 1
    ci[5] = axisColour; // axis annotation, coord 2
    ci[6] = axisColour; // title

    cpgsbox(blc, trc, idents, opt, 2100, 0, ci, gcode, 0.0, 0, grid1, 0, grid2,
	    0, pgwcsl_, 1, WCSLEN, 1, nlcprm, (int *)wcs, 
	    nldprm, 256, &ic, cache, &ierr);

    cpgsci(existingColour);
    cpgsch(existingSize);
    cpgslw(existingLineWidth);
  }



  void drawSpectralRange(Plot::SpectralPlot &plot, Detection &obj, FitsHeader &head)
  {
    /// @details

    /// A front-end to drawing the lines delimiting the spectral
    /// extent of the detection. This takes into account the channel
    /// widths, offsetting outwards by half a channel (for instance, a
    /// single-channel detection will not have the separation of one
    /// channel).
    /// If the world coordinate is being plotted, the correct offset
    /// is calcuated by transforming from the central spatial
    /// positions and the offsetted min/max z-pixel extents

    if(head.isWCS()){
      double x=obj.getXcentre(),y=obj.getYcentre(),z;
      z=obj.getZmin()-0.5;
      float vmin=head.pixToVel(x,y,z);
      z=obj.getZmax()+0.5;
      float vmax=head.pixToVel(x,y,z);
      plot.drawVelRange(vmin,vmax);
    }
    else{
      plot.drawVelRange(obj.getZmin()-0.5,obj.getZmax()+0.5);
    }


  }

  void drawSpectralRange(Plot::SimpleSpectralPlot &plot, Detection &obj, FitsHeader &head)
  {
    /// @details

    /// A front-end to drawing the lines delimiting the spectral
    /// extent of the detection. This takes into account the channel
    /// widths, offsetting outwards by half a channel (for instance, a
    /// single-channel detection will not have the separation of one
    /// channel).
    /// If the world coordinate is being plotted, the correct offset
    /// is calcuated by transforming from the central spatial
    /// positions and the offsetted min/max z-pixel extents

    if(head.isWCS()){
      double x=obj.getXcentre(),y=obj.getYcentre(),z;
      z=obj.getZmin()-0.5;
      float vmin=head.pixToVel(x,y,z);
      z=obj.getZmax()+0.5;
      float vmax=head.pixToVel(x,y,z);
      plot.drawVelRange(vmin,vmax);
    }
    else{
      plot.drawVelRange(obj.getZmin()-0.5,obj.getZmax()+0.5);
    }

  }



}

