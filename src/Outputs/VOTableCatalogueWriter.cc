// -----------------------------------------------------------------------
// VOTableCatalogueWriter.cc: Writing output catalogues to VOTable files
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
#include <duchamp/Outputs/VOTableCatalogueWriter.hh>
#include <duchamp/Outputs/CatalogueWriter.hh>
#include <duchamp/Outputs/FileCatalogueWriter.hh>
#include <duchamp/Outputs/columns.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/VOField.hh>
#include <duchamp/Utils/VOParam.hh>
#include <ios>
#include <iostream>
#include <fstream>

namespace duchamp {

   VOTableCatalogueWriter::VOTableCatalogueWriter():
    FileCatalogueWriter()
  {
    this->itsTableName="";
    this->itsTableDescription="";
  }

  VOTableCatalogueWriter::VOTableCatalogueWriter(std::string name):
    FileCatalogueWriter(name)
  {
    this->itsTableName="";
    this->itsTableDescription="";
  }

  VOTableCatalogueWriter::VOTableCatalogueWriter(const VOTableCatalogueWriter& other)
  {
    this->operator=(other);
  }

  VOTableCatalogueWriter& VOTableCatalogueWriter::operator= (const VOTableCatalogueWriter& other)
  {
    if(this==&other) return *this;
    ((FileCatalogueWriter &) *this) = other;
    this->itsTableName=other.itsTableName;
    this->itsTableDescription=other.itsTableDescription;
    return *this;
  }
  
  void VOTableCatalogueWriter::writeHeader()
  {
    if(this->itsOpenFlag){

      this->itsFileStream<<"<?xml version=\"1.0\"?>\n";
      this->itsFileStream<<"<VOTABLE version=\"1.1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
      this->itsFileStream<<" xsi:noNamespaceSchemaLocation=\"http://www.ivoa.net/xml/VOTable/VOTable/v1.1\">\n";
      
      std::string ID,equinox,system;
      if(std::string(this->itsHead->WCS().lngtyp)=="RA") { //J2000 or B1950
	if(this->itsHead->WCS().equinox==2000.){
	  ID=equinox= "J2000";
	  system="eq_FK5";
	}
	else{
	  ID=equinox="B1950";
	  system="eq_FK4";
	}
	this->itsFileStream <<"  <COOSYS ID=\""<<ID<<"\" equinox=\""<<equinox<<"\" system=\""<<system<<"\"/>\n";
      }
      else{
	ID=system="galactic";
	this->itsFileStream <<"  <COOSYS ID=\""<<ID<<"\" system=\""<<system<<"\"/>\n";
      }
      this->itsFileStream<<"  <RESOURCE name=\"Duchamp Output\">\n";
      this->itsFileStream<<"    <TABLE name=\""<<this->itsTableName<<"\">\n";
      this->itsFileStream<<"      <DESCRIPTION>"<<this->itsTableDescription<<"</DESCRIPTION>\n";

      
    }
  }

  void VOTableCatalogueWriter::writeParameters()
  {
    if(this->itsOpenFlag){
      std::vector<VOParam> paramList = this->itsParam->getVOParams();
      for(std::vector<VOParam>::iterator param=paramList.begin();param<paramList.end();param++){
	this->itsFileStream << "      ";
	param->printParam(this->itsFileStream);
      }    
    }
  }

  void VOTableCatalogueWriter::writeStats()
  {
    if(this->itsOpenFlag){
      
      VOParam threshParam("thresholdActual","","float",this->itsStats->getThreshold(),0,this->itsHead->getFluxUnits());
      this->itsFileStream << "      ";
      threshParam.printParam(this->itsFileStream);
      if(!this->itsParam->getFlagUserThreshold()){
	VOParam middleParam("noiseMeanActual","","float",this->itsStats->getMiddle(),0,this->itsHead->getFluxUnits());
	this->itsFileStream << "      ";
	middleParam.printParam(this->itsFileStream);
	VOParam spreadParam("noiseSpreadActual","","float",this->itsStats->getSpread(),0,this->itsHead->getFluxUnits());
	this->itsFileStream << "      ";
	spreadParam.printParam(this->itsFileStream);
      }
    }
  }

  void VOTableCatalogueWriter::writeTableHeader()
  {
    std::map<std::string,std::string> posUCDmap;
    posUCDmap.insert(std::pair<std::string,std::string>("ra","pos.eq.ra"));
    posUCDmap.insert(std::pair<std::string,std::string>("dec","pos.eq.dec"));
    posUCDmap.insert(std::pair<std::string,std::string>("glon","pos.galactic.lng"));
    posUCDmap.insert(std::pair<std::string,std::string>("glat","pos.galactic.lat"));
    Catalogues::Column &raCol=this->itsColumnSpecification->column("RAJD");
    std::string lngUCDbase = posUCDmap[makelower(raCol.getName())];
    Catalogues::Column &decCol=this->itsColumnSpecification->column("DECJD");
    std::string latUCDbase = posUCDmap[makelower(decCol.getName())];

    std::map<std::string,std::string> specUCDmap;
    specUCDmap.insert(std::pair<std::string,std::string>("VELO","phys.veloc;spect.dopplerVeloc"));
    specUCDmap.insert(std::pair<std::string,std::string>("VOPT","phys.veloc;spect.dopplerVeloc.opt"));
    specUCDmap.insert(std::pair<std::string,std::string>("VRAD","phys.veloc;spect.dopplerVeloc.rad"));
    specUCDmap.insert(std::pair<std::string,std::string>("FREQ","em.freq"));
    specUCDmap.insert(std::pair<std::string,std::string>("ENER","em.energy"));
    specUCDmap.insert(std::pair<std::string,std::string>("WAVN","em.wavenumber"));
    specUCDmap.insert(std::pair<std::string,std::string>("WAVE","em.wl"));
    specUCDmap.insert(std::pair<std::string,std::string>("AWAV","em.wl"));
    specUCDmap.insert(std::pair<std::string,std::string>("ZOPT","src.redshift"));
    specUCDmap.insert(std::pair<std::string,std::string>("BETA","src.redshift; spect.dopplerVeloc"));
    std::string specUCDbase = specUCDmap[this->itsColumnSpecification->column("VEL").getName()];

    for(size_t i=0;i<this->itsColumnSpecification->size();i++){
      
      Catalogues::Column *col = this->itsColumnSpecification->pCol(i);
      bool useFint=this->itsHead->isSpecOK();
      if(col->doCol(Catalogues::VOTABLE,useFint)){
	VOField field(*col); 
	if(col->type()=="RA")    field.setUCD(lngUCDbase);
	if(col->type()=="RAJD")  field.setUCD(lngUCDbase+";meta.main");
	if(col->type()=="WRA")   field.setUCD("phys.angSize;"+lngUCDbase);
	if(col->type()=="DEC")   field.setUCD(latUCDbase);
	if(col->type()=="DECJD") field.setUCD(latUCDbase+";meta.main");
	if(col->type()=="WDEC")  field.setUCD("phys.angSize;"+latUCDbase);	
	if(col->type()=="VEL")   field.setUCD(specUCDbase+";meta.main");
	if(col->type()=="W20")   field.setUCD("spect.line.width.20;"+specUCDbase);
	if(col->type()=="W50")   field.setUCD("spect.line.width.50;"+specUCDbase);
	if(col->type()=="WVEL")  field.setUCD("spect.line.width.full;"+specUCDbase);
	if(!this->itsHead->is2D()){
	  if(col->type()=="FINT") field.setUCD(field.UCD()+"spect.line.intensity");
	  if(col->type()=="FTOT") field.setUCD(field.UCD()+"spect.line.intensity");
	  if(col->type()=="FPEAK") field.setUCD(field.UCD()+"spect.line.intensity");
	}
	this->itsFileStream << "      ";
	field.printField(this->itsFileStream);
      }

    }

    this->itsFileStream<<"      <DATA>\n"
			<<"        <TABLEDATA>\n";

  }

  void VOTableCatalogueWriter::writeEntry(Detection *object)
  {
    this->itsFileStream.setf(std::ios::fixed);  

    this->itsFileStream<<"        <TR>\n";
    this->itsFileStream<<"          ";
    for(size_t i=0;i<this->itsColumnSpecification->size();i++){
      Catalogues::Column *col = this->itsColumnSpecification->pCol(i);
      if(col->doCol(Catalogues::VOTABLE,this->itsHead->isSpecOK())){
	this->itsFileStream<<"<TD>";
	object->printTableEntry(this->itsFileStream, *col);
	this->itsFileStream<<"</TD>";
      }
    }
    this->itsFileStream<<"\n";
    this->itsFileStream<<"        </TR>\n";

  }

  void VOTableCatalogueWriter::writeFooter()
  {
    this->itsFileStream<<"        </TABLEDATA>\n";
    this->itsFileStream<<"      </DATA>\n";
    this->itsFileStream<<"    </TABLE>\n";
    this->itsFileStream<<"  </RESOURCE>\n";
    this->itsFileStream<<"</VOTABLE>\n";
    resetiosflags(std::ios::fixed);
  }




}
