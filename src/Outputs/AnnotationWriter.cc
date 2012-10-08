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
    this->itsComment = "#";
  }

  AnnotationWriter::AnnotationWriter(std::string name):
    FileCatalogueWriter(name)
  {
    this->itsComment = "#";
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
    return *this;
  }
  

  void AnnotationWriter::writeHeader()
  {
    if(this->itsOpenFlag){
      this->itsFileStream << "# Duchamp Source Finder v."<< VERSION <<"\n";
      this->itsFileStream << this->itsComment << " Results for FITS file: " << this->itsParam->getFullImageFile() << "\n";
    }
  }

  void AnnotationWriter::writeParameters()
  {
    if(this->itsOpenFlag){
      
      if(this->itsParam->getFlagFDR())
	this->itsFileStream<<this->itsComment << " FDR Significance = " << this->itsParam->getAlpha() << "\n";
      else
	this->itsFileStream<<this->itsComment << " Threshold = " << this->itsParam->getCut() << "\n";
      if(this->itsParam->getFlagATrous()){
	this->itsFileStream<<this->itsComment << " The a trous reconstruction method was used, with the following parameters.\n";
	this->itsFileStream<<this->itsComment << "  Dimension = " << this->itsParam->getReconDim() << "\n";
	this->itsFileStream<<this->itsComment << "  Threshold = " << this->itsParam->getAtrousCut() << "\n";
	this->itsFileStream<<this->itsComment << "  Minimum Scale =" << this->itsParam->getMinScale() << "\n";
	this->itsFileStream<<this->itsComment << "  Filter = " << this->itsParam->getFilterName() << "\n";
      }
      else if(this->itsParam->getFlagSmooth()){
	this->itsFileStream<<this->itsComment << " The data was smoothed prior to searching, with the following parameters.\n";
	this->itsFileStream<<this->itsComment << "  Smoothing type = " << this->itsParam->getSmoothType() << "\n";
	if(this->itsParam->getSmoothType()=="spectral"){
	  this->itsFileStream << this->itsComment << "  Hanning width = " << this->itsParam->getHanningWidth() << "\n";
	}
	else{
	  this->itsFileStream << this->itsComment << "  Kernel Major axis = " << this->itsParam->getKernMaj() << "\n";
	  if(this->itsParam->getKernMin()>0) 
	    this->itsFileStream << this->itsComment << "  Kernel Minor axis = " << this->itsParam->getKernMin() << "\n";
	  else
	    this->itsFileStream << this->itsComment << "  Kernel Minor axis = " << this->itsParam->getKernMaj() << "\n";
	  this->itsFileStream << this->itsComment << "  Kernel Major axis = " << this->itsParam->getKernPA() << "\n";
	}
      }
      this->itsFileStream << this->itsComment << "\n";

    }
  }

  void AnnotationWriter::writeStats()
  {
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
