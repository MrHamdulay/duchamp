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
    /// @brief Default constructor
    AnnotationWriter();
    /// @brief Constructor with given filename
    AnnotationWriter(std::string name);
    /// @brief Copy constructor
    AnnotationWriter(const AnnotationWriter& other);
    /// @brief Copy operator
    AnnotationWriter& operator= (const AnnotationWriter& other);
    /// @brief Default destructor
    virtual ~AnnotationWriter(){};

    /// @brief Define how comments are represented - this string will be placed at the start of a comment line
    void setCommentString(std::string s){itsComment = s;};
    /// @brief Set the colour string
    void setColourString(std::string s){itsColour = s;};

    /// @brief Write header text describing what the file is for
    virtual void writeHeader();
    /// @brief Write a summary of the input parameters
    virtual void writeParameters();
    /// @brief Write a summary of the statistics used for detection
    virtual void writeStats(); 
    /// @brief Write the global properties for the file (colour, WCS info, ...)
    virtual void writeTableHeader()=0;
    /// @brief Write information describing a detected object
    virtual void writeEntry(Detection *object);

    /// @brief Write any information to go at the end of the file
    virtual void writeFooter(){};

    /// @brief Annotate with a text string
    virtual void text(double x, double y, std::string text)=0;
    /// @brief Annotate with a single line
    virtual void line(double x1, double x2, double y1, double y2)=0;
    /// @brief Annotate with a circle
    virtual void circle(double x, double y, double r)=0;
    /// @brief Annotate with a box
    virtual void box(double x1, double x2, double y1, double y2, std::string label="")=0;
    /// @brief Annotate with an ellipse
    virtual void ellipse(double x, double y, double r1, double r2, double angle)=0;
    /// @brief Annotate with a series of lines connecting points
    virtual void joinTheDots(std::vector<double> x, std::vector<double> y)=0;

  protected:
    std::string itsComment; ///< How comments are denoted in the annotation file (may be just a single char, eg. '#', but make it flexible)
    std::string itsColour; ///< What colour to make the annotations - set at the start of the file and applies to all annotations
  };

}

#endif
