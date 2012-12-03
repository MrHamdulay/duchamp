#ifndef DUCHAMP_CUBE_READ_EXISTING_HH_
#define DUCHAMP_CUBE_READ_EXISTING_HH_

#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp {

  class ReadExisting
  {
  public:
    ReadExisting();
    ReadExisting(Cube *cube);
    ReadExisting(const ReadExisting& other);
    ReadExisting& operator= (const ReadExisting& other);
    virtual ~ReadExisting(){};

    void setCube(Cube *cube){itsCube=cube;};
    void setArray(float *array){itsArray=array;};
    void setFilename(std::string filename){itsFilename=filename;};
    void setFITSptr(fitsfile *fptr){itsFptr=fptr;};

    OUTCOME openFile();
    OUTCOME read();
    OUTCOME closeFile();

    virtual OUTCOME checkPars() = 0;
    virtual OUTCOME checkFile() = 0;
    OUTCOME checkDim();
    virtual OUTCOME checkHeaders() = 0;
    virtual OUTCOME readFromFile();

  protected:
    Cube *itsCube;
    float *itsArray;
    std::string itsFilename;
    fitsfile *itsFptr;

  };

}

#endif
