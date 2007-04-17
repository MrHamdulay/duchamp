#include <iostream>
#include <iomanip>
#include <PixelMap/Voxel.hh>

namespace PixelInfo
{

  Voxel::Voxel(long x, long y, long z, float f)
  { 
    this->itsX=x; 
    this->itsY=y; 
    this->itsZ=z; 
    this->itsF=f;
  }
  //--------------------------------------------------------------------

  Voxel::Voxel(const Voxel& v)
  {
    this->itsX=v.itsX; 
    this->itsY=v.itsY; 
    this->itsZ=v.itsZ; 
    this->itsF=v.itsF;
  }
  //--------------------------------------------------------------------

  Voxel& Voxel::operator= (const Voxel& v)
  {
    if(this == &v) return *this;
    this->itsX=v.itsX; 
    this->itsY=v.itsY; 
    this->itsZ=v.itsZ; 
    this->itsF=v.itsF;
    return *this;
  }
  //--------------------------------------------------------------------

  std::ostream& operator<< ( std::ostream& theStream, Voxel& vox)
  {
    /**
     * A convenient way of printing the coordinate and flux values of
     * a voxel.  They are all printed to a single line (with no
     * carriage-return), with the flux to precision of 4.
     */  

    theStream << std::setw(4) << vox.itsX ;
    theStream << " " << std::setw(4) << vox.itsY;
    theStream << " " << std::setw(4) << vox.itsZ;
    theStream << std::setprecision(4);
    theStream << "  " << vox.itsF;
    return theStream;

  }
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------

  Pixel::Pixel(long x, long y, float f)
  { 
    this->itsX=x; 
    this->itsY=y; 
    this->itsF=f;
  }
  //--------------------------------------------------------------------

  Pixel::Pixel(const Pixel& p)
  {
    this->itsX=p.itsX; 
    this->itsY=p.itsY; 
    this->itsF=p.itsF;
  }
  //--------------------------------------------------------------------

  Pixel& Pixel::operator= (const Pixel& p)
  {
    if(this == &p) return *this;
    this->itsX=p.itsX; 
    this->itsY=p.itsY; 
    this->itsF=p.itsF;
    return *this;
  }
  //--------------------------------------------------------------------

}
