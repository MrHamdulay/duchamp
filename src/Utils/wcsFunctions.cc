#include <iostream>
#include <sstream>
#include <math.h>
#include <wcs.h>
#include <wcsunits.h>
#include <duchamp.hh>
#include <Utils/utils.hh>

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

  if(wcs->naxis>nelem) nelem = wcs->naxis;
  // Test to see if there are other axes present, eg. stokes

  double *newpix = new double[nelem*npts];
  // correct from 0-indexed to 1-indexed pixel array
  for(int i=0;i<3;i++)     newpix[i] = pix[i] + 1.;  
  for(int i=3;i<nelem;i++) newpix[i] = 1.;  // this adds entries for other axes

  int    *stat      = new int[npts];
  double *imgcrd    = new double[nelem*npts];
  double *tempworld = new double[nelem*npts];
  double *phi       = new double[npts];
  double *theta     = new double[npts];
  if(flag=wcsp2s(wcs, npts, nelem, newpix, imgcrd, phi, theta, tempworld, stat)>0){
    std::stringstream errmsg;
    errmsg << "WCS Error Code = " <<flag<<": " << wcs_errmsg[flag] 
	   << "\nstat value is " << stat[0] << std::endl;
    duchampError("pixToWCSSingle",errmsg.str());
  }

  //return just the spatial/velocity information
  for(int i=0;i<3;i++) world[i] = tempworld[i];  

  delete [] stat;
  delete [] imgcrd;
  delete [] tempworld;
  delete [] phi;
  delete [] theta;
  delete [] newpix;
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

  if(wcs->naxis>nelem) nelem = wcs->naxis;
  // Test to see if there are other axes present, eg. stokes

  double *tempworld = new double[nelem*npts];
  for(int i=0;i<3;i++)     tempworld[i] = world[i];  
  for(int i=3;i<nelem;i++) tempworld[i] = 0.; // this adds entries for other axes

  int    *stat   = new int[npts];
  double *temppix = new double[nelem*npts];
  double *imgcrd = new double[nelem*npts];
  double *phi    = new double[npts];
  double *theta  = new double[npts];
  if(flag=wcss2p(wcs, npts, nelem, tempworld, phi, theta, imgcrd, temppix, stat)>0){
    std::stringstream errmsg;
    errmsg << "WCS Error Code = " <<flag<<": " << wcs_errmsg[flag] 
	   << "\nstat value is " << stat[0] << std::endl;
    duchampError("wcsToPixSingle",errmsg.str());
  }

  for(int i=0;i<3;i++) pix[i] = temppix[i] - 1.;  
  // correct from 1-indexed to 0-indexed pixel array
  //  and only return the spatial & frequency information

  delete [] stat;
  delete [] imgcrd;
  delete [] temppix;
  delete [] phi;
  delete [] theta;
  delete [] tempworld;
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

  // Test to see if there are other axes present, eg. stokes
  if(wcs->naxis>nelem) nelem = wcs->naxis;

  double *newpix = new double[nelem*npts];
  // correct from 0-indexed to 1-indexed pixel array

  // Add entries for any other axes that are present, keeping the 
  //   order of pixel positions the same
  for(int elem=0;elem<nelem;elem++){
    for(int pt=0;pt<npts;pt++){
      if(elem<3) newpix[pt*nelem+elem] = pix[pt*3 + elem] + 1.;
      else newpix[pt*nelem+elem] = 1.;
    }
  }

  int    *stat      = new int[npts];
  double *imgcrd    = new double[nelem*npts];
  double *tempworld = new double[nelem*npts];
  double *phi       = new double[npts];
  double *theta     = new double[npts];

  if(flag=wcsp2s(wcs, npts, nelem, newpix, imgcrd, phi, theta, tempworld, stat)>0){
    std::stringstream errmsg;
    errmsg << "WCS Error Code = " <<flag<<": " << wcs_errmsg[flag] 
	   << "\nstat value is " << stat[0] << std::endl;
    duchampError("pixToWCSMulti",errmsg.str());
  }
  else{
    //return just the spatial/velocity information, keeping the
    //  order of the pixel positions the same.
    for(int elem=0;elem<nelem;elem++){
      for(int pt=0;pt<npts;pt++){
	if(elem<3) world[pt*3+elem] = tempworld[pt*nelem + elem];
      }
    }
  }

  delete [] stat;
  delete [] imgcrd;
  delete [] tempworld;
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

  int nelem=3,flag=0;
  // Test to see if there are other axes present, eg. stokes
  if(wcs->naxis>nelem) nelem = wcs->naxis;

  // Add entries for any other axes that are present, keeping the 
  //   order of pixel positions the same
  double *tempworld = new double[nelem*npts];
  for(int elem=0;elem<nelem;elem++){
    for(int pt=0;pt<npts;pt++){
      if(elem<3) tempworld[pt*nelem + elem] = world[pt*3+elem];
      else tempworld[pt*nelem + elem] = 1.;
    }
  }

  int    *stat   = new int[npts];
  double *temppix = new double[nelem*npts];
  double *imgcrd = new double[nelem*npts];
  double *phi    = new double[npts];
  double *theta  = new double[npts];
  if(flag=wcss2p(wcs, npts, nelem, tempworld, phi, theta, imgcrd, temppix, stat)>0){
    std::stringstream errmsg;
    errmsg << "WCS Error Code = " <<flag<<": " <<wcs_errmsg[flag] 
	   << "\nstat value is " << stat[0] << std::endl;
    duchampError("wcsToPixMulti",errmsg.str());
  }
  else{
    // correct from 1-indexed to 0-indexed pixel array 
    //  and return just the spatial/velocity information, 
    //  keeping the order of the pixel positions the same.
    for(int elem=0;elem<nelem;elem++){
      for(int pt=0;pt<npts;pt++){
	if(elem<3) pix[pt*3+elem] = temppix[pt*nelem + elem] + 1.;
      }
    }
  }

  delete [] stat;
  delete [] imgcrd;
  delete [] temppix;
  delete [] phi;
  delete [] theta;
  delete [] tempworld;
  return flag;
}

double pixelToVelocity(wcsprm *wcs, double &x, double &y, double &z, string velUnits)
{
  /** 
   *  pixelToVelocity(wcsprm *wcs, double &x, double &y, double &z, string velUnits)
   *   Uses wcs to convert the three-dimensional pixel position (x,y,z)
   *   to world coordinates.
   *   Returns the velocity in the units given by velUnits.
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
    std::stringstream errmsg;
    errmsg <<"WCS Error Code = "<<flag<<": "<<wcs_errmsg[flag] 
	   << "\nstat value is "<<stat[0]<<std::endl;
    duchampError("pixelToVelocity",errmsg.str());
  }

  double vel = coordToVel(wcs, world[2], velUnits);

  delete [] stat;
  delete [] pixcrd;
  delete [] imgcrd;
  delete [] world;
  delete [] phi;
  delete [] theta;
  
  return vel;
}
 
double coordToVel(wcsprm *wcs, const double coord, string outputUnits)
{
  /** 
   *  coordToVel(wcsprm *wcs, const double coord, string outputUnits)
   *   Convert the wcs coordinate given by coord to a velocity in km/s.
   *   Does this by checking the ztype parameter in wcs to see if it is FREQ or otherwise,
   *   and converts accordingly.
   */

  static int errflag = 0;
  double scale, offset, power;
  int flag = wcsunits( wcs->cunit[2], outputUnits.c_str(), &scale, &offset, &power);

  if(flag>0){
    if(errflag==0){
      std::stringstream errmsg;
      errmsg << "WCSUNITS Error Code = " << flag << ":"
	     << wcsunits_errmsg[flag];
      if(flag==10) errmsg << "\nTried to get conversion from " << wcs->cunit[2]
			  << " to " << outputUnits.c_str();
      errmsg << "\nUsing coordinate value instead.\n";
      duchampError("coordToVel", errmsg.str());
      errflag = 1;
    }
    return coord;
  }
  else return pow( (coord*scale + offset), power);

}

double velToCoord(wcsprm *wcs, const float velocity, string outputUnits)
{
  /** 
   *  velToCoord(wcsprm *wcs, const double coord)
   *   Convert the velocity given to the appropriate world coordinate for wcs.
   *   Does this by checking the ztype parameter in wcs to see if it is FREQ or otherwise,
   *   and converts accordingly.
   */

  double scale, offset, power;
  int flag = wcsunits( wcs->cunit[2], outputUnits.c_str(), &scale, &offset, &power);

  if(flag>0){
    std::stringstream errmsg;
    errmsg << "WCSUNITS Error Code = " << flag << ":"
	   << wcsunits_errmsg[flag] << "\nUsing coordinate value instead.\n";
    duchampError("velToCoord",errmsg.str());
    return velocity;
  }
  else return (pow(velocity, 1./power) - offset) / scale;
  
}
