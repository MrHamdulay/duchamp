// -----------------------------------------------------------------------
// VOTable.cc: Output of the detected objects to a VOTable
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

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <time.h>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Cubes/cubes.hh> 
#include <duchamp/Detection/detection.hh>
#include <duchamp/Outputs/columns.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/VOParam.hh>
#include <duchamp/Utils/VOField.hh>
#include <duchamp/Outputs/VOTableCatalogueWriter.hh>

namespace duchamp
{
  
  void Cube::outputDetectionsVOTable()
  {
    VOTableCatalogueWriter writer(this->pars().getVOTFile());
    writer.setup(this);
    writer.setTableName("Detections");
    writer.setTableDescription("Detected sources and parameters from running the Duchamp source finder.");
    writer.openCatalogue();
    writer.writeHeader();
    writer.writeParameters();
    writer.writeStats();
    writer.writeTableHeader();
    writer.writeEntries();
    writer.writeFooter();
    writer.closeCatalogue();

  }

  void Cube::outputDetectionsVOTable(std::ostream &stream)
  {
    /// @details
    ///  Prints to a stream (provided) the list of detected objects in the cube
    ///   in a VOTable format.
    ///  Uses WCS information and assumes WCS parameters have been calculated for each
    ///   detected object. 
    /// \param stream The output stream to send the text to.
    
    stream<<"<?xml version=\"1.0\"?>\n";
    stream<<"<VOTABLE version=\"1.1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
    stream<<" xsi:noNamespaceSchemaLocation=\"http://www.ivoa.net/xml/VOTable/VOTable/v1.1\">\n";

    std::string ID,equinox,system;
    if(std::string(this->head.WCS().lngtyp)=="RA") { //J2000 or B1950
      if(this->head.WCS().equinox==2000.){
	ID=equinox= "J2000";
	system="eq_FK5";
      }
      else{
	ID=equinox="B1950";
	system="eq_FK4";
      }
      stream <<"  <COOSYS ID=\""<<ID<<"\" equinox=\""<<equinox<<"\" system=\""<<system<<"\"/>\n";
    }
    else{
      ID=system="galactic";
      stream <<"  <COOSYS ID=\""<<ID<<"\" system=\""<<system<<"\"/>\n";
    }
    stream<<"  <RESOURCE name=\"Duchamp Output\">\n";
    stream<<"    <TABLE name=\"Detections\">\n";
    stream<<"      <DESCRIPTION>Detected sources and parameters from running the Duchamp source finder.</DESCRIPTION>\n";

    // PARAM section -- parts that are not entry-specific ie. apply to whole dataset
    std::vector<VOParam> paramList = this->par.getVOParams();
    for(std::vector<VOParam>::iterator param=paramList.begin();param<paramList.end();param++){
      stream << "      ";
      param->printParam(stream);
    }    


    if(this->fullCols.size()==0) this->setupColumns(); 
    // in case cols haven't been set -- need the column names

    std::map<std::string,std::string> posUCDmap;
    posUCDmap.insert(std::pair<std::string,std::string>("ra","pos.eq.ra"));
    posUCDmap.insert(std::pair<std::string,std::string>("dec","pos.eq.dec"));
    posUCDmap.insert(std::pair<std::string,std::string>("glon","pos.galactic.lng"));
    posUCDmap.insert(std::pair<std::string,std::string>("glat","pos.galactic.lat"));
    std::string lngUCDbase = posUCDmap[makelower(this->fullCols[Catalogues::RAJD].getName())];
    std::string latUCDbase = posUCDmap[makelower(this->fullCols[Catalogues::DECJD].getName())];

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
    std::string specUCDbase = specUCDmap[this->fullCols[Catalogues::VEL].getName()];

    std::vector<Catalogues::Column>::iterator col;
    for(col=this->fullCols.begin();col<this->fullCols.end();col++){

      if(col->doCol(Catalogues::VOTABLE,this->head.isSpecOK())){
	// VOField field;
	// field.define(*col);
	VOField field(*col); 
	if(col->type()=="RAJD")  field.setUCD(lngUCDbase+";meta.main");
	if(col->type()=="WRA")   field.setUCD("phys.angSize;"+lngUCDbase);
	if(col->type()=="DECJD") field.setUCD(latUCDbase+";meta.main");
	if(col->type()=="WDEC")  field.setUCD("phys.angSize;"+latUCDbase);	
	if(col->type()=="VEL")   field.setUCD(specUCDbase+";meta.main");
	if(col->type()=="W20")   field.setUCD("spect.line.width;"+specUCDbase);
	if(col->type()=="W50")   field.setUCD("spect.line.width;"+specUCDbase);
	if(col->type()=="WVEL")  field.setUCD("spect.line.width;"+specUCDbase);
	stream << "      ";
	field.printField(stream);
      }

    }

    stream<<"      <DATA>\n"
	  <<"        <TABLEDATA>\n";

    stream.setf(std::ios::fixed);  

    std::vector<Detection>::iterator obj;
    for(obj=this->objectList->begin();obj<this->objectList->end();obj++){

      stream<<"        <TR>\n";
      stream<<"          ";
      std::vector<Catalogues::Column>::iterator col;
      for(col=this->fullCols.begin();col<this->fullCols.end();col++){
	if(col->doCol(Catalogues::VOTABLE,this->head.isSpecOK())){
	  stream<<"<TD>";
	  obj->printTableEntry(stream, *col);
	  stream<<"</TD>";
	}
      }
      stream<<"\n";
      stream<<"        </TR>\n";

    }

    stream<<"        </TABLEDATA>\n";
    stream<<"      </DATA>\n";
    stream<<"    </TABLE>\n";
    stream<<"  </RESOURCE>\n";
    stream<<"</VOTABLE>\n";
    resetiosflags(std::ios::fixed);
  
  }




}
