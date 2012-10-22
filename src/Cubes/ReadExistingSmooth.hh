#ifndef DUCHAMP_CUBE_READ_EXISTING_SMOOTH_HH_
#define DUCHAMP_CUBE_READ_EXISTING_SMOOTH_HH_

#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/ReadExisting.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp {

  class ReadExistingSmooth : public ReadExisting
  {
  public:
    ReadExistingSmooth();
    ReadExistingSmooth(Cube *cube);
    ReadExistingSmooth(const ReadExistingSmooth& other);
    ReadExistingSmooth(const ReadExisting& base);
    ReadExistingSmooth& operator= (const ReadExistingSmooth& other);
    ReadExistingSmooth& operator= (const ReadExisting& other);
    virtual ~ReadExistingSmooth(){};

    OUTCOME checkPars();
    OUTCOME checkFile();
    OUTCOME checkHeaders();
    OUTCOME readFromFile();

  protected:

  };

}

#endif
