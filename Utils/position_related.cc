#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <math.h>
#include <Utils/utils.hh>

using std::setw;
using std::setfill;
using std::setprecision;

string getIAUNameEQ(double ra, double dec, float equinox)
{
  /**
   * string getIAUNameEQ(double, double, float)
   *  both ra and dec are assumed to be in degrees.
   *  returns name of the form J1234-4321 for equinox = 2000, 
   *   and B1234-4321 otherwise
   */

  double raHrs = ra / 15.;
  int h = int(raHrs);
  int m = (int)(fmod(raHrs,1.)*60.);
  std::stringstream ss(std::stringstream::out);
  ss.setf(std::ios::showpoint);
  ss.setf(std::ios::fixed);
  if(equinox==2000.) ss << "J"; 
  else ss << "B";
  ss<<setw(2)<<setfill('0')<<h;
  ss<<setw(2)<<setfill('0')<<m;
  int sign = int( dec / fabs(dec) );
  double d = dec / sign;
  h = int(d);
  m = (int)(fmod(d,1.)*60.);
  if(sign==1) ss<<"+"; else ss<<"-";
  ss<<setw(2)<<setfill('0')<<h;
  ss.unsetf(std::ios::showpos);
  ss<<setw(2)<<setfill('0')<<m;
  return ss.str();
}

string getIAUNameGAL(double lon, double lat)
{
  /**
   * string getIAUNameGAL(double, double)
   *  both ra and dec are assumed to be in degrees.
   *  returns name of the form G321.12+01.23
   */

  std::stringstream ss(std::stringstream::out);
  ss.setf(std::ios::showpoint);
  ss.setf(std::ios::fixed);
  ss<<"G";
  ss<<setw(6)<<setfill('0')<<setprecision(2)<<lon;
  ss.setf(std::ios::showpos);
  ss.setf(std::ios::internal);
  ss<<setw(6)<<setfill('0')<<setprecision(2)<<lat;
  ss.unsetf(std::ios::internal);
  ss.unsetf(std::ios::showpos);
  ss.unsetf(std::ios::showpoint);
  ss.unsetf(std::ios::fixed);
  return ss.str();
}

string decToDMS(double dec, bool doSign)
{
  /** 
   *  string decToDMS(double, bool)
   *   converts a decimal angle (in degrees) to dd:mm:ss.ss format.
   *   if doSign is positive, the sign of the angle is shown 
   *    (as would be necessary for Declination angles) 
   *   Returns a string in format 12:34:56.78 or -23:23:23.23 
   */

  double dec_abs,sec;
  int deg,min;
  const double onemin=1./60.;
  bool isNeg = (dec < 0.);

  dec_abs = fabs(dec);
  deg = int(dec_abs);//floor(d)
  min = (int)(fmod(dec_abs,1.)*60.);
  sec = fmod(dec_abs,onemin)*3600.;
  if(fabs(sec-60.)<1.e-10){ /* to prevent rounding errors stuffing things up*/
    sec=0.;
    min++;
  }
  std::stringstream ss(std::stringstream::out);
  ss.setf(std::ios::showpoint);
  ss.setf(std::ios::fixed);
  if(doSign){
    if(isNeg) ss << "-";
    else ss << "+";
  }
  ss << setw(2)<<setfill('0')<<deg<<":";
  ss<<setw(2)<<setfill('0')<<min<<":";
  ss<<setw(5)<<setprecision(2)<<sec;
  return ss.str();
}

double dmsToDec(string dms)
{
  /** 
   *  double dmsToDec(string)
   *   Converts a string in the format +12:23:34.45 to a decimal angle in degrees.
   *   Assumes the angle given is in degrees, so if passing RA as the argument,
   *   need to multiply by 15 to get the result in degrees rather than hours.
   *   The sign of the angle is preserved, if present.
   */


  bool isNeg = false;
  if(dms[0]=='-') isNeg = true;

  std::stringstream ss;
  ss.str(dms);
  string deg,min,sec;
  getline(ss,deg,':');
  getline(ss,min,':');
  getline(ss,sec);
  char *end;
  double d = strtod(deg.c_str(),&end);
  double m = strtod(min.c_str(),&end);
  double s = strtod(sec.c_str(),&end);  

  double dec = fabs(d) + m/60. + s/3600.;
  if(isNeg) dec = dec * -1.;

  return dec;

}
 
double angularSeparation(double &ra1, double &dec1, double &ra2, double &dec2)
{
  /**
   * double angularSeparation(double &,double &,double &,double &);
   *  Enter ra & dec for two positions.
   *    (all positions in degrees)
   *  Returns the angular separation in degrees.
   */

  double angsep = cos((ra1-ra2)*M_PI/180.)*cos(dec1*M_PI/180.)*cos(dec2*M_PI/180.) 
    + sin(dec1*M_PI/180.)*sin(dec2*M_PI/180.);
  angsep = acos(angsep)*180./M_PI;

  return angsep;

}
