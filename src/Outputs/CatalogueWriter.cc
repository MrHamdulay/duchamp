#include <duchamp/Outputs/CatalogueWriter.hh>

namespace duchamp {

  CatalogueWriter::CatalogueWriter()
  {
    itsName="";
    itsOpenFlag=false;
  }

  CatalogueWriter::CatalogueWriter(std::string name):
    itsName(name)
  {
    itsOpenFlag=false;
  }
  
  CatalogueWriter::CatalogueWriter(const CatalogueWriter& other)
  {
    this->operator=(other);
  }

  CatalogueWriter& CatalogueWriter::operator= (const CatalogueWriter& other)
  {
    if(this == &other) return *this;
    this->itsName = other.itsName;
    this->itsOpenFlag = other.itsOpenFlag;
    return *this;
  }


}
