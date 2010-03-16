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
#include <algorithm>
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
    /// Deletes the offsets array if the sizeOffsets parameter is
    /// positive.
    if(this->sizeOffsets>0) delete [] this->offsets;
  }

  Param::Param()
  {
    this->defaultValues();
  }

  void Param::defaultValues()
  {
    /// Provides default intial values for the parameters. Note that
    /// imageFile has no default value!

    std::string baseSection = "[*,*,*]";
    // Input files
    this->imageFile         = "";
    this->flagSubsection    = false;
    this->pixelSec.setSection(baseSection);
    this->flagReconExists   = false;
    this->reconFile         = "";
    this->flagSmoothExists  = false;
    this->smoothFile        = "";
    this->usePrevious       = false;
    this->objectList        = "";
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
    this->fileOutputMask    = "";
    this->flagMaskWithObjectNum = false;
    this->flagOutputSmooth  = false;
    this->fileOutputSmooth  = "";
    this->flagOutputRecon   = false;
    this->fileOutputRecon   = "";
    this->flagOutputResid   = false;
    this->fileOutputResid   = "";
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
    this->areaBeam          = 10.;
    this->fwhmBeam          = 10.;
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
    this->FDRnumCorChan     = 2;
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
    this->sortingParam      = "vel";
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
    this->usePrevious       = p.usePrevious;
    this->objectList        = p.objectList;
    this->flagLog           = p.flagLog;        
    this->logFile           = p.logFile;       
    this->outFile           = p.outFile;        
    this->flagSeparateHeader= p.flagSeparateHeader;
    this->headerFile        = p.headerFile;
    this->spectraFile       = p.spectraFile;    
    this->flagTextSpectra   = p.flagTextSpectra;    
    this->spectraTextFile   = p.spectraTextFile;    
    this->flagOutputMask    = p.flagOutputMask;
    this->fileOutputMask    = p.fileOutputMask;
    this->flagMaskWithObjectNum = p.flagMaskWithObjectNum;
    this->flagOutputSmooth  = p.flagOutputSmooth;
    this->fileOutputSmooth  = p.fileOutputSmooth;
    this->flagOutputRecon   = p.flagOutputRecon;
    this->fileOutputRecon   = p.fileOutputRecon;
    this->flagOutputResid   = p.flagOutputResid;
    this->fileOutputResid   = p.fileOutputResid;
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
    this->areaBeam          = p.areaBeam;     
    this->fwhmBeam          = p.fwhmBeam;     
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
    this->growthThreshold   = p.growthThreshold;
    this->flagUserGrowthThreshold = p.flagUserGrowthThreshold;
    this->flagFDR           = p.flagFDR;
    this->alphaFDR          = p.alphaFDR;
    this->FDRnumCorChan     = p.FDRnumCorChan;
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
    this->sortingParam      = p.sortingParam;
    this->borders           = p.borders;
    this->blankEdge         = p.blankEdge;
    this->verbose           = p.verbose;
    return *this;
  }
  //--------------------------------------------------------------------

  int Param::getopts(int argc, char ** argv, std::string progname)
  {
    ///   A function that reads in the command-line options, in a manner 
    ///    tailored for use with the main Duchamp program.
    /// 
    ///   \param argc The number of command line arguments.
    ///   \param argv The array of command line arguments.

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
    ///  Tests whether the value passed as the argument is BLANK or not.
    ///  \param value Pixel value to be tested.
    ///  \return False if flagBlankPix is false. Else, compare to the
    ///  relevant FITS keywords, using integer comparison.

    return this->flagBlankPix &&
      (this->blankKeyword == int((value-this->bzeroKeyword)/this->bscaleKeyword));
  }

  bool *Param::makeBlankMask(float *array, int size)
  {
    ///  This returns an array of bools, saying whether each pixel in the
    ///  given array is BLANK or not. If the pixel is BLANK, set mask to
    ///  false, else set to true. The array is allocated by the function.

    bool *mask = new bool[size];
    for(int i=0;i<size;i++) mask[i] = !this->isBlank(array[i]);
    return mask;
  }


  bool *Param::makeStatMask(float *array, long *dim)
  {
    ///  This returns an array of bools, saying whether each pixel in
    ///  the given array is suitable for a stats calculation. It needs
    ///  to be in the StatSec (if defined), not blank and not a MW
    ///  channel. The array is allocated by the function with a 'new' call. 

    bool *mask = new bool[dim[0]*dim[1]*dim[2]];
    for(int x=0;x<dim[0];x++) {
      for(int y=0;y<dim[1];y++) {
	for(int z=0;z<dim[2];z++) {
	  int i = x+y*dim[0]+z*dim[0]*dim[1];
	  mask[i] = !this->isBlank(array[i]) && !this->isInMW(z) && this->isStatOK(x,y,z);
	}
      }
    }
    return mask;
  }


  bool Param::isInMW(int z)
  {
    ///  Tests whether we are flagging Milky Way channels, and if so
    /// whether the given channel number is in the Milky Way range. The
    /// channels are assumed to start at number 0.  
    /// \param z The channel number 
    /// \return True if we are flagging Milky Way channels and z is in
    ///  the range.

    return ( flagMW && (z>=minMW) && (z<=maxMW) );
  }

  bool Param::isStatOK(int x, int y, int z)
  {
   /// Test whether a given pixel position lies within the subsection
   /// given by the statSec parameter. Only tested if the flagSubsection
   /// parameter is true -- if it isn't, we just return true since all
   /// pixels are therefore available for statstical calculations.
   /// \param x X-value of pixel being tested.
   /// \param y Y-value of pixel being tested.
   /// \param z Z-value of pixel being tested.
   /// \return True if pixel is able to be used for statistical
   /// calculations. False otherwise.

    int xval=x,yval=y,zval=z;
    if(flagSubsection){
      xval += pixelSec.getStart(0);
      yval += pixelSec.getStart(1);
      zval += pixelSec.getStart(2);
    }
    return !flagStatSec || statSec.isInside(xval,yval,zval);
  }

  std::vector<int> Param::getObjectRequest()
  {
    ///  Returns a list of the object numbers requested via the objectList parameter. 
    /// \return a vector of integers, one for each number in the objectList set.

    std::stringstream ss1;
    std::string tmp;
    std::vector<int> tmplist;
    ss1.str(this->objectList);
    while(!ss1.eof()){
      getline(ss1,tmp,',');
      for(size_t i=0;i<tmp.size();i++) if(tmp[i]=='-') tmp[i]=' ';
      int a,b;
      std::stringstream ss2;
      ss2.str(tmp);
      ss2 >>a;
      if(!ss2.eof()) ss2 >> b;
      else b=a;
      for(int n=a;n<=b;n++){
	tmplist.push_back(n);
      }      
    }
    return tmplist;
  }

  std::vector<bool> Param::getObjectChoices()
  {
    ///  Returns a list of bool values, indicating whether a given
    ///  object was requested or not. The size of the vector is
    ///  determined by the maximum value in objectList. For instance,
    ///  if objectList="2,3,5-8", then the returned vector will be
    ///  [0,1,1,0,1,1,1,1].
    ///  \return Vector of bool values.

    std::vector<int> objectChoices = this->getObjectRequest();
    int maxNum = *std::max_element(objectChoices.begin(), objectChoices.end());
    std::vector<bool> choices(maxNum,false);
    for(std::vector<int>::iterator obj = objectChoices.begin();obj!=objectChoices.end();obj++) 
      choices[*obj-1] = true;
    return choices;
  }

  std::vector<bool> Param::getObjectChoices(int numObjects)
  {
    ///  Returns a list of bool values, indicating whether a given
    ///  object was requested or not. The size of the vector is given
    ///  by the numObjects parameter. So, if objectList="2,3,5-8", then
    ///  the returned vector from a getObjectChoices(10) call will be
    ///  [0,1,1,0,1,1,1,1,0,0].
    ///  \param numObjects How many objects there are in total.
    ///  \return Vector of bool values.

    if(this->objectList==""){
      std::vector<bool> choices(numObjects,true);
      return choices;
    }
    else{
      std::vector<int> objectChoices = this->getObjectRequest();
      std::vector<bool> choices(numObjects,false);
      for(std::vector<int>::iterator obj = objectChoices.begin();obj!=objectChoices.end();obj++) 
	if(*obj<=numObjects) choices[*obj-1] = true;
      return choices;
    }
  }

  /****************************************************************/
  /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// 
  /// / Other Functions using the  Parameter class:
  /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// /// 

  std::string makelower( std::string s )
  {
    // "borrowed" from Matt Howlett's 'fred'
    std::string out = "";
    for( size_t i=0; i<s.size(); ++i ) {
      out += tolower(s[i]);
    }
    return out;
  }

  inline std::string stringize(bool b)
  {
   /// Convert a bool variable to the textual equivalent. 
   /// \return A std::string with the english equivalent of the bool.

    std::string output;
    if(b) output="true";
    else output="false";
    return output;
  }

  inline bool boolify( std::string s )
  {
    ///  Convert a std::string to a bool variable: 
    ///  "1" and "true" get converted to true;
    ///  "0" and "false" (and anything else) get converted to false.
    /// \return The bool equivalent of the string.

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
    /// The parameters are read in from a disk file, on the assumption that each
    ///  line of the file has the format "parameter value" (eg. alphafdr 0.1)
    /// 
    /// The case of the parameter name does not matter, nor does the
    /// formatting of the spaces (it can be any amount of whitespace or
    /// tabs). 
    /// 
    /// \param paramfile A std::string containing the parameter filename.
    /// 
    /// \return FAILURE if the parameter file does not exist. SUCCESS if
    /// it is able to read it.

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
	if(arg=="beamarea")        this->areaBeam = readFval(ss); 
	if(arg=="beamfwhm")        this->fwhmBeam = readFval(ss); 
	if(arg=="useprevious")     this->usePrevious = readFlag(ss);
	if(arg=="objectlist")      this->objectList = readSval(ss);

	if(arg=="flaglog")         this->flagLog = readFlag(ss); 
	if(arg=="logfile")         this->logFile = readSval(ss); 
	if(arg=="outfile")         this->outFile = readSval(ss); 
	if(arg=="flagseparateheader") this->flagSeparateHeader = readFlag(ss);
	if(arg=="headerfile")      this->headerFile = readSval(ss);
	if(arg=="spectrafile")     this->spectraFile = readSval(ss); 
	if(arg=="flagtextspectra") this->flagTextSpectra = readFlag(ss); 
	if(arg=="spectratextfile") this->spectraTextFile = readSval(ss); 
	if(arg=="flagoutputmask")  this->flagOutputMask = readFlag(ss); 
	if(arg=="fileoutputmask")  this->fileOutputMask = readSval(ss);
	if(arg=="flagmaskwithobjectnum") this->flagMaskWithObjectNum = readFlag(ss);
	if(arg=="flagoutputsmooth")this->flagOutputSmooth = readFlag(ss); 
	if(arg=="fileoutputsmooth")this->fileOutputSmooth = readSval(ss);
	if(arg=="flagoutputrecon") this->flagOutputRecon = readFlag(ss); 
	if(arg=="fileoutputrecon") this->fileOutputRecon = readSval(ss);
	if(arg=="flagoutputresid") this->flagOutputResid = readFlag(ss); 
	if(arg=="fileoutputresid") this->fileOutputResid = readSval(ss);
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
	if(arg=="fdrnumcorchan")   this->FDRnumCorChan = readIval(ss);
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
	if(arg=="sortingparam")    this->sortingParam = makelower(readSval(ss));
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
	if(arg=="beamsize"){
	  this->areaBeam = readFlag(ss);
	  std::stringstream errmsg;
	  errmsg <<"The parameter beamSize is deprecated.\n"
		 <<"You can specify the beam size by beamArea or beamFWHM.\n"
		 <<"Setting beamArea = " << stringize(this->areaBeam) << ".\n";
	  duchampWarning("Reading parameters",errmsg.str());
	}

      }
    }

    this->checkPars();

    return SUCCESS;

  }
  
  void Param::checkPars()
  {

    // If we have usePrevious=false, set the objectlist to blank so that we use all of them
    if(!this->usePrevious) this->objectList = "";

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

    // Make sure the growth level is less than the detection level. Else turn off growing.
    if(this->flagGrowth){
      std::stringstream errmsg;
      bool doWarn = false;
      if(this->flagUserThreshold &&
	 ( (this->threshold < this->growthThreshold)
	   || (this->snrCut < this->growthCut) ) ){
	errmsg << "Your \"growthThreshold\" parameter is larger than your \"threshold\".\n"
	       << "The growth function is being turned off.\n";
	doWarn = true;
      }
      
      if(!this->flagUserThreshold &&
	 (this->snrCut < this->growthCut)) {
	errmsg << "Your \"growthCut\" parameter is larger than your \"snrCut\".\n"
	       << "The growth function is being turned off.\n";
	doWarn = true;
      }

      if(doWarn) duchampWarning("Reading parameters",errmsg.str());

    }

    // Make sure the annnotationType is an acceptable option -- default is "borders"
    if((this->annotationType != "borders") && (this->annotationType!="circles")){
      std::stringstream errmsg;
      errmsg << "The requested value of the parameter annotationType, \""
	     << this->annotationType << "\", is invalid.\n"
	     << "Changing to \"borders\".\n";
      duchampWarning("Reading parameters",errmsg.str());
      this->annotationType = "borders";
    }
      
    // Make sure smoothType is an acceptable type -- default is "spectral"
    if((this->smoothType!="spectral")&&
       (this->smoothType!="spatial")){
      std::stringstream errmsg;
      errmsg << "The requested value of the parameter smoothType, \""
	     << this->smoothType << "\", is invalid.\n"
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
	     << this->spectralMethod << "\", is invalid.\n"
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
	     << this->pixelCentre << "\", is invalid.\n"
	     << "Changing to \"centroid\".\n";
      duchampWarning("Reading parameters",errmsg.str());
      this->pixelCentre = "centroid";
    }

    // Make sure sortingParam is an acceptable type -- default is "vel"
    bool OK = false;
    for(int i=0;i<numSortingParamOptions;i++) 
      OK = OK || this->sortingParam==sortingParamOptions[i];
    if(!OK){
      std::stringstream errmsg;
      errmsg << "The requested value of the parameter sortingParam, \""
	     << this->sortingParam << "\", is invalid.\n"
	     << "Changing to \"vel\".\n";
      duchampWarning("Reading parameters",errmsg.str());
      this->sortingParam = "vel";
    }
      
  }

  void recordParameters(std::ostream& theStream, std::string paramName, std::string paramDesc, std::string paramValue)
  {
    
    const int widthText = 38;
    const int widthPar  = 18;

    theStream << std::setw(widthText) << paramDesc
	      << std::setw(widthPar) << setiosflags(std::ios::right) << paramName
	      << "  =  " << resetiosflags(std::ios::right) << paramValue 
	      <<std::endl;
  }

  std::string fileOption(bool flag, std::string file)
  {
    std::ostringstream ss;
    ss << stringize(flag);
    if(flag) ss << " --> " << file;
    return ss.str();
    
  }

  std::ostream& operator<< ( std::ostream& theStream, Param& par)
  {
    /// Print out the parameter set in a formatted, easy to read style.
    /// Lists the parameters, a description of them, and their value.

    // Only show the [beamSize] bit if we are using the parameter
    // otherwise we have read it from the FITS header.
    std::string beamParam = "";
    if(par.getFlagUsingBeam()) beamParam = "[beamSize]";

    // BUG -- can get error: `boolalpha' is not a member of type `ios' -- old compilers: gcc 2.95.3?
    //   theStream.setf(std::ios::boolalpha);
    theStream.setf(std::ios::left);
    theStream  <<"\n---- Parameters ----"<<std::endl;
    theStream  << std::setfill('.');
    if(par.getFlagSubsection())
      recordParam(theStream, "[imageFile]", "Image to be analysed", par.getImageFile()<<par.getSubsection());
    else 
      recordParam(theStream, "[imageFile]", "Image to be analysed", par.getImageFile());
    if(par.getFlagReconExists() && par.getFlagATrous()){
      recordParam(theStream, "[reconExists]", "Reconstructed array exists?", stringize(par.getFlagReconExists()));
      recordParam(theStream, "[reconFile]", "FITS file containing reconstruction", par.getReconFile());
    }
    if(par.getFlagSmoothExists() && par.getFlagSmooth()){
      recordParam(theStream, "[smoothExists]", "Smoothed array exists?", stringize(par.getFlagSmoothExists()));
      recordParam(theStream, "[smoothFile]", "FITS file containing smoothed array", par.getSmoothFile());
    }
    recordParam(theStream, "[logFile]", "Intermediate Logfile", par.getLogFile());
    recordParam(theStream, "[outFile]", "Final Results file", par.getOutFile());
    if(par.getFlagSeparateHeader()){
      recordParam(theStream, "[headerFile]", "Header for results file", par.getHeaderFile());
    }
    recordParam(theStream, "[spectraFile]", "Spectrum file", par.getSpectraFile());
    if(par.getFlagTextSpectra()){
      recordParam(theStream, "[spectraTextFile]", "Text file with ascii spectral data", par.getSpectraTextFile());
    }
    if(par.getFlagVOT()){
      recordParam(theStream, "[votFile]", "VOTable file", par.getVOTFile());
    }
    if(par.getFlagKarma()){
      recordParam(theStream, "[karmaFile]", "Karma annotation file" , par.getKarmaFile());
    }
    if(par.getFlagMaps()){
      recordParam(theStream, "[momentMap]", "0th Moment Map", par.getMomentMap());
      recordParam(theStream, "[detectionMap]", "Detection Map", par.getDetectionMap());
    }
    recordParam(theStream, "[flagXOutput]", "Display a map in a pgplot xwindow?", stringize(par.getFlagXOutput()));
    if(par.getFlagATrous()){
      recordParam(theStream, "[flagOutputRecon]", "Saving reconstructed cube?", fileOption(par.getFlagOutputRecon(),par.outputReconFile()));
      recordParam(theStream, "[flagOutputResid]", "Saving residuals from reconstruction?", fileOption(par.getFlagOutputResid(),par.outputResidFile()));
    }						       
    if(par.getFlagSmooth()){	
      recordParam(theStream, "[flagOutputSmooth]", "Saving smoothed cube?", fileOption(par.getFlagOutputSmooth(),par.outputSmoothFile()));
    }						       
    recordParam(theStream, "[flagOutputMask]", "Saving mask cube?", fileOption(par.getFlagOutputMask(),par.outputMaskFile()));

    theStream  <<"------"<<std::endl;

    if(par.getFlagBlankPix()){
      recordParam(theStream, "", "Blank Pixel Value", par.getBlankPixVal());
    }
    recordParam(theStream, "[flagTrim]", "Trimming Blank Pixels?", stringize(par.getFlagTrim()));
    recordParam(theStream, "[flagNegative]", "Searching for Negative features?", stringize(par.getFlagNegative()));
    recordParam(theStream, "[flagMW]", "Removing Milky Way channels?", stringize(par.getFlagMW()));
    if(par.getFlagMW()){
      recordParam(theStream, "[minMW - maxMW]", "Milky Way Channels", par.getMinMW()<<"-"<<par.getMaxMW());
    }
    recordParam(theStream, beamParam, "Beam Size (pixels)", par.getBeamSize());
    recordParam(theStream, "[flagBaseline]", "Removing baselines before search?", stringize(par.getFlagBaseline()));
    recordParam(theStream, "[flagSmooth]", "Smoothing data prior to searching?", stringize(par.getFlagSmooth()));
    if(par.getFlagSmooth()){	       
      recordParam(theStream, "[smoothType]", "Type of smoothing", par.getSmoothType());
      if(par.getSmoothType()=="spectral")
	recordParam(theStream, "[hanningWidth]", "Width of hanning filter", par.getHanningWidth());
      else{
	recordParam(theStream, "[kernMaj]", "Gaussian kernel semi-major axis [pix]", par.getKernMaj());
	recordParam(theStream, "[kernMin]", "Gaussian kernel semi-minor axis [pix]", par.getKernMin());
	recordParam(theStream, "[kernPA]",  "Gaussian kernel position angle [deg]",  par.getKernPA());
      }
    }
    recordParam(theStream, "[flagATrous]", "Using A Trous reconstruction?", stringize(par.getFlagATrous()));
    if(par.getFlagATrous()){			       
      recordParam(theStream, "[reconDim]", "Number of dimensions in reconstruction", par.getReconDim());
      if(par.getMaxScale()>0){
	recordParam(theStream, "[scaleMin-scaleMax]", "Scales used in reconstruction", par.getMinScale()<<"-"<<par.getMaxScale());
      }
      else{
	recordParam(theStream, "[scaleMin]", "Minimum scale in reconstruction", par.getMinScale());
      }
      recordParam(theStream, "[snrRecon]", "SNR Threshold within reconstruction", par.getAtrousCut());
      recordParam(theStream, "[filterCode]", "Filter being used for reconstruction", par.getFilterCode()<<" ("<<par.getFilterName()<<")");
    }	     					       
    recordParam(theStream, "[flagRobustStats]", "Using Robust statistics?", stringize(par.getFlagRobustStats()));
    if(par.getFlagStatSec()){
      recordParam(theStream, "[statSec]", "Section used by statistics calculation", par.statSec.getSection());
    }
    recordParam(theStream, "[flagFDR]", "Using FDR analysis?", stringize(par.getFlagFDR()));
    if(par.getFlagFDR()){				       
      recordParam(theStream, "[alphaFDR]", "Alpha value for FDR analysis", par.getAlpha());
      recordParam(theStream, "[FDRnumCorChan]", "Number of correlated channels for FDR", par.getFDRnumCorChan());
    }	     					       
    else {
      if(par.getFlagUserThreshold()){
	recordParam(theStream, "[threshold]", "Detection Threshold", par.getThreshold());
      }
      else{
	recordParam(theStream, "[snrCut]", "SNR Threshold (in sigma)", par.getCut());
      }
    }
    recordParam(theStream, "[minPix]", "Minimum # Pixels in a detection", par.getMinPix());
    recordParam(theStream, "[minChannels]", "Minimum # Channels in a detection", par.getMinChannels());
    recordParam(theStream, "[flagGrowth]", "Growing objects after detection?", stringize(par.getFlagGrowth()));
    if(par.getFlagGrowth()) {			       
      if(par.getFlagUserGrowthThreshold()){
	recordParam(theStream, "[growthThreshold]", "Threshold for growth", par.getGrowthThreshold());
      }
      else{
	recordParam(theStream, "[growthCut]", "SNR Threshold for growth", par.getGrowthCut());
      }
    }
    recordParam(theStream, "[flagAdjacent]", "Using Adjacent-pixel criterion?", stringize(par.getFlagAdjacent()));
    if(!par.getFlagAdjacent()){
      recordParam(theStream, "[threshSpatial]", "Max. spatial separation for merging", par.getThreshS());
    }
    recordParam(theStream, "[threshVelocity]", "Max. velocity separation for merging", par.getThreshV());
    recordParam(theStream, "[spectralMethod]", "Method of spectral plotting", par.getSpectralMethod());
    recordParam(theStream, "[pixelCentre]", "Type of object centre used in results", par.getPixelCentre());

    theStream  <<"--------------------\n\n";
    theStream  << std::setfill(' ');
    theStream.unsetf(std::ios::left);
    //  theStream.unsetf(std::ios::boolalpha);
    return theStream;
  }


  void Param::copyHeaderInfo(FitsHeader &head)
  {
    ///  A function to copy across relevant header keywords from the 
    ///  FitsHeader class to the Param class, as they are needed by
    ///  functions in the Param class.
    ///  The parameters are the keywords BLANK, BSCALE, BZERO, and the beam size. 

    this->blankKeyword  = head.getBlankKeyword();
    this->bscaleKeyword = head.getBscaleKeyword();
    this->bzeroKeyword  = head.getBzeroKeyword();
    this->blankPixValue = this->blankKeyword * this->bscaleKeyword + 
      this->bzeroKeyword;

    this->areaBeam    = head.getBeamSize();
  }

  std::string Param::outputMaskFile()
  {
    ///  This function produces the required filename in which to save
    ///  the mask image, indicating which pixels have been detected as
    ///  part of an object. If the input image is image.fits, then the
    ///  output will be image.MASK.fits.

    if(this->fileOutputMask==""){
      std::string inputName = this->imageFile;
      std::stringstream ss;
      ss << inputName.substr(0,inputName.size()-5);  
      // remove the ".fits" on the end.
      ss << ".MASK.fits";
      return ss.str();
    }
    else return this->fileOutputMask;
  }

  std::string Param::outputSmoothFile()
  {
    ///  This function produces the required filename in which to save
    ///   the smoothed array. If the input image is image.fits, then
    ///   the output will be:
    ///    <ul><li> Spectral smoothing: image.SMOOTH-1D-3.fits, where the
    ///             width of the Hanning filter was 3 pixels.
    ///        <li> Spatial smoothing : image.SMOOTH-2D-3-2-20.fits, where
    ///             kernMaj=3, kernMin=2 and kernPA=20 degrees.
    ///    </ul>

    if(this->fileOutputSmooth==""){
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
    else return this->fileOutputSmooth;
  }

  std::string Param::outputReconFile()
  {
    /// This function produces the required filename in which to save
    ///  the reconstructed array. If the input image is image.fits, then
    ///  the output will be eg. image.RECON-3-2-4-1.fits, where the numbers are
    ///  3=reconDim, 2=filterCode, 4=snrRecon, 1=minScale

    if(this->fileOutputRecon==""){
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
    else return this->fileOutputRecon;
  }

  std::string Param::outputResidFile()
  {
    /// This function produces the required filename in which to save
    ///  the reconstructed array. If the input image is image.fits, then
    ///  the output will be eg. image.RESID-3-2-4-1.fits, where the numbers are
    ///  3=reconDim, 2=filterCode, 4=snrRecon, 1=scaleMin

    if(this->fileOutputResid==""){
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
    else return this->fileOutputResid;
  }

}
