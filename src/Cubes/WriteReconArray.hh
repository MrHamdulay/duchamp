#ifndef DUCHAMP_WRITE_RECON_ARRAY_HH_
#define DUCHAMP_WRITE_RECON_ARRAY_HH_

#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/WriteArray.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp {

  class WriteReconArray : public WriteArray
  {
  public:
    WriteReconArray();
    WriteReconArray(Cube *cube);
    WriteReconArray(const WriteReconArray& other);
    WriteReconArray(const WriteArray& base);
    WriteReconArray& operator= (const WriteReconArray& other);
    WriteReconArray& operator= (const WriteArray& other);
    virtual ~WriteReconArray(){};

    void setIsRecon(bool b){itsIsRecon = b;};

    OUTCOME writeHeader();
    OUTCOME writeData();

  protected:
    bool itsIsRecon;
  };

}

#endif
