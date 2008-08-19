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
#include <duchamp/Cubes/plots.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/mycpgplot.hh>

using namespace mycpgplot;
using namespace PixelInfo;

namespace duchamp
{

  void getSmallVelRange(Detection &obj, FitsHeader head, float *minvel, float *maxvel);
  void getSmallZRange(Detection &obj, float *minz, float *maxz);

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
    /** 
     * The way to display individual detected objects. The standard way
     * is plot the full spectrum, plus a zoomed-in spectrum showing just
     * the object, plus the 0th-moment map. If there is no spectral
     * axis, just the 0th moment map is plotted (using
     * Cube::plotSource() rather than Cube::plotSpectrum()).
     *
     * It makes use of the SpectralPlot or CutoutPlot classes from
     * plots.h, which size everything correctly.
     *
     * The main choice for SpectralPlot() is whether to use the peak
     * pixel, in which case the spectrum is just that of the peak pixel,
     * or the sum, where the spectrum is summed over all spatial pixels
     * that are in the object.  If a reconstruction has been done, that
     * spectrum is plotted in red.  The limits of the detection are
     * marked in blue.  A 0th moment map of the detection is also
     * plotted, with a scale bar indicating the spatial scale.
     */

    if(this->fullCols.size()==0) this->setupColumns(); 
    // in case cols haven't been set -- need the precisions for printing values.

    std::vector<bool> objectChoice = this->par.getObjectChoices(this->objectList->size());

    std::string spectrafile = this->par.getSpectraFile() + "/vcps";
    if(this->getDimZ()<=1){
      Plot::CutoutPlot newplot;
      if(newplot.setUpPlot(spectrafile.c_str())>0) {

	for(int nobj=0;nobj<this->objectList->size();nobj++){
	  // for each object in the cube, assuming it is wanted:
	  if(objectChoice[nobj]) this->plotSource(this->objectList->at(nobj),newplot);
	}// end of loop over objects.
	cpgclos();

	for(int nobj=0;nobj<this->objectList->size();nobj++){
	  if(objectChoice[nobj] && this->par.getFlagUsePrevious()){
	    std::cerr << "Will output individual plot to "
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

	for(int nobj=0;nobj<this->objectList->size();nobj++){
	  // for each object in the cube, assuming it is wanted:
	  if(objectChoice[nobj])  this->plotSpectrum(nobj,newplot);
	}// end of loop over objects.
	cpgclos();

	for(int nobj=0;nobj<this->objectList->size();nobj++){
	  if(objectChoice[nobj] && this->par.getFlagUsePrevious()){
	    std::cerr << "Will output individual plot to "
		      << getIndivPlotName(this->par.getSpectraFile(),nobj+1,this->objectList->size()) << "\n";
	    Plot::SpectralPlot indivplot;
	    indivplot.setUpPlot(getIndivPlotName(this->par.getSpectraFile(),nobj+1,this->objectList->size())+"/vcps");
	    this->plotSpectrum(nobj,indivplot);
	    cpgclos();
	  }
	}	        

      }
      
      if(this->par.getFlagTextSpectra()){
	if(this->par.isVerbose()) std::cout << "Saving spectra in text file ... ";
	this->writeSpectralData();
	if(this->par.isVerbose()) std::cout << "Done. ";
      }
    }
  }
  //--------------------------------------------------------------------

  void Cube::writeSpectralData()
  {
    /**
     *  A function to write, in ascii form, the spectra of each
     *  detected object to a file. The file consists of a column for
     *  the spectral coordinates, and one column for each object
     *  showing the flux at that spectral position. The units are the
     *  same as those shown in the graphical output. The filename is
     *  given by the Param::spectraTextFile parameter in the Cube::par
     *  parameter set.
     */

    const int zdim = this->axisDim[2];
    const int numObj = this->objectList->size();
    float *specxOut = new float[zdim];
    float *spectra = new float[numObj*zdim];
    
    for(int obj=0; obj<numObj; obj++){
      float *temp = new float[zdim];
      float *specx = new float[zdim];
      float *recon = new float[zdim];
      float *base = new float[zdim];
      this->getSpectralArrays(obj, specx, temp, recon, base);
      for(int z=0;z<zdim;z++) spectra[obj*zdim+z] = temp[z];
      if(obj==0) for(int z=0;z<zdim;z++) specxOut[z] = specx[z];
      delete [] specx;
      delete [] recon;
      delete [] base;
      delete [] temp;
    }
    
    std::ofstream fspec(this->par.getSpectraTextFile().c_str());
    fspec.setf(std::ios::fixed);

    for(int z=0;z<zdim;z++){
      
      fspec << std::setprecision(8);
      fspec << specxOut[z] << "  ";
      for(int obj=0;obj<numObj; obj++) {
	fspec << spectra[obj*zdim+z] << "  ";
      }
      fspec << "\n";

    }
    fspec.close();

    delete [] spectra;
    delete [] specxOut;

  }
  //--------------------------------------------------------------------

  void Cube::plotSpectrum(int objNum, Plot::SpectralPlot &plot)
  {
    /** 
     * The way to print out the spectrum of a Detection.
     * Makes use of the SpectralPlot class in plots.hh, which sizes 
     *  everything correctly.
     *
     * The main choice for the user is whether to use the peak pixel, in
     * which case the spectrum is just that of the peak pixel, or the
     * sum, where the spectrum is summed over all spatial pixels that
     * are in the object.
     *
     * If a reconstruction has been done, that spectrum is plotted in
     * red, and if a baseline has been calculated that is also shown, in
     * yellow.  The spectral limits of the detection are marked in blue.
     * A 0th moment map of the detection is also plotted, with a scale
     * bar indicating the spatial size.
     *
     * \param objNum The number of the Detection to be plotted.
     * \param plot The SpectralPlot object defining the PGPLOT device
     *        to plot the spectrum on.
     */

    long zdim = this->axisDim[2];

    this->objectList->at(objNum).calcFluxes(this->array, this->axisDim);

    double minMWvel,maxMWvel,xval,yval,zval;
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
    if(makelower(fluxUnits.substr(fluxUnits.size()-5,
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
    label = this->objectList->at(objNum).outputLabelWidths();
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
    if(this->head.isWCS()) plot.drawVelRange(this->objectList->at(objNum).getVelMin(),this->objectList->at(objNum).getVelMax());
    else plot.drawVelRange(this->objectList->at(objNum).getZmin(),this->objectList->at(objNum).getZmax());

    /**************************/
    // ZOOM IN SPECTRALLY ON THE DETECTION.

    float minvel,maxvel;
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
    if(this->head.isWCS()) plot.drawVelRange(this->objectList->at(objNum).getVelMin(),
					     this->objectList->at(objNum).getVelMax());
    else plot.drawVelRange(this->objectList->at(objNum).getZmin(),this->objectList->at(objNum).getZmax());
    
    /**************************/

    // DRAW THE MOMENT MAP OF THE DETECTION -- SUMMED OVER ALL CHANNELS
    plot.gotoMap();
    this->drawMomentCutout(this->objectList->at(objNum));

    delete [] specx;
    delete [] specy;
    delete [] specy2;
    delete [] base;
  
  }
  //--------------------------------------------------------------------

  void getSmallVelRange(Detection &obj, FitsHeader head, 
			float *minvel, float *maxvel)
  {
    /** 
     *  Routine to calculate the velocity range for the zoomed-in region.
     *  This range should be the maximum of 20 pixels, or 3x the wdith of 
     *   the detection.
     *  Need to :
     *      Calculate pixel width of a 3x-detection-width region.
     *      If smaller than 20, calculate velocities of central vel +- 10 pixels
     *      If not, use the 3x-detection-width
     *  Range returned via "minvel" and "maxvel" parameters.
     *  \param obj Detection under examination.
     *  \param head FitsHeader, containing the WCS information.
     *  \param minvel Returned value of minimum velocity
     *  \param maxvel Returned value of maximum velocity
     */

    double *pixcrd = new double[3];
    double *world  = new double[3];
    float minpix,maxpix;
    // define new velocity extrema 
    //    -- make it 3x wider than the width of the detection.
    *minvel = 0.5*(obj.getVelMin()+obj.getVelMax()) - 1.5*obj.getVelWidth();
    *maxvel = 0.5*(obj.getVelMin()+obj.getVelMax()) + 1.5*obj.getVelWidth();
    // Find velocity range in number of pixels:
    world[0] = obj.getRA();
    world[1] = obj.getDec();
    world[2] = head.velToSpec(*minvel);
    head.wcsToPix(world,pixcrd);
    minpix = pixcrd[2];
    world[2] = head.velToSpec(*maxvel);
    head.wcsToPix(world,pixcrd);
    maxpix = pixcrd[2];
    if(maxpix<minpix) std::swap(maxpix,minpix);
    
    if((maxpix - minpix + 1) < 20){
      pixcrd[0] = double(obj.getXcentre());
      pixcrd[1] = double(obj.getYcentre());
      pixcrd[2] = obj.getZcentre() - 10.;
      head.pixToWCS(pixcrd,world);
      //    *minvel = setVel_kms(wcs,world[2]);
      *minvel = head.specToVel(world[2]);
      pixcrd[2] = obj.getZcentre() + 10.;
      head.pixToWCS(pixcrd,world);
      //     *maxvel = setVel_kms(wcs,world[2]);
      *maxvel = head.specToVel(world[2]);
      if(*maxvel<*minvel) std::swap(*maxvel,*minvel);
    }
    delete [] pixcrd;
    delete [] world;

  }
  //--------------------------------------------------------------------

  void getSmallZRange(Detection &obj, float *minz, float *maxz)
  {
    /** 
     *  Routine to calculate the pixel range for the zoomed-in spectrum.
     *  This range should be the maximum of 20 pixels, or 3x the width 
     *   of the detection.
     *  Need to :
     *      Calculate pixel width of a 3x-detection-width region.
     *       If smaller than 20, use central pixel +- 10 pixels
     *  Range returned via "minz" and "maxz" parameters.
     *  \param obj Detection under examination.
     *  \param minz Returned value of minimum z-pixel coordinate
     *  \param maxz Returned value of maximum z-pixel coordinate
     */

    *minz = 2.*obj.getZmin() - obj.getZmax();
    *maxz = 2.*obj.getZmax() - obj.getZmin();
    
    if((*maxz - *minz + 1) < 20){
      *minz = obj.getZcentre() - 10.;
      *maxz = obj.getZcentre() + 10.;
    }

  }
  //--------------------------------------------------------------------

  void Cube::plotSource(Detection obj, Plot::CutoutPlot &plot)
  {
    /** 
     * The way to print out the 2d image cutout of a Detection.
     * Makes use of the CutoutPlot class in plots.hh, which sizes 
     *  everything correctly.
     *
     * A 0th moment map of the detection is plotted, with a scale
     * bar indicating the spatial size.
     *
     * Basic information on the source is printed next to it as well. 
     *
     * \param obj The Detection to be plotted.
     * \param plot The PGPLOT device to plot the spectrum on.
     */

    obj.calcFluxes(this->array, this->axisDim);

    std::string label;
    plot.gotoHeader();

    if(this->head.isWCS()){
      label = obj.outputLabelWCS();
      plot.firstHeaderLine(label);
      label = obj.outputLabelFluxes();
      plot.secondHeaderLine(label);
    }
    label = obj.outputLabelWidths();
    plot.thirdHeaderLine(label);
    label = obj.outputLabelPix();
    plot.fourthHeaderLine(label);
    
    // DRAW THE MOMENT MAP OF THE DETECTION -- SUMMED OVER ALL CHANNELS
    plot.gotoMap();
    this->drawMomentCutout(obj);

  }

}
