#ifndef OBJECT_GROWER_H
#define OBJECT_GROWER_H

#include <iostream>
#include <duchamp/duchamp.hh>
#include <Detection/detection.hh>
#include <Cubes/cubes.hh>
#include <duchamp/Utils/Statistics.hh>

namespace duchamp {

  /// @brief Flags defining the state of each pixel
  enum STATE {AVAILABLE, DETECTED, BLANK, MW};

  /// @brief A class to manage the growing of objects to a secondary
  /// threshold
  /// @details This class provides a mechanism for handling the
  /// growing of objects. By keeping track of the state of each pixel,
  /// through an array of flags indicating whether a pixel is
  /// available or in an object, it is able to efficiently grow the
  /// objects from pixels on the edge, rather than spending time
  /// examining pixels that are completely surrounded by other object
  /// pixels.
  class ObjectGrower
  {
  public:
    /// @brief Default constructor
    ObjectGrower();
    /// @brief Destrctor
    virtual ~ObjectGrower(){};
    /// @brief Copy constructor
    ObjectGrower(ObjectGrower &o);
    /// @brief Copy operator
    ObjectGrower& operator=(const ObjectGrower &o);

    /// @brief Set up the class with parameters & pointers from the cube
    void define(Cube *theCube);
    /// @brief Update a Cube's detectMap based on the flag array
    void updateDetectMap(short *map);
    /// @brief Grow an object
    virtual void grow(Detection *theObject);
    /// @brief Grow out from a single voxel, returning the list of new voxels.
    std::vector<Voxel> growFromPixel(Voxel &vox);

  protected:
    std::vector<STATE> itsFlagArray;                   ///< The array of pixel flags
    std::vector<size_t> itsArrayDim;                     ///< The dimensions of the array
    Statistics::StatsContainer<float> itsGrowthStats;  ///< The statistics used to determine membership of an object
    int itsSpatialThresh;                              ///< The spatial threshold for merging
    int itsVelocityThresh;                             ///< The spectral threshold for merging
    float* itsFluxArray;                               ///< The location of the pixel values
  };

}


#endif
