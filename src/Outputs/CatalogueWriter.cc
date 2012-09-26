#include <duchamp/Outputs/CatalogueWriter.hh>

namespace duchamp {

  CatalogueWriter::CatalogueWriter()
  {
    itsName="";
    itsOpenFlag=false;
    this->itsParam=0;
    this->itsStats=0;
    this->itsHead=0;
    this->itsObjectList=0;
  }

  CatalogueWriter::CatalogueWriter(std::string name):
    itsName(name)
  {
    itsOpenFlag=false;
    this->itsParam=0;
    this->itsStats=0;
    this->itsHead=0;
    this->itsObjectList=0;
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
    this->itsColumnSpecification = other.itsColumnSpecification;
    this->itsParam=other.itsParam;
    this->itsStats=other.itsStats;
    this->itsHead=other.itsHead;
    this->itsObjectList=other.itsObjectList;
    return *this;
  }

  void CatalogueWriter::setup(Cube *cube)
  {
    this->itsParam = &(cube->pars());
    this->itsStats = &(cube->stats());
    this->itsHead = &(cube->header());
    this->itsObjectList = cube->pObjectList();
    this->itsColumnSpecification = cube->pFullCols();
  }


}
