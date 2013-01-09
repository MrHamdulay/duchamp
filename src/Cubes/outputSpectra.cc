// -----------------------------------------------------------------------
// outputSpectra.cc: Print the spectra of the detected objects.
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
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cpgplot.h>
#include <math.h>
#include <wcslib/wcs.h>
#include <duchamp/param.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/PixelMap/Object3D.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Cubes/cubeUtils.hh>
#include <duchamp/Cubes/plots.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/mycpgplot.hh>

using namespace mycpgplot;
using namespace PixelInfo;

namespace duchamp
{

  std::string getIndivPlotName(std::string baseName, int objNum, int maxNumObj)
  {
    int width = int(floor(log10(float(maxNumObj))))+1;
    if(baseName.substr(baseName.size()-3,baseName.size())==".ps"){
      std::stringstream ss;
      ss << baseName.substr(0,baseName.size()-3) 
	 << "-" << std::setw(width) << std::setfill('0') << objNum
	 << ".ps";
      return ss.str();
    }
    else{
      std::stringstream ss;
      ss << baseName
	 << "-" << std::setw(width) << std::setfill('0') << objNum
	 << ".ps";
      return ss.str();
    }
  }

  void Cube::outputSpectra()
  {
    ///  @details
    /// The way to display individual detected objects. The standard way
    /// is plot the full spectrum, plus a zoomed-in spectrum showing just
    /// the object, plus the 0th-moment map. If there is no spectral
    /// axis, just the 0th moment map is plotted (using
    /// Cube::plotSource() rather than Cube::plotSpectrum()).
    /// 
    /// It makes use of the SpectralPlot or CutoutPlot classes from
    /// plots.h, which size everything correctly.
    /// 
    /// The main choice for SpectralPlot() is whether to use the peak
    /// pixel, in which case the spectrum is just that of the peak pixel,
    /// or the sum, where the spectrum is summed over all spatial pixels
    /// that are in the object.  If a reconstruction has been done, that
    /// spectrum is plotted in red.  The limits of the detection are
    /// marked in blue.  A 0th moment map of the detection is also
    /// plotted, with a scale bar indicating the spatial scale.

    if(this->fullCols.size()==0) this->setupColumns(); 
    // in case cols haven't been set -- need the precisions for printing values.

    std::vector<bool> objectChoice = this->par.getObjectChoices(this->objectList->size());

    std::string spectrafile = this->par.getSpectraFile() + "/vcps";
    if(this->getDimZ()<=1){
      Plot::CutoutPlot newplot;
      if(newplot.setUpPlot(spectrafile.c_str())>0) {

	for(size_t nobj=0;nobj<this->objectList->size();nobj++){
	  // for each object in the cube, assuming it is wanted:
	  if(objectChoice[nobj]) this->plotSource(this->objectList->at(nobj),newplot);
	}// end of loop over objects.
	cpgclos();
	
	if(this->par.getFlagUsePrevious()) std::cout << "\n";
	for(size_t nobj=0;nobj<this->objectList->size();nobj++){
	  if(objectChoice[nobj] && this->par.getFlagUsePrevious()){
	    std::cout << "  Will output individual plot to "
		      << getIndivPlotName(this->par.getSpectraFile(),nobj+1,this->objectList->size()) << "\n";
	    Plot::CutoutPlot indivplot;
	    indivplot.setUpPlot(getIndivPlotName(this->par.getSpectraFile(),nobj+1,this->objectList->size())+"/vcps");
	    this->plotSource(this->objectList->at(nobj),indivplot);
	    cpgclos();
	  }
	}	        
      }
    }
    else{
      Plot::SpectralPlot newplot;
      if(newplot.setUpPlot(spectrafile.c_str())>0) {

	for(size_t nobj=0;nobj<this->objectList->size();nobj++){
	  // for each object in the cube, assuming it is wanted:
	  if(objectChoice[nobj])  this->plotSpectrum(nobj,newplot);
	}// end of loop over objects.
	cpgclos();

	if(this->par.getFlagUsePrevious()) std::cout << "\n";
	for(size_t nobj=0;nobj<this->objectList->size();nobj++){
	  if(objectChoice[nobj] && this->par.getFlagUsePrevious()){
	    std::cout << "  Will output individual plot to "
		      << getIndivPlotName(this->par.getSpectraFile(),nobj+1,this->objectList->size()) << "\n";
	    Plot::SpectralPlot indivplot;
	    indivplot.setUpPlot(getIndivPlotName(this->par.getSpectraFile(),nobj+1,this->objectList->size())+"/vcps");
	    this->plotSpectrum(nobj,indivplot);
	    cpgclos();
	  }
	}	        

      }
      
    }
  }
  //--------------------------------------------------------------------

  void Cube::plotSpectrum(int objNum, Plot::SpectralPlot &plot)
  {
    ///  @details
    /// The way to print out the spectrum of a Detection.
    /// Makes use of the SpectralPlot class in plots.hh, which sizes 
    ///  everything correctly.
    /// 
    /// The main choice for the user is whether to use the peak pixel, in
    /// which case the spectrum is just that of the peak pixel, or the
    /// sum, where the spectrum is summed over all spatial pixels that
    /// are in the object.
    /// 
    /// If a reconstruction has been done, that spectrum is plotted in
    /// red, and if a baseline has been calculated that is also shown, in
    /// yellow.  The spectral limits of the detection are marked in blue.
    /// A 0th moment map of the detection is also plotted, with a scale
    /// bar indicating the spatial size.
    /// 
    /// \param objNum The number of the Detection to be plotted.
    /// \param plot The SpectralPlot object defining the PGPLOT device
    ///        to plot the spectrum on.

    long zdim = this->axisDim[2];

    this->objectList->at(objNum).calcFluxes(this->array, this->axisDim);

    double minMWvel=0,maxMWvel=0,xval,yval,zval;
    xval = double(this->objectList->at(objNum).getXcentre());
    yval = double(this->objectList->at(objNum).getYcentre());
    if(this->par.getFlagMW()){
      zval = double(this->par.getMinMW());
      minMWvel = this->head.pixToVel(xval,yval,zval);
      zval = double(this->par.getMaxMW());
      maxMWvel = this->head.pixToVel(xval,yval,zval);
    }

    float *specx  = new float[zdim];
    float *specy  = new float[zdim];
    float *specy2 = new float[zdim];
    float *base   = new float[zdim];
//     float *specx, *specy, *specy2, *base;

    this->getSpectralArrays(objNum,specx,specy,specy2,base);

    std::string fluxLabel = "Flux";
    std::string fluxUnits = this->head.getFluxUnits();
    std::string intFluxUnits;// = this->head.getIntFluxUnits();
    // Rather than use the intFluxUnits from the header, which will be like Jy MHz, 
    // we just use the pixel units, removing the /beam if necessary.

    if(fluxUnits.size()>5 &&
       makelower(fluxUnits.substr(fluxUnits.size()-5,
				  fluxUnits.size()   )) == "/beam"){
      intFluxUnits = fluxUnits.substr(0,fluxUnits.size()-5);
    }
    else intFluxUnits = fluxUnits;
    

    if(this->par.getSpectralMethod()=="sum"){
      fluxLabel = "Integrated " + fluxLabel;
      if(this->head.isWCS()) {
	fluxLabel += " ["+intFluxUnits+"]";
      }
    }
    else {// if(par.getSpectralMethod()=="peak"){
      fluxLabel = "Peak " + fluxLabel;
      if(this->head.isWCS()) fluxLabel += " ["+fluxUnits+"]";
    }
    
    float vmax,vmin,width;
    vmax = vmin = specx[0];
    for(int i=1;i<zdim;i++){
      if(specx[i]>vmax) vmax=specx[i];
      if(specx[i]<vmin) vmin=specx[i];
    }
  
    float max,min;
    int loc=0;
    if(this->par.getMinMW()>0) max = min = specy[0];
    else max = min = specy[this->par.getMaxMW()+1];
    for(int i=0;i<zdim;i++){
      if(!this->par.isInMW(i)){
	if(specy[i]>max) max=specy[i];
	if(specy[i]<min){
	  min=specy[i];
	  loc = i;
	}
      }
    }
    // widen the ranges slightly so that the top & bottom & edges don't 
    // lie on the axes.
    width = max - min;
    max += width * 0.05;
    min -= width * 0.05;
    width = vmax - vmin;
    vmax += width * 0.01;
    vmin -= width * 0.01;

    // now plot the resulting spectrum
    std::string label;
    if(this->head.isWCS()){
      label = this->head.getSpectralDescription() + " [" + 
	this->head.getSpectralUnits() + "]";
      plot.gotoHeader(label);
    }
    else plot.gotoHeader("Spectral pixel value");

    if(this->head.isWCS()){
      label = this->objectList->at(objNum).outputLabelWCS();
      plot.firstHeaderLine(label);
      label = this->objectList->at(objNum).outputLabelFluxes();
      plot.secondHeaderLine(label);
    }
    label = this->objectList->at(objNum).outputLabelWidths(this->head);
    plot.thirdHeaderLine(label);
    label = this->objectList->at(objNum).outputLabelPix();
    plot.fourthHeaderLine(label);
    
    plot.gotoMainSpectrum(vmin,vmax,min,max,fluxLabel);
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
    if(this->par.getFlagMW()) plot.drawMWRange(minMWvel,maxMWvel);
    drawSpectralRange(plot,this->objectList->at(objNum),this->head);

    if(this->par.getSpectralMethod()=="peak")
      plot.drawThresholds(this->par,this->Stats);

    /**************************/
    // ZOOM IN SPECTRALLY ON THE DETECTION.

    double minvel,maxvel;
    if(this->head.isWCS()) getSmallVelRange(this->objectList->at(objNum),this->head,&minvel,&maxvel);
    else getSmallZRange(this->objectList->at(objNum),&minvel,&maxvel);

    // Find new max & min flux values
    std::swap(max,min);
    int ct = 0;
    for(int i=0;i<zdim;i++){
      if((!this->par.isInMW(i))&&(specx[i]>=minvel)&&(specx[i]<=maxvel)){
	ct++;
	if(specy[i]>max) max=specy[i];
	if(specy[i]<min) min=specy[i];
	if(this->par.getFlagBaseline()){
	  if(base[i]>max) max=base[i];
	  if(base[i]<min) min=base[i];
	}
	if(this->reconExists){
	  if(specy2[i]>max) max=specy2[i];
	  if(specy2[i]<min) min=specy2[i];
	}
      }
    }
    // widen the flux range slightly so that the top & bottom don't lie
    // on the axes.
    width = max - min;
    max += width * 0.05;
    min -= width * 0.05;

    plot.gotoZoomSpectrum(minvel,maxvel,min,max);
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
    if(this->par.getFlagMW()) plot.drawMWRange(minMWvel,maxMWvel);
    drawSpectralRange(plot,this->objectList->at(objNum),this->head);
    
    if(this->par.getSpectralMethod()=="peak")
      plot.drawThresholds(this->par,this->Stats);

    /**************************/

    // DRAW THE MOMENT MAP OF THE DETECTION -- SUMMED OVER ALL CHANNELS
    if(this->numNondegDim>1){
      plot.gotoMap();
      this->drawMomentCutout(this->objectList->at(objNum));
    }

    delete [] specx;
    delete [] specy;
    delete [] specy2;
    delete [] base;
  
  }
  //--------------------------------------------------------------------


  void Cube::plotSource(Detection obj, Plot::CutoutPlot &plot)
  {
    ///  @details
    /// The way to print out the 2d image cutout of a Detection.
    /// Makes use of the CutoutPlot class in plots.hh, which sizes 
    ///  everything correctly.
    /// 
    /// A 0th moment map of the detection is plotted, with a scale
    /// bar indicating the spatial size.
    /// 
    /// Basic information on the source is printed next to it as well. 
    /// 
    /// \param obj The Detection to be plotted.
    /// \param plot The PGPLOT device to plot the spectrum on.

    obj.calcFluxes(this->array, this->axisDim);

    std::string label;
    plot.gotoHeader();

    if(this->head.isWCS()){
      label = obj.outputLabelWCS();
      plot.firstHeaderLine(label);
      label = obj.outputLabelFluxes();
      plot.secondHeaderLine(label);
    }
    label = obj.outputLabelWidths(this->head);
    plot.thirdHeaderLine(label);
    label = obj.outputLabelPix();
    plot.fourthHeaderLine(label);
    
    // DRAW THE MOMENT MAP OF THE DETECTION -- SUMMED OVER ALL CHANNELS
    plot.gotoMap();
    this->drawMomentCutout(obj);

  }

}
