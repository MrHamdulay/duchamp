#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <stdio.h>
#include <cpgplot.h>
#include <math.h>
#include <wcs.h>
#include <Cubes/cubes.hh>
#include <Cubes/plots.hh>
#include <Utils/utils.hh>

void getSmallVelRange(Detection &obj, wcsprm *wcs, float *minvel, float *maxvel);
void Cube::outputSpectra()
{

  string spectrafile = this->par.getSpectraFile() + "/vcps";
  Plot::SpectralPlot newplot;
  newplot.setUpPlot(spectrafile.c_str());

  long xdim = this->axisDim[0];
  long ydim = this->axisDim[1];
  long zdim = this->axisDim[2];
  int numObjects = this->objectList.size();
  float beam = this->par.getBeamSize();

  float *specx  = new float[zdim];
  float *specy  = new float[zdim];
  for(int i=0;i<zdim;i++) specy[i] = 0.;
  float *specy2 = new float[zdim];
  for(int i=0;i<zdim;i++) specy2[i] = 0.;

  for(int nobj=0;nobj<numObjects;nobj++){
    // for each object in the cube:
    Detection *obj = new Detection;
    *obj = this->objectList[nobj];
    obj->calcParams();

    for(int i=0;i<zdim;i++) specy[i] = 0.;
    if(this->par.getFlagATrous())  
      for(int i=0;i<zdim;i++) specy2[i] = 0.;
    
    double x = double(obj->getXcentre());
    double y = double(obj->getYcentre());
    for(double z=0;z<zdim;z++) specx[int(z)] = pixelToVelocity(this->wcs,x,y,z);

    bool *done = new bool[xdim*ydim]; 
    for(int i=0;i<xdim*ydim;i++) done[i]=false;
    int thisSize = obj->getSize();
    for(int pix=0;pix<thisSize;pix++){
      int pos = obj->getX(pix) + xdim * obj->getY(pix);
      if(!done[pos]){
	done[pos] = true;
	for(int z=0;z<zdim;z++){
	  if(!(this->par.isBlank(this->array[pos+z*xdim*ydim]))){
	    specy[z] += this->array[pos + z*xdim*ydim] / beam;
	    if(this->par.getFlagATrous())
	      specy2[z] += this->recon[pos + z*xdim*ydim] / beam;
	  }
	}
      }
    }
    delete [] done;
    
    float vmax,vmin;
    vmax = vmin = specx[0];
    for(int i=1;i<zdim;i++){
      if(specx[i]>vmax) vmax=specx[i];
      if(specx[i]<vmin)	vmin=specx[i];
    }
    float max,min;
    int loc=0;
    max = min = specy[0];
    for(int i=1;i<zdim;i++){
      if(specy[i]>max) max=specy[i];
      if(specy[i]<min){
	min=specy[i];
	loc = i;
      }
    }

    // now plot the resulting spectrum
    newplot.gotoHeader("Velocity [km s\\u-1\\d]");

    string label = obj->outputLabelWCS();
    newplot.firstHeaderLine(label);
    label = obj->outputLabelInfo();
    newplot.secondHeaderLine(label);
    label = obj->outputLabelPix();
    newplot.thirdHeaderLine(label);

    newplot.gotoMainSpectrum(vmin,vmax,min,max,"Flux [Jy]");
    cpgline(zdim,specx,specy);
    if(this->par.getFlagATrous()){
      cpgsci(2);
      cpgline(zdim,specx,specy2);    
      cpgsci(1);
    }
    newplot.drawVelRange(obj->getVelMin(),obj->getVelMax());

    /**************************/
    // ZOOM IN SPECTRALLY ON THE DETECTION.

    float minvel,maxvel;
    getSmallVelRange(*obj,wcs,&minvel,&maxvel);

    // Find new max & min flux values
    swap(max,min);
    int ct = 0;
    for(int i=0;i<zdim;i++){
      if((specx[i]>=minvel)&&(specx[i]<=maxvel)){
	ct++;
	if(specy[i]>max) max=specy[i];
	if(specy[i]<min) min=specy[i];
      }
    }
    // widen the flux range slightly so that the top & bottom don't lie on the axes.
    float width = max - min;
    max += width * 0.05;
    min -= width * 0.05;

    newplot.gotoZoomSpectrum(minvel,maxvel,min,max);
    cpgline(zdim,specx,specy);
    if(this->par.getFlagATrous()){
      cpgsci(2);
      cpgline(zdim,specx,specy2);    
      cpgsci(1);
    }
    newplot.drawVelRange(obj->getVelMin(),obj->getVelMax());
    
    /**************************/

    // DRAW THE MOMENT MAP OF THE DETECTION -- SUMMED OVER ALL CHANNELS
    newplot.gotoMap();
    drawMomentCutout(*this,*obj);

    delete obj;

  }// end of loop over objects.

  delete [] specx;
  delete [] specy;
  delete [] specy2;
  
}

void getSmallVelRange(Detection &obj, wcsprm *wcs, float *minvel, float *maxvel)
{
  // Want the velocity range for the zoomed in region to be maximum of 
  // 16 pixels or 3x width of detection. Need to:
  //   * Calculate pixel width of a 3x-detection-width region.
  //   * If smaller than 16, calculate velocities of central vel +- 8pixels
  //   * If not, use those velocities

  double *pixcrd = new double[3];
  double *world  = new double[3];
  float minpix,maxpix;
  // define new velocity extrema -- make it 3x wider than the width of the detection.
  *minvel = 0.5*(obj.getVelMin()+obj.getVelMax()) - 1.5*obj.getVelWidth();
  *maxvel = 0.5*(obj.getVelMin()+obj.getVelMax()) + 1.5*obj.getVelWidth();
  // Find velocity range in number of pixels:
  world[0] = obj.getRA();
  world[1] = obj.getDec();
  world[2] = velToCoord(wcs,*minvel);
  wcsToPixSingle(wcs,world,pixcrd);
  minpix = pixcrd[2];
  world[2] = velToCoord(wcs,*maxvel);
  wcsToPixSingle(wcs,world,pixcrd);
  maxpix = pixcrd[2];
  if(maxpix<minpix) swap(maxpix,minpix);
    
  if((maxpix - minpix + 1) < 16){
    pixcrd[0] = double(obj.getXcentre());
    pixcrd[1] = double(obj.getYcentre());
    pixcrd[2] = floor(obj.getZcentre() - 8.);
    pixToWCSSingle(wcs,pixcrd,world);
    *minvel = setVel_kms(wcs,world[2]);
    pixcrd[2] = ceil(obj.getZcentre() + 8.);
    pixToWCSSingle(wcs,pixcrd,world);
    *maxvel = setVel_kms(wcs,world[2]);
    if(*maxvel<*minvel) swap(*maxvel,*minvel);
  }
  delete [] pixcrd;
  delete [] world;

}
