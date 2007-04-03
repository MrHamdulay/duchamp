#include <iostream>
#include <iomanip>
#include <Detection/voxel.hh>


std::ostream& operator<< ( std::ostream& theStream, Voxel& vox)
{
  /**
   * A convenient way of printing the coordinate and flux values of a voxel.
   * They are all printed to a single line (with no carriage-return), with the
   * flux to precision of 4.
   */  

//   theStream << vox.itsX << " " <<vox.itsY << " " << vox.itsZ;

  theStream << std::setw(4) << vox.itsX ;
  theStream << " " << std::setw(4) << vox.itsY;
  theStream << " " << std::setw(4) << vox.itsZ;
  theStream << std::setprecision(4);
  theStream << "  " << vox.itsF;

}
//--------------------------------------------------------------------
bool operator< (Voxel lhs, Voxel rhs)
{
  if     (lhs.itsZ != rhs.itsZ) return (lhs.itsZ < rhs.itsZ);
  else if(lhs.itsY != rhs.itsY) return (lhs.itsY < rhs.itsY);
  else                          return (lhs.itsX < rhs.itsX);
}
//------------------------------------------------------

