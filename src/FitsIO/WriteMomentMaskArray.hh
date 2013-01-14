#ifndef DUCHAMP_WRITE_MOMENTMASK_ARRAY_HH_
#define DUCHAMP_WRITE_MOMENTMASK_ARRAY_HH_

#include <duchamp/duchamp.hh>
#include <duchamp/FitsIO/WriteArray.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp {

  class WriteMomentMaskArray : public WriteArray
  {
  public:
    WriteMomentMaskArray();
    WriteMomentMaskArray(Cube *cube);
    WriteMomentMaskArray(const WriteMomentMaskArray& other);
    WriteMomentMaskArray(const WriteArray& base);
    WriteMomentMaskArray& operator= (const WriteMomentMaskArray& other);
    WriteMomentMaskArray& operator= (const WriteArray& other);
    virtual ~WriteMomentMaskArray(){};

    OUTCOME writeHeader();
    OUTCOME writeData();

  protected:

  };

}

#endif
