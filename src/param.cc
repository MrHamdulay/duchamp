#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
#include <wcs.h>
#include <wcsunits.h>
#include <duchamp.hh>
#include <param.hh>
#include <config.h>
#include <Utils/utils.hh>

// Define funtion to print bools as words, in case the compiler doesn't recognise
// the setf(ios::boolalpha) command...
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

FitsHeader::FitsHeader(){
  this->wcs = new wcsprm;
  this->wcs->flag=-1;
  wcsini(true,3,this->wcs); 
  this->wcsIsGood = false;
  this->nwcs = 0;
  this->scale=1.;
  this->offset=0.;
  this->power=1.;
  this->fluxUnits="counts";
}

FitsHeader::FitsHeader(const FitsHeader& h){
  this->wcs = new wcsprm;
  this->wcs->flag=-1;
  wcsini(true,h.wcs->naxis,this->wcs); 
  wcscopy(true,h.wcs,this->wcs); 
  wcsset(this->wcs);
  nwcs = h.nwcs;
  wcsIsGood = h.wcsIsGood;
  spectralUnits = h.spectralUnits;
  fluxUnits = h.fluxUnits;
  intFluxUnits = h.intFluxUnits;
  beamSize = h.beamSize;
  bmajKeyword = h.bmajKeyword;
  bminKeyword = h.bminKeyword;
  blankKeyword = h.blankKeyword;
  bzeroKeyword = h.bzeroKeyword;
  bscaleKeyword = h.bscaleKeyword;
  scale = h.scale;
  offset = h.offset;
  power = h.power;
}

FitsHeader& FitsHeader::operator= (const FitsHeader& h){
  this->wcs = new wcsprm;
  this->wcs->flag=-1;
  wcsini(true,h.wcs->naxis,this->wcs); 
  wcscopy(true,h.wcs,this->wcs); 
  wcsset(this->wcs);
  nwcs = h.nwcs;
  wcsIsGood = h.wcsIsGood;
  spectralUnits = h.spectralUnits;
  fluxUnits = h.fluxUnits;
  intFluxUnits = h.intFluxUnits;
  beamSize = h.beamSize;
  bmajKeyword = h.bmajKeyword;
  bminKeyword = h.bminKeyword;
  blankKeyword = h.blankKeyword;
  bzeroKeyword = h.bzeroKeyword;
  bscaleKeyword = h.bscaleKeyword;
  scale = h.scale;
  offset = h.offset;
  power = h.power;
}

void FitsHeader::setWCS(wcsprm *w)
{
  /** 
   * FitsHeader::setWCS(wcsprm *)
   *  A function that assigns the wcs parameters, and runs
   *   wcsset to set it up correctly.
   *  Performs a check to see if the WCS is good (by looking at 
   *   the lng and lat wcsprm parameters), and sets the wcsIsGood 
   *   flag accordingly.
   */
  wcscopy(true,w,this->wcs);
  wcsset(this->wcs);
  if( (w->lng!=-1) && (w->lat!=-1) ) this->wcsIsGood = true;
}

wcsprm *FitsHeader::getWCS()
{
  /** 
   * FitsHeader::getWCS()
   *  A function that returns a wcsprm object corresponding to the WCS.
   */
  wcsprm *wNew = new wcsprm;
  wNew->flag=-1;
  wcsini(true,this->wcs->naxis,wNew); 
  wcscopy(true,this->wcs,wNew); 
  wcsset(wNew);
  return wNew;
}

int     FitsHeader::wcsToPix(const double *world, double *pix){
  return wcsToPixSingle(this->wcs,world,pix);}
int     FitsHeader::wcsToPix(const double *world, double *pix, const int npts){
  return wcsToPixMulti(this->wcs,world,pix,npts);}
int     FitsHeader::pixToWCS(const double *pix, double *world){
  return pixToWCSSingle(this->wcs,pix,world);}
int     FitsHeader::pixToWCS(const double *pix, double *world, const int npts){
  return pixToWCSMulti(this->wcs,pix,world,npts);}

double  FitsHeader::pixToVel(double &x, double &y, double &z)
{
//   return pixelToVelocity(this->wcs,x,y,z,this->spectralUnits);};
  double *pix   = new double[3]; 
  double *world = new double[3];
  pix[0] = x; pix[1] = y; pix[2] = z;
  pixToWCSSingle(this->wcs,pix,world);
  double vel = this->specToVel(world[2]);
  delete [] pix;
  delete [] world;
  return vel;
}

double* FitsHeader::pixToVel(double &x, double &y, double *zarray, int size)
{
//   return pixelToVelocity(this->wcs,x,y,z,this->spectralUnits);};
  double *pix   = new double[size*3];
  for(int i=0;i<size;i++){
    pix[3*i] = x; pix[3*i+1] = y; pix[3*i+2] = zarray[i];
  }
  double *world = new double[size*3];
  pixToWCSMulti(this->wcs,pix,world,size);
  delete [] pix;
  double *newzarray = new double[size];
  for(int i=0;i<size;i++) newzarray[i] = this->specToVel(world[3*i+2]);
  delete [] world;
  return newzarray;
}

double  FitsHeader::specToVel(const double &coord)
{
//   return coordToVel(this->wcs,coord,this->spectralUnits);};
  double vel;
  if(power==1.0){
//     std::cerr << coord << " --> " << coord*this->scale + this->offset << std::endl;
    vel =  coord*this->scale + this->offset;
  }
  else vel = pow( (coord*this->scale + this->offset), this->power);

  return vel;
}

double  FitsHeader::velToSpec(const float &velocity)
{
//   return velToCoord(this->wcs,velocity,this->spectralUnits);};
  return (pow(velocity, 1./this->power) - this->offset) / this->scale;}

string  FitsHeader::getIAUName(double ra, double dec)
{
  if(strcmp(this->wcs->lngtyp,"RA")==0) return getIAUNameEQ(ra, dec, this->wcs->equinox);
  else return getIAUNameGAL(ra, dec);
}

void FitsHeader::fixUnits(Param &par)
{
  // define spectral units from the param set
  this->spectralUnits = par.getSpectralUnits();

  double sc=1.;
  double of=0.;
  double po=1.;;
  if(this->wcsIsGood){
    int flag = wcsunits( this->wcs->cunit[2], this->spectralUnits.c_str(), &sc, &of, &po);
    if(flag>0){
      std::stringstream errmsg;
      errmsg << "WCSUNITS Error, Code = " << flag << ": "<< wcsunits_errmsg[flag];
      if(flag==10) errmsg << "\nTried to get conversion from '" << wcs->cunit[2]
			  << "' to '" << this->spectralUnits.c_str() << "'\n";
      this->spectralUnits = this->wcs->cunit[2];
      if(this->spectralUnits==""){
	errmsg << "Setting coordinates to 'XX' for clarity.\n";
	this->spectralUnits = "XX";
      }
      duchampError("fixUnits", errmsg.str());
      
    }
  }
  this->scale = sc;
  this->offset= of;
  this->power = po;

  // work out the integrated flux units, based on the spectral units
  // if flux is per beam, trim the /beam from the flux units and multiply by the spectral units.
  // otherwise, just muliply by the spectral units.
  if(this->fluxUnits.size()>0){
    if(this->fluxUnits.substr(this->fluxUnits.size()-5,this->fluxUnits.size())=="/beam"){
      this->intFluxUnits = this->fluxUnits.substr(0,this->fluxUnits.size()-5)
	+" " +this->spectralUnits;
    }
    else this->intFluxUnits = fluxUnits + " " + this->spectralUnits;
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
  this->imageFile       = "";
  this->flagSubsection  = false;
  this->subsection      = "[*,*,*]";
  this->flagReconExists = false;
  this->reconFile       = "";
  // Output files
  this->flagLog         = true;
  this->logFile         = "duchamp-Logfile.txt";
  this->outFile         = "duchamp-Results.txt";
  this->spectraFile     = "duchamp-Spectra.ps";
  this->flagOutputRecon = false;
  this->flagOutputResid = false;
  this->flagVOT         = false;
  this->votFile         = "duchamp-Results.xml";
  this->flagKarma       = false;
  this->karmaFile       = "duchamp-Results.ann";
  this->flagMaps        = true;
  this->detectionMap    = "duchamp-DetectionMap.ps";
  this->momentMap       = "duchamp-MomentMap.ps";
  // Cube related parameters 
  this->flagBlankPix    = true;
  this->blankPixValue   = -8.00061;
  this->blankKeyword    = 1;
  this->bscaleKeyword   = -8.00061;
  this->bzeroKeyword    = 0.;
  this->flagUsingBlank  = false;
  this->flagMW          = false;
  this->maxMW           = 112;
  this->minMW           = 75;
  this->numPixBeam      = 0.;
  // Trim-related         
  this->flagTrimmed     = false;
  this->borderLeft      = 0;
  this->borderRight     = 0;
  this->borderBottom    = 0;
  this->borderTop       = 0;
  // Subsection offsets
  this->xSubOffset      = 0;
  this->ySubOffset      = 0;
  this->zSubOffset      = 0;
  // Baseline related
  this->flagBaseline    = false;
  // Detection-related    
  this->flagNegative    = false;
  // Object growth        
  this->flagGrowth      = false;
  this->growthCut       = 2.;
  // FDR analysis         
  this->flagFDR         = false;
  this->alphaFDR        = 0.01;
  // Other detection      
  this->snrCut          = 3.;
  // A trous reconstruction parameters
  this->flagATrous      = true;
  this->reconDim        = 3;
  this->scaleMin        = 1;
  this->snrRecon        = 4.;
  this->filterCode      = 1;
  // Volume-merging parameters
  this->flagAdjacent    = true;
  this->threshSpatial   = 3.;
  this->threshVelocity  = 7.;
  this->minChannels     = 3;
  this->minPix          = 2;
  // Input-Output related
  this->spectralMethod  = "peak";
  this->borders         = true;
  this->verbose         = true;
  this->spectralUnits   = "km/s";
};

/****************************************************************/
///////////////////////////////////////////////////
//// Other Functions using the  Parameter class:
///////////////////////////////////////////////////

string makelower( string s )
{
  // "borrowed" from Matt Howlett's 'fred'
  string out = "";
  for( int i=0; i<s.size(); ++i ) {
    out += tolower(s[i]);
  }
  return out;
}

bool boolify( string s )
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

void Param::readParams(string paramfile)
{

  std::ifstream fin(paramfile.c_str());
  string line;
  string sval;
  float fval;
  int ival;
  while( !std::getline(fin,line,'\n').eof()){

    if(line[0]!='#'){
      std::stringstream ss;
      ss.str(line);
      string arg;
      ss >> arg;      
      if(makelower(arg)=="imagefile"){       ss >> sval; this->imageFile = sval;}
      if(makelower(arg)=="flagsubsection"){  ss >> sval; this->flagSubsection = boolify(sval); }
      if(makelower(arg)=="subsection"){      ss >> sval; this->subsection = sval; }
      if(makelower(arg)=="flagreconexists"){ ss >> sval; this->flagReconExists = boolify(sval); }
      if(makelower(arg)=="reconfile"){       ss >> sval; this->reconFile = sval; }

      if(makelower(arg)=="flaglog"){         ss >> sval; this->flagLog = boolify(sval); }
      if(makelower(arg)=="logfile"){         ss >> sval; this->logFile = sval; }
      if(makelower(arg)=="outfile"){         ss >> sval; this->outFile = sval; }
      if(makelower(arg)=="spectrafile"){     ss >> sval; this->spectraFile = sval; }
      if(makelower(arg)=="flagoutputrecon"){ ss >> sval; this->flagOutputRecon = boolify(sval); }
      if(makelower(arg)=="flagoutputresid"){ ss >> sval; this->flagOutputResid = boolify(sval); }
      if(makelower(arg)=="flagvot"){         ss >> sval; this->flagVOT = boolify(sval); }
      if(makelower(arg)=="votfile"){         ss >> sval; this->votFile = sval; }
      if(makelower(arg)=="flagkarma"){       ss >> sval; this->flagKarma = boolify(sval); }
      if(makelower(arg)=="karmafile"){       ss >> sval; this->karmaFile = sval; }
      if(makelower(arg)=="flagmaps"){        ss >> sval; this->flagMaps = boolify(sval); }
      if(makelower(arg)=="detectionmap"){    ss >> sval; this->detectionMap = sval; }
      if(makelower(arg)=="momentmap"){       ss >> sval; this->momentMap = sval; }

      if(makelower(arg)=="flagnegative"){    ss >> sval; this->flagNegative = boolify(sval);}
      if(makelower(arg)=="flagblankpix"){    ss >> sval; this->flagBlankPix = boolify(sval); }
      if(makelower(arg)=="blankpixvalue"){   ss >> fval; this->blankPixValue = fval; }
      if(makelower(arg)=="flagmw"){          ss >> sval; this->flagMW = boolify(sval); }
      if(makelower(arg)=="maxmw"){           ss >> ival; this->maxMW = ival; }
      if(makelower(arg)=="minmw"){           ss >> ival; this->minMW = ival; }

      if(makelower(arg)=="flagbaseline"){    ss >> sval; this->flagBaseline = boolify(sval); }
      if(makelower(arg)=="minpix"){          ss >> ival; this->minPix = ival; }
      if(makelower(arg)=="flaggrowth"){      ss >> sval; this->flagGrowth = boolify(sval); }
      if(makelower(arg)=="growthcut"){       ss >> fval; this->growthCut = fval; }

      if(makelower(arg)=="flagfdr"){         ss >> sval; this->flagFDR = boolify(sval); }
      if(makelower(arg)=="alphafdr"){        ss >> fval; this->alphaFDR = fval; }

      if(makelower(arg)=="snrcut"){          ss >> fval; this->snrCut = fval; }

      if(makelower(arg)=="flagatrous"){      ss >> sval; this->flagATrous = boolify(sval); }
      if(makelower(arg)=="recondim"){        ss >> ival; this->reconDim = ival; }
      if(makelower(arg)=="scalemin"){        ss >> ival; this->scaleMin = ival; }
      if(makelower(arg)=="snrrecon"){        ss >> fval; this->snrRecon = fval; }
      if(makelower(arg)=="filtercode"){      ss >> ival; this->filterCode = ival; }

      if(makelower(arg)=="flagadjacent"){    ss >> sval; this->flagAdjacent = boolify(sval); }
      if(makelower(arg)=="threshspatial"){   ss >> fval; this->threshSpatial = fval; }
      if(makelower(arg)=="threshvelocity"){  ss >> fval; this->threshVelocity = fval; }
      if(makelower(arg)=="minchannels"){     ss >> ival; this->minChannels = ival; }

      if(makelower(arg)=="spectralmethod"){  ss >> sval; this->spectralMethod = makelower(sval); }
      if(makelower(arg)=="spectralunits"){   ss >> sval; this->spectralUnits = makelower(sval); }
      if(makelower(arg)=="drawborders"){     ss >> sval; this->borders = boolify(sval); }
      if(makelower(arg)=="verbose"){         ss >> sval; this->verbose = boolify(sval); }
    }
  }
}


std::ostream& operator<< ( std::ostream& theStream, Param& par)
{
  // Only show the [blankPixValue] bit if we are using the parameter
  // otherwise we have read it from the FITS header.
  string blankParam = "";
  if(par.getFlagUsingBlank()) blankParam = "[blankPixValue]";

  // BUG -- can get error: `boolalpha' is not a member of type `ios' -- old compilers: gcc 2.95.3?
//   theStream.setf(std::ios::boolalpha);
  theStream.setf(std::ios::left);
  theStream  <<"---- Parameters ----"<<endl;
  theStream  << std::setfill('.');
  if(par.getFlagSubsection())
    theStream<<setw(38)<<"Image to be analysed"                 
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[imageFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)
	     <<(par.getImageFile()+par.getSubsection())   <<endl;
  else 
    theStream<<setw(38)<<"Image to be analysed"                 
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[imageFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getImageFile()      <<endl;
  if(par.getFlagReconExists() && par.getFlagATrous()){
    theStream<<setw(38)<<"Reconstructed array exists?"          
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[reconExists]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<stringize(par.getFlagReconExists())<<endl;
    theStream<<setw(38)<<"FITS file containing reconstruction"  
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[reconFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getReconFile()      <<endl;
  }
  theStream  <<setw(38)<<"Intermediate Logfile"
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[logFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getLogFile()        <<endl;
  theStream  <<setw(38)<<"Final Results file"                   
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[outFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getOutFile()        <<endl;
  theStream  <<setw(38)<<"Spectrum file"                        
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[spectraFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getSpectraFile()    <<endl;
  if(par.getFlagVOT()){
    theStream<<setw(38)<<"VOTable file"                         
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[votFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getVOTFile()        <<endl;
  }
  if(par.getFlagKarma()){
    theStream<<setw(38)<<"Karma annotation file"                
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[karmaFile]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getKarmaFile()      <<endl;
  }
  if(par.getFlagMaps()){
    theStream<<setw(38)<<"0th Moment Map"                       
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[momentMap]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getMomentMap()      <<endl;
    theStream<<setw(38)<<"Detection Map"                        
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[detectionMap]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getDetectionMap()   <<endl;
  }
  if(par.getFlagATrous()){			       
    theStream<<setw(38)<<"Saving reconstructed cube?"           
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[flagoutputrecon]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<stringize(par.getFlagOutputRecon())<<endl;
    theStream<<setw(38)<<"Saving residuals from reconstruction?"
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[flagoutputresid]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<stringize(par.getFlagOutputResid())<<endl;
  }						       
  theStream  <<"------"<<endl;
  theStream  <<setw(38)<<"Searching for Negative features?"     
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[flagNegative]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<stringize(par.getFlagNegative())   <<endl;
  theStream  <<setw(38)<<"Fixing Blank Pixels?"                 
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[flagBlankPix]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<stringize(par.getFlagBlankPix())   <<endl;
  if(par.getFlagBlankPix()){
    theStream<<setw(38)<<"Blank Pixel Value"                    
	     <<setw(18)<<setiosflags(std::ios::right)  << blankParam
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getBlankPixVal()    <<endl;
  }
  theStream  <<setw(38)<<"Removing Milky Way channels?"         
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[flagMW]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getFlagMW()         <<endl;
  if(par.getFlagMW()){
    theStream<<setw(38)<<"Milky Way Channels"                   
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[minMW - maxMW]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getMinMW()
	     <<"-" <<par.getMaxMW()          <<endl;
  }
  theStream  <<setw(38)<<"Beam Size (pixels)"                   
	     <<setw(18)<<setiosflags(std::ios::right)  <<""
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getBeamSize()       <<endl;
  theStream  <<setw(38)<<"Removing baselines before search?"    
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[flagBaseline]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<stringize(par.getFlagBaseline())   <<endl;
  theStream  <<setw(38)<<"Minimum # Pixels in a detection"      
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[minPix]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getMinPix()         <<endl;
  theStream  <<setw(38)<<"Minimum # Channels in a detection"    
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[minChannels]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getMinChannels()    <<endl;
  theStream  <<setw(38)<<"Growing objects after detection?"     
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[flagGrowth]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<stringize(par.getFlagGrowth())     <<endl;
  if(par.getFlagGrowth()) {			       
    theStream<<setw(38)<<"SNR Threshold for growth"             
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[growthCut]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getGrowthCut()      <<endl;
  }
  theStream  <<setw(38)<<"Using A Trous reconstruction?"        
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[flagATrous]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<stringize(par.getFlagATrous())     <<endl;
  if(par.getFlagATrous()){			       
    theStream<<setw(38)<<"Number of dimensions in reconstruction"      
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[reconDim]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getReconDim()       <<endl;
    theStream<<setw(38)<<"Minimum scale in reconstruction"      
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[scaleMin]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getMinScale()       <<endl;
    theStream<<setw(38)<<"SNR Threshold within reconstruction"  
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[snrRecon]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getAtrousCut()      <<endl;
    theStream<<setw(38)<<"Filter being used for reconstruction" 
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[filterCode]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getFilterCode() 
	     << " (" << par.getFilterName()  << ")" <<endl;
  }	     					       
  theStream  <<setw(38)<<"Using FDR analysis?"                  
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[flagFDR]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<stringize(par.getFlagFDR())        <<endl;
  if(par.getFlagFDR()){				       
    theStream<<setw(38)<<"Alpha value for FDR analysis"         
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[alphaFDR]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getAlpha()          <<endl;
  }	     					       
  else {
    theStream<<setw(38)<<"SNR Threshold"                        
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[snrCut]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getCut()            <<endl;
  }
  theStream  <<setw(38)<<"Using Adjacent-pixel criterion?"      
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[flagAdjacent]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<stringize(par.getFlagAdjacent())   <<endl;
  if(!par.getFlagAdjacent()){
    theStream<<setw(38)<<"Max. spatial separation for merging"  
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[threshSpatial]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getThreshS()        <<endl;
  }
  theStream  <<setw(38)<<"Max. velocity separation for merging" 
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[threshVelocity]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getThreshV()        <<endl;
  theStream  <<setw(38)<<"Method of spectral plotting"          
	     <<setw(18)<<setiosflags(std::ios::right)  <<"[spectralMethod]"
	     <<"  =  " <<resetiosflags(std::ios::right)<<par.getSpectralMethod() <<endl;
  theStream  <<"--------------------"<<endl;
  theStream  << std::setfill(' ');
  theStream.unsetf(std::ios::left);
  //  theStream.unsetf(std::ios::boolalpha);
}

void Param::parseSubsection()
{
  /**
   *  Param::parseSubsection()
   *
   *   Convert the Param::subsection string to a series of offset values
   *    for each dimension.
   *   This involves reading the individual substrings and converting to
   *    integers, and storing in the {x,y,z}SubOffset parameters.
   *   Steps in the subsection string are not dealt with -- a warning message
   *    is written to the screen, and the step values are ignored.
   */

  std::stringstream ss;
  ss.str(this->subsection);
  bool removeStep = false;
  string x,y,z;
  getline(ss,x,'[');
  getline(ss,x,',');
  getline(ss,y,',');
  getline(ss,z,']');
  char *end;
  if(x!="*") {
    int x1,x2,dx;
    int a = x.find(':');  // first occurence of ':' in x-subsection string
    int b = x.find(':',a+1); // location of second ':' -- will be -1 if there is no second occurence.
    x1 = atoi( x.substr(0,a).c_str() ); // minimum x in subsection
    this->xSubOffset = x1 - 1;
    if(b>0){ // there is a dx component
      x2 = atoi( x.substr(a+1,b-a-1).c_str() ); // maximum x in subsection
      dx = atoi( x.substr(b+1,x.size()).c_str() ); // delta x in subsection
      x = x.substr(0,b); // rewrite x without the delta-x part.
      removeStep = true;
    }
  }
  if(y!="*") {
    int y1,y2,dy;
    int a = y.find(':');  // first occurence of ':' in y-subsection string
    int b = y.find(':',a+1); // location of second ':' -- will be -1 if there is no second occurence.
    y1 = atoi( y.substr(0,a).c_str() ); // minimum y in subsection
    this->ySubOffset = y1 - 1;
    if(b>0){ // there is a dy component
      y2 = atoi( y.substr(a+1,b-a-1).c_str() ); // maximum y in subsection
      dy = atoi( y.substr(b+1,y.size()).c_str() ); // delta y in subsection
      y = y.substr(0,b); // rewrite y without the delta-y part.
      removeStep = true;
    }
  }
  if(z!="*") {
    int z1,z2,dz;
    int a = z.find(':');  // first occurence of ':' in z-subsection string
    int b = z.find(':',a+1); // location of second ':' -- will be -1 if there is no second occurence.
    z1 = atoi( z.substr(0,a).c_str() ); // minimum z in subsection
    this->zSubOffset = z1 - 1;
    if(b>0){ // there is a dz component
      z2 = atoi( z.substr(a+1,b-a-1).c_str() ); // maximum z in subsection
      dz = atoi( z.substr(b+1,z.size()).c_str() ); // delta z in subsection
      z = z.substr(0,b); // rewrite z without the delta-z part.
      removeStep = true;
    }
  }

  if(removeStep){
    std::cerr << "\a\tThe subsection given is " << this->subsection <<".\n";
    std::cerr << "\tDuchamp is currently unable to deal with pixel steps in the subsection.\n";
    std::cerr << "\tThese have been ignored, and so the subection used is ";    
    this->subsection = "[" + x + "," + y + "," + z + "]";  // rewrite subsection without any step sizes.
    std::cerr << this->subsection << std::endl;
  }

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
  this->blankPixValue = this->blankKeyword * this->bscaleKeyword + this->bzeroKeyword;

  this->numPixBeam    = head.getBeamSize();
}

bool Param::isBlank(float &value)
{
  /** 
   *  Param::isBlank(float)
   *   Tests whether the value passed as the argument is BLANK or not.
   *   If flagBlankPix is false, return false.
   *   Otherwise, compare to the relevant FITS keywords, using integer comparison.
   *   Return a bool.
   */

  return this->flagBlankPix &&
    (this->blankKeyword == int((value-this->bzeroKeyword)/this->bscaleKeyword));
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
