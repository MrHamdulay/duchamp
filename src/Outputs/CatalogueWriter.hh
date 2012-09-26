#ifndef DUCHAMP_CATALOGUE_WRITER_H_
#define DUCHAMP_CATALOGUE_WRITER_H_

#include <string>
#include <vector>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Detection/columns.hh>
#include <duchamp/Utils/Statistics.hh>
#include <duchamp/Cubes/cubes.hh>

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

    virtual void writeParameters() = 0;

    virtual void writeEntry(Detection *object) = 0;

    virtual void writeFooter() = 0;

    virtual bool closeCatalogue() = 0;

    void setName(std::string s){itsName=s;};
    std::string name(){return itsName;};
    bool isOpen(){return itsOpenFlag;};
    std::vector<Column::Col> columnSpec(){return *itsColumnSpecification;};
    void setColumnSpec(std::vector<Column::Col> *cols){itsColumnSpecification = cols;};
    void setParam(Param *par){itsParam=par;};
    void setStats(Statistics::StatsContainer<float> *stats){itsStats=stats;};
    void setHead(FitsHeader *head){itsHead=head;};
    void setObjectList(std::vector<Detection> *objlist){itsObjectList=objlist;};
    void setup(Cube *cube);

  protected:
    std::string itsName;
    bool        itsOpenFlag;
    std::vector<Column::Col> *itsColumnSpecification;
    Param *itsParam;
    Statistics::StatsContainer<float> *itsStats;
    FitsHeader *itsHead;
    std::vector<Detection> *itsObjectList;

  };

}


#endif
