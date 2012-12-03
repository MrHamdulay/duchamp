#ifndef DUCHAMP_WRITE_SMOOTH_ARRAY_HH_
#define DUCHAMP_WRITE_SMOOTH_ARRAY_HH_

#include <duchamp/duchamp.hh>
#include <duchamp/FitsIO/WriteArray.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp {

  class WriteSmoothArray : public WriteArray
  {
  public:
    WriteSmoothArray();
    WriteSmoothArray(Cube *cube);
    WriteSmoothArray(const WriteSmoothArray& other);
    WriteSmoothArray(const WriteArray& base);
    WriteSmoothArray& operator= (const WriteSmoothArray& other);
    WriteSmoothArray& operator= (const WriteArray& other);
    virtual ~WriteSmoothArray(){};

    OUTCOME writeHeader();
    OUTCOME writeData();

  protected:

  };

}

#endif
