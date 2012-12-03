#ifndef DUCHAMP_WRITE_MASK_ARRAY_HH_
#define DUCHAMP_WRITE_MASK_ARRAY_HH_

#include <duchamp/duchamp.hh>
#include <duchamp/FitsIO/WriteArray.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp {

  class WriteMaskArray : public WriteArray
  {
  public:
    WriteMaskArray();
    WriteMaskArray(Cube *cube);
    WriteMaskArray(const WriteMaskArray& other);
    WriteMaskArray(const WriteArray& base);
    WriteMaskArray& operator= (const WriteMaskArray& other);
    WriteMaskArray& operator= (const WriteArray& other);
    virtual ~WriteMaskArray(){};

    OUTCOME writeHeader();
    OUTCOME writeData();

  protected:

  };

}

#endif
