#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cpgplot.h>
#include <math.h>
#include <wcs.h>
#include <Cubes/cubes.hh>
#include <Cubes/plots.hh>
#include <Utils/utils.hh>

void getSmallVelRange(Detection &obj, FitsHeader head, float *minvel, float *maxvel);
void getSmallZRange(Detection &obj, float *minz, float *maxz);

void Cube::outputSpectra()
{
  /** 
   *   Cube::outputSpectra()
   *
   *    The way to print out the spectra of the detected objects.
   *    Make use of the SpectralPlot class in plots.h, which sizes everything correctly.
   *    Main choice is whether to use the peak pixel, in which case the spectrum is just
   *     that of the peak pixel, or the sum, where the spectrum is summed over all spatial
   *     pixels that are in the object.
   *    If a reconstruction has been done, that spectrum is plotted in red.
   *    The limits of the detection are marked in blue.
   *    A 0th moment map of the detection is also plotted, with a scale bar indicating the 
   *     spatial size.
   */

  string spectrafile = this->par.getSpectraFile() + "/vcps";
  Plot::SpectralPlot newplot;
  newplot.setUpPlot(spectrafile.c_str());

  for(int nobj=0;nobj<this->objectList.size();nobj++){
    // for each object in the cube:
    this->plotSpectrum(this->objectList[nobj],newplot);

  }// end of loop over objects.

  cpgclos();

}

void Cube::plotSpectrum(Detection obj, Plot::SpectralPlot &plot)
{
  /** 
   *   Cube::plotSpectrum(obj)
   *
   *    The way to print out the spectrum of a Detection.
   *    Make use of the SpectralPlot class in plots.hh, which sizes everything correctly.
   *    Main choice is whether to use the peak pixel, in which case the spectrum is just
   *     that of the peak pixel, or the sum, where the spectrum is summed over all spatial
   *     pixels that are in the object.
   *    If a reconstruction has been done, that spectrum is plotted in red.
   *    The limits of the detection are marked in blue.
   *    A 0th moment map of the detection is also plotted, with a scale bar indicating the 
   *     spatial size.
   */

  long xdim = this->axisDim[0];
  long ydim = this->axisDim[1];
  long zdim = this->axisDim[2];
  float beam = this->par.getBeamSize();

  obj.calcParams();

  double minMWvel,maxMWvel,xval,yval,zval;
  xval = double(obj.getXcentre());
  yval = double(obj.getYcentre());
  if(this->par.getFlagMW()){
    zval = double(this->par.getMinMW());
    minMWvel = this->head.pixToVel(xval,yval,zval);
    zval = double(this->par.getMaxMW());
    maxMWvel = this->head.pixToVel(xval,yval,zval);
  }

  float *specx  = new float[zdim];
  float *specy  = new float[zdim];
  for(int i=0;i<zdim;i++) specy[i] = 0.;
  float *specy2 = new float[zdim];
  for(int i=0;i<zdim;i++) specy2[i] = 0.;

  for(int i=0;i<zdim;i++) specy[i] = 0.;
  if(this->par.getFlagATrous())  
    for(int i=0;i<zdim;i++) specy2[i] = 0.;
    
  if(this->head.isWCS())
    for(zval=0;zval<zdim;zval++) specx[int(zval)] = this->head.pixToVel(xval,yval,zval);
  else 
    for(zval=0;zval<zdim;zval++) specx[int(zval)] = zval;

  string fluxLabel = "Flux";

  if(this->par.getSpectralMethod()=="sum"){
    if(this->head.isWCS()) fluxLabel += " [Jy]";
    bool *done = new bool[xdim*ydim]; 
    for(int i=0;i<xdim*ydim;i++) done[i]=false;
    int thisSize = obj.getSize();
    for(int pix=0;pix<thisSize;pix++){
      int pos = obj.getX(pix) + xdim * obj.getY(pix);
      if(!done[pos]){
	done[pos] = true;
	for(int z=0;z<zdim;z++){
	  if(!(this->isBlank(pos+z*xdim*ydim))){
	    specy[z] += this->array[pos + z*xdim*ydim] / beam;
	    if(this->par.getFlagATrous())
	      specy2[z] += this->recon[pos + z*xdim*ydim] / beam;
	  }
	}
      }
    }
    delete [] done;
  }
  else {// if(par.getSpectralMethod()=="peak"){
    if(this->head.isWCS()) fluxLabel += " [Jy/beam]";
    for(int z=0;z<zdim;z++){
      int pos = obj.getXPeak() + xdim*obj.getYPeak();
      specy[z] = this->array[pos + z*xdim*ydim];
      if(this->par.getFlagATrous()) specy2[z] = this->recon[pos + z*xdim*ydim];
    }
  }
    
  float vmax,vmin;
  vmax = vmin = specx[0];
  for(int i=1;i<zdim;i++){
    if(specx[i]>vmax) vmax=specx[i];
    if(specx[i]<vmin) vmin=specx[i];
  }
  float max,min;
  int loc=0;
  if(this->par.getMinMW()>0) max = min = specy[0];
  else max = min = specx[this->par.getMaxMW()+1];
  for(int i=0;i<zdim;i++){
    if(!this->par.isInMW(i)){
      if(specy[i]>max) max=specy[i];
      if(specy[i]<min){
	min=specy[i];
	loc = i;
      }
    }
  }
  // widen the flux range slightly so that the top & bottom don't lie on the axes.
  float width = max - min;
  max += width * 0.05;
  min -= width * 0.05;

  // now plot the resulting spectrum
  string label;
  if(this->head.isWCS()){
    label = "Velocity [" + this->head.getSpectralUnits() + "]";
    plot.gotoHeader(label);
  }
  else plot.gotoHeader("Spectral pixel value");

  if(this->head.isWCS()){
    label = obj.outputLabelWCS();
    plot.firstHeaderLine(label);
  }
  label = obj.outputLabelInfo();
  plot.secondHeaderLine(label);
  label = obj.outputLabelPix();
  plot.thirdHeaderLine(label);
    
  plot.gotoMainSpectrum(vmin,vmax,min,max,fluxLabel);
  cpgline(zdim,specx,specy);
  if(this->par.getFlagATrous()){
    cpgsci(2);
    cpgline(zdim,specx,specy2);    
    cpgsci(1);
  }
  if(this->par.getFlagMW()) plot.drawMWRange(minMWvel,maxMWvel);
  if(this->head.isWCS()) plot.drawVelRange(obj.getVelMin(),obj.getVelMax());
  else plot.drawVelRange(obj.getZmin(),obj.getZmax());

  /**************************/
  // ZOOM IN SPECTRALLY ON THE DETECTION.

  float minvel,maxvel;
  if(this->head.isWCS()) getSmallVelRange(obj,this->head,&minvel,&maxvel);
  else getSmallZRange(obj,&minvel,&maxvel);

  // Find new max & min flux values
  swap(max,min);
  int ct = 0;
  for(int i=0;i<zdim;i++){
    if((!this->par.isInMW(i))&&(specx[i]>=minvel)&&(specx[i]<=maxvel)){
      ct++;
      if(specy[i]>max) max=specy[i];
      if(specy[i]<min) min=specy[i];
    }
  }
  // widen the flux range slightly so that the top & bottom don't lie on the axes.
  width = max - min;
  max += width * 0.05;
  min -= width * 0.05;

  plot.gotoZoomSpectrum(minvel,maxvel,min,max);
  cpgline(zdim,specx,specy);
  if(this->par.getFlagATrous()){
    cpgsci(2);
    cpgline(zdim,specx,specy2);    
    cpgsci(1);
  }
  if(this->par.getFlagMW()) plot.drawMWRange(minMWvel,maxMWvel);
  if(this->head.isWCS()) plot.drawVelRange(obj.getVelMin(),obj.getVelMax());
  else plot.drawVelRange(obj.getZmin(),obj.getZmax());
    
  /**************************/

  // DRAW THE MOMENT MAP OF THE DETECTION -- SUMMED OVER ALL CHANNELS
  plot.gotoMap();
  this->drawMomentCutout(obj);

  delete [] specx;
  delete [] specy;
  delete [] specy2;
  
}


void getSmallVelRange(Detection &obj, FitsHeader head, float *minvel, float *maxvel)
{
  /** 
   * getSmallVelRange(obj,wcs,minvel,maxvel)
   *  Routine to calculate the velocity range for the zoomed-in region.
   *  This range should be the maximum of 20 pixels, or 3x the wdith of the detection.
   *  Need to :
   *      Calculate pixel width of a 3x-detection-width region.
   *      If smaller than 20, calculate velocities of central vel +- 10 pixels
   *      If not, use the 3x-detection-width
   *  Range returned via "minvel" and "maxvel" parameters.
   */

  double *pixcrd = new double[3];
  double *world  = new double[3];
  float minpix,maxpix;
  // define new velocity extrema -- make it 3x wider than the width of the detection.
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
  if(maxpix<minpix) swap(maxpix,minpix);
    
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
    if(*maxvel<*minvel) swap(*maxvel,*minvel);
  }
  delete [] pixcrd;
  delete [] world;

}

void getSmallZRange(Detection &obj, float *minz, float *maxz)
{
  /** 
   * getSmallZRange(obj,minz,maxz)
   *  Routine to calculate the pixel range for the zoomed-in spectrum.
   *  This range should be the maximum of 20 pixels, or 3x the wdith of the detection.
   *  Need to :
   *      Calculate pixel width of a 3x-detection-width region.
   *       If smaller than 20, use central pixel +- 10 pixels
   *  Range returned via "minz" and "maxz" parameters.
   */

  *minz = 2.*obj.getZmin() - obj.getZmax();
  *maxz = 2.*obj.getZmax() - obj.getZmin();
    
  if((*maxz - *minz + 1) < 20){
    *minz = obj.getZcentre() - 10.;
    *maxz = obj.getZcentre() + 10.;
  }

}
