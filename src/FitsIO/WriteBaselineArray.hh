#ifndef DUCHAMP_WRITE_BASELINE_ARRAY_HH_
#define DUCHAMP_WRITE_BASELINE_ARRAY_HH_

#include <duchamp/duchamp.hh>
#include <duchamp/FitsIO/WriteArray.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp {

  class WriteBaselineArray : public WriteArray
  {
  public:
    WriteBaselineArray();
    WriteBaselineArray(Cube *cube);
    WriteBaselineArray(const WriteBaselineArray& other);
    WriteBaselineArray(const WriteArray& base);
    WriteBaselineArray& operator= (const WriteBaselineArray& other);
    WriteBaselineArray& operator= (const WriteArray& other);
    virtual ~WriteBaselineArray(){};

    OUTCOME writeHeader();
    OUTCOME writeData();

  protected:

  };

}

#endif
