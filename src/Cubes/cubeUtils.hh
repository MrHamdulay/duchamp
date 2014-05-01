// -----------------------------------------------------------------------
// cubeUtils.hh: Utility functions that are not members of classes
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
#ifndef CUBEUTILS_H
#define CUBEUTILS_H

#include <iostream>
#include <string>
#include <vector>

#include <duchamp/duchamp.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Plotting/SpectralPlot.hh>
#include <duchamp/Plotting/SimpleSpectralPlot.hh>

namespace duchamp
{

  //////////////////////////////////////////////////////////////
  // Prototypes for functions that use assorted Duchamp classes
  //////////////////////////////////////////////////////////////

  /// @brief Draw the edge of the BLANK region on a map.
  void drawBlankEdges(float *dataArray, size_t xdim, size_t ydim, Param &par);

  // In Cubes/spectraUtils.cc
  void getSpecAbscissae(Detection &object, FitsHeader &head, size_t zdim, float *output);
  void getSpecAbscissae(FitsHeader &head, float xpt, float ypt, size_t zdim, float *output);
  // void getIntSpec(Detection &object, float *fluxArray, size_t *dimArray, std::vector<bool> mask, float beamCorrection, float *output);
  void getIntSpec(Detection &object, float *fluxArray, size_t *dimArray, std::vector<bool> mask, float beamCorrection, float *output);
  void getPeakSpec(Detection &object, float *fluxArray, size_t *dimArray, std::vector<bool> mask, float *output);
  void getSmallVelRange(Detection &obj, FitsHeader &head, double *minvel, double *maxvel);
  void getSmallZRange(Detection &obj, double *minz, double *maxz);

  // In Cubes/plotting.cc
  void drawSpectralRange(Plot::SpectralPlot &plot, Detection &obj, FitsHeader &head);
  void drawSpectralRange(Plot::SimpleSpectralPlot &plot, Detection &obj, FitsHeader &head);

}

#endif

