#ifndef DUCHAMP_WRITE_MOMENTMAP_ARRAY_HH_
#define DUCHAMP_WRITE_MOMENTMAP_ARRAY_HH_

#include <duchamp/duchamp.hh>
#include <duchamp/FitsIO/WriteArray.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp {

  class WriteMomentMapArray : public WriteArray
  {
  public:
    WriteMomentMapArray();
    WriteMomentMapArray(Cube *cube);
    WriteMomentMapArray(const WriteMomentMapArray& other);
    WriteMomentMapArray(const WriteArray& base);
    WriteMomentMapArray& operator= (const WriteMomentMapArray& other);
    WriteMomentMapArray& operator= (const WriteArray& other);
    virtual ~WriteMomentMapArray(){};

    OUTCOME writeHeader();
    OUTCOME writeData();

  protected:

  };

}

#endif
