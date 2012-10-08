// -----------------------------------------------------------------------
// DS9AnnotationWriter.hh: Class for writing results to DS9 annotation files.
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
#ifndef DUCHAMP_DS9_ANNOTATION_WRITER_H_
#define DUCHAMP_DS9_ANNOTATION_WRITER_H_
#include <duchamp/Outputs/AnnotationWriter.hh>
#include <duchamp/Outputs/CatalogueWriter.hh>
#include <duchamp/Outputs/FileCatalogueWriter.hh>
#include <duchamp/Detection/detection.hh>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

namespace duchamp {

  class DS9AnnotationWriter : public AnnotationWriter
  {
  public:
    DS9AnnotationWriter();
    DS9AnnotationWriter(std::string name);
    DS9AnnotationWriter(const DS9AnnotationWriter& other);
    DS9AnnotationWriter& operator= (const DS9AnnotationWriter& other);
    virtual ~DS9AnnotationWriter(){};

    void writeTableHeader();

    void text(float x, float y, std::string text);
    void line(float x1, float x2, float y1, float y2);
    void circle(float x, float y, float r);
    void ellipse(float x, float y, float r1, float r2, float angle);

  };

}

#endif

