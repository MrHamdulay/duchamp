#include <iostream>
#include <wcs.h>
#include <Utils/utils.hh>

double pixelToVelocity(wcsprm *wcs, double &x, double &y, double &z)
{
  int    *stat   = new int[1];
  double *pixcrd = new double[3];
  double *imgcrd = new double[3];
  double *world  = new double[3];
  double *phi    = new double[1];
  double *theta  = new double[1];
  pixcrd[0] = x;
  pixcrd[1] = y;
  pixcrd[2] = z;
  if(int flag=wcsp2s(wcs, 1, 3, pixcrd, imgcrd, phi, theta, world, stat)>0){
    std::cerr << "WCS Error in pixelToVelocity(): Code = " <<flag<<": "<<wcs_errmsg[flag]<<std::endl;
    std::cerr << "  stat value is " << stat[0] << std::endl;
  }
  string ztype = wcs->ctype[2];
  double vel,nuRest=wcs->restfrq;
  if(ztype!="FREQ") vel = world[2]/1000.;
  else vel = C_kms * (nuRest*nuRest - world[2]*world[2])/(nuRest*nuRest + world[2]*world[2]);
  delete [] stat;
  delete [] pixcrd;
  delete [] imgcrd;
  delete [] world;
  delete [] phi;
  delete [] theta;
  
  return vel;
}
 

int pixToWCSSingle(wcsprm *wcs, const double *pix, double *world)
{
  int nelem=3,npts=1,flag;
  int    *stat   = new int[npts];
  double *imgcrd = new double[nelem*npts];
  double *phi    = new double[npts];
  double *theta  = new double[npts];
  if(flag=wcsp2s(wcs, npts, nelem, pix, imgcrd, phi, theta, world, stat)>0){
    std::cerr << "WCS Error in pixToWCSSingle(): Code = " <<flag<<": "<<wcs_errmsg[flag]<<std::endl;
    std::cerr << "  stat value is " << stat[0] << std::endl;
  }
  delete [] stat;
  delete [] imgcrd;
  delete [] phi;
  delete [] theta;
  return flag;
}

int wcsToPixSingle(wcsprm *wcs, const double *world, double *pix)
{
  int nelem=3,npts=1,flag;
  int    *stat   = new int[npts];
  double *imgcrd = new double[nelem*npts];
  double *phi    = new double[npts];
  double *theta  = new double[npts];
  if(flag=wcss2p(wcs, npts, nelem, world, phi, theta, imgcrd, pix, stat)>0){
    std::cerr << "WCS Error in wcsToPixSingle(): Code = " <<flag<<": "<<wcs_errmsg[flag]<<std::endl;
    std::cerr << "  stat value is " << stat[0] << std::endl;
  }
  delete [] stat;
  delete [] imgcrd;
  delete [] phi;
  delete [] theta;
  return flag;
}

float setVel_kms(wcsprm *wcs, const double coord)
{
  string ztype = wcs->ctype[2];
  if(ztype!="FREQ") {
//     std::cerr<<".";
    return coord/1000.;
  }
  else{
//     std::cerr<<"!";
    return C_kms * (wcs->restfrq*wcs->restfrq - coord*coord) / (wcs->restfrq*wcs->restfrq + coord*coord);
  }
}

double velToCoord(wcsprm *wcs, const float velocity)
{
  string ztype = wcs->ctype[2];
  if(ztype!="FREQ")
    return velocity * 1000.;
  else
    return wcs->restfrq * (1. - velocity/C_kms) / (1. + velocity/C_kms);
  
}
