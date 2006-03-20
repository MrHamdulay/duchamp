#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
#include <param.hh>

using std::setw;
using std::endl;

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
  // Output files
  this->flagLog         = true;
  this->logFile         = "./duchamp-Logfile";
  this->outFile         = "./duchamp-Results";
  this->spectraFile     = "./duchamp-Spectra.ps";
  this->flagOutputRecon = false;
  this->flagOutputResid = false;
  this->flagVOT         = false;
  this->votFile         = "./duchamp-Results.xml";
  this->flagMaps        = true;
  this->detectionMap    = "./latest-detection-map.ps";
  this->momentMap       = "./latest-moment-map.ps";
  // Cube related parameters 
  this->flagBlankPix    = true;
  this->blankPixValue   = -8.00061;
  this->blankKeyword    = 1;
  this->bscaleKeyword   = -8.00061;
  this->bzeroKeyword    = 0.;
  this->nanAsBlank      = false;
  this->flagMW          = true;
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
  this->minPix          = 2;
  // Object growth        
  this->flagGrowth      = true;
  this->growthCut       = 1.5;
  // FDR analysis         
  this->flagFDR         = false;
  this->alphaFDR        = 0.01;
  // Other detection      
  this->snrCut          = 3.;
  // A trous reconstruction parameters
  this->flagATrous      = true;
  this->scaleMin        = 1;
  this->snrRecon        = 4.;
  this->filterCode      = 2;
  // Volume-merging parameters
  this->flagAdjacent    = true;
  this->threshSpatial   = 3.;
  this->threshVelocity  = 7.;
  this->minChannels     = 3;
  // Input-Output related
  this->borders         = true;
  this->verbose         = true;
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

void Param::readParams(string &paramfile)
{

  std::ifstream fin(paramfile.c_str());
  string line;
  string sval;
  float fval;
  int ival;
  bool bval;
  while( !std::getline(fin,line,'\n').eof()){

    if(line[0]!='#'){
      std::stringstream ss;
      ss.str(line);
      string arg;
      ss >> arg;      
      if(makelower(arg)=="imagefile"){       ss >> sval; this->imageFile = sval;}
      if(makelower(arg)=="flaglog"){         ss >> bval; this->flagLog = bval; }
      if(makelower(arg)=="logfile"){         ss >> sval; this->logFile = sval; }
      if(makelower(arg)=="outfile"){         ss >> sval; this->outFile = sval; }
      if(makelower(arg)=="spectrafile"){     ss >> sval; this->spectraFile = sval; }
      if(makelower(arg)=="flagoutputrecon"){ ss >> bval; this->flagOutputRecon = bval; }
      if(makelower(arg)=="flagoutputresid"){ ss >> bval; this->flagOutputResid = bval; }
      if(makelower(arg)=="flagvot"){         ss >> bval; this->flagVOT = bval; }
      if(makelower(arg)=="votfile"){         ss >> sval; this->votFile = sval; }
      if(makelower(arg)=="flagmaps"){        ss >> bval; this->flagMaps = bval; }
      if(makelower(arg)=="detectionmap"){    ss >> sval; this->detectionMap = sval; }
      if(makelower(arg)=="momentmap"){       ss >> sval; this->momentMap = sval; }

      if(makelower(arg)=="flagblankpix"){    ss >> bval; this->flagBlankPix = bval; }
      if(makelower(arg)=="blankpixvalue"){   ss >> fval; this->blankPixValue = fval; }
      if(makelower(arg)=="flagmw"){          ss >> bval; this->flagMW = bval; }
      if(makelower(arg)=="maxmw"){           ss >> ival; this->maxMW = ival; }
      if(makelower(arg)=="minmw"){           ss >> ival; this->minMW = ival; }
      if(makelower(arg)=="minpix"){          ss >> ival; this->minPix = ival; }
      if(makelower(arg)=="flagfdr"){         ss >> bval; this->flagFDR = bval; }
      if(makelower(arg)=="alphafdr"){        ss >> fval; this->alphaFDR = fval; }
      if(makelower(arg)=="flagbaseline"){    ss >> bval; this->flagBaseline = bval; }
      if(makelower(arg)=="snrcut"){          ss >> fval; this->snrCut = fval; }
      if(makelower(arg)=="flaggrowth"){      ss >> bval; this->flagGrowth = bval; }
      if(makelower(arg)=="growthcut"){       ss >> fval; this->growthCut = fval; }
      if(makelower(arg)=="flagatrous"){      ss >> bval; this->flagATrous = bval; }
      if(makelower(arg)=="scalemin"){        ss >> ival; this->scaleMin = ival; }
      if(makelower(arg)=="snrrecon"){        ss >> fval; this->snrRecon = fval; }
      if(makelower(arg)=="filtercode"){      ss >> ival; this->filterCode = ival; }
      if(makelower(arg)=="flagadjacent"){    ss >> bval; this->flagAdjacent = bval; }
      if(makelower(arg)=="threshspatial"){   ss >> fval; this->threshSpatial = fval; }
      if(makelower(arg)=="threshvelocity"){  ss >> fval; this->threshVelocity = fval; }
      if(makelower(arg)=="minchannels"){     ss >> ival; this->minChannels = ival; }
      if(makelower(arg)=="flagsubsection"){  ss >> bval; this->flagSubsection = bval; }
      if(makelower(arg)=="subsection"){      ss >> sval; this->subsection = sval; }
      if(makelower(arg)=="drawborders"){     ss >> bval; this->borders = bval; }
      if(makelower(arg)=="verbose"){         ss >> bval; this->verbose = bval; }
    }
  }
}


std::ostream& operator<< ( std::ostream& theStream, Param& par)
{
  theStream.setf(std::ios::boolalpha);
  theStream.setf(std::ios::left);
  theStream  <<"---- Parameters ----"<<endl;
  if(par.getFlagSubsection())
    theStream<<setw(40)<<"Image to be analysed"                 <<"= "<<(par.getImageFile()+
									 par.getSubsection())   <<endl;
  else 
    theStream<<setw(40)<<"Image to be analysed"                 <<"= "<<par.getImageFile()      <<endl;
  theStream  <<setw(40)<<"Intermediate Logfile"                 <<"= "<<par.getLogFile()        <<endl;
  theStream  <<setw(40)<<"Final Results file"                   <<"= "<<par.getOutFile()        <<endl;
  theStream  <<setw(40)<<"Spectrum file"                        <<"= "<<par.getSpectraFile()    <<endl;
  if(par.getFlagVOT()){
    theStream<<setw(40)<<"VOTable file"                         <<"= "<<par.getVOTFile()        <<endl;
  }
  if(par.getFlagMaps()){
    theStream<<setw(40)<<"1st Moment Map"                       <<"= "<<par.getMomentMap()      <<endl;
    theStream<<setw(40)<<"Detection Map"                        <<"= "<<par.getDetectionMap()   <<endl;
  }
  if(par.getFlagATrous()){			       
    theStream<<setw(40)<<"Saving reconstructed cube?"           <<"= "<<par.getFlagOutputRecon()<<endl;
    theStream<<setw(40)<<"Saving residuals from reconstruction?"<<"= "<<par.getFlagOutputResid()<<endl;
  }						       
  theStream  <<"------"<<endl;				       
  theStream  <<setw(40)<<"Fixing Blank Pixels?"                 <<"= "<<par.getFlagBlankPix()   <<endl;
  if(par.getFlagBlankPix()){
    theStream<<setw(40)<<"Blank Pixel Value"                    <<"= "<<par.getBlankPixVal()    <<endl;
  }
  theStream  <<setw(40)<<"Removing Milky Way channels?"         <<"= "<<par.getFlagMW()         <<endl;
  if(par.getFlagMW()){
    theStream<<setw(40)<<"Milky Way Channels"                   <<"= "<<par.getMinMW()
	               <<"-"                                          <<par.getMaxMW()          <<endl;
  }
  theStream  <<setw(40)<<"Beam Size (pixels)"                   <<"= "<<par.getBeamSize()       <<endl;
  theStream  <<setw(40)<<"Removing baselines before search?"    <<"= "<<par.getFlagBaseline()   <<endl;
  theStream  <<setw(40)<<"Minimum # Pixels in a detection"      <<"= "<<par.getMinPix()         <<endl;
  theStream  <<setw(40)<<"Growing objects after detection?"     <<"= "<<par.getFlagGrowth()     <<endl;
  if(par.getFlagGrowth()) {			       
    theStream<<setw(40)<<"SNR Threshold for growth"             <<"= "<<par.getGrowthCut()      <<endl;
  }
  theStream  <<setw(40)<<"Using A Trous reconstruction?"        <<"= "<<par.getFlagATrous()     <<endl;
  if(par.getFlagATrous()){			       
    theStream<<setw(40)<<"Minimum scale in reconstruction"      <<"= "<<par.getMinScale()       <<endl;
    theStream<<setw(40)<<"SNR Threshold within reconstruction"  <<"= "<<par.getAtrousCut()      <<endl;
    theStream<<setw(40)<<"Filter being used for reconstruction" <<"= "<<par.getFilterName()     <<endl;
  }	     					       
  theStream  <<setw(40)<<"Using FDR analysis?"                  <<"= "<<par.getFlagFDR()        <<endl;
  if(par.getFlagFDR()){				       
    theStream<<setw(40)<<"Alpha value for FDR analysis"         <<"= "<<par.getAlpha()          <<endl;
  }	     					       
  else {
    theStream<<setw(40)<<"SNR Threshold"                        <<"= "<<par.getCut()            <<endl;
  }
  theStream  <<setw(40)<<"Using Adjacent-pixel criterion?"      <<"= "<<par.getFlagAdjacent()   <<endl;
  if(!par.getFlagAdjacent()){
    theStream<<setw(40)<<"Max. spatial separation for merging"  <<"= "<<par.getThreshS()        <<endl;
    theStream<<setw(40)<<"Max. velocity separation for merging" <<"= "<<par.getThreshV()        <<endl;
  }
  theStream  <<setw(40)<<"Min. # channels for merging"          <<"= "<<par.getMinChannels()    <<endl;
  theStream  <<"--------------------"<<endl;
  theStream.unsetf(std::ios::left);
  theStream.unsetf(std::ios::boolalpha);
}

void Param::parseSubsection()
{
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
  
//   if(x!="*") this->xSubOffset = strtol(x.c_str(),&end,10) - 1;    
//   if(y!="*") this->ySubOffset = strtol(y.c_str(),&end,10) - 1;    
//   if(z!="*") this->zSubOffset = strtol(z.c_str(),&end,10) - 1;    
}

bool Param::isBlank(float &val)
{
//   std::cerr << "val = " << val << " " << int((val-this->bzeroKeyword)/this->bscaleKeyword) 
// 	    <<" " <<this->blankKeyword << std::endl;
  if(this->flagBlankPix){
    if(this->nanAsBlank) return bool(isnan(val));
    else//  return (val == this->blankPixValue);
      return ( this->blankKeyword == int((val-this->bzeroKeyword)/this->bscaleKeyword) );
  }
  else return false;
}
