// -----------------------------------------------------------------------
// CasaAnnotationWriter.hh: Class for writing results to CASA annotation files.
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
#include <duchamp/Outputs/CasaAnnotationWriter.hh>
#include <duchamp/Outputs/AnnotationWriter.hh>
#include <duchamp/Outputs/CatalogueWriter.hh>
#include <duchamp/Outputs/FileCatalogueWriter.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Utils/utils.hh>
#include <ios>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

namespace duchamp {

   CasaAnnotationWriter::CasaAnnotationWriter():
    AnnotationWriter()
  {
    this->itsSpatialUnits="deg";
  }

  CasaAnnotationWriter::CasaAnnotationWriter(std::string name):
    AnnotationWriter(name)
  {
    this->itsSpatialUnits="deg";
  }

  CasaAnnotationWriter::CasaAnnotationWriter(const CasaAnnotationWriter& other)
  {
    this->operator=(other);
  }

  CasaAnnotationWriter& CasaAnnotationWriter::operator= (const CasaAnnotationWriter& other)
  {
    if(this==&other) return *this;
    ((AnnotationWriter &) *this) = other;
    this->itsSpatialUnits = other.itsSpatialUnits;
    return *this;
  }

  void CasaAnnotationWriter::writeHeader()
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "#CRTFv0\n";  //This MUST be written as the first line in a CASA region file
      this->AnnotationWriter::writeHeader();
    }

  }

  void CasaAnnotationWriter::writeTableHeader()
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "global color=" << makelower(this->itsColour) << ", ";
      if(this->itsHead->isWCS()){
	if(this->itsHead->getWCS()->equinox == 1950.) this->itsFileStream << "coord=1950 ";
	else  this->itsFileStream << "coord=J2000 ";
      }
      else this->itsSpatialUnits="pix";
      this->itsFileStream<<"\n#\n";
      this->itsFileStream.setf(std::ios::fixed);
      this->itsFileStream << std::setprecision(6);
    }
  }

  void CasaAnnotationWriter::writeEntry(Detection *object)
  {
    if(this->itsOpenFlag){
      double pix[6],wld[6];
      pix[0]=object->getXmin()-0.5;
      pix[1]=object->getYmin()-0.5;
      pix[2]=object->getZcentre();
      pix[3]=object->getXmax()+0.5;
      pix[4]=object->getYmax()+0.5;
      pix[5]=object->getZcentre();
      this->itsHead->pixToWCS(pix,wld,2);
      std::stringstream ss;
      ss << object->getID();
      this->box(wld[0],wld[3],wld[1],wld[4],ss.str());
      this->AnnotationWriter::writeEntry(object);
    }
  }

  void CasaAnnotationWriter::text(double x, double y, std::string text)
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "text[[" << x << this->itsSpatialUnits<<"," << y << this->itsSpatialUnits<<"], '" << text<<"']\n";
    }
  }
  void CasaAnnotationWriter::line(double x1, double x2, double y1, double y2)
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "line[[" << x1 << this->itsSpatialUnits << "," << y1 << this->itsSpatialUnits << "], [" << x2 << this->itsSpatialUnits << "," << y2 << this->itsSpatialUnits << "]]\n";
    }
  }
  void CasaAnnotationWriter::circle(double x, double y, double r)
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "ann circle[[" << x << this->itsSpatialUnits << "," << y << this->itsSpatialUnits << "], " << r << this->itsSpatialUnits << "]\n";
    }
  }
  void CasaAnnotationWriter::box(double x1, double x2, double y1, double y2, std::string label)
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "box[["<<x1<<this->itsSpatialUnits<<","<<y1<<this->itsSpatialUnits<<"], ["<<x2<<this->itsSpatialUnits<<","<<y2<<this->itsSpatialUnits<<"]]";
      if(label=="") this->itsFileStream << "\n";
      else this->itsFileStream << ", label='"<<label<<"'\n";
    }
  }
  void CasaAnnotationWriter::ellipse(double x, double y, double r1, double r2, double angle)
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "ann ellipse[[" << x << this->itsSpatialUnits << "," << y << this->itsSpatialUnits << "], [" << r1 << this->itsSpatialUnits << "," << r2 << this->itsSpatialUnits << "], " << angle << "deg]\n";
    }
  }

  void CasaAnnotationWriter::joinTheDots(std::vector<double> x, std::vector<double> y)
  {
    if(this->itsOpenFlag){
      if(x.size()==y.size()){
	this->itsFileStream << "ann poly";
	for(size_t i=0;i<x.size();i++) this->itsFileStream <<" " << x[i] << " " << y[i];
	this->itsFileStream << "\n";
      }
    }
  }
  
}
