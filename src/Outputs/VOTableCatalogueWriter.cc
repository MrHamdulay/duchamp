#include <duchamp/Outputs/VOTableCatalogueWriter.hh>
#include <duchamp/Outputs/CatalogueWriter.hh>
#include <duchamp/Detection/columns.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/VOField.hh>
#include <duchamp/Utils/VOParam.hh>
#include <ios>
#include <iostream>
#include <fstream>

namespace duchamp {

   VOTableCatalogueWriter::VOTableCatalogueWriter():
    CatalogueWriter()
  {
    //    this->itsFileStream=0;
    this->itsTableName="";
    this->itsTableDescription="";
  }

  VOTableCatalogueWriter::VOTableCatalogueWriter(std::string name):
    CatalogueWriter(name)
  {
    //    this->itsFileStream=0;
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
    ((CatalogueWriter &) *this) = other;
    //    this->itsFileStream = other.itsFileStream;
    this->itsOpenFlag=false;
    this->itsTableName=other.itsTableName;
    this->itsTableDescription=other.itsTableDescription;
    return *this;
  }
  
  bool VOTableCatalogueWriter::openCatalogue(std::ios_base::openmode mode)
  {
    if(this->itsName == ""){
      DUCHAMPERROR("VOTableCatalogueWriter","No catalogue name provided");
      this->itsOpenFlag = false;
    }
    else {
      this->itsFileStream.open(this->itsName.c_str(),mode);
      //      this->itsOpenFlag = this->itsFileStream.is_open();
      this->itsOpenFlag = !this->itsFileStream.fail();
    }
    if(!this->itsOpenFlag) 
      DUCHAMPERROR("VOTableCatalogueWriter","Could not open file \""<<this->itsName<<"\"");
    return this->itsOpenFlag;

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

  void VOTableCatalogueWriter::writeTableHeader()
  {
    std::map<std::string,std::string> posUCDmap;
    posUCDmap.insert(std::pair<std::string,std::string>("ra","pos.eq.ra"));
    posUCDmap.insert(std::pair<std::string,std::string>("dec","pos.eq.dec"));
    posUCDmap.insert(std::pair<std::string,std::string>("glon","pos.galactic.lng"));
    posUCDmap.insert(std::pair<std::string,std::string>("glat","pos.galactic.lat"));
    Column::Col raCol=this->itsColumnSpecification->at(Column::RAJD);
    std::string lngUCDbase = posUCDmap[makelower(raCol.getName())];
    Column::Col decCol=this->itsColumnSpecification->at(Column::DECJD);
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
    std::string specUCDbase = specUCDmap[this->itsColumnSpecification->at(Column::VEL).getName()];

    std::vector<Column::Col>::iterator col;
    for(col=this->itsColumnSpecification->begin();col<this->itsColumnSpecification->end();col++){

      if(col->doCol(Column::VOTABLE,this->itsHead->isSpecOK())){
	VOField field(*col); 
	if(col->getType()==Column::RAJD)  field.setUCD(lngUCDbase+";meta.main");
	if(col->getType()==Column::WRA)   field.setUCD("phys.angSize;"+lngUCDbase);
	if(col->getType()==Column::DECJD) field.setUCD(latUCDbase+";meta.main");
	if(col->getType()==Column::WDEC)  field.setUCD("phys.angSize;"+latUCDbase);	
	if(col->getType()==Column::VEL)   field.setUCD(specUCDbase+";meta.main");
	if(col->getType()==Column::W20)   field.setUCD("spect.line.width;"+specUCDbase);
	if(col->getType()==Column::W50)   field.setUCD("spect.line.width;"+specUCDbase);
	if(col->getType()==Column::WVEL)  field.setUCD("spect.line.width;"+specUCDbase);
	this->itsFileStream << "      ";
	field.printField(this->itsFileStream);
      }

    }

    this->itsFileStream<<"      <DATA>\n"
			<<"        <TABLEDATA>\n";

  }

  void VOTableCatalogueWriter::writeEntries()
  {
    this->itsFileStream.setf(std::ios::fixed);  

    for(std::vector<Detection>::iterator obj=this->itsObjectList->begin();obj<this->itsObjectList->end();obj++)
      this->writeEntry(&*obj);
  }

  void VOTableCatalogueWriter::writeEntry(Detection *object)
  {

    this->itsFileStream<<"        <TR>\n";
    this->itsFileStream<<"          ";
    std::vector<Column::Col>::iterator col;
    for(col=this->itsColumnSpecification->begin();col<this->itsColumnSpecification->end();col++){
      if(col->doCol(Column::VOTABLE,this->itsHead->isSpecOK())){
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

  bool VOTableCatalogueWriter::closeCatalogue()
  {
    this->itsFileStream.close();
    return !this->itsFileStream.fail();
  }




}
