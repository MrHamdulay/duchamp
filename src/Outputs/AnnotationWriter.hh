// -----------------------------------------------------------------------
// AnnotationWriter.hh: Base class for writing results to annotation files,
//                      annotating images in some FITS viewer.
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
#ifndef DUCHAMP_ANNOTATION_WRITER_H_
#define DUCHAMP_ANNOTATION_WRITER_H_

#include <duchamp/Outputs/CatalogueWriter.hh>
#include <duchamp/Outputs/FileCatalogueWriter.hh>
#include <duchamp/Detection/detection.hh>
#include <ios>
#include <iostream>
#include <fstream>

namespace duchamp {

  /// @brief Base class for writing FITS annotation files
  /// @details This class supports the creation of annotation files
  /// that can be used to supplement the viewing of FITS files in
  /// external viewers. Examples include kvis, ds9, casaviewer. The
  /// class will support the I/O, writing of different types of
  /// annotations (text, lines, circles, ellipses) and other basic
  /// functionality. Classes to write to specific types of annotation
  /// files should derive from this and implement their own header and
  /// primitive methods.

  class AnnotationWriter : public FileCatalogueWriter
  {
  public:
    AnnotationWriter();
    AnnotationWriter(std::string name);
    AnnotationWriter(const AnnotationWriter& other);
    AnnotationWriter& operator= (const AnnotationWriter& other);
    virtual ~AnnotationWriter(){};

    void setCommentString(std::string s){itsComment = s;};

    virtual void writeHeader();
    virtual void writeParameters();
    virtual void writeStats(); 
    virtual void writeTableHeader()=0;
    virtual void writeEntry(Detection *object);

    virtual void writeFooter(){};

    virtual void text(double x, double y, std::string text)=0;
    virtual void line(double x1, double x2, double y1, double y2)=0;
    virtual void circle(double x, double y, double r)=0;
    virtual void ellipse(double x, double y, double r1, double r2, double angle)=0;

  protected:
    std::string itsComment; ///< How comments are denoted in the annotation file (may be just a single char, eg. '#', but make it flexible)
  };

}

#endif
