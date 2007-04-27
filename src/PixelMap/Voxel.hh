#ifndef VOXEL_H
#define VOXEL_H

#include <iostream>

namespace PixelInfo
{
  //==========================================================================

  /**
   * Voxel class.
   *  A 3-dimensional pixel, with x,y,z position + flux
   */

  class Voxel
  {
  public:
    /** Default constructor. */
    Voxel(){};
    /** Specific constructor, defining an (x,y,z) location and flux f. */
    Voxel(long x, long y, long z, float f);
    /** Copy constructor. */
    Voxel(const Voxel& v);
    /** Assignment operator. */
    Voxel& operator= (const Voxel& v);
    virtual ~Voxel(){};

    // accessor functions
    void   setX(long x){itsX = x;};
    void   setY(long y){itsY = y;};
    void   setZ(long z){itsZ = z;};
    void   setF(float f){itsF = f;};
    /* Define an (x,y) coordinate */
    void   setXY(long x, long y){itsX = x; itsY = y;};
    /* Define an (x,y,z) coordinate */
    void   setXYZ(long x, long y, long z){itsX = x; itsY = y; itsZ = z;};
    /* Define an (x,y) coordinate with a flux f */
    void   setXYF(long x, long y, float f){itsX = x; itsY = y; itsF = f;};
    /* Define an (x,y,z) coordinate with a flux f */
    void   setXYZF(long x, long y, long z, float f){itsX = x; itsY = y; itsZ = z; itsF = f;};
    long   getX(){return itsX;};
    long   getY(){return itsY;};
    long   getZ(){return itsZ;};
    float  getF(){return itsF;};
    //
    /** Operator to print information of voxel. */
    friend std::ostream& operator<< ( std::ostream& theStream, Voxel& vox);

  protected:
    long  itsX;         ///< x-position of pixel
    long  itsY;         ///< y-position of pixel
    long  itsZ;         ///< z-position of pixel
    float itsF;         ///< flux of pixel
  };

  //==========================================================================

  /**
   * Pixel class.
   *  A 2-dimensional type of voxel, with just x & y position + flux
   */


  class Pixel : public Voxel
  {
  public:
    Pixel(){itsZ=0;};
    Pixel(long x, long y, float f);
    Pixel(const Pixel& p);
    Pixel& operator= (const Pixel& p);
    virtual ~Pixel(){};
    // accessor functions
    void  setXY(long x, long y){itsX = x; itsY = y;};
    void  setXYF(long x, long y, float f){itsX = x; itsY = y; itsF = f;};

  };


}

#endif // VOXEL_H