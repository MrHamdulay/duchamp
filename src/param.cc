// -----------------------------------------------------------------------
// param.cc: Dealing with the set of parameters for Duchamp.
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/pgheader.hh>
#include <duchamp/ATrous/filter.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/Section.hh>
#include <duchamp/Detection/columns.hh>

namespace duchamp
{

  /****************************************************************/
  ///////////////////////////////////////////////////
  //// Accessor Functions for Parameter class:
  ///////////////////////////////////////////////////
  Param::~Param()
  {
    /**
     * Deletes the offsets array if the sizeOffsets parameter is
     * positive.
     */
    if(this->sizeOffsets>0) delete [] this->offsets;
  }

  Param::Param()
  {
    this->defaultValues();
  }

  void Param::defaultValues()
  {
    /** 
     * Provides default intial values for the parameters. Note that
     * imageFile has no default value!
     */
    std::string baseSection = "[*,*,*]";
    // Input files
    this->imageFile         = "";
    this->flagSubsection    = false;
    this->pixelSec.setSection(baseSection);
    this->flagReconExists   = false;
    this->reconFile         = "";
    this->flagSmoothExists  = false;
    this->smoothFile        = "";
    // Output files
    this->flagLog           = false;
    this->logFile           = "duchamp-Logfile.txt";
    this->outFile           = "duchamp-Results.txt";
    this->flagSeparateHeader= false;
    this->headerFile        = "duchamp-Results.hdr";
    this->spectraFile       = "duchamp-Spectra.ps";
    this->flagTextSpectra   = false;
    this->spectraTextFile   = "duchamp-Spectra.txt";
    this->flagOutputMask    = false;
    this->flagOutputSmooth  = false;
    this->flagOutputRecon   = false;
    this->flagOutputResid   = false;
    this->flagVOT           = false;
    this->votFile           = "duchamp-Results.xml";
    this->flagKarma         = false;
    this->karmaFile         = "duchamp-Results.ann";
    this->annotationType    = "borders";
    this->flagMaps          = true;
    this->detectionMap      = "duchamp-DetectionMap.ps";
    this->momentMap         = "duchamp-MomentMap.ps";
    this->flagXOutput       = true;
    this->precFlux          = Column::prFLUX;
    this->precVel           = Column::prVEL;
    this->precSNR           = Column::prSNR;
    // Cube related parameters 
    this->flagBlankPix      = true;
    this->blankPixValue     = -8.00061;
    this->blankKeyword      = 1;
    this->bscaleKeyword     = -8.00061;
    this->bzeroKeyword      = 0.;
    this->newFluxUnits      = "";
    // Milky-Way parameters
    this->flagMW            = false;
    this->maxMW             = 112;
    this->minMW             = 75;
    this->numPixBeam        = 10.;
    this->flagUsingBeam     = false;
    // Trim-related         
    this->flagTrim          = false;
    this->hasBeenTrimmed    = false;
    this->borderLeft        = 0;
    this->borderRight       = 0;
    this->borderBottom      = 0;
    this->borderTop         = 0;
    // Subsection offsets
    this->sizeOffsets       = 0;
    this->xSubOffset        = 0;
    this->ySubOffset        = 0;
    this->zSubOffset        = 0;
    // Baseline related
    this->flagBaseline      = false;
    // Detection-related    
    this->flagNegative      = false;
    // Object growth        
    this->flagGrowth        = false;
    this->growthCut         = 3.;
    this->flagUserGrowthThreshold = false;
    this->growthThreshold   = 0.;
    // FDR analysis         
    this->flagFDR           = false;
    this->alphaFDR          = 0.01;
    // Other detection      
    this->flagStatSec       = false;
    this->statSec.setSection(baseSection);
    this->flagRobustStats   = true;
    this->snrCut            = 3.;
    this->threshold         = 0.;
    this->flagUserThreshold = false;
    // Smoothing 
    this->flagSmooth        = false;
    this->smoothType        = "spectral";
    this->hanningWidth      = 5;
    this->kernMaj           = 3.;
    this->kernMin           = -1.;
    this->kernPA            = 0.;
    // A trous reconstruction parameters
    this->flagATrous        = false;
    this->reconDim          = 1;
    this->scaleMin          = 1;
    this->scaleMax          = 0;
    this->snrRecon          = 4.;
    this->filterCode        = 1;
    this->reconFilter.define(this->filterCode);
    // Volume-merging parameters
    this->flagAdjacent      = true;
    this->threshSpatial     = 3.;
    this->threshVelocity    = 7.;
    this->minChannels       = 3;
    this->minPix            = 2;
    // Input-Output related
    this->spectralMethod    = "peak";
    this->spectralUnits     = "km/s";
    this->pixelCentre       = "centroid";
    this->borders           = true;
    this->blankEdge         = true;
    this->verbose           = true;
  }

  Param::Param (const Param& p)
  {
    operator=(p);
  }

  Param& Param::operator= (const Param& p)
  {
    if(this == &p) return *this;
    this->imageFile         = p.imageFile;
    this->flagSubsection    = p.flagSubsection; 
    this->pixelSec          = p.pixelSec; 
    this->flagReconExists   = p.flagReconExists;
    this->reconFile         = p.reconFile;      
    this->flagSmoothExists  = p.flagSmoothExists;
    this->smoothFile        = p.smoothFile;      
    this->flagLog           = p.flagLog;        
    this->logFile           = p.logFile;       
    this->outFile           = p.outFile;        
    this->flagSeparateHeader= p.flagSeparateHeader;
    this->headerFile        = p.headerFile;
    this->spectraFile       = p.spectraFile;    
    this->flagTextSpectra   = p.flagTextSpectra;    
    this->spectraTextFile   = p.spectraTextFile;    
    this->flagOutputMask    = p.flagOutputMask;
    this->flagOutputSmooth  = p.flagOutputSmooth;
    this->flagOutputRecon   = p.flagOutputRecon;
    this->flagOutputResid   = p.flagOutputResid;
    this->flagVOT           = p.flagVOT;         
    this->votFile           = p.votFile;        
    this->flagKarma         = p.flagKarma;      
    this->karmaFile         = p.karmaFile;      
    this->annotationType    = p.annotationType;
    this->flagMaps          = p.flagMaps;       
    this->detectionMap      = p.detectionMap;   
    this->momentMap         = p.momentMap;      
    this->flagXOutput       = p.flagXOutput;       
    this->precFlux          = p.precFlux;
    this->precVel           = p.precVel;
    this->precSNR           = p.precSNR;
    this->flagBlankPix      = p.flagBlankPix;   
    this->blankPixValue     = p.blankPixValue;  
    this->blankKeyword      = p.blankKeyword;   
    this->bscaleKeyword     = p.bscaleKeyword;  
    this->bzeroKeyword      = p.bzeroKeyword;   
    this->newFluxUnits      = p.newFluxUnits;
    this->flagMW            = p.flagMW;         
    this->maxMW             = p.maxMW;          
    this->minMW             = p.minMW;         
    this->numPixBeam        = p.numPixBeam;     
    this->flagTrim          = p.flagTrim;    
    this->hasBeenTrimmed    = p.hasBeenTrimmed;    
    this->borderLeft        = p.borderLeft;     
    this->borderRight       = p.borderRight;    
    this->borderBottom      = p.borderBottom;   
    this->borderTop         = p.borderTop;      
    if(this->sizeOffsets>0) delete [] this->offsets;
    this->sizeOffsets       = p.sizeOffsets;
    if(this->sizeOffsets>0){
      this->offsets           = new long[this->sizeOffsets];
      for(int i=0;i<this->sizeOffsets;i++) this->offsets[i] = p.offsets[i];
    }
    this->xSubOffset        = p.xSubOffset;     
    this->ySubOffset        = p.ySubOffset;     
    this->zSubOffset        = p.zSubOffset;
    this->flagBaseline      = p.flagBaseline;
    this->flagNegative      = p.flagNegative;
    this->flagGrowth        = p.flagGrowth;
    this->growthCut         = p.growthCut;
    this->growthThreshold   = p.threshold;
    this->flagUserGrowthThreshold = p.flagUserThreshold;
    this->flagFDR           = p.flagFDR;
    this->alphaFDR          = p.alphaFDR;
    this->flagStatSec       = p.flagStatSec; 
    this->statSec           = p.statSec;
    this->flagRobustStats   = p.flagRobustStats;
    this->snrCut            = p.snrCut;
    this->threshold         = p.threshold;
    this->flagUserThreshold = p.flagUserThreshold;
    this->flagSmooth        = p.flagSmooth;
    this->smoothType        = p.smoothType;
    this->hanningWidth      = p.hanningWidth;
    this->kernMaj           = p.kernMaj;
    this->kernMin           = p.kernMin;
    this->kernPA            = p.kernPA;
    this->flagATrous        = p.flagATrous;
    this->reconDim          = p.reconDim;
    this->scaleMin          = p.scaleMin;
    this->scaleMax          = p.scaleMax;
    this->snrRecon          = p.snrRecon;
    this->filterCode        = p.filterCode;
    this->reconFilter       = p.reconFilter;
    this->flagAdjacent      = p.flagAdjacent;
    this->threshSpatial     = p.threshSpatial;
    this->threshVelocity    = p.threshVelocity;
    this->minChannels       = p.minChannels;
    this->minPix            = p.minPix;
    this->spectralMethod    = p.spectralMethod;
    this->spectralUnits     = p.spectralUnits;
    this->pixelCentre       = p.pixelCentre;
    this->borders           = p.borders;
    this->verbose           = p.verbose;
    return *this;
  }
  //--------------------------------------------------------------------

  int Param::getopts(int argc, char ** argv, std::string progname)
  {
    /**  
     *   A function that reads in the command-line options, in a manner 
     *    tailored for use with the main Duchamp program.
     *
     *   \param argc The number of command line arguments.
     *   \param argv The array of command line arguments.
     */

    int returnValue = FAILURE;
    if(argc==1){
      if(progname=="Selavy") std::cout << SELAVY_ERR_USAGE_MSG;
      else if(progname=="Duchamp") std::cout << ERR_USAGE_MSG;
      else std::cout << ERR_USAGE_MSG;
      returnValue = FAILURE;
    }
    else {
      std::string file;
      bool changeX = false;
      this->defaultValues();
      char c;
      while( ( c = getopt(argc,argv,"p:f:hvx") )!=-1){
	switch(c) {
	case 'p':
	  file = optarg;
	  if(this->readParams(file)==FAILURE){
	    std::stringstream errmsg;
	    errmsg << "Could not open parameter file " << file << ".\n";
	    duchampError(progname,errmsg.str());
	  }
	  else returnValue = SUCCESS;
	  break;
	case 'f':
	  file = optarg;
	  this->imageFile = file;
	  returnValue = SUCCESS;
	  break;
	case 'v':
	  std::cout << PROGNAME << " version " << VERSION << std::endl;
	  break;
	case 'x':
	  changeX = true;
	  break;
	case 'h':
	default :
	  if(progname=="Selavy") std::cout << SELAVY_ERR_USAGE_MSG;
	  else if(progname=="Duchamp") std::cout << ERR_USAGE_MSG;
	  else std::cout << ERR_USAGE_MSG;
	  break;
	}
      }
      if(changeX){
	if(returnValue == SUCCESS) this->setFlagXOutput(false);
	else {
	  duchampError(progname,
		       "You need to specify either a parameter file or FITS image.\n");
	  std::cout << "\n" << ERR_USAGE_MSG;
	}
      }
    }
    return returnValue;
  }
  //--------------------------------------------------------------------

  bool Param::isBlank(float &value)
  {
    /** 
     *  Tests whether the value passed as the argument is BLANK or not.
     *  \param value Pixel value to be tested.
     *  \return False if flagBlankPix is false. Else, compare to the
     *  relevant FITS keywords, using integer comparison.
     */
    return this->flagBlankPix &&
      (this->blankKeyword == int((value-this->bzeroKeyword)/this->bscaleKeyword));
  }

  bool *Param::makeBlankMask(float *array, int size)
  {
    /**
     *  This returns an array of bools, saying whether each pixel in the
     *  given array is BLANK or not. If the pixel is BLANK, set mask to
     *  false, else set to true. The array is allocated by the function.
     */ 
    bool *mask = new bool[size];
    for(int i=0;i<size;i++) mask[i] = !this->isBlank(array[i]);
    return mask;
  }


  bool Param::isInMW(int z)
  {
    /** 
     *  Tests whether we are flagging Milky Way channels, and if so
     * whether the given channel number is in the Milky Way range. The
     * channels are assumed to start at number 0.  
     * \param z The channel number 
     * \return True if we are flagging Milky Way channels and z is in
     *  the range.
     */
    return ( flagMW && (z>=minMW) && (z<=maxMW) );
  }

  bool Param::isStatOK(int x, int y, int z)
  {
    /** 
     * Test whether a given pixel position lies within the subsection
     * given by the statSec parameter. Only tested if the flagSubsection
     * parameter is true -- if it isn't, we just return true since all
     * pixels are therefore available for statstical calculations.
     * \param x X-value of pixel being tested.
     * \param y Y-value of pixel being tested.
     * \param z Z-value of pixel being tested.
     * \return True if pixel is able to be used for statistical
     * calculations. False otherwise.
     */
    int xval=x,yval=y,zval=z;
    if(flagSubsection){
      xval += pixelSec.getStart(0);
      yval += pixelSec.getStart(1);
      zval += pixelSec.getStart(2);
    }
    return !flagStatSec || statSec.isInside(xval,yval,zval);
  }

  /****************************************************************/
  ///////////////////////////////////////////////////
  //// Other Functions using the  Parameter class:
  ///////////////////////////////////////////////////

  std::string makelower( std::string s )
  {
    // "borrowed" from Matt Howlett's 'fred'
    std::string out = "";
    for( int i=0; i<s.size(); ++i ) {
      out += tolower(s[i]);
    }
    return out;
  }

  inline std::string stringize(bool b)
  {
    /** 
     * Convert a bool variable to the textual equivalent. 
     * \return A std::string with the english equivalent of the bool.
     */
    std::string output;
    if(b) output="true";
    else output="false";
    return output;
  }

  inline bool boolify( std::string s )
  {
    /**
     *  Convert a std::string to a bool variable: 
     *  "1" and "true" get converted to true;
     *  "0" and "false" (and anything else) get converted to false.
     * \return The bool equivalent of the string.
     */
    if((s=="1") || (makelower(s)=="true")) return true;
    else if((s=="0") || (makelower(s)=="false")) return false;
    else return false;
  }

  inline std::string readSval(std::stringstream& ss)
  {
    std::string val; ss >> val; return val;
  }

  inline bool readFlag(std::stringstream& ss)
  {
    std::string val; ss >> val; return boolify(val);
  }

  inline float readFval(std::stringstream& ss)
  {
    float val; ss >> val; return val;
  }

  inline int readIval(std::stringstream& ss)
  {
    int val; ss >> val; return val;
  }

  int Param::readParams(std::string paramfile)
  {
    /**
     * The parameters are read in from a disk file, on the assumption that each
     *  line of the file has the format "parameter value" (eg. alphafdr 0.1)
     * 
     * The case of the parameter name does not matter, nor does the
     * formatting of the spaces (it can be any amount of whitespace or
     * tabs). 
     *
     * \param paramfile A std::string containing the parameter filename.
     *
     * \return FAILURE if the parameter file does not exist. SUCCESS if
     * it is able to read it.
     */
    std::ifstream fin(paramfile.c_str());
    if(!fin.is_open()) return FAILURE;
    std::string line;
    while( !std::getline(fin,line,'\n').eof()){

      if(line[0]!='#'){
	std::stringstream ss;
	ss.str(line);
	std::string arg;
	ss >> arg;
	arg = makelower(arg);
	if(arg=="imagefile")       this->imageFile = readSval(ss);
	if(arg=="flagsubsection")  this->flagSubsection = readFlag(ss); 
	if(arg=="subsection")      this->pixelSec.setSection(readSval(ss));
	if(arg=="flagreconexists") this->flagReconExists = readFlag(ss); 
	if(arg=="reconfile")       this->reconFile = readSval(ss); 
	if(arg=="flagsmoothexists")this->flagSmoothExists = readFlag(ss); 
	if(arg=="smoothfile")      this->smoothFile = readSval(ss); 
	if(arg=="beamsize")        this->numPixBeam = readFval(ss); 

	if(arg=="flaglog")         this->flagLog = readFlag(ss); 
	if(arg=="logfile")         this->logFile = readSval(ss); 
	if(arg=="outfile")         this->outFile = readSval(ss); 
	if(arg=="flagseparateheader") this->flagSeparateHeader = readFlag(ss);
	if(arg=="headerfile")      this->headerFile = readSval(ss);
	if(arg=="spectrafile")     this->spectraFile = readSval(ss); 
	if(arg=="flagtextspectra") this->flagTextSpectra = readFlag(ss); 
	if(arg=="spectraTextfile") this->spectraTextFile = readSval(ss); 
	if(arg=="flagoutputmask")  this->flagOutputMask = readFlag(ss); 
	if(arg=="flagoutputsmooth")this->flagOutputSmooth = readFlag(ss); 
	if(arg=="flagoutputrecon") this->flagOutputRecon = readFlag(ss); 
	if(arg=="flagoutputresid") this->flagOutputResid = readFlag(ss); 
	if(arg=="flagvot")         this->flagVOT = readFlag(ss); 
	if(arg=="votfile")         this->votFile = readSval(ss); 
	if(arg=="flagkarma")       this->flagKarma = readFlag(ss); 
	if(arg=="karmafile")       this->karmaFile = readSval(ss); 
	if(arg=="annotationtype")  this->annotationType = readSval(ss); 
	if(arg=="flagmaps")        this->flagMaps = readFlag(ss); 
	if(arg=="detectionmap")    this->detectionMap = readSval(ss); 
	if(arg=="momentmap")       this->momentMap = readSval(ss); 
	if(arg=="flagxoutput")     this->flagXOutput = readFlag(ss); 
	if(arg=="newfluxunits")    this->newFluxUnits = readSval(ss);
	if(arg=="precflux")        this->precFlux = readIval(ss);
	if(arg=="precvel")         this->precVel = readIval(ss);
	if(arg=="precsnr")         this->precSNR = readIval(ss);

	if(arg=="flagtrim")        this->flagTrim = readFlag(ss); 
	if(arg=="flagmw")          this->flagMW = readFlag(ss); 
	if(arg=="maxmw")           this->maxMW = readIval(ss); 
	if(arg=="minmw")           this->minMW = readIval(ss); 
	if(arg=="flagbaseline")    this->flagBaseline = readFlag(ss); 

	if(arg=="flagnegative")    this->flagNegative = readFlag(ss);
	if(arg=="minpix")          this->minPix = readIval(ss); 
	if(arg=="flaggrowth")      this->flagGrowth = readFlag(ss); 
	if(arg=="growthcut")       this->growthCut = readFval(ss); 
	if(arg=="growththreshold"){
	  this->growthThreshold = readFval(ss);
	  this->flagUserGrowthThreshold = true;
	}

	if(arg=="flagfdr")         this->flagFDR = readFlag(ss); 
	if(arg=="alphafdr")        this->alphaFDR = readFval(ss); 

	if(arg=="flagstatsec")     this->flagStatSec = readFlag(ss); 
	if(arg=="statsec")         this->statSec.setSection(readSval(ss));
	if(arg=="flagrobuststats") this->flagRobustStats = readFlag(ss); 
	if(arg=="snrcut")          this->snrCut = readFval(ss); 
	if(arg=="threshold"){
	  this->threshold = readFval(ss);
	  this->flagUserThreshold = true;
	}
      
	if(arg=="flagsmooth")      this->flagSmooth = readFlag(ss);
	if(arg=="smoothtype")      this->smoothType = readSval(ss);
	if(arg=="hanningwidth")    this->hanningWidth = readIval(ss);
	if(arg=="kernmaj")         this->kernMaj = readFval(ss);
	if(arg=="kernmin")         this->kernMin = readFval(ss);
	if(arg=="kernpa")          this->kernPA = readFval(ss);

	if(arg=="flagatrous")      this->flagATrous = readFlag(ss); 
	if(arg=="recondim")        this->reconDim = readIval(ss); 
	if(arg=="scalemin")        this->scaleMin = readIval(ss); 
	if(arg=="scalemax")        this->scaleMax = readIval(ss); 
	if(arg=="snrrecon")        this->snrRecon = readFval(ss); 
	if(arg=="filtercode"){
	  this->filterCode = readIval(ss); 
	  this->reconFilter.define(this->filterCode);
	}

	if(arg=="flagadjacent")    this->flagAdjacent = readFlag(ss); 
	if(arg=="threshspatial")   this->threshSpatial = readFval(ss); 
	if(arg=="threshvelocity")  this->threshVelocity = readFval(ss); 
	if(arg=="minchannels")     this->minChannels = readIval(ss); 

	if(arg=="spectralmethod")  this->spectralMethod=makelower(readSval(ss));
	if(arg=="spectralunits")   this->spectralUnits = makelower(readSval(ss));
	if(arg=="pixelcentre")     this->pixelCentre = makelower(readSval(ss));
	if(arg=="drawborders")     this->borders = readFlag(ss); 
	if(arg=="drawblankedges")  this->blankEdge = readFlag(ss); 
	if(arg=="verbose")         this->verbose = readFlag(ss); 

	// Dealing with deprecated parameters.
	if(arg=="flagblankpix"){
	  this->flagTrim = readFlag(ss);
	  std::stringstream errmsg;
	  errmsg <<"The parameter flagBlankPix is deprecated. "
		 <<"Please use the flagTrim parameter in future.\n"
		 <<"Setting flagTrim = " << stringize(this->flagTrim) << ".\n";
	  duchampWarning("Reading parameters",errmsg.str());
	}
	if(arg=="blankpixvalue"){
	  std::stringstream errmsg;
	  errmsg <<"The parameter blankPixValue is deprecated.\n"
		 <<"This value is only taken from the FITS header.\n";
	  duchampWarning("Reading parameters",errmsg.str());
	}

      }
    }

    // If pgplot was not included in the compilation, need to set flagXOutput to false
    if(!USE_PGPLOT) this->flagXOutput = false;

    // Correcting bad precision values -- if negative, set to 0
    if(this->precFlux<0) this->precFlux = 0;
    if(this->precVel<0)  this->precVel = 0;
    if(this->precSNR<0)  this->precSNR = 0;

    // The wavelet reconstruction takes precendence over the smoothing.
    if(this->flagATrous) this->flagSmooth = false;

    if(this->flagUserThreshold){

      // If we specify a manual threshold, need to also specify a manual growth threshold
      // If we haven't done so, turn growing off
      if(this->flagGrowth && !this->flagUserGrowthThreshold){
	std::stringstream errmsg;
	errmsg << "You have specified a manual search threshold, but not a manual growth threshold.\n"
	       << "You need to do so using the \"growthThreshold\" parameter.\n"
	       << "The growth function is being turned off.\n";
	duchampWarning("Reading parameters",errmsg.str());
	this->flagGrowth = false;
      }

      // If we specify a manual threshold, we don't need the FDR method, so turn it off if requested.
      if(this->flagFDR){
	std::stringstream errmsg;
	errmsg << "You have specified a manual search threshold, so we don't need to use the FDR method.\n"
	       << "Setting \"flagFDR=false\".\n";
	duchampWarning("Reading parameters",errmsg.str());
	this->flagFDR = false;
      }

    }	


    // Make sure the annnotationType is an acceptable option -- default is "borders"
    if((this->annotationType != "borders") && (this->annotationType!="circles")){
      std::stringstream errmsg;
      errmsg << "The requested value of the parameter annotationType, \""
	     << this->annotationType << "\" is invalid.\n"
	     << "Changing to \"borders\".\n";
      duchampWarning("Reading parameters",errmsg.str());
      this->annotationType = "borders";
    }
      
    // Make sure smoothType is an acceptable type -- default is "spectral"
    if((this->smoothType!="spectral")&&
       (this->smoothType!="spatial")){
      std::stringstream errmsg;
      errmsg << "The requested value of the parameter smoothType, \""
	     << this->smoothType << "\" is invalid.\n"
	     << "Changing to \"spectral\".\n";
      duchampWarning("Reading parameters",errmsg.str());
      this->smoothType = "spectral";
    }
    // If kernMin has not been given, or is negative, make it equal to kernMaj
    if(this->kernMin < 0) this->kernMin = this->kernMaj;

    // Make sure spectralMethod is an acceptable type -- default is "peak"
    if((this->spectralMethod!="peak")&&
       (this->spectralMethod!="sum")){
      std::stringstream errmsg;
      errmsg << "The requested value of the parameter spectralMethod, \""
	     << this->spectralMethod << "\" is invalid.\n"
	     << "Changing to \"peak\".\n";
      duchampWarning("Reading parameters",errmsg.str());
      this->spectralMethod = "peak";
    }

    // Make sure pixelCentre is an acceptable type -- default is "peak"
    if((this->pixelCentre!="centroid")&&
       (this->pixelCentre!="average") &&
       (this->pixelCentre!="peak")       ){
      std::stringstream errmsg;
      errmsg << "The requested value of the parameter pixelCentre, \""
	     << this->pixelCentre << "\" is invalid.\n"
	     << "Changing to \"centroid\".\n";
      duchampWarning("Reading parameters",errmsg.str());
      this->pixelCentre = "centroid";
    }

    return SUCCESS;
  }


  std::ostream& operator<< ( std::ostream& theStream, Param& par)
  {
    /**
     * Print out the parameter set in a formatted, easy to read style.
     * Lists the parameters, a description of them, and their value.
     */

    // Only show the [beamSize] bit if we are using the parameter
    // otherwise we have read it from the FITS header.
    std::string beamParam = "";
    if(par.getFlagUsingBeam()) beamParam = "[beamSize]";

    // BUG -- can get error: `boolalpha' is not a member of type `ios' -- old compilers: gcc 2.95.3?
    //   theStream.setf(std::ios::boolalpha);
    theStream.setf(std::ios::left);
    theStream  <<"\n---- Parameters ----"<<std::endl;
    theStream  << std::setfill('.');
    const int widthText = 38;
    const int widthPar  = 18;
    if(par.getFlagSubsection())
      theStream<<std::setw(widthText)<<"Image to be analysed"                 
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[imageFile]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<(par.getImageFile()+par.getSubsection()) <<std::endl;
    else 
      theStream<<std::setw(widthText)<<"Image to be analysed"                 
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[imageFile]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getImageFile()      <<std::endl;
    if(par.getFlagReconExists() && par.getFlagATrous()){
      theStream<<std::setw(widthText)<<"Reconstructed array exists?"          
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[reconExists]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<stringize(par.getFlagReconExists())<<std::endl;
      theStream<<std::setw(widthText)<<"FITS file containing reconstruction"  
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[reconFile]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getReconFile()      <<std::endl;
    }
    if(par.getFlagSmoothExists() && par.getFlagSmooth()){
      theStream<<std::setw(widthText)<<"Smoothed array exists?"          
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[smoothExists]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<stringize(par.getFlagSmoothExists())<<std::endl;
      theStream<<std::setw(widthText)<<"FITS file containing smoothed array"  
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[smoothFile]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getSmoothFile()      <<std::endl;
    }
    theStream  <<std::setw(widthText)<<"Intermediate Logfile"
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[logFile]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getLogFile()        <<std::endl;
    theStream  <<std::setw(widthText)<<"Final Results file"                   
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[outFile]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getOutFile()        <<std::endl;
    if(par.getFlagSeparateHeader())
      theStream <<std::setw(widthText)<<"Header for results file" 
		<<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[headerFile]"
		<<"  =  " <<resetiosflags(std::ios::right)
		<<par.getHeaderFile()        <<std::endl;
    theStream  <<std::setw(widthText)<<"Spectrum file"                        
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[spectraFile]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getSpectraFile()    <<std::endl;
    if(par.getFlagTextSpectra()){
      theStream<<std::setw(widthText)<<"Text file with ascii spectral data"                         
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[spectraTextFile]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getSpectraTextFile()<<std::endl;
    }
    if(par.getFlagVOT()){
      theStream<<std::setw(widthText)<<"VOTable file"                         
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[votFile]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getVOTFile()        <<std::endl;
    }
    if(par.getFlagKarma()){
      theStream<<std::setw(widthText)<<"Karma annotation file"                
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[karmaFile]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	     
	       <<par.getKarmaFile()      <<std::endl;
    }
    if(par.getFlagMaps()){
      theStream<<std::setw(widthText)<<"0th Moment Map"                       
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[momentMap]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getMomentMap()      <<std::endl;
      theStream<<std::setw(widthText)<<"Detection Map"                        
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[detectionMap]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getDetectionMap()   <<std::endl;
    }
    theStream  <<std::setw(widthText)<<"Display a map in a pgplot xwindow?" 
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[flagXOutput]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<stringize(par.getFlagXOutput())     <<std::endl;
    if(par.getFlagATrous()){			       
      theStream<<std::setw(widthText)<<"Saving reconstructed cube?"           
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[flagOutputRecon]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<stringize(par.getFlagOutputRecon())<<std::endl;
      theStream<<std::setw(widthText)<<"Saving residuals from reconstruction?"
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[flagOutputResid]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<stringize(par.getFlagOutputResid())<<std::endl;
    }						       
    if(par.getFlagSmooth()){			       
      theStream<<std::setw(widthText)<<"Saving smoothed cube?"           
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[flagOutputSmooth]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<stringize(par.getFlagOutputSmooth())<<std::endl;
    }						       
    theStream<<std::setw(widthText)<<"Saving mask cube?"           
	     <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[flagOutputMask]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<stringize(par.getFlagOutputMask())<<std::endl;
    theStream  <<"------"<<std::endl;
    if(par.getFlagBlankPix()){
      theStream<<std::setw(widthText)<<"Blank Pixel Value"                    
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<< ""
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getBlankPixVal()    <<std::endl;
    }
    theStream  <<std::setw(widthText)<<"Trimming Blank Pixels?"                 
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[flagTrim]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<stringize(par.getFlagTrim())   <<std::endl;
    theStream  <<std::setw(widthText)<<"Searching for Negative features?"     
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[flagNegative]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<stringize(par.getFlagNegative())   <<std::endl;
    theStream  <<std::setw(widthText)<<"Removing Milky Way channels?"         
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[flagMW]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<stringize(par.getFlagMW())         <<std::endl;
    if(par.getFlagMW()){
      theStream<<std::setw(widthText)<<"Milky Way Channels"                   
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[minMW - maxMW]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getMinMW()
	       <<"-" <<par.getMaxMW()          <<std::endl;
    }
    theStream  <<std::setw(widthText)<<"Beam Size (pixels)"                   
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<< beamParam
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getBeamSize()       <<std::endl;
    theStream  <<std::setw(widthText)<<"Removing baselines before search?"    
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[flagBaseline]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<stringize(par.getFlagBaseline())   <<std::endl;
    theStream  <<std::setw(widthText)<<"Smoothing each spectrum first?"        
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[flagSmooth]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<stringize(par.getFlagSmooth())     <<std::endl;
    if(par.getFlagSmooth()){	       
      theStream<<std::setw(widthText)<<"Type of smoothing"      
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[smoothType]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getSmoothType()       <<std::endl;
      if(par.getSmoothType()=="spectral")
	theStream<<std::setw(widthText)<<"Width of hanning filter"      
		 <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[hanningWidth]"
		 <<"  =  " <<resetiosflags(std::ios::right)
		 <<par.getHanningWidth()       <<std::endl;
      else{
	theStream<<std::setw(widthText)<<"Gaussian kernel semi-major axis [pix]"
		 <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[kernMaj]"
		 <<"  =  " <<resetiosflags(std::ios::right)
		 <<par.getKernMaj() << std::endl;
	theStream<<std::setw(widthText)<<"Gaussian kernel semi-minor axis [pix]"
		 <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[kernMin]"
		 <<"  =  " <<resetiosflags(std::ios::right)
		 <<par.getKernMin() << std::endl;
	theStream<<std::setw(widthText)<<"Gaussian kernel position angle [deg]"
		 <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[kernPA]"
		 <<"  =  " <<resetiosflags(std::ios::right)
		 <<par.getKernPA() << std::endl;
      }
    }
    theStream  <<std::setw(widthText)<<"Using A Trous reconstruction?"        
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[flagATrous]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<stringize(par.getFlagATrous())     <<std::endl;
    if(par.getFlagATrous()){			       
      theStream<<std::setw(widthText)<<"Number of dimensions in reconstruction"      
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[reconDim]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getReconDim()       <<std::endl;
      if(par.getMaxScale()>0){
	theStream<<std::setw(widthText-1)<<"Scales used in reconstruction"      
		 <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[scaleMin-scaleMax]"
		 <<"  =  " <<resetiosflags(std::ios::right)
		 <<par.getMinScale() << "-" << par.getMaxScale()   <<std::endl;
      }
      else{
	theStream<<std::setw(widthText)<<"Minimum scale in reconstruction"      
		 <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[scaleMin]"
		 <<"  =  " <<resetiosflags(std::ios::right)
		 <<par.getMinScale()      <<std::endl;
      }
      theStream<<std::setw(widthText)<<"SNR Threshold within reconstruction"  
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[snrRecon]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getAtrousCut()      <<std::endl;
      theStream<<std::setw(widthText)<<"Filter being used for reconstruction" 
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[filterCode]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getFilterCode() 
	       << " (" << par.getFilterName()  << ")" <<std::endl;
    }	     					       
    theStream  <<std::setw(widthText)<<"Using Robust statistics?"                  
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[flagRobustStats]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<stringize(par.getFlagRobustStats()) <<std::endl;
    if(par.getFlagStatSec()){
      theStream<<std::setw(widthText)<<"Section used by statistics calculation"
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[statSec]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.statSec.getSection()          <<std::endl;
    }
    theStream  <<std::setw(widthText)<<"Using FDR analysis?"                  
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[flagFDR]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<stringize(par.getFlagFDR())        <<std::endl;
    if(par.getFlagFDR()){				       
      theStream<<std::setw(widthText)<<"Alpha value for FDR analysis"         
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[alphaFDR]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getAlpha()          <<std::endl;
    }	     					       
    else {
      if(par.getFlagUserThreshold()){
	theStream<<std::setw(widthText)<<"Detection Threshold"                        
		 <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[threshold]"
		 <<"  =  " <<resetiosflags(std::ios::right)
		 <<par.getThreshold()            <<std::endl;
      }
      else{
	theStream<<std::setw(widthText)<<"SNR Threshold (in sigma)"
		 <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[snrCut]"
		 <<"  =  " <<resetiosflags(std::ios::right)
		 <<par.getCut()            <<std::endl;
      }
    }
    theStream  <<std::setw(widthText)<<"Minimum # Pixels in a detection"      
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[minPix]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getMinPix()         <<std::endl;
    theStream  <<std::setw(widthText)<<"Minimum # Channels in a detection"    
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[minChannels]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getMinChannels()    <<std::endl;
    theStream  <<std::setw(widthText)<<"Growing objects after detection?"     
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[flagGrowth]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<stringize(par.getFlagGrowth())     <<std::endl;
    if(par.getFlagGrowth()) {			       
      theStream<<std::setw(widthText)<<"SNR Threshold for growth"             
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[growthCut]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getGrowthCut()      <<std::endl;
    }
    theStream  <<std::setw(widthText)<<"Using Adjacent-pixel criterion?"      
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[flagAdjacent]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<stringize(par.getFlagAdjacent())   <<std::endl;
    if(!par.getFlagAdjacent()){
      theStream<<std::setw(widthText)<<"Max. spatial separation for merging"  
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[threshSpatial]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getThreshS()        <<std::endl;
    }
    theStream  <<std::setw(widthText)<<"Max. velocity separation for merging" 
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[threshVelocity]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getThreshV()        <<std::endl;
    theStream  <<std::setw(widthText)<<"Method of spectral plotting"          
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[spectralMethod]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getSpectralMethod() <<std::endl;
    theStream  <<std::setw(widthText)<<"Type of object centre used in results"
	       <<std::setw(widthPar)<<setiosflags(std::ios::right)<<"[pixelCentre]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getPixelCentre() <<std::endl;
    theStream  <<"--------------------\n\n";
    theStream  << std::setfill(' ');
    theStream.unsetf(std::ios::left);
    //  theStream.unsetf(std::ios::boolalpha);
    return theStream;
  }


  void Param::copyHeaderInfo(FitsHeader &head)
  {
    /**
     * A function to copy across relevant header keywords from the 
     *  FitsHeader class to the Param class, as they are needed by
     *  functions in the Param class.
     * The parameters are the keywords BLANK, BSCALE, BZERO, and the beam size. 
     */

    this->blankKeyword  = head.getBlankKeyword();
    this->bscaleKeyword = head.getBscaleKeyword();
    this->bzeroKeyword  = head.getBzeroKeyword();
    this->blankPixValue = this->blankKeyword * this->bscaleKeyword + 
      this->bzeroKeyword;

    this->numPixBeam    = head.getBeamSize();
  }

  std::string Param::outputMaskFile()
  {
    /**
     *  This function produces the required filename in which to save
     *  the mask image, indicating which pixels have been detected as
     *  part of an object. If the input image is image.fits, then the
     *  output will be image.MASK.fits.
     */
    std::string inputName = this->imageFile;
    std::stringstream ss;
    ss << inputName.substr(0,inputName.size()-5);  
    // remove the ".fits" on the end.
    ss << ".MASK.fits";
    return ss.str();
  }

  std::string Param::outputSmoothFile()
  {
    /** 
     * This function produces the required filename in which to save
     *  the smoothed array. If the input image is image.fits, then
     *  the output will be:
     *   <ul><li> Spectral smoothing: image.SMOOTH-1D-3.fits, where the
     *            width of the Hanning filter was 3 pixels.
     *       <li> Spatial smoothing : image.SMOOTH-2D-3-2-20.fits, where
     *            kernMaj=3, kernMin=2 and kernPA=20 degrees.
     *   </ul>
     */
    std::string inputName = this->imageFile;
    std::stringstream ss;
    ss << inputName.substr(0,inputName.size()-5);  
    // remove the ".fits" on the end.
    if(this->flagSubsection) ss<<".sub";
    if(this->smoothType=="spectral")
      ss << ".SMOOTH-1D-" << this->hanningWidth << ".fits";
    else if(this->smoothType=="spatial")
      ss << ".SMOOTH-2D-" 
	 << this->kernMaj << "-"
	 << this->kernMin << "-"
	 << this->kernPA  << ".fits";
    return ss.str();
  }

  std::string Param::outputReconFile()
  {
    /** 
     * This function produces the required filename in which to save
     *  the reconstructed array. If the input image is image.fits, then
     *  the output will be eg. image.RECON-3-2-4-1.fits, where the numbers are
     *  3=reconDim, 2=filterCode, 4=snrRecon, 1=minScale
     */
    std::string inputName = this->imageFile;
    std::stringstream ss;
    // First we remove the ".fits" from the end of the filename.
    ss << inputName.substr(0,inputName.size()-5);  
    if(this->flagSubsection) ss<<".sub";
    ss << ".RECON-" << this->reconDim 
       << "-"       << this->filterCode
       << "-"       << this->snrRecon
       << "-"       << this->scaleMin
       << ".fits";
    return ss.str();
  }

  std::string Param::outputResidFile()
  {
    /** 
     * This function produces the required filename in which to save
     *  the reconstructed array. If the input image is image.fits, then
     *  the output will be eg. image.RESID-3-2-4-1.fits, where the numbers are
     *  3=reconDim, 2=filterCode, 4=snrRecon, 1=scaleMin
     */
    std::string inputName = this->imageFile;
    std::stringstream ss;
    // First we remove the ".fits" from the end of the filename.
    ss << inputName.substr(0,inputName.size()-5);
    if(this->flagSubsection) ss<<".sub";
    ss << ".RESID-" << this->reconDim 
       << "-"       << this->filterCode
       << "-"       << this->snrRecon
       << "-"       << this->scaleMin
       << ".fits";
    return ss.str();
  }

}
