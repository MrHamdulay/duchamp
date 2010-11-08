#ifndef OBJECT_GROWER_H
#define OBJECT_GROWER_H

#include <iostream>
#include <duchamp/duchamp.hh>
#include <Detection/detection.hh>
#include <Cubes/cubes.hh>
#include <duchamp/Utils/Statistics.hh>

namespace duchamp {


  enum STATE {AVAILABLE, DETECTED, BLANK, MW};

  class ObjectGrower
  {
  public:
    ObjectGrower();
    virtual ~ObjectGrower(){};
    ObjectGrower(ObjectGrower &o);
    ObjectGrower& operator=(const ObjectGrower &o);

    void define(Cube *theCube);
    void grow(Detection *theObject);

  protected:
    std::vector<STATE> itsFlagArray;
    std::vector<long> itsArrayDim;
    Statistics::StatsContainer<float> itsGrowthStats;
    int itsSpatialThresh;
    int itsVelocityThresh;
    float* itsFluxArray;
  };

}


#endif
