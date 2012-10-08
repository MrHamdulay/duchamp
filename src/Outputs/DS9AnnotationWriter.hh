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
    /// @brief Default constructor
    DS9AnnotationWriter();
    /// @brief Constructor with given filename
    DS9AnnotationWriter(std::string name);
    /// @brief Copy constructor
    DS9AnnotationWriter(const DS9AnnotationWriter& other);
    /// @brief Copy operator
    DS9AnnotationWriter& operator= (const DS9AnnotationWriter& other);
    /// @brief Default destructor
    virtual ~DS9AnnotationWriter(){};

    /// @brief Write the global properties for the file (colour, WCS info, ...)
    void writeTableHeader();

    /// @brief Annotate with a text string
    void text(double x, double y, std::string text);
    /// @brief Annotate with a single line
    void line(double x1, double x2, double y1, double y2);
    /// @brief Annotate with a circle
    void circle(double x, double y, double r);
    /// @brief Annotate with an ellipse
    void ellipse(double x, double y, double r1, double r2, double angle);
    /// @brief Annotate with a series of lines connecting points
    void joinTheDots(std::vector<double> x, std::vector<double> y);

  };

}

#endif

