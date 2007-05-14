#ifndef DUCHAMP_HH
#define DUCHAMP_HH

#include <iostream>
#include <string>

#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#include "config.h"

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
const char duchampVelocityType[9] = "VELO-F2V";
/** The spectral type that we want the wcsprm structs to be in when no
    velocity info is present. */
const char duchampFrequencyType[9] = "FREQ    ";

/** Descriptions of the various spectral axis types */
enum TYPEDESC {FREQUENCY=0,VELOCITY,WAVELENGTH};
/** Human-readable descriptions of the various spectral axis types */
const std::string duchampSpectralDescription[3]=
  {"Frequency", "Velocity", "Wavelength"};

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
#endif

