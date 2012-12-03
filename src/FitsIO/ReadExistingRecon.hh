#ifndef DUCHAMP_CUBE_READ_EXISTING_RECON_HH_
#define DUCHAMP_CUBE_READ_EXISTING_RECON_HH_

#include <duchamp/duchamp.hh>
#include <duchamp/FitsIO/ReadExisting.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp {

  class ReadExistingRecon : public ReadExisting
  {
  public:
    ReadExistingRecon();
    ReadExistingRecon(Cube *cube);
    ReadExistingRecon(const ReadExistingRecon& other);
    ReadExistingRecon(const ReadExisting& base);
    ReadExistingRecon& operator= (const ReadExistingRecon& other);
    ReadExistingRecon& operator= (const ReadExisting& other);
    virtual ~ReadExistingRecon(){};

    OUTCOME checkPars();
    OUTCOME checkFile();
    OUTCOME checkHeaders();
    OUTCOME readFromFile();

  protected:

  };

}

#endif
