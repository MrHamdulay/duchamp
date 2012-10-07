#ifndef DUCHAMP_VOTABLE_CATALOGUE_WRITER_H_
#define DUCHAMP_VOTABLE_CATALOGUE_WRITER_H_

#include <duchamp/Outputs/CatalogueWriter.hh>
#include <duchamp/Detection/detection.hh>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

namespace duchamp {

  class VOTableCatalogueWriter : public CatalogueWriter
  {
  public:
    VOTableCatalogueWriter();
    VOTableCatalogueWriter(std::string name);
    VOTableCatalogueWriter(const VOTableCatalogueWriter& other);
    VOTableCatalogueWriter& operator= (const VOTableCatalogueWriter& other);
    virtual ~VOTableCatalogueWriter(){};

    /// @brief open the catalogue for writing
    bool openCatalogue(std::ios_base::openmode mode = std::ios_base::out);

    /// @brief Write header information - not including parameters
    void writeHeader();

    void writeParameters();
    void writeStats(); 
    void writeTableHeader();
    void writeEntries();
    void writeEntry(Detection *object);

    void writeFooter();

    bool closeCatalogue();

    void setTableName(std::string s){itsTableName=s;};
    void setTableDescription(std::string s){itsTableDescription=s;};

  protected:
    std::ofstream itsFileStream;
    std::string itsTableName;
    std::string itsTableDescription;
  };

}

#endif
