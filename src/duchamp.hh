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

// how to convey whether a function has worked
enum OUTCOME {SUCCESS=0, FAILURE};

const std::string ERR_USAGE_MSG =
"Usage:: Duchamp -p [parameter file]\n\
  Other options:\n\
    -f <file>  Use default parameters with imageFile=<file>\n\
    -v         Version number\n\
    -h         This help information\n";

const std::string ERR_USAGE_MSG_SHORT="Usage:: Duchamp -p [parameter file]\n";

const std::string PROGNAME = PACKAGE_NAME;

const std::string VERSION = PACKAGE_VERSION;

// Specialised functions to report warnings and errors.
// These are defined in duchamp.cc
void duchampWarning(std::string subroutine, std::string warning);
void duchampError(std::string subroutine, std::string error);


// The spectral type that we want the wcsprm structs to be in
const char duchampVelocityType[9] = "VELO-F2V";
const char duchampFrequencyType[9] = "FREQ";

enum TYPEDESC {FREQUENCY=0,VELOCITY,WAVELENGTH};
const std::string duchampSpectralDescription[3]=
  {"Frequency", "Velocity", "Wavelength"};

// The following are the FITS Header Keywords corresponding to the
//  parameters related to the atrous reconstruction.
const std::string keyword_scaleMin     = "DU_MINSC";
const std::string keyword_snrRecon     = "DU_ATCUT";
const std::string keyword_reconDim     = "DU_ATDIM";
const std::string keyword_filterCode   = "DU_FILTR";
const std::string keyword_ReconResid   = "DU_RECON";//reconstruction/residual?
const std::string keyword_hanningwidth = "DU_WHANN"; // hanning width
const std::string keyword_subsection   = "DU_IMSUB";

// And these are the comments corresponding to the relevant keywords
const std::string comment_scaleMin     = "Duchamp parameter scaleMin";
const std::string comment_snrRecon     = "Duchamp parameter snrRecon";
const std::string comment_reconDim     = "Duchamp parameter reconDim";
const std::string comment_filterCode   = "Duchamp parameter filterCode";
const std::string comment_ReconResid   = "Is this the reconstruction or residual?";
const std::string comment_hanningwidth = "Duchamp parameter hanningWidth";
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

// Descriptive Headers: for the Hanning-smoothing case
const std::string header_smoothHistory = 
"Hanning smoothed by Duchamp v." + VERSION;
const std::string header_smoothHistory_input = 
"Original (input) image used by Duchamp follows";
const std::string header_smoothSubsection_comment  = 
"A subsection of the original was smoothed by Duchamp";

#endif

