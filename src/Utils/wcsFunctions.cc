#include <iostream>
#include <sstream>
#include <math.h>
#include <wcs.h>
#include <wcsunits.h>
#include <duchamp.hh>
#include <Utils/utils.hh>

int pixToWCSSingle(struct wcsprm *wcs, const double *pix, double *world)
{
  /** 
   *  pixToWCSSingle(struct wcsprm *wcs, const double *pix, double *world)
   *   Uses wcs to convert the three-dimensional pixel position referenced 
   *    by pix to world coordinates, which are placed in the array world[].
   *   Assumes that pix only has one point with an x,y,and z pixel positions.
   *   If there are any other axes present (eg. Stokes), these are set to the
   *    first pixel in that axis.
   *   Offsets these pixel values by 1 to account for the C arrays being 
   *    indexed to 0.
   */

  int naxis=wcs->naxis,npts=1,status;

  double *newpix = new double[naxis*npts];
  // correct from 0-indexed to 1-indexed pixel array
  for(int i=0;i<naxis;i++) newpix[i] = 1.;
  newpix[wcs->lng] += pix[0];
  newpix[wcs->lat] += pix[1];
  newpix[wcs->spec]+= pix[2];

  int    *stat      = new int[npts];
  double *imgcrd    = new double[naxis*npts];
  double *tempworld = new double[naxis*npts];
  double *phi       = new double[npts];
  double *theta     = new double[npts];
  if(status=wcsp2s(wcs, npts, naxis, newpix, imgcrd, phi, 
		   theta, tempworld, stat)>0){
    std::stringstream errmsg;
    errmsg << "WCS Error Code = " << status <<": " << wcs_errmsg[status] 
	   << "\nstat value is " << stat[0] << std::endl;
    duchampError("pixToWCSSingle",errmsg.str());
  }

  //return just the spatial/velocity information
  world[0] = tempworld[wcs->lng];
  world[1] = tempworld[wcs->lat];
  world[2] = tempworld[wcs->spec];

  delete [] stat;
  delete [] imgcrd;
  delete [] tempworld;
  delete [] phi;
  delete [] theta;
  delete [] newpix;
  return status;
}

int wcsToPixSingle(struct wcsprm *wcs, const double *world, double *pix)
{
  /** 
   *  wcsToPixSingle(struct wcsprm *wcs, const double *world, double *pix)
   *   Uses wcs to convert the three-dimensional world coordinate position 
   *    referenced by world to pixel coordinates, which are placed in the 
   *    array pix[].
   *   Assumes that world only has one point (three values eg RA Dec Velocity)
   *   Offsets the pixel values by 1 to account for the C arrays being 
   *    indexed to 0.
   */

  int naxis=wcs->naxis,npts=1,status;

  double *tempworld = new double[naxis*npts];
  for(int i=0;i<naxis;i++) tempworld[i] = wcs->crval[i];
  tempworld[wcs->lng]  = world[0];
  tempworld[wcs->lat]  = world[1];
  tempworld[wcs->spec] = world[2];

  int    *stat    = new int[npts];
  double *temppix = new double[naxis*npts];
  double *imgcrd  = new double[naxis*npts];
  double *phi     = new double[npts];
  double *theta   = new double[npts];

  status=wcss2p(wcs, npts, naxis, tempworld, phi, theta, imgcrd, temppix, stat);

  if( status > 0 ){
    std::stringstream errmsg;
    errmsg << "WCS Error Code = " <<status<<": " << wcs_errmsg[status] 
	   << "\nstat value is " << stat[0] << std::endl;
    duchampError("wcsToPixSingle",errmsg.str());
  }

  pix[0] = temppix[wcs->lng] - 1.;
  pix[1] = temppix[wcs->lat] - 1.;
  pix[2] = temppix[wcs->spec] - 1.;
  // correct from 1-indexed to 0-indexed pixel array
  //  and only return the spatial & frequency information

  delete [] stat;
  delete [] imgcrd;
  delete [] temppix;
  delete [] phi;
  delete [] theta;
  delete [] tempworld;
  return status;
}

int pixToWCSMulti(struct wcsprm *wcs, const double *pix, double *world, const int npts)
{
  /** 
   *  pixToWCSSingle(struct wcsprm *wcs, const double *pix, double *world)
   *   Uses wcs to convert the three-dimensional pixel positions referenced 
   *    by pix to world coordinates, which are placed in the array world[].
   *   pix is assumed to hold the positions of npts points.
   *   Offsets these pixel values by 1 to account for the C arrays being 
   *    indexed to 0.
   */

  int naxis=wcs->naxis,status;

  // correct from 0-indexed to 1-indexed pixel array
  // Add entries for any other axes that are present, keeping the 
  //   order of pixel positions the same
  double *newpix = new double[naxis*npts];
  for(int pt=0;pt<npts;pt++){
    for(int i=0;i<naxis;i++) newpix[pt*naxis+i] = 1.;
    newpix[pt*naxis+wcs->lng]  += pix[pt*3+0];
    newpix[pt*naxis+wcs->lat]  += pix[pt*3+1];
    newpix[pt*naxis+wcs->spec] += pix[pt*3+2];
  }

  int    *stat      = new int[npts];
  double *imgcrd    = new double[naxis*npts];
  double *tempworld = new double[naxis*npts];
  double *phi       = new double[npts];
  double *theta     = new double[npts];

  if(status=wcsp2s(wcs, npts, naxis, newpix, imgcrd, 
		 phi, theta, tempworld, stat)     >0){
    std::stringstream errmsg;
    errmsg << "WCS Error Code = " <<status<<": " << wcs_errmsg[status] 
	   << "\nstat value is " << stat[0] << std::endl;
    duchampError("pixToWCSMulti",errmsg.str());
  }
  else{
    //return just the spatial/velocity information, keeping the
    //  order of the pixel positions the same.
    for(int pt=0;pt<npts;pt++){
      world[pt*3+0] = tempworld[pt*naxis + wcs->lng];
      world[pt*3+1] = tempworld[pt*naxis + wcs->lat];
      world[pt*3+2] = tempworld[pt*naxis + wcs->spec];
    }
    
  }

  delete [] stat;
  delete [] imgcrd;
  delete [] tempworld;
  delete [] phi;
  delete [] theta;
  delete [] newpix;
  return status;
}

int wcsToPixMulti(struct wcsprm *wcs, const double *world, double *pix, const int npts)
{
  /** 
   *  wcsToPixSingle(struct wcsprm *wcs, const double *world, double *pix)
   *   Uses wcs to convert the three-dimensional world coordinate position 
   *    referenced by world to pixel coordinates, which are placed in the
   *    array pix[].
   *   world is assumed to hold the positions of npts points.
   *   Offsets the pixel values by 1 to account for the C arrays being 
   *    indexed to 0.
   */

  int naxis=wcs->naxis,status=0;
  // Test to see if there are other axes present, eg. stokes
  if(wcs->naxis>naxis) naxis = wcs->naxis;

  // Add entries for any other axes that are present, keeping the 
  //   order of pixel positions the same
  double *tempworld = new double[naxis*npts];
  for(int pt=0;pt<npts;pt++){
    for(int axis=0;axis<naxis;axis++) 
      tempworld[pt*naxis+axis] = wcs->crval[axis];
    tempworld[pt*naxis + wcs->lng]  = world[pt*3+0];
    tempworld[pt*naxis + wcs->lat]  = world[pt*3+1];
    tempworld[pt*naxis + wcs->spec] = world[pt*3+2];
  }

  int    *stat   = new int[npts];
  double *temppix = new double[naxis*npts];
  double *imgcrd = new double[naxis*npts];
  double *phi    = new double[npts];
  double *theta  = new double[npts];
  if(status=wcss2p(wcs, npts, naxis, tempworld, 
		 phi, theta, imgcrd, temppix, stat)>0){
    std::stringstream errmsg;
    errmsg << "WCS Error Code = " <<status<<": " <<wcs_errmsg[status] 
	   << "\nstat value is " << stat[0] << std::endl;
    duchampError("wcsToPixMulti",errmsg.str());
  }
  else{
    // correct from 1-indexed to 0-indexed pixel array 
    //  and return just the spatial/velocity information, 
    //  keeping the order of the pixel positions the same.
    for(int pt=0;pt<npts;pt++){
      pix[pt*naxis + 0] = temppix[pt*naxis + wcs->lng] - 1.;
      pix[pt*naxis + 1] = temppix[pt*naxis + wcs->lat] - 1.;
      pix[pt*naxis + 2] = temppix[pt*naxis + wcs->spec] - 1.;
    }
  }

  delete [] stat;
  delete [] imgcrd;
  delete [] temppix;
  delete [] phi;
  delete [] theta;
  delete [] tempworld;
  return status;
}

double pixelToVelocity(struct wcsprm *wcs, double &x, double &y, double &z, 
		       std::string velUnits)
{
  /** 
   *  pixelToVelocity(struct wcsprm *wcs, double &x, double &y, double &z, 
   *                  std::string velUnits)
   *   Uses wcs to convert the three-dimensional pixel position (x,y,z)
   *   to world coordinates.
   *   Returns the velocity in the units given by velUnits.
   */

  int naxis=wcs->naxis,status=0;
  int    *stat   = new int[1];
  double *pixcrd = new double[naxis];
  double *imgcrd = new double[naxis];
  double *world  = new double[naxis];
  double *phi    = new double[1];
  double *theta  = new double[1];
  // correct from 0-indexed to 1-indexed pixel array by adding 1 to pixel values.
  for(int i=0;i<naxis;i++) pixcrd[i] = 1.;
  pixcrd[wcs->lng] += x;
  pixcrd[wcs->lat] += y;
  pixcrd[wcs->spec]+= z;

  if(status=wcsp2s(wcs, 1, naxis, pixcrd, imgcrd, phi, theta, world, stat)>0){
    std::stringstream errmsg;
    errmsg <<"WCS Error Code = "<<status<<": "<<wcs_errmsg[status] 
	   << "\nstat value is "<<stat[0]<<std::endl;
    duchampError("pixelToVelocity",errmsg.str());
  }

  double vel = coordToVel(wcs, world[wcs->spec], velUnits);

  delete [] stat;
  delete [] pixcrd;
  delete [] imgcrd;
  delete [] world;
  delete [] phi;
  delete [] theta;
  
  return vel;
}
 
double coordToVel(struct wcsprm *wcs, const double coord, std::string outputUnits)
{
  /** 
   *  coordToVel(struct wcsprm *wcs, const double coord, std::string outputUnits)
   *   Convert the wcs coordinate given by coord to a velocity in km/s.
   *   Does this by checking the ztype parameter in wcs to see if it is 
   *    FREQ or otherwise, and converts accordingly.
   */

  static int errflag = 0;
  double scale, offset, power;
  int specIndex = wcs->spec;
  int status = wcsunits( wcs->cunit[specIndex], outputUnits.c_str(), 
			 &scale, &offset, &power);

  if(status > 0){
    if(errflag==0){
      std::stringstream errmsg;
      errmsg << "WCSUNITS Error Code = " << status << ":"
	     << wcsunits_errmsg[status];
      if(status == 10) errmsg << "\nTried to get conversion from " 
			      << wcs->cunit[specIndex]
			      << " to " << outputUnits.c_str();
      errmsg << "\nUsing coordinate value instead.\n";
      duchampError("coordToVel", errmsg.str());
      errflag = 1;
    }
    return coord;
  }
  else return pow( (coord*scale + offset), power);

}

double velToCoord(struct wcsprm *wcs, const float velocity, std::string outputUnits)
{
  /** 
   *  velToCoord(struct wcsprm *wcs, const double coord)
   *   Convert the velocity given to the appropriate world coordinate for wcs.
   *   Does this by checking the ztype parameter in wcs to see if it is 
   *    FREQ or otherwise, and converts accordingly.
   */

  double scale, offset, power;
  int specIndex = wcs->spec;
  int status = wcsunits( wcs->cunit[specIndex], outputUnits.c_str(), 
			 &scale, &offset, &power);

  if(status > 0){
    std::stringstream errmsg;
    errmsg << "WCSUNITS Error Code = " << status << ":"
	   << wcsunits_errmsg[status] << "\nUsing coordinate value instead.\n";
    duchampError("velToCoord",errmsg.str());
    return velocity;
  }
  else return (pow(velocity, 1./power) - offset) / scale;
  
}
