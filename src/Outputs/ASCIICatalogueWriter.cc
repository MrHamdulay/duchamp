#include <duchamp/Outputs/ASCIICatalogueWriter.hh>
#include <duchamp/Outputs/CatalogueWriter.hh>
#include <duchamp/param.hh>
#include <duchamp/Detection/detection.hh>
#include <iostream>
#include <fstream>

namespace duchamp {

  ASCIICatalogueWriter::ASCIICatalogueWriter():
    CatalogueWriter()
  {
  }

  ASCIICatalogueWriter::ASCIICatalogueWriter(std::string name):
    CatalogueWriter(name)
  {
  }

  ASCIICatalogueWriter::ASCIICatalogueWriter(DESTINATION dest):
    itsDestination(dest)
  {
  }

  ASCIICatalogueWriter::ASCIICatalogueWriter(std::string name, DESTINATION dest):
    CatalogueWriter(name), itsDestination(dest)
  {
  }

  ASCIICatalogueWriter::ASCIICatalogueWriter(const ASCIICatalogueWriter& other)
  {
    this->operator=(other);
  }

  ASCIICatalogueWriter& ASCIICatalogueWriter::operator= (const ASCIICatalogueWriter& other)
  {
    if(this==&other) return *this;
    ((CatalogueWriter &) *this) = other;
    this->itsFileStream = other.itsFileStream;
    return *this;
  }
  
  bool ASCIICatalogueWriter::openCatalogue()
  {
    if(this->itsName == ""){
      DUCHAMPERROR("ASCIICatalogueWriter","No catalogue name provided");
      this->itsOpenFlag = false;
    }
    else {
      if(this->itsDestination==SCREEN){
	this->itsFileStream = &std::cout;
	this->itsOpenFlag = true;
      }
      else{
	std::ofstream file(this->itsName.c_str());
	this->itsFileStream = &file;
	this->itsOpenFlag = file.is_open();
      }
      if(!this->itsOpenFlag) 
	DUCHAMPERROR("ASCIICatalogueWriter","Could not open file \""<<this->itsName<<"\"");
    }
  }

  void ASCIICatalogueWriter::writeHeader()
  {
    if(this->itsOpenFlag){
      *this->itsFileStream <<"Results of the Duchamp source finder v."<<VERSION<<": ";
      time_t now = time(NULL);
      *this->itsFileStream << asctime( localtime(&now) );
    }
  }

  void ASCIICatalogueWriter::writeParameters(Param &par)
  {
    if(this->itsOpenFlag){
      *this->itsFileStream << par;
      *this->itsFileStream <<"--------------------\n";
    }
  }

  void ASCIICatalogueWriter::writeEntry(Detection &object)
  {

  }

}
