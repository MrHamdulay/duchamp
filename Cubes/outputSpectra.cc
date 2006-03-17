#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <stdio.h>
#include <cpgplot.h>
#include <math.h>
#include <wcs.h>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>

void getSmallVelRange(Detection &obj, wcsprm *wcs, float *minvel, float *maxvel);
void Cube::outputSpectra()
{

  string spectrafile = this->par.getSpectraFile() + "/vcps";
  cpgopen(spectrafile.c_str());
  cpgsubp(1,5);

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
    cpgsch(3);
    cpgpage();
    cpgvstd();

    string label = obj->outputLabelWCS();
    cpgmtxt("t",3.,0.5,0.5,label.c_str());
    label = obj->outputLabelInfo();
    cpgmtxt("t",1.8,0.5,0.5,label.c_str());
    label = obj->outputLabelPix();
    cpgmtxt("t",0.6,0.5,0.5,label.c_str());
    cpgmtxt("b",2.2,0.5,0.5,"Velocity [km s\\u-1\\d]");

    float x1,x2,y1,y2;
    cpgqvp(0,&x1,&x2,&y1,&y2);
    cpgsvp(x1,x1+0.75*(x2-x1),y1,y2);
    cpgsch(2.);
    cpgswin(vmin,vmax,min,max);
    cpgbox("1bcnst",0.,0,"bcnst1v",0.,0);
    cpgmtxt("l",3.5,0.5,0.5,"Flux [Jy]");
    cpgline(zdim,specx,specy);
    if(this->par.getFlagATrous()){
      cpgsci(2);
      cpgline(zdim,specx,specy2);    
    }

    cpgsci(3);
    cpgsls(2);
    cpgmove(obj->getVelMin(),min);  cpgdraw(obj->getVelMin(),max);
    cpgmove(obj->getVelMax(),min);  cpgdraw(obj->getVelMax(),max);
    cpgsci(1);
    cpgsls(1);

    /**************************/
    // ZOOM IN SPECTRALLY ON THE DETECTION.

    cpgsvp(x1+0.78*(x2-x1),x1+0.88*(x2-x1),y1,y2);

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

    cpgswin(minvel,maxvel,min,max);
    cpgbox("bc",0.,0,"bcstn",0.,0);
    float length,disp;
    std::stringstream labelstream;
    for(int i=1;i<10;i++){
      labelstream.str("");
      switch(i){
      case 2:
	length = 0.6;
	labelstream<<minvel+(maxvel-minvel)*float(i)/10.;
	disp = 0.2;
	break;
      case 8:
	length = 0.6;
	labelstream<<minvel+(maxvel-minvel)*float(i)/10.;
	disp = 1.2;
	break;
      default:
	length = 0.3;
	break;
      }
      cpgtick(minvel,min,maxvel,min,float(i)/10.,length,0.3,disp,0.,labelstream.str().c_str());
      cpgtick(minvel,max,maxvel,max,float(i)/10.,0.,length,0.,0.,"");
    }
    cpgline(zdim,specx,specy);
    if(this->par.getFlagATrous()){
      cpgsci(2);
      cpgline(zdim,specx,specy2);    
    }
    cpgsci(3);
    cpgsls(2);
    cpgmove(obj->getVelMin(),min);  cpgdraw(obj->getVelMin(),max);
    cpgmove(obj->getVelMax(),min);  cpgdraw(obj->getVelMax(),max);
    cpgsci(1);
    cpgsls(1);
    
    /**************************/

    // DRAW THE MOMENT MAP OF THE DETECTION -- SUMMED OVER ALL CHANNELS
//     cpgsvp(0,0.15*(x2-x1),y1,y2);
    float p1,p2,p3,p4;
    cpgqvsz(1,&p1,&p2,&p3,&p4);
    cpgsvp(x1+0.9*(x2-x1),x1+0.9*(x2-x1)+(y2-y1)*p4/p2,y1,y2);
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
