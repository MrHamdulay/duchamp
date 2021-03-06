// -----------------------------------------------------------------------
// KarmaAnnotationWriter.hh: Class for writing results to karma annotation files.
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
#include <duchamp/Outputs/KarmaAnnotationWriter.hh>
#include <duchamp/Outputs/AnnotationWriter.hh>
#include <duchamp/Outputs/CatalogueWriter.hh>
#include <duchamp/Outputs/FileCatalogueWriter.hh>
#include <duchamp/Detection/detection.hh>
#include <ios>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

namespace duchamp {

   KarmaAnnotationWriter::KarmaAnnotationWriter():
    AnnotationWriter()
  {
  }

  KarmaAnnotationWriter::KarmaAnnotationWriter(std::string name):
    AnnotationWriter(name)
  {
  }

  KarmaAnnotationWriter::KarmaAnnotationWriter(const KarmaAnnotationWriter& other)
  {
    this->operator=(other);
  }

  KarmaAnnotationWriter& KarmaAnnotationWriter::operator= (const KarmaAnnotationWriter& other)
  {
    if(this==&other) return *this;
    ((AnnotationWriter &) *this) = other;
    return *this;
  }
  void KarmaAnnotationWriter::writeTableHeader()
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "COLOR " << this->itsColour<<"\n";
      this->itsFileStream << "PA STANDARD\n";
      this->itsFileStream << "COORD W\n";
      this->itsFileStream << std::setprecision(6);
      this->itsFileStream.setf(std::ios::fixed);
    }
  }

  void KarmaAnnotationWriter::text(double x, double y, std::string text)
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "TEXT " << x << " " << y << " " << text<<"\n";
    }
  }
  void KarmaAnnotationWriter::line(double x1, double x2, double y1, double y2)
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "LINE " << x1 << " " << y1 << " " << x2 << " " << y2 << "\n";
    }
  }
  void KarmaAnnotationWriter::circle(double x, double y, double r)
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "CIRCLE " << x << " " << y << " " << r << "\n";
    }
  }
  void KarmaAnnotationWriter::box(double x1, double x2, double y1, double y2, std::string label)
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "BOX " << x1 << " " << y1 << " " << x2 << " " << y2 <<"\n";
    }
  }
  void KarmaAnnotationWriter::ellipse(double x, double y, double r1, double r2, double angle)
  {
    if(this->itsOpenFlag){
      double angleUsed=angle;
      // Need to correct the angle here. This is what the Karma documentation says, at http://www.atnf.csiro.au/computing/software/karma/user-manual/node17.html:
      //
      // Standard Postition Angles are generally defined in the Cartesian sense where PA=0 specifies {X>0,Y=0}, PA=90 specifies {X=0,Y>0}, and so forth. 
      // In a coordinate system where X increases from left to right, and Y increases from bottom to top, PA will increase in a counter-clockwise direction. 
      // However this will be reversed if the X increases from right to left, or Y from top to bottom (though not both). So be careful! To summarize: 
      //  PA is measured CCW from right in R coords, and CW from right in P coords. How it is measured in W coords depends on the coordinate system: 
      // in many cases, it will be the same as R coords, but this is not always true! For example, most astronomical maps of the sky have the X coordinate 
      // increasing toward the left, in which case, PA will be measured CW from left! 
      //
      // So, we need to invert the sign of the PA when RA increases to the left (as is usual), then subtract 90degrees.
      if(this->itsHead->getWCS()->cdelt[0]<0.) angleUsed *= -1.; 
      angleUsed -= 90.;
      this->itsFileStream << "ELLIPSE " << x << " " << y << " " << r1 << " " << r2 << " " << angleUsed << "\n";
    }
  }

  void KarmaAnnotationWriter::joinTheDots(std::vector<double> x, std::vector<double> y)
  {
    if(this->itsOpenFlag){
      if(x.size()==y.size()){
	// this->itsFileStream << "CLINES";
	// for(size_t i=0;i<x.size();i++) this->itsFileStream <<" " << x[i] << " " << y[i];
	// this->itsFileStream << "\n";
	  
	  for(size_t i=0;i<x.size()-1;i++) this->line(x[i],x[i+1],y[i],y[i+1]);
	  this->line(x[x.size()-1],x[0],y[x.size()-1],y[0]);

      }
    }
  }

  
}
