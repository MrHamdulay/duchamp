// -----------------------------------------------------------------------
// lutz_detect.hh: Header file for 2D source detection
// -----------------------------------------------------------------------
// Copyright (C) 2006, Matthew Whiting, ATNF
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// Duchamp is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with Duchamp; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
//
// Correspondence concerning Duchamp may be directed to:
//    Internet email: Matthew.Whiting [at] atnf.csiro.au
//    Postal address: Dr. Matthew Whiting
//                    Australia Telescope National Facility, CSIRO
//                    PO Box 76
//                    Epping NSW 1710
//                    AUSTRALIA
// -----------------------------------------------------------------------
#include <duchamp/PixelMap/Object2D.hh>
#include <duchamp/PixelMap/Scan.hh>
#include <vector>

namespace duchamp 
{

  /// @brief The main 2D source-detection function
  std::vector<PixelInfo::Object2D> lutz_detect(std::vector<bool> &array, long xdim, long ydim, unsigned int minSize);

  /// @brief A source detection function that operates on a 1D spectrum
  std::vector<PixelInfo::Scan> spectrumDetect(std::vector<bool> &array, long dim, unsigned int minSize);

}
