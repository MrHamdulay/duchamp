// -----------------------------------------------------------------------
// duchamp.hh: Definitions for use with Duchamp
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
#ifndef DUCHAMP_HH
#define DUCHAMP_HH

#include <iostream>
#include <string>
#include <duchamp/Utils/mycpgplot.hh>

#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef HAVE_PGPLOT
#include "config.h"

namespace duchamp
{

  /** how to convey whether a function has worked */
  enum OUTCOME {SUCCESS=0, FAILURE};

  /** Usage message for command line help. */
  const std::string ERR_USAGE_MSG =
    "Usage: Duchamp [OPTION] [FILE]\n\
Duchamp is an object finder for spectral-line FITS cubes.\n\
\n\
  -p FILE      Read in parameters from FILE, including FITS image location.\n\
  -f FILE      Use default parameters with imageFile=FILE\n\
  -x           Do not use X-windows PGPLOT output\n\
               (equivalent to setting flagXOutput=false -- overrides the parameter file)\n\n\
  -v           Return version number and exit\n\
  -h           Display this help information and exit\n";

  /** Usage message for command line help for Selavy. */
  const std::string SELAVY_ERR_USAGE_MSG =
    "Usage: Selavy [OPTION] [FILE]\n\
Selavy re-analyses and re-plots objects found by Duchamp.\n\
\n\
  -p FILE      Read in parameters from FILE, including FITS image location.\n\
  -f FILE      Use default parameters with imageFile=FILE\n\
  -x           Do not use X-windows PGPLOT output\n\
               (equivalent to setting flagXOutput=false -- overrides the parameter file)\n\n\
  -v           Return version number and exit\n\
  -h           Display this help information and exit\n";

  /** Shorter Usage message for command line help. */
  const std::string ERR_USAGE_MSG_SHORT="Usage: Duchamp -p [parameter file]\n";

  /** The program name. (Duchamp) */
  const std::string PROGNAME = PACKAGE_NAME;

  /** The program version */
  const std::string VERSION = PACKAGE_VERSION;

  // Specialised functions to report warnings and errors -- in duchamp.cc
  /** Print a warning message to the stderr */
  void duchampWarning(std::string subroutine, std::string warning);
  /** Print an error message to the stderr and sound the bell */
  void duchampError(std::string subroutine, std::string error);


  /** The spectral type that we want the wcsprm structs to be in. */
  const char duchampVelocityType[9] = "VOPT-F2W";
  /** The spectral type that we want the wcsprm structs to be in when no
      velocity info is present. */
  const char duchampFrequencyType[9] = "FREQ    ";

  /** Descriptions of the various spectral axis types */
  enum TYPEDESC {FREQUENCY=0,VELOCITY,WAVELENGTH};
  /** Human-readable descriptions of the various spectral axis types */
  const std::string duchampSpectralDescription[3]=
    {"Frequency", "Velocity", "Wavelength"};

  // Colours used in graphical output
  /** The colour for the Blank edges */
  const int DUCHAMP_BLANK_EDGE_COLOUR = mycpgplot::MAGENTA;
  /** The colour for the edge of the cube */
  const int DUCHAMP_CUBE_EDGE_COLOUR = mycpgplot::YELLOW;
  /** The colour for the reconstructed spectra */
  const int DUCHAMP_RECON_SPECTRA_COLOUR = mycpgplot::RED;
  /** The colour for the baseline spectra */
  const int DUCHAMP_BASELINE_SPECTRA_COLOUR = mycpgplot::YELLOW;
  /** The colour for the object outline */
  const int DUCHAMP_OBJECT_OUTLINE_COLOUR = mycpgplot::BLUE;
  /** The colour for the Milky-way region spectral boundaries */
  const int DUCHAMP_MILKY_WAY_COLOUR = mycpgplot::DARKGREEN;
  /** The colour for the tick marks in the image cutouts */
  const int DUCHAMP_TICKMARK_COLOUR = mycpgplot::RED;
  /** The colour for the text identifying objects on the maps */
  const int DUCHAMP_ID_TEXT_COLOUR = mycpgplot::RED;
  /** The colour for the WCS axes on the maps */
  const int DUCHAMP_WCS_AXIS_COLOUR = mycpgplot::WCSGREEN;

  // The following are the FITS Header Keywords corresponding to the
  // parameters related to the atrous reconstruction.
  /** FITS header keyword for min atrous scale*/
  const std::string keyword_scaleMin     = "DU_MINSC"; 
  /** FITS header keyword for S/N used in atrous reconstruction*/
  const std::string keyword_snrRecon     = "DU_ATCUT";
  /** FITS header keyword for number of dimensions used in atrous
      reconstruction*/
  const std::string keyword_reconDim     = "DU_ATDIM";
  /** FITS header keyword for the code number of the filter used in
      atrous reconstruction*/
  const std::string keyword_filterCode   = "DU_FILTR";
  /** FITS header keyword: does this file hold the reconstructed array
      or the residual?*/
  const std::string keyword_ReconResid   = "DU_RECON";
  /** FITS header keyword: type of smoothing done. */
  const std::string keyword_smoothtype   = "DU_SMTYP"; 
  /** FITS header keyword for the gaussian kernel major axis FWHM*/
  const std::string keyword_kernmaj      = "DU_KMAJ"; 
  /** FITS header keyword for the gaussian kernel minor axis FWHM*/
  const std::string keyword_kernmin      = "DU_KMIN"; 
  /** FITS header keyword for the gaussian kernel position angle*/
  const std::string keyword_kernpa       = "DU_KPA"; 
  /** FITS header keyword for the Hanning filter width*/
  const std::string keyword_hanningwidth = "DU_WHANN"; 
  /** FITS header keyword for the image subsection used*/
  const std::string keyword_subsection   = "DU_IMSUB";

  // And these are the comments corresponding to the relevant keywords
  /** FITS header comment for DU_MINSC keyword*/
  const std::string comment_scaleMin     = "Duchamp parameter scaleMin";
  /** FITS header comment for DU_ATCUT keyword*/
  const std::string comment_snrRecon     = "Duchamp parameter snrRecon";
  /** FITS header comment for DU_ATDIM keyword*/
  const std::string comment_reconDim     = "Duchamp parameter reconDim";
  /** FITS header comment for DU_FILTR keyword*/
  const std::string comment_filterCode   = "Duchamp parameter filterCode";
  /** FITS header comment for DU_RECON keyword*/
  const std::string comment_ReconResid   = "Is this the reconstruction or residual?";
  /** FITS header comment for DU_SMTYP keyword*/
  const std::string comment_smoothtype   = "Type of smoothing done";
  /** FITS header comment for DU_KMAJ  keyword*/
  const std::string comment_kernmaj      = "Duchamp parameter kernMaj";
  /** FITS header comment for DU_KMIN  keyword*/
  const std::string comment_kernmin      = "Duchamp parameter kernMin";
  /** FITS header comment for DU_KPA   keyword*/
  const std::string comment_kernpa       = "Duchamp parameter kernPA";
  /** FITS header comment for DU_WHANN keyword*/
  const std::string comment_hanningwidth = "Duchamp parameter hanningWidth";
  /** FITS header comment for DU_IMSUB keyword*/
  const std::string comment_subsection   = "Subsection of the original image";

  // Descriptive Headers: for the reconstruction case
  const std::string header_reconHistory1 = 
    "Reconstructed with a trous wavelet technique";
  const std::string header_reconHistory2 = 
    "Reconstruction by Duchamp v." + VERSION;
  const std::string header_reconHistory_input = 
    "Original (input) image used by Duchamp follows";
  const std::string header_reconSubsection_comment  = 
    "A subsection of the original was reconstructed by Duchamp";
  const std::string header_atrous_comment  = 
    "The following are the Duchamp parameters used in reconstruction";

  // Descriptive Headers: for the Smoothing case
  const std::string header_smoothHistory = 
    "Smoothed by Duchamp v." + VERSION;
  const std::string header_smoothHistory_input = 
    "Original (input) image used by Duchamp follows";
  const std::string header_smoothSubsection_comment  = 
    "A subsection of the original was smoothed by Duchamp";
  const std::string header_smoothSpatial = "Spatial, gaussian kernel";
  const std::string header_smoothSpectral= "Spectral, hanning filter";

  // Descriptive Headers: for the output Mask image
  const std::string header_maskHistory =
    "Results of searching by Duchamp v." + VERSION;
  const std::string header_maskHistory_input = 
    "Input image used by Duchamp follows";
  const std::string header_maskSubsection_comment = 
    "A subsection of the original was searched by Duchamp";

}

#endif

