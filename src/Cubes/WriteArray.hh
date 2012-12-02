#ifndef DUCHAMP_WRITE_ARRAY_HH_
#define DUCHAMP_WRITE_ARRAY_HH_

#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>
#include <fitsio.h>

namespace duchamp {

  class WriteArray
  {
  public:
    WriteArray();
    WriteArray(Cube *cube);
    WriteArray(Cube *cube, int bitpix);
    WriteArray(const WriteArray& other);
    WriteArray& operator= (const WriteArray& other);
    virtual ~WriteArray(){};

    void setCube(Cube *cube){itsCube=cube;};
    void setBitpix(int bitpix){itsBitpix = bitpix;};
    void setFilename(std::string filename){itsFilename=filename;};
    void setFITSptr(fitsfile *fptr){itsFptr=fptr;};
    void set2D(bool b){itsFlag2D=b;};

    OUTCOME openFile();
    OUTCOME writeBasicHeader();
    virtual OUTCOME writeHeader() = 0;
    virtual OUTCOME writeData() = 0;
    OUTCOME closeFile();

    OUTCOME write();

  protected:
    Cube *itsCube;
    std::string itsFilename;
    int itsBitpix;
    bool itsFlag2D;
    fitsfile *itsFptr;
  };

}

#endif
