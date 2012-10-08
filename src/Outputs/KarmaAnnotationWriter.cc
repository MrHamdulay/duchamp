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
      this->itsFileStream << "COLOR RED\n";
      if(this->itsHead->isWCS()) this->itsFileStream << "COORD W\n";
      else this->itsFileStream << "COORD P\n";
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
  void KarmaAnnotationWriter::ellipse(double x, double y, double r1, double r2, double angle)
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "ELLIPSE " << x << " " << y << " " << r1 << " " << r2 << " " << angle << "\n";
    }
  }

  
}
