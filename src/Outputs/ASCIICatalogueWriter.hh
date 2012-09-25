#ifndef DUCHAMP_ASCII_CATALOGUE_WRITER_H_
#define DUCHAMP_ASCII_CATALOGUE_WRITER_H_

#include <duchamp/Outputs/CatalogueWriter.hh>
#include <duchamp/param.hh>
#include <duchamp/Detection/detection.hh>
#include <iostream>

namespace duchamp {

  enum DESTINATION {FILE, SCREEN, LOG};

  class ASCIICatalogueWriter : public CatalogueWriter
  {
  public:
    ASCIICatalogueWriter();
    ASCIICatalogueWriter(std::string name);
    ASCIICatalogueWriter(DESTINATION dest);
    ASCIICatalogueWriter(std::string name, DESTINATION dest);
    ASCIICatalogueWriter(const ASCIICatalogueWriter& other);
    ASCIICatalogueWriter& operator= (const ASCIICatalogueWriter& other);
    virtual ~ASCIICatalogueWriter();
    
    /// @brief open the catalogue for writing
    bool openCatalogue();

    /// @brief Write header information - not including parameters
    void writeHeader();

    void writeParameters(Param &par);

    void writeTableHeader();
    void writeEntry(Detection &object);

    void writeFooter();

    bool closeCatalogue();

  protected:
    std::ostream *itsFileStream;
    DESTINATION itsDestination;

  };


}


#endif
