// -----------------------------------------------------------------------
// position_related.cc: General utility functions related to WCS positions
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
#include <sstream>
#include <iomanip>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <duchamp/Utils/utils.hh>

using std::setw;
using std::setfill;
using std::setprecision;

std::string getIAUNameEQ(double ra, double dec, float equinox)
{
  /**
   * std::string getIAUNameEQ(double, double, float)
   *  both ra and dec are assumed to be in degrees.
   *  returns name of the form J123456-654321 for equinox = 2000, 
   *   and B1234-4321 otherwise
   */

  std::string rastr=decToDMS(ra,"RA",0);
  rastr.erase(rastr.begin()+2);
  rastr.erase(rastr.begin()+4);
  if(equinox==2000.)
    rastr.erase(rastr.begin()+6,rastr.end());
  else
    rastr.erase(rastr.begin()+4,rastr.end());
  std::string decstr=decToDMS(dec,"DEC",0);
  decstr.erase(decstr.begin()+3);
  decstr.erase(decstr.begin()+5);
  if(equinox==2000.)
    decstr.erase(decstr.begin()+7,decstr.end());
  else
    decstr.erase(decstr.begin()+5,decstr.end());
    
  std::stringstream ss;
  if(equinox==2000.) ss<<"J";
  else ss<<"B";
  ss<<rastr<<decstr;
  return ss.str();

}

std::string getIAUNameGAL(double lon, double lat)
{
  /**
   * std::string getIAUNameGAL(double, double)
   *  both ra and dec are assumed to be in degrees.
   *  returns name of the form G321.123+01.234
   */

  std::stringstream ss(std::stringstream::out);
  ss.setf(std::ios::showpoint);
  ss.setf(std::ios::fixed);
  ss<<"G";
  double goodlon=lon;
  if(lon<0.) goodlon += 360.;
  ss<<setw(7)<<setfill('0')<<setprecision(3)<<goodlon;
  ss.setf(std::ios::showpos);
  ss.setf(std::ios::internal);
  ss<<setw(7)<<setfill('0')<<setprecision(3)<<lat;
  ss.unsetf(std::ios::internal);
  ss.unsetf(std::ios::showpos);
  ss.unsetf(std::ios::showpoint);
  ss.unsetf(std::ios::fixed);
  return ss.str();
}

std::string decToDMS(const double dec, const std::string type, int decPrecision)
{
  /** 
   *Converts a decimal angle (in degrees) to a format reflecting the axis type:
   *  RA   (right ascension):     hh:mm:ss.ss, with dec modulo 360. (24hrs)
   *  DEC  (declination):        sdd:mm:ss.ss  (with sign, either + or -)
   *  GLON (galactic longitude): ddd:mm:ss.ss, with dec made modulo 360.
   *  GLAT (galactic latitude):  sdd:mm:ss.ss  (with sign, either + or -)
   *    Any other type defaults to RA, and prints warning.
   *
   * \param dec Decimal value of the angle, in degrees.
   * \param type String indicating desired type of output. Options RA, DEC, 
   *              GLON, GLAT
   * \return String with angle in desired format.
   */

  double dec_abs,degD,minD,minDint,sec;
  int deg,min;
  const double minPerHour=60.;
  const double degPerHour=15.;
  double thisDec = dec;
  std::string sign="";
  int degSize = 2; // number of figures in the degrees part of the output.

  int precision=std::max(0,decPrecision);
  if(type=="RA") precision++;

  if((type=="RA")||(type=="GLON")){
    if(type=="GLON")  degSize = 3; // longitude has three figures in degrees.
    // Make these modulo 360.;
    while (thisDec < 0.) { thisDec += 360.; }
    while (thisDec >= 360.) { thisDec -= 360.; }
    if(type=="RA") thisDec /= degPerHour;  // Convert to hours.
  }
  else if((type=="DEC")||(type=="GLAT")){
    if(thisDec<0.) sign = "-";
    else sign = "+";
  }
  else { // UNKNOWN TYPE -- DEFAULT TO RA.
    std::cerr << "WARNING <decToDMS> : Unknown axis type ("
	      << type << "). Defaulting to using RA.\n";
    while (thisDec < 0.) { thisDec += 360.; }
    while (thisDec >= 360.) { thisDec -= 360.; }
    thisDec /= degPerHour;
  }
  
  dec_abs = fabs(thisDec);
  minD = modf(dec_abs, &degD) * minPerHour;
  sec = modf(minD, &minDint) * minPerHour;
  deg = int(degD);
  min = int(minDint);

  if(fabs(sec-minPerHour)<pow(10,-precision)){ // to prevent rounding errors stuffing things up
    sec=0.;
    min++;
    if(min==60){
      min=0;
      deg++;
    }
  }

  std::stringstream ss(std::stringstream::out);
  ss.setf(std::ios::showpoint);
  ss.setf(std::ios::fixed);
  ss << sign;
  ss << setw(degSize)<<setfill('0')<<deg<<":";
  ss<<setw(2)<<setfill('0')<<min<<":";
  if(precision>0)
    ss<<setw(precision+3)<<setprecision(precision)<<sec;
  else {
    ss << setw(2) << int(sec);
  }

  return ss.str();
}


double dmsToDec(std::string dms)
{
  /** 
   *  double dmsToDec(string)
   *   Converts a std::string in the format +12:23:34.45 to a decimal angle in degrees.
   *   Assumes the angle given is in degrees, so if passing RA as the argument,
   *   need to multiply by 15 to get the result in degrees rather than hours.
   *   The sign of the angle is preserved, if present.
   */


  bool isNeg = false;
  if(dms[0]=='-') isNeg = true;

  std::stringstream ss;
  ss.str(dms);
  std::string deg,min,sec;
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

const long double degToRadian=M_PI/180.;
 
double angularSeparation(double &ra1, double &dec1, double &ra2, double &dec2)
{
  /**
   * double angularSeparation(double &,double &,double &,double &);
   *  Enter ra & dec for two positions.
   *    (all positions in degrees)
   *  Returns the angular separation in degrees.
   */

  long double dra = (ra1-ra2)*degToRadian;
  long double d1 = dec1*degToRadian;
  long double d2 = dec2*degToRadian;
  long double angsep;
  if((fabs(ra1-ra2) < 1./3600.)&&(fabs(dec1-dec2)<1./3600.))
    return sqrt(dra*dra + (d1-d2)*(d1-d2)) / degToRadian;
  else {
    if(fabs(ra1-ra2) < 1./3600.)
      angsep = cos(d1)*cos(d2) - dra*dra*cos(d1)*cos(d2)/2. + sin(d1)*sin(d2);
    else
      angsep = cos(dra)*cos(d1)*cos(d2) + sin(d1)*sin(d2);
    double dangsep = acos(angsep) / degToRadian;
    return dangsep;
  }

}
