#include <string.h>

// how to convey whether a function has worked
enum OUTCOME {SUCCESS, FAILURE};


const std::string ERR_USAGE_MSG = 
"Usage:: Duchamp.x -p [parameter file]\n\
  Other options:\n\
    -v       Version number\n\
    -h       This help information\n";

const std::string ERR_USAGE_MSG_SHORT = "Usage:: Duchamp.x -p [parameter file]\n";

const std::string PROGNAME = "Duchamp";

const std::string VERSION = "0.9.1";

// The following are the FITS Header Keywords corresponding to the 
//  parameters related to the atrous reconstruction.
const std::string keyword_scaleMin   = "DU_MINSC";
const std::string keyword_snrRecon   = "DU_ATCUT";
const std::string keyword_filterCode = "DU_FILTR";
const std::string keyword_ReconResid = "DU_RECON"; // is this file the reconstruction or the residual?
// And these are the comments corresponding to the relevant keywords
const std::string comment_scaleMin   = "Duchamp parameter scaleMin";
const std::string comment_snrRecon   = "Duchamp parameter snrRecon";
const std::string comment_filterCode = "Duchamp parameter filterCode";
const std::string comment_ReconResid = "Is this the reconstruction or residual?";
// Descriptive Headers:
const std::string header_history1 = "Reconstructed with a trous wavelet technique";
const std::string header_history2 = "Reconstruction by Duchamp v." + VERSION;
const std::string header_comment  = "The following are the Duchamp parameters used in reconstruction";
