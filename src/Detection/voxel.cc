#include <iostream>
#include <Detection/voxel.hh>


std::ostream& operator<< ( std::ostream& theStream, Voxel& vox)
{
  /**
   * A convenient way of printing the coordinate and flux values of a voxel.
   * They are all printed to a single line (with no carriage-return), with the
   * flux to precision of 4.
   */  

  theStream << setw(4) << vox.itsX ;
  theStream << " " << setw(4) << vox.itsY;
  theStream << " " << setw(4) << vox.itsZ;
  theStream << setprecision(4);
  theStream << "  " << vox.itsF;

}
//--------------------------------------------------------------------
