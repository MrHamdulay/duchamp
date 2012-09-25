#ifndef DUCHAMP_CATALOGUE_WRITER_H_
#define DUCHAMP_CATALOGUE_WRITER_H_

#include <string>
#include <duchamp/param.hh>
#include <duchamp/Detection/detection.hh>

namespace duchamp {

  class CatalogueWriter
  {
  public:
    CatalogueWriter();
    CatalogueWriter(std::string name);
    CatalogueWriter(const CatalogueWriter& other);
    CatalogueWriter& operator= (const CatalogueWriter& other);
    virtual ~CatalogueWriter(){};
    
    /// @brief open the catalogue for writing
    virtual bool openCatalogue()=0;

    /// @brief Write header information - not including parameters
    virtual void writeHeader() = 0;

    virtual void writeParameters(Param &par) = 0;

    virtual void writeEntry(Detection &object) = 0;

    virtual void writeFooter() = 0;

    virtual bool closeCatalogue() = 0;

    void setName(std::string s){itsName=s;};
    std::string name(){return itsName;};
    bool isOpen(){return itsOpenFlag;};

  protected:
    std::string itsName;
    bool        itsOpenFlag;

  };

}


#endif
