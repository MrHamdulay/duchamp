#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <wcs.h>
#include <wcsunits.h>
#include <param.hh>
#include <config.h>
#include <duchamp.hh>
#include <Utils/utils.hh>

// Define funtion to print bools as words, in case the compiler doesn't 
//  recognise the setf(ios::boolalpha) command...
#ifdef HAVE_STDBOOL_H
string stringize(bool b){
  std::stringstream ss;
  ss.setf(std::ios::boolalpha);
  ss << b;
  return ss.str();
}
#else
string stringize(bool b){
  std::string output;
  if(b) output="true";
  else output="false";
  return output;
}
#endif

using std::setw;
using std::endl;

/****************************************************************/
///////////////////////////////////////////////////
//// Functions for FitsHeader class:
///////////////////////////////////////////////////

FitsHeader::FitsHeader()
{
  this->wcs = (struct wcsprm *)calloc(1,sizeof(struct wcsprm));
  this->wcs->flag=-1;
  wcsini(true, 3, this->wcs); 
  this->wcsIsGood = false;
  this->nwcs = 0;
  this->scale=1.;
  this->offset=0.;
  this->power=1.;
  this->fluxUnits="counts";
}

FitsHeader::FitsHeader(const FitsHeader& h)
{
  this->wcs = (struct wcsprm *)calloc(1,sizeof(struct wcsprm));
  this->wcs->flag=-1;
  wcsini(true, h.wcs->naxis, this->wcs); 
  wcscopy(true, h.wcs, this->wcs); 
  wcsset(this->wcs);
  this->nwcs = h.nwcs;
  this->wcsIsGood = h.wcsIsGood;
  this->spectralUnits = h.spectralUnits;
  this->fluxUnits = h.fluxUnits;
  this->intFluxUnits = h.intFluxUnits;
  this->beamSize = h.beamSize;
  this->bmajKeyword = h.bmajKeyword;
  this->bminKeyword = h.bminKeyword;
  this->blankKeyword = h.blankKeyword;
  this->bzeroKeyword = h.bzeroKeyword;
  this->bscaleKeyword = h.bscaleKeyword;
  this->scale = h.scale;
  this->offset = h.offset;
  this->power = h.power;
}

FitsHeader& FitsHeader::operator= (const FitsHeader& h)
{
  this->wcs = (struct wcsprm *)calloc(1,sizeof(struct wcsprm));
  this->wcs->flag=-1;
  wcsini(true, h.wcs->naxis, this->wcs); 
  wcscopy(true, h.wcs, this->wcs); 
  wcsset(this->wcs);
  this->nwcs = h.nwcs;
  this->wcsIsGood = h.wcsIsGood;
  this->spectralUnits = h.spectralUnits;
  this->fluxUnits = h.fluxUnits;
  this->intFluxUnits = h.intFluxUnits;
  this->beamSize = h.beamSize;
  this->bmajKeyword = h.bmajKeyword;
  this->bminKeyword = h.bminKeyword;
  this->blankKeyword = h.blankKeyword;
  this->bzeroKeyword = h.bzeroKeyword;
  this->bscaleKeyword = h.bscaleKeyword;
  this->scale = h.scale;
  this->offset = h.offset;
  this->power = h.power;
}

void FitsHeader::setWCS(struct wcsprm *w)
{
  /** 
   * FitsHeader::setWCS(struct wcsprm)
   *  A function that assigns the wcs parameters, and runs
   *   wcsset to set it up correctly.
   *  Performs a check to see if the WCS is good (by looking at 
   *   the lng and lat wcsprm parameters), and sets the wcsIsGood 
   *   flag accordingly.
   */
  wcscopy(true, w, this->wcs);
  wcsset(this->wcs);
  if( (w->lng!=-1) && (w->lat!=-1) ) this->wcsIsGood = true;
}

struct wcsprm *FitsHeader::getWCS()
{
  /** 
   * FitsHeader::getWCS()
   *  A function that returns a wcsprm object corresponding to the WCS.
   */
  struct wcsprm *wNew = (struct wcsprm *)calloc(1,sizeof(struct wcsprm));
  wNew->flag=-1;
  wcsini(true, this->wcs->naxis, wNew); 
  wcscopy(true, this->wcs, wNew); 
  wcsset(wNew);
  return wNew;
}

int     FitsHeader::wcsToPix(const double *world, double *pix){
  return wcsToPixSingle(this->wcs, world, pix);}
int     FitsHeader::wcsToPix(const double *world, double *pix, const int npts){
  return wcsToPixMulti(this->wcs, world, pix, npts);}
int     FitsHeader::pixToWCS(const double *pix, double *world){
  return pixToWCSSingle(this->wcs, pix, world);}
int     FitsHeader::pixToWCS(const double *pix, double *world, const int npts){
  return pixToWCSMulti(this->wcs, pix,world, npts);}

double FitsHeader::pixToVel(double &x, double &y, double &z)
{
  double vel;
  if(this->wcsIsGood){
    double *pix   = new double[3]; 
    double *world = new double[3];
    pix[0] = x; pix[1] = y; pix[2] = z;
    pixToWCSSingle(this->wcs,pix,world);
    vel = this->specToVel(world[2]);
    delete [] pix;
    delete [] world;
  }
  else vel = z;
  return vel;
}

double* FitsHeader::pixToVel(double &x, double &y, double *zarray, int size)
{
  double *newzarray = new double[size];
  if(this->wcsIsGood){
    double *pix   = new double[size*3];
    for(int i=0;i<size;i++){
      pix[3*i]   = x; 
      pix[3*i+1] = y; 
      pix[3*i+2] = zarray[i];
    }
    double *world = new double[size*3];
    pixToWCSMulti(this->wcs,pix,world,size);
    delete [] pix;
    for(int i=0;i<size;i++) newzarray[i] = this->specToVel(world[3*i+2]);
    delete [] world;
  }
  else{
    for(int i=0;i<size;i++) newzarray[i] = zarray[i];
  }
  return newzarray;
}

double  FitsHeader::specToVel(const double &coord)
{
  double vel;
  if(power==1.0) vel =  coord*this->scale + this->offset;
  else vel = pow( (coord*this->scale + this->offset), this->power);
  return vel;
}

double  FitsHeader::velToSpec(const float &velocity)
{
//   return velToCoord(this->wcs,velocity,this->spectralUnits);};
  return (pow(velocity, 1./this->power) - this->offset) / this->scale;}

string  FitsHeader::getIAUName(double ra, double dec)
{
  if(strcmp(this->wcs->lngtyp,"RA")==0) 
    return getIAUNameEQ(ra, dec, this->wcs->equinox);
  else 
    return getIAUNameGAL(ra, dec);
}

void FitsHeader::fixUnits(Param &par)
{
  // define spectral units from the param set
  this->spectralUnits = par.getSpectralUnits();

  double sc=1.;
  double of=0.;
  double po=1.;;
  if(this->wcsIsGood){
    int status = wcsunits( this->wcs->cunit[this->wcs->spec], 
			 this->spectralUnits.c_str(), 
			 &sc, &of, &po);
    if(status > 0){
      std::stringstream errmsg;
      errmsg << "WCSUNITS Error, Code = " << status
	     << ": " << wcsunits_errmsg[status];
      if(status == 10) errmsg << "\nTried to get conversion from '" 
			      << this->wcs->cunit[this->wcs->spec] << "' to '" 
			      << this->spectralUnits.c_str() << "'\n";
      this->spectralUnits = this->wcs->cunit[this->wcs->spec];
      if(this->spectralUnits==""){
	errmsg << 
	  "Spectral units not specified. Setting them to 'XX' for clarity.\n";
	this->spectralUnits = "XX";
      }
      duchampError("fixUnits", errmsg.str());
      
    }
  }
  this->scale = sc;
  this->offset= of;
  this->power = po;

  // Work out the integrated flux units, based on the spectral units.
  // If flux is per beam, trim the /beam from the flux units and multiply 
  //  by the spectral units.
  // Otherwise, just muliply by the spectral units.
  if(this->fluxUnits.size()>0){
    if(this->fluxUnits.substr(this->fluxUnits.size()-5,
			      this->fluxUnits.size()   ) == "/beam"){
      this->intFluxUnits = this->fluxUnits.substr(0,this->fluxUnits.size()-5)
	+" " +this->spectralUnits;
    }
    else this->intFluxUnits = this->fluxUnits + " " + this->spectralUnits;
  }

}

/****************************************************************/
///////////////////////////////////////////////////
//// Accessor Functions for Parameter class:
///////////////////////////////////////////////////
Param::Param(){
  /** 
   * Param()
   *  Default intial values for the parameters.
   * imageFile has no default value!
   */
  // Input files
  this->imageFile         = "";
  this->flagSubsection    = false;
  this->subsection        = "[*,*,*]";
  this->flagReconExists   = false;
  this->reconFile         = "";
  this->flagSmoothExists  = false;
  this->smoothFile        = "";
  // Output files
  this->flagLog           = true;
  this->logFile           = "duchamp-Logfile.txt";
  this->outFile           = "duchamp-Results.txt";
  this->spectraFile       = "duchamp-Spectra.ps";
  this->flagOutputSmooth  = false;
  this->flagOutputRecon   = false;
  this->flagOutputResid   = false;
  this->flagVOT           = false;
  this->votFile           = "duchamp-Results.xml";
  this->flagKarma         = false;
  this->karmaFile         = "duchamp-Results.ann";
  this->flagMaps          = true;
  this->detectionMap      = "duchamp-DetectionMap.ps";
  this->momentMap         = "duchamp-MomentMap.ps";
  this->flagXOutput       = true;
  // Cube related parameters 
  this->flagBlankPix      = true;
  this->blankPixValue     = -8.00061;
  this->blankKeyword      = 1;
  this->bscaleKeyword     = -8.00061;
  this->bzeroKeyword      = 0.;
  this->flagUsingBlank    = false;
  this->flagMW            = false;
  this->maxMW             = 112;
  this->minMW             = 75;
  this->numPixBeam        = 10.;
  this->flagUsingBeam     = false;
  // Trim-related         
  this->flagTrimmed       = false;
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
  this->growthCut         = 2.;
  // FDR analysis         
  this->flagFDR           = false;
  this->alphaFDR          = 0.01;
  // Other detection      
  this->snrCut            = 3.;
  this->threshold         = 0.;
  this->flagUserThreshold = false;
  // Hanning Smoothing 
  this->flagSmooth        = false;
  this->hanningWidth      = 5;
  // A trous reconstruction parameters
  this->flagATrous        = true;
  this->reconDim          = 3;
  this->scaleMin          = 1;
  this->snrRecon          = 4.;
  this->filterCode        = 1;
  // Volume-merging parameters
  this->flagAdjacent      = true;
  this->threshSpatial     = 3.;
  this->threshVelocity    = 7.;
  this->minChannels       = 3;
  this->minPix            = 2;
  // Input-Output related
  this->spectralMethod    = "peak";
  this->borders           = true;
  this->blankEdge         = true;
  this->verbose           = true;
  this->spectralUnits     = "km/s";
};

Param::Param (const Param& p)
{
  this->imageFile         = p.imageFile;
  this->flagSubsection    = p.flagSubsection; 
  this->subsection        = p.subsection;     
  this->flagReconExists   = p.flagReconExists;
  this->reconFile         = p.reconFile;      
  this->flagSmoothExists  = p.flagSmoothExists;
  this->smoothFile        = p.smoothFile;      
  this->flagLog           = p.flagLog;        
  this->logFile           = p.logFile;       
  this->outFile           = p.outFile;        
  this->spectraFile       = p.spectraFile;    
  this->flagOutputSmooth  = p.flagOutputSmooth;
  this->flagOutputRecon   = p.flagOutputRecon;
  this->flagOutputResid   = p.flagOutputResid;
  this->flagVOT           = p.flagVOT;         
  this->votFile           = p.votFile;        
  this->flagKarma         = p.flagKarma;      
  this->karmaFile         = p.karmaFile;      
  this->flagMaps          = p.flagMaps;       
  this->detectionMap      = p.detectionMap;   
  this->momentMap         = p.momentMap;      
  this->flagXOutput       = p.flagXOutput;       
  this->flagBlankPix      = p.flagBlankPix;   
  this->blankPixValue     = p.blankPixValue;  
  this->blankKeyword      = p.blankKeyword;   
  this->bscaleKeyword     = p.bscaleKeyword;  
  this->bzeroKeyword      = p.bzeroKeyword;   
  this->flagUsingBlank    = p.flagUsingBlank; 
  this->flagMW            = p.flagMW;         
  this->maxMW             = p.maxMW;          
  this->minMW             = p.minMW;         
  this->numPixBeam        = p.numPixBeam;     
  this->flagTrimmed       = p.flagTrimmed;    
  this->borderLeft        = p.borderLeft;     
  this->borderRight       = p.borderRight;    
  this->borderBottom      = p.borderBottom;   
  this->borderTop         = p.borderTop;      
  this->sizeOffsets       = p.sizeOffsets;
  if(this->sizeOffsets>0)
    for(int i=0;i<this->sizeOffsets;i++) this->offsets[i] = p.offsets[i];
  this->xSubOffset        = p.xSubOffset;     
  this->ySubOffset        = p.ySubOffset;     
  this->zSubOffset        = p.zSubOffset;
  this->flagBaseline      = p.flagBaseline;
  this->flagNegative      = p.flagNegative;
  this->flagGrowth        = p.flagGrowth;
  this->growthCut         = p.growthCut;
  this->flagFDR           = p.flagFDR;
  this->alphaFDR          = p.alphaFDR;
  this->snrCut            = p.snrCut;
  this->threshold         = p.threshold;
  this->flagUserThreshold = p.flagUserThreshold;
  this->flagSmooth        = p.flagSmooth;
  this->hanningWidth      = p.hanningWidth;
  this->flagATrous        = p.flagATrous;
  this->reconDim          = p.reconDim;
  this->scaleMin          = p.scaleMin;
  this->snrRecon          = p.snrRecon;
  this->filterCode        = p.filterCode;
  this->flagAdjacent      = p.flagAdjacent;
  this->threshSpatial     = p.threshSpatial;
  this->threshVelocity    = p.threshVelocity;
  this->minChannels       = p.minChannels;
  this->minPix            = p.minPix;
  this->spectralMethod    = p.spectralMethod;
  this->borders           = p.borders;
  this->verbose           = p.verbose;
  this->spectralUnits     = p.spectralUnits;
}

Param& Param::operator= (const Param& p)
{
  this->imageFile         = p.imageFile;
  this->flagSubsection    = p.flagSubsection; 
  this->subsection        = p.subsection;     
  this->flagReconExists   = p.flagReconExists;
  this->reconFile         = p.reconFile;      
  this->flagSmoothExists  = p.flagSmoothExists;
  this->smoothFile        = p.smoothFile;      
  this->flagLog           = p.flagLog;        
  this->logFile           = p.logFile;       
  this->outFile           = p.outFile;        
  this->spectraFile       = p.spectraFile;    
  this->flagOutputSmooth  = p.flagOutputSmooth;
  this->flagOutputRecon   = p.flagOutputRecon;
  this->flagOutputResid   = p.flagOutputResid;
  this->flagVOT           = p.flagVOT;         
  this->votFile           = p.votFile;        
  this->flagKarma         = p.flagKarma;      
  this->karmaFile         = p.karmaFile;      
  this->flagMaps          = p.flagMaps;       
  this->detectionMap      = p.detectionMap;   
  this->momentMap         = p.momentMap;      
  this->flagXOutput       = p.flagXOutput;       
  this->flagBlankPix      = p.flagBlankPix;   
  this->blankPixValue     = p.blankPixValue;  
  this->blankKeyword      = p.blankKeyword;   
  this->bscaleKeyword     = p.bscaleKeyword;  
  this->bzeroKeyword      = p.bzeroKeyword;   
  this->flagUsingBlank    = p.flagUsingBlank; 
  this->flagMW            = p.flagMW;         
  this->maxMW             = p.maxMW;          
  this->minMW             = p.minMW;         
  this->numPixBeam        = p.numPixBeam;     
  this->flagTrimmed       = p.flagTrimmed;    
  this->borderLeft        = p.borderLeft;     
  this->borderRight       = p.borderRight;    
  this->borderBottom      = p.borderBottom;   
  this->borderTop         = p.borderTop;      
  this->sizeOffsets       = p.sizeOffsets;
  if(this->sizeOffsets>0)
    for(int i=0;i<this->sizeOffsets;i++) this->offsets[i] = p.offsets[i];
  this->xSubOffset        = p.xSubOffset;     
  this->ySubOffset        = p.ySubOffset;     
  this->zSubOffset        = p.zSubOffset;
  this->flagBaseline      = p.flagBaseline;
  this->flagNegative      = p.flagNegative;
  this->flagGrowth        = p.flagGrowth;
  this->growthCut         = p.growthCut;
  this->flagFDR           = p.flagFDR;
  this->alphaFDR          = p.alphaFDR;
  this->snrCut            = p.snrCut;
  this->threshold         = p.threshold;
  this->flagUserThreshold = p.flagUserThreshold;
  this->flagSmooth        = p.flagSmooth;
  this->hanningWidth      = p.hanningWidth;
  this->flagATrous        = p.flagATrous;
  this->reconDim          = p.reconDim;
  this->scaleMin          = p.scaleMin;
  this->snrRecon          = p.snrRecon;
  this->filterCode        = p.filterCode;
  this->flagAdjacent      = p.flagAdjacent;
  this->threshSpatial     = p.threshSpatial;
  this->threshVelocity    = p.threshVelocity;
  this->minChannels       = p.minChannels;
  this->minPix            = p.minPix;
  this->spectralMethod    = p.spectralMethod;
  this->borders           = p.borders;
  this->verbose           = p.verbose;
  this->spectralUnits     = p.spectralUnits;
}

/****************************************************************/
///////////////////////////////////////////////////
//// Other Functions using the  Parameter class:
///////////////////////////////////////////////////

inline string makelower( string s )
{
  // "borrowed" from Matt Howlett's 'fred'
  string out = "";
  for( int i=0; i<s.size(); ++i ) {
    out += tolower(s[i]);
  }
  return out;
}

inline bool boolify( string s )
{
  /**
   * bool boolify(string)
   *  Convert a string to a bool variable
   *  "1" and "true" get converted to true
   *  "0" and "false" (and anything else) get converted to false
   */
  if((s=="1") || (makelower(s)=="true")) return true;
  else if((s=="0") || (makelower(s)=="false")) return false;
  else return false;
}

inline string readSval(std::stringstream& ss)
{
  string val; ss >> val; return val;
}

inline bool readFlag(std::stringstream& ss)
{
  string val; ss >> val; return boolify(val);
}

inline float readFval(std::stringstream& ss)
{
  float val; ss >> val; return val;
}

inline int readIval(std::stringstream& ss)
{
  int val; ss >> val; return val;
}

int Param::readParams(string paramfile)
{

  std::ifstream fin(paramfile.c_str());
  if(!fin.is_open()) return FAILURE;
  string line;
  while( !std::getline(fin,line,'\n').eof()){

    if(line[0]!='#'){
      std::stringstream ss;
      ss.str(line);
      string arg;
      ss >> arg;
      arg = makelower(arg);
      if(arg=="imagefile")       this->imageFile = readSval(ss);
      if(arg=="flagsubsection")  this->flagSubsection = readFlag(ss); 
      if(arg=="subsection")      this->subsection = readSval(ss); 
      if(arg=="flagreconexists") this->flagReconExists = readFlag(ss); 
      if(arg=="reconfile")       this->reconFile = readSval(ss); 
      if(arg=="flagsmoothexists")this->flagSmoothExists = readFlag(ss); 
      if(arg=="smoothfile")      this->smoothFile = readSval(ss); 

      if(arg=="flaglog")         this->flagLog = readFlag(ss); 
      if(arg=="logfile")         this->logFile = readSval(ss); 
      if(arg=="outfile")         this->outFile = readSval(ss); 
      if(arg=="spectrafile")     this->spectraFile = readSval(ss); 
      if(arg=="flagoutputsmooth")this->flagOutputSmooth = readFlag(ss); 
      if(arg=="flagoutputrecon") this->flagOutputRecon = readFlag(ss); 
      if(arg=="flagoutputresid") this->flagOutputResid = readFlag(ss); 
      if(arg=="flagvot")         this->flagVOT = readFlag(ss); 
      if(arg=="votfile")         this->votFile = readSval(ss); 
      if(arg=="flagkarma")       this->flagKarma = readFlag(ss); 
      if(arg=="karmafile")       this->karmaFile = readSval(ss); 
      if(arg=="flagmaps")        this->flagMaps = readFlag(ss); 
      if(arg=="detectionmap")    this->detectionMap = readSval(ss); 
      if(arg=="momentmap")       this->momentMap = readSval(ss); 
      if(arg=="flagxoutput")     this->flagXOutput = readFlag(ss); 

      if(arg=="flagnegative")    this->flagNegative = readFlag(ss);
      if(arg=="flagblankpix")    this->flagBlankPix = readFlag(ss); 
      if(arg=="blankpixvalue")   this->blankPixValue = readFval(ss); 
      if(arg=="flagmw")          this->flagMW = readFlag(ss); 
      if(arg=="maxmw")           this->maxMW = readIval(ss); 
      if(arg=="minmw")           this->minMW = readIval(ss); 
      if(arg=="beamsize")        this->numPixBeam = readFval(ss); 

      if(arg=="flagbaseline")    this->flagBaseline = readFlag(ss); 
      if(arg=="minpix")          this->minPix = readIval(ss); 
      if(arg=="flaggrowth")      this->flagGrowth = readFlag(ss); 
      if(arg=="growthcut")       this->growthCut = readFval(ss); 

      if(arg=="flagfdr")         this->flagFDR = readFlag(ss); 
      if(arg=="alphafdr")        this->alphaFDR = readFval(ss); 

      if(arg=="snrcut")          this->snrCut = readFval(ss); 
      if(arg=="threshold"){
	                         this->threshold = readFval(ss);
                                 this->flagUserThreshold = true;
      }
      
      if(arg=="flagsmooth")      this->flagSmooth = readFlag(ss);
      if(arg=="hanningwidth")    this->hanningWidth = readIval(ss);

      if(arg=="flagatrous")      this->flagATrous = readFlag(ss); 
      if(arg=="recondim")        this->reconDim = readIval(ss); 
      if(arg=="scalemin")        this->scaleMin = readIval(ss); 
      if(arg=="snrrecon")        this->snrRecon = readFval(ss); 
      if(arg=="filtercode")      this->filterCode = readIval(ss); 

      if(arg=="flagadjacent")    this->flagAdjacent = readFlag(ss); 
      if(arg=="threshspatial")   this->threshSpatial = readFval(ss); 
      if(arg=="threshvelocity")  this->threshVelocity = readFval(ss); 
      if(arg=="minchannels")     this->minChannels = readIval(ss); 

      if(arg=="spectralmethod")  this->spectralMethod=makelower(readSval(ss));
      if(arg=="spectralunits")   this->spectralUnits=makelower(readSval(ss));
      if(arg=="drawborders")     this->borders = readFlag(ss); 
      if(arg=="drawblankedges")  this->blankEdge = readFlag(ss); 
      if(arg=="verbose")         this->verbose = readFlag(ss); 
    }
  }
  if(this->flagATrous) this->flagSmooth = false;
  return SUCCESS;
}


std::ostream& operator<< ( std::ostream& theStream, Param& par)
{
  // Only show the [blankPixValue] bit if we are using the parameter
  // otherwise we have read it from the FITS header.
  string blankParam = "";
  if(par.getFlagUsingBlank()) blankParam = "[blankPixValue]";
  string beamParam = "";
  if(par.getFlagUsingBeam()) beamParam = "[beamSize]";

  // BUG -- can get error: `boolalpha' is not a member of type `ios' -- old compilers: gcc 2.95.3?
  //   theStream.setf(std::ios::boolalpha);
  theStream.setf(std::ios::left);
  theStream  <<"---- Parameters ----"<<endl;
  theStream  << std::setfill('.');
  const int widthText = 38;
  const int widthPar  = 18;
  if(par.getFlagSubsection())
    theStream<<setw(widthText)<<"Image to be analysed"                 
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[imageFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<(par.getImageFile()+par.getSubsection()) <<endl;
  else 
    theStream<<setw(widthText)<<"Image to be analysed"                 
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[imageFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getImageFile()      <<endl;
  if(par.getFlagReconExists() && par.getFlagATrous()){
    theStream<<setw(widthText)<<"Reconstructed array exists?"          
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[reconExists]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<stringize(par.getFlagReconExists())<<endl;
    theStream<<setw(widthText)<<"FITS file containing reconstruction"  
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[reconFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getReconFile()      <<endl;
  }
  if(par.getFlagSmoothExists() && par.getFlagSmooth()){
    theStream<<setw(widthText)<<"Hanning-smoothed array exists?"          
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[smoothExists]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<stringize(par.getFlagSmoothExists())<<endl;
    theStream<<setw(widthText)<<"FITS file containing smoothed array"  
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[smoothFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getSmoothFile()      <<endl;
  }
  theStream  <<setw(widthText)<<"Intermediate Logfile"
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[logFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getLogFile()        <<endl;
  theStream  <<setw(widthText)<<"Final Results file"                   
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[outFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getOutFile()        <<endl;
  theStream  <<setw(widthText)<<"Spectrum file"                        
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[spectraFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getSpectraFile()    <<endl;
  if(par.getFlagVOT()){
    theStream<<setw(widthText)<<"VOTable file"                         
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[votFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getVOTFile()        <<endl;
  }
  if(par.getFlagKarma()){
    theStream<<setw(widthText)<<"Karma annotation file"                
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[karmaFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     
	     <<par.getKarmaFile()      <<endl;
  }
  if(par.getFlagMaps()){
    theStream<<setw(widthText)<<"0th Moment Map"                       
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[momentMap]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getMomentMap()      <<endl;
    theStream<<setw(widthText)<<"Detection Map"                        
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[detectionMap]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getDetectionMap()   <<endl;
  }
  theStream  <<setw(widthText)<<"Display a map in a pgplot xwindow?" 
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[flagXOutput]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<stringize(par.getFlagXOutput())     <<endl;
  if(par.getFlagATrous()){			       
    theStream<<setw(widthText)<<"Saving reconstructed cube?"           
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[flagoutputrecon]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<stringize(par.getFlagOutputRecon())<<endl;
    theStream<<setw(widthText)<<"Saving residuals from reconstruction?"
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[flagoutputresid]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<stringize(par.getFlagOutputResid())<<endl;
  }						       
  if(par.getFlagSmooth()){			       
    theStream<<setw(widthText)<<"Saving Hanning-smoothed cube?"           
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[flagoutputsmooth]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<stringize(par.getFlagOutputSmooth())<<endl;
  }						       
  theStream  <<"------"<<endl;
  theStream  <<setw(widthText)<<"Searching for Negative features?"     
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[flagNegative]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<stringize(par.getFlagNegative())   <<endl;
  theStream  <<setw(widthText)<<"Fixing Blank Pixels?"                 
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[flagBlankPix]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<stringize(par.getFlagBlankPix())   <<endl;
  if(par.getFlagBlankPix()){
    theStream<<setw(widthText)<<"Blank Pixel Value"                    
	     <<setw(widthPar)<<setiosflags(std::ios::right)<< blankParam
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getBlankPixVal()    <<endl;
  }
  theStream  <<setw(widthText)<<"Removing Milky Way channels?"         
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[flagMW]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<stringize(par.getFlagMW())         <<endl;
  if(par.getFlagMW()){
    theStream<<setw(widthText)<<"Milky Way Channels"                   
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[minMW - maxMW]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getMinMW()
	     <<"-" <<par.getMaxMW()          <<endl;
  }
  theStream  <<setw(widthText)<<"Beam Size (pixels)"                   
	     <<setw(widthPar)<<setiosflags(std::ios::right)<< beamParam
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getBeamSize()       <<endl;
  theStream  <<setw(widthText)<<"Removing baselines before search?"    
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[flagBaseline]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<stringize(par.getFlagBaseline())   <<endl;
  theStream  <<setw(widthText)<<"Minimum # Pixels in a detection"      
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[minPix]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getMinPix()         <<endl;
  theStream  <<setw(widthText)<<"Minimum # Channels in a detection"    
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[minChannels]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getMinChannels()    <<endl;
  theStream  <<setw(widthText)<<"Growing objects after detection?"     
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[flagGrowth]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<stringize(par.getFlagGrowth())     <<endl;
  if(par.getFlagGrowth()) {			       
    theStream<<setw(widthText)<<"SNR Threshold for growth"             
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[growthCut]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getGrowthCut()      <<endl;
  }
  theStream  <<setw(widthText)<<"Hanning-smoothing each spectrum first?"        
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[flagSmooth]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<stringize(par.getFlagSmooth())     <<endl;
  if(par.getFlagSmooth())			       
    theStream<<setw(widthText)<<"Width of hanning filter"      
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[hanningWidth]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getHanningWidth()       <<endl;
  theStream  <<setw(widthText)<<"Using A Trous reconstruction?"        
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[flagATrous]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<stringize(par.getFlagATrous())     <<endl;
  if(par.getFlagATrous()){			       
    theStream<<setw(widthText)<<"Number of dimensions in reconstruction"      
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[reconDim]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getReconDim()       <<endl;
    theStream<<setw(widthText)<<"Minimum scale in reconstruction"      
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[scaleMin]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getMinScale()       <<endl;
    theStream<<setw(widthText)<<"SNR Threshold within reconstruction"  
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[snrRecon]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getAtrousCut()      <<endl;
    theStream<<setw(widthText)<<"Filter being used for reconstruction" 
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[filterCode]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getFilterCode() 
	     << " (" << par.getFilterName()  << ")" <<endl;
  }	     					       
  theStream  <<setw(widthText)<<"Using FDR analysis?"                  
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[flagFDR]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<stringize(par.getFlagFDR())        <<endl;
  if(par.getFlagFDR()){				       
    theStream<<setw(widthText)<<"Alpha value for FDR analysis"         
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[alphaFDR]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getAlpha()          <<endl;
  }	     					       
  else {
    if(par.getFlagUserThreshold()){
      theStream<<setw(widthText)<<"Detection Threshold"                        
	       <<setw(widthPar)<<setiosflags(std::ios::right)<<"[threshold]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getThreshold()            <<endl;
    }
    else{
      theStream<<setw(widthText)<<"SNR Threshold (in sigma)"
	       <<setw(widthPar)<<setiosflags(std::ios::right)<<"[snrCut]"
	       <<"  =  " <<resetiosflags(std::ios::right)
	       <<par.getCut()            <<endl;
    }
  }
  theStream  <<setw(widthText)<<"Using Adjacent-pixel criterion?"      
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[flagAdjacent]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<stringize(par.getFlagAdjacent())   <<endl;
  if(!par.getFlagAdjacent()){
    theStream<<setw(widthText)<<"Max. spatial separation for merging"  
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[threshSpatial]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getThreshS()        <<endl;
  }
  theStream  <<setw(widthText)<<"Max. velocity separation for merging" 
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[threshVelocity]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getThreshV()        <<endl;
  theStream  <<setw(widthText)<<"Method of spectral plotting"          
	     <<setw(widthPar)<<setiosflags(std::ios::right)<<"[spectralMethod]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<par.getSpectralMethod() <<endl;
  theStream  <<"--------------------"<<endl;
  theStream  << std::setfill(' ');
  theStream.unsetf(std::ios::left);
  //  theStream.unsetf(std::ios::boolalpha);
}


void Param::copyHeaderInfo(FitsHeader &head)
{
  /**
   *  Param::copyHeaderInfo(FitsHeader &head)
   * A function to copy across relevant header keywords from the 
   *  FitsHeader class to the Param class, as they are needed by
   *  functions in the Param class.
   */

  this->blankKeyword  = head.getBlankKeyword();
  this->bscaleKeyword = head.getBscaleKeyword();
  this->bzeroKeyword  = head.getBzeroKeyword();
  this->blankPixValue = this->blankKeyword * this->bscaleKeyword + 
    this->bzeroKeyword;

  this->numPixBeam    = head.getBeamSize();
}

bool Param::isBlank(float &value)
{
  /** 
   *  Param::isBlank(float)
   *   Tests whether the value passed as the argument is BLANK or not.
   *   If flagBlankPix is false, return false.
   *   Otherwise, compare to the relevant FITS keywords, using integer 
   *     comparison.
   *   Return a bool.
   */

  return this->flagBlankPix &&
    (this->blankKeyword == int((value-this->bzeroKeyword)/this->bscaleKeyword));
}

string Param::outputSmoothFile()
{
  /** 
   *  outputSmoothFile()
   *   This function produces the required filename in which to save
   *    the Hanning-smoothed array. If the input image is image.fits, then
   *    the output will be eg. image.SMOOTH-3.fits, where the width of the 
   *    Hanning filter was 3 pixels.
   */
  string inputName = this->imageFile;
  std::stringstream ss;
  ss << inputName.substr(0,inputName.size()-5);  
                          // remove the ".fits" on the end.
  if(this->flagSubsection) ss<<".sub";
  ss << ".SMOOTH-" << this->hanningWidth
     << ".fits";
  return ss.str();
}

string Param::outputReconFile()
{
  /** 
   *  outputReconFile()
   *
   *   This function produces the required filename in which to save
   *    the reconstructed array. If the input image is image.fits, then
   *    the output will be eg. image.RECON-3-2-4-1.fits, where the numbers are
   *    3=reconDim, 2=filterCode, 4=snrRecon, 1=minScale
   */
  string inputName = this->imageFile;
  std::stringstream ss;
  ss << inputName.substr(0,inputName.size()-5);  // remove the ".fits" on the end.
  if(this->flagSubsection) ss<<".sub";
  ss << ".RECON-" << this->reconDim 
     << "-"       << this->filterCode
     << "-"       << this->snrRecon
     << "-"       << this->scaleMin
     << ".fits";
  return ss.str();
}

string Param::outputResidFile()
{
  /** 
   *  outputResidFile()
   *
   *   This function produces the required filename in which to save
   *    the reconstructed array. If the input image is image.fits, then
   *    the output will be eg. image.RESID-3-2-4-1.fits, where the numbers are
   *    3=reconDim, 2=filterCode, 4=snrRecon, 1=scaleMin
   */
  string inputName = this->imageFile;
  std::stringstream ss;
  ss << inputName.substr(0,inputName.size()-5);  // remove the ".fits" on the end.
  if(this->flagSubsection) ss<<".sub";
  ss << ".RESID-" << this->reconDim 
     << "-"       << this->filterCode
     << "-"       << this->snrRecon
     << "-"       << this->scaleMin
     << ".fits";
  return ss.str();
}
