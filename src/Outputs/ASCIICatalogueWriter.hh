#ifndef DUCHAMP_ASCII_CATALOGUE_WRITER_H_
#define DUCHAMP_ASCII_CATALOGUE_WRITER_H_

#include <duchamp/Outputs/CatalogueWriter.hh>
#include <duchamp/Detection/columns.hh>
#include <duchamp/Detection/detection.hh>
#include <ios>
#include <iostream>
#include <fstream>

namespace duchamp {

  class ASCIICatalogueWriter : public CatalogueWriter
  {
  public:
    ASCIICatalogueWriter();
    ASCIICatalogueWriter(std::string name);
    ASCIICatalogueWriter(Column::DESTINATION dest);
    ASCIICatalogueWriter(std::string name, Column::DESTINATION dest);
    ASCIICatalogueWriter(const ASCIICatalogueWriter& other);
    ASCIICatalogueWriter& operator= (const ASCIICatalogueWriter& other);
    virtual ~ASCIICatalogueWriter(){};
    
    /// @brief open the catalogue for writing
    bool openCatalogue(std::ios_base::openmode mode = std::ios_base::out );

    /// @brief Write header information - not including parameters
    void writeHeader();
    void writeCommandLineEntry(int argc, char *argv[]);

    void writeParameters();
    void writeStats();
    void writeTableHeader();
    void writeEntries();
    void writeEntry(Detection *object);
    void writeCubeSummary();
    void writeFooter(){};

    bool closeCatalogue();


  protected:
    std::ostream *itsStream;
    std::ofstream itsFileStream;
    Column::DESTINATION itsDestination;
     

  };


}


#endif