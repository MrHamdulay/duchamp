#include <iostream>
#include <wcs.h>
#include <Utils/utils.hh>

double pixelToVelocity(wcsprm *wcs, double &x, double &y, double &z)
{
  /** 
   *  pixelToVelocity(wcsprm *wcs, double &x, double &y, double &z)
   *   Uses wcs to convert the three-dimensional pixel position (x,y,z)
   *   to world coordinates.
   *   Returns the velocity.
   */

  int    *stat   = new int[1];
  double *pixcrd = new double[3];
  double *imgcrd = new double[3];
  double *world  = new double[3];
  double *phi    = new double[1];
  double *theta  = new double[1];
  // correct from 0-indexed to 1-indexed pixel array by adding 1 to pixel values.
  pixcrd[0] = x + 1;
  pixcrd[1] = y + 1;
  pixcrd[2] = z + 1;
  if(int flag=wcsp2s(wcs, 1, 3, pixcrd, imgcrd, phi, theta, world, stat)>0){
    std::cerr<<"WCS Error in pixelToVelocity(): Code = "<<flag<<": "<<wcs_errmsg[flag]<<std::endl;
    std::cerr<<"  stat value is "<<stat[0]<<std::endl;
  }

  double vel = setVel_kms(wcs, world[2]);

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
  /** 
   *  pixToWCSSingle(wcsprm *wcs, const double *pix, double *world)
   *   Uses wcs to convert the three-dimensional pixel position referenced by pix
   *   to world coordinates, which are placed in the array world[].
   *   Assumes that pix only has one point with an x,y,and z pixel positions.
   *   Offsets these pixel values by 1 to account for the C arrays being indexed to 0.
   */

  int nelem=3,npts=1,flag;

  double *newpix = new double[nelem*npts];
  for(int i=0;i<nelem*npts;i++) newpix[i] = pix[i] + 1.;  
  // correct from 0-indexed to 1-indexed pixel array

  int    *stat   = new int[npts];
  double *imgcrd = new double[nelem*npts];
  double *phi    = new double[npts];
  double *theta  = new double[npts];
  if(flag=wcsp2s(wcs, npts, nelem, newpix, imgcrd, phi, theta, world, stat)>0){
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
  /** 
   *  wcsToPixSingle(wcsprm *wcs, const double *world, double *pix)
   *   Uses wcs to convert the three-dimensional world coordinate position referenced by world
   *   to pixel coordinates, which are placed in the array pix[].
   *   Assumes that world only has one point (three values eg. RA, Dec, Velocity)
   *   Offsets the pixel values by 1 to account for the C arrays being indexed to 0.
   */

  int nelem=3,npts=1,flag;
  int    *stat   = new int[npts];
  double *imgcrd = new double[nelem*npts];
  double *phi    = new double[npts];
  double *theta  = new double[npts];
  if(flag=wcss2p(wcs, npts, nelem, world, phi, theta, imgcrd, pix, stat)>0){
    std::cerr << "WCS Error in wcsToPixSingle(): Code = " <<flag<<": "<<wcs_errmsg[flag]<<std::endl;
    std::cerr << "  stat value is " << stat[0] << std::endl;
  }

  for(int i=0;i<nelem;i++) pix[i] -= 1.;  // correct from 1-indexed to 0-indexed pixel array

  delete [] stat;
  delete [] imgcrd;
  delete [] phi;
  delete [] theta;
  return flag;
}

int pixToWCSMulti(wcsprm *wcs, const double *pix, double *world, const int npts)
{
  /** 
   *  pixToWCSSingle(wcsprm *wcs, const double *pix, double *world)
   *   Uses wcs to convert the three-dimensional pixel positions referenced by pix
   *   to world coordinates, which are placed in the array world[].
   *   pix is assumed to hold the positions of npts points.
   *   Offsets these pixel values by 1 to account for the C arrays being indexed to 0.
   */

  int nelem=3,flag;

  double *newpix = new double[nelem*npts];
  for(int i=0;i<nelem*npts;i++) newpix[i] = pix[i] + 1.;  
  // correct from 0-indexed to 1-indexed pixel array

  int    *stat   = new int[npts];
  double *imgcrd = new double[nelem*npts];
  double *phi    = new double[npts];
  double *theta  = new double[npts];
  if(flag=wcsp2s(wcs, npts, nelem, newpix, imgcrd, phi, theta, world, stat)>0){
    std::cerr << "WCS Error in pixToWCSSingle(): Code = " <<flag<<": "<<wcs_errmsg[flag]<<std::endl;
    std::cerr << "  stat value is " << stat[0] << std::endl;
  }
  delete [] stat;
  delete [] imgcrd;
  delete [] phi;
  delete [] theta;
  delete [] newpix;
  return flag;
}

int wcsToPixMulti(wcsprm *wcs, const double *world, double *pix, const int npts)
{
  /** 
   *  wcsToPixSingle(wcsprm *wcs, const double *world, double *pix)
   *   Uses wcs to convert the three-dimensional world coordinate position referenced by world
   *   to pixel coordinates, which are placed in the array pix[].
   *   world is assumed to hold the positions of npts points.
   *   Offsets the pixel values by 1 to account for the C arrays being indexed to 0.
   */
  int nelem=3,flag;
  int    *stat   = new int[npts];
  double *imgcrd = new double[nelem*npts];
  double *phi    = new double[npts];
  double *theta  = new double[npts];
  if(flag=wcss2p(wcs, npts, nelem, world, phi, theta, imgcrd, pix, stat)>0){
    std::cerr << "WCS Error in wcsToPixSingle(): Code = " <<flag<<": "<<wcs_errmsg[flag]<<std::endl;
    std::cerr << "  stat value is " << stat[0] << std::endl;
  }

  for(int i=0;i<nelem;i++) pix[i] -= 1.;  // correct from 1-indexed to 0-indexed pixel array

  delete [] stat;
  delete [] imgcrd;
  delete [] phi;
  delete [] theta;
  return flag;
}

float setVel_kms(wcsprm *wcs, const double coord)
{
  /** 
   *  setVel_kms(wcsprm *wcs, const double coord)
   *   Convert the wcs coordinate given by coord to a velocity in km/s.
   *   Does this by checking the ztype parameter in wcs to see if it is FREQ or otherwise,
   *   and converts accordingly.
   */

  string ztype = wcs->ctype[2];
  if(ztype!="FREQ") {
    return coord/1000.;
  }
  else{
    return C_kms * (wcs->restfrq*wcs->restfrq - coord*coord) / (wcs->restfrq*wcs->restfrq + coord*coord);
  }
}

double velToCoord(wcsprm *wcs, const float velocity)
{
  /** 
   *  velToCoord(wcsprm *wcs, const double coord)
   *   Convert the velocity given to the appropriate world coordinate for wcs.
   *   Does this by checking the ztype parameter in wcs to see if it is FREQ or otherwise,
   *   and converts accordingly.
   */

  string ztype = wcs->ctype[2];
  if(ztype!="FREQ")
    return velocity * 1000.;
  else
    return wcs->restfrq * (1. - velocity/C_kms) / (1. + velocity/C_kms);
  
}
