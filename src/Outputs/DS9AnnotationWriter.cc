// -----------------------------------------------------------------------
// DS9AnnotationWriter.hh: Class for writing results to ds9 annotation files.
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
#include <duchamp/Outputs/DS9AnnotationWriter.hh>
#include <duchamp/Outputs/AnnotationWriter.hh>
#include <duchamp/Outputs/CatalogueWriter.hh>
#include <duchamp/Outputs/FileCatalogueWriter.hh>
#include <duchamp/Detection/detection.hh>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

namespace duchamp {

   DS9AnnotationWriter::DS9AnnotationWriter():
    AnnotationWriter()
  {
  }

  DS9AnnotationWriter::DS9AnnotationWriter(std::string name):
    AnnotationWriter(name)
  {
  }

  DS9AnnotationWriter::DS9AnnotationWriter(const DS9AnnotationWriter& other)
  {
    this->operator=(other);
  }

  DS9AnnotationWriter& DS9AnnotationWriter::operator= (const DS9AnnotationWriter& other)
  {
    if(this==&other) return *this;
    ((AnnotationWriter &) *this) = other;
    return *this;
  }
  void DS9AnnotationWriter::writeTableHeader()
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "global color=red ";
      if(this->itsHead->isWCS()) this->itsFileStream << "wcs=wcs";
      this->itsFileStream << "\n";
    }
  }

  void DS9AnnotationWriter::text(float x, float y, std::string text)
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "text " << x << " " << y << " {" << text<<"}\n";
    }
  }
  void DS9AnnotationWriter::line(float x1, float x2, float y1, float y2)
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "line " << x1 << " " << y1 << " " << x2 << " " << y2 << "\n";
    }
  }
  void DS9AnnotationWriter::circle(float x, float y, float r)
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "circle " << x << " " << y << " " << r << "\n";
    }
  }
  void DS9AnnotationWriter::ellipse(float x, float y, float r1, float r2, float angle)
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "ellipse " << x << " " << y << " " << r1 << " " << r2 << " " << angle << "\n";
    }
  }

  
}
