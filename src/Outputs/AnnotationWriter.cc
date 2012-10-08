// -----------------------------------------------------------------------
// AnnotationWriter.cc: Writing output catalogues to VOTable files
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
#include <duchamp/Outputs/AnnotationWriter.hh>
#include <duchamp/Outputs/CatalogueWriter.hh>
#include <duchamp/Outputs/FileCatalogueWriter.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/Detection/detection.hh>
#include <ios>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


namespace duchamp {

  AnnotationWriter::AnnotationWriter():
    FileCatalogueWriter()
  {
    /// @details Sets the comment string to the default of '#' and the colour to.
    this->itsComment = "#";
    this->itsColour = duchamp::annotationColour;
  }

  AnnotationWriter::AnnotationWriter(std::string name):
    FileCatalogueWriter(name)
  {
    /// @details Sets the comment string to the default of '#'.
    this->itsComment = "#";
    this->itsColour = duchamp::annotationColour;
 }

  AnnotationWriter::AnnotationWriter(const AnnotationWriter& other)
  {
    this->operator=(other);
  }

  AnnotationWriter& AnnotationWriter::operator= (const AnnotationWriter& other)
  {
    if(this==&other) return *this;
    ((FileCatalogueWriter &) *this) = other;
    this->itsComment = other.itsComment;
    this->itsColour = other.itsColour;
    return *this;
  }
  

  void AnnotationWriter::writeHeader()
  {
    /// @details Writes, as comments, two lines indicating the file
    /// comes from Duchamp (giving the version number) and the name of
    /// the FITS file (with subsection if used).

    if(this->itsOpenFlag){
      this->itsFileStream << this->itsComment << " Duchamp Source Finder v."<< VERSION <<"\n";
      this->itsFileStream << this->itsComment << " Results for FITS file: " << this->itsParam->getFullImageFile() << "\n";
    }
  }

  void AnnotationWriter::writeParameters()
  {
    /// @details Writes, as comments, a summary of the parameters used
    /// in running Duchamp. Uses the VOParam interface (as for
    /// VOTableCatalogeWriter), taking just the name and the value -
    /// this provides the effective parameter set that generated the
    /// detection list

    if(this->itsOpenFlag){
      
      std::vector<VOParam> paramList = this->itsParam->getVOParams();
      size_t width=0;
      for(std::vector<VOParam>::iterator param=paramList.begin();param<paramList.end();param++) width=std::max(width,param->name().size()+2);
      for(std::vector<VOParam>::iterator param=paramList.begin();param<paramList.end();param++){
	this->itsFileStream.setf(std::ios::left);
	this->itsFileStream << this->itsComment << " "<< std::setw(width) << param->name() << param->value() <<"\n";
      }

    }
  }

  void AnnotationWriter::writeStats()
  {
    /// @details Writes, as comments, the detection threshold, plus
    /// the mean and std.deviation (or their robust estimates) of the
    /// noise. If robust methods are used, a note is added to that
    /// effect.

    if(this->itsOpenFlag){
      this->itsFileStream << this->itsComment << " Detection threshold used = " << this->itsStats->getThreshold() <<"\n";
      this->itsFileStream << this->itsComment << " Mean of noise background = " << this->itsStats->getMiddle() << "\n";
      this->itsFileStream << this->itsComment << " Std. Deviation of noise background = " << this->itsStats->getSpread() << "\n";
      if(this->itsParam->getFlagRobustStats())
	this->itsFileStream << this->itsComment << "  [Using robust methods]\n";
    }
  }

  void AnnotationWriter::writeEntry(Detection *object)
  {
    /// @details The given object is written as an annotation. If the
    /// parameter annotationType = "borders", then vertical &
    /// horizontal lines are drawn around the spatial extent of the
    /// detection, otherwise (annotationType = "circles") a circle is
    /// drawn with radius of (half) the maximum spatial width. A text
    /// string is also written showing the object ID. In all cases,
    /// the derived class must define the actual methods for writing
    /// lines, circles, text.

    if(this->itsOpenFlag){

      if(this->itsParam->getAnnotationType() == "borders"){
	double *pix = new double[3];
	double *wld = new double[3];
	double x1,x2,y1,y2;
	std::vector<int> vertexSet = object->getVertexSet();
	for(size_t i=0;i<vertexSet.size()/4;i++){
	  pix[0] = vertexSet[i*4]-0.5;
	  pix[1] = vertexSet[i*4+1]-0.5;
	  pix[2] = object->getZcentre();
	  if(this->itsHead->isWCS()){
	    this->itsHead->pixToWCS(pix,wld);
	    x1=wld[0];
	    y1=wld[1];
	  }
	  else{
	    x1=pix[0];
	    y1=pix[1];
	  }
	  pix[0] = vertexSet[i*4+2]-0.5;
	  pix[1] = vertexSet[i*4+3]-0.5;
	  if(this->itsHead->isWCS()){
	    this->itsHead->pixToWCS(pix,wld);
	    x2=wld[0];
	    y2=wld[1];
	  }
	  else{
	    x2=wld[0];
	    y2=wld[1];
	  }
	  this->line(x1,x2,y1,y2);
	}
      }
      else if(this->itsParam->getAnnotationType()=="circles"){
        float radius = std::max(object->getRAWidth(),object->getDecWidth())/120.;
	this->circle(object->getRA(),object->getDec(),radius);
      }

      std::stringstream ss;
      ss << object->getID();
      this->text(object->getRA(),object->getDec(),ss.str());
      this->itsFileStream << "\n";

    }
  }


}
