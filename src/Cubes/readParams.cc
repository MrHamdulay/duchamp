#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <Cubes/cubes.hh>

string makelower( string s )
{
  // "borrowed" from Matt Howlett's 'fred'
  string out = "";
  for( int i=0; i<s.size(); ++i ) {
    out += tolower(s[i]);
  }
  return out;
}

void readParams(string paramfile, Param &par)
{

  ifstream fin(paramfile.c_str());
  string input;
  string sval;
  float fval;
  int ival;
  bool bval;
  while( fin >> input){

    if(input[0]!='#'){
      if(makelower(input)=="nullpixvalue"){    fin >> fval; par.setNullPixVal(fval); }
      if(makelower(input)=="flagnullpix"){     fin >> bval; par.setFlagNullPix(bval); }
      if(makelower(input)=="flagmw"){          fin >> bval; par.setFlagMW(bval); }
      if(makelower(input)=="maxmw"){           fin >> ival; par.setMaxMW(ival); }
      if(makelower(input)=="minmw"){           fin >> ival; par.setMinMW(ival); }
      if(makelower(input)=="minpix"){          fin >> ival; par.setMinPix(ival); }
      if(makelower(input)=="flagfdr"){         fin >> bval; par.setFlagFDR(bval); }
      if(makelower(input)=="alphafdr"){        fin >> fval; par.setAlpha(fval); }
      if(makelower(input)=="numpixpsf"){       fin >> ival; par.setSizePSF(ival); }
      if(makelower(input)=="snrcut"){          fin >> fval; par.setCut(fval); }
      if(makelower(input)=="flaggrowth"){      fin >> bval; par.setFlagGrowth(bval); }
      if(makelower(input)=="growthcut"){       fin >> fval; par.setGrowthCut(fval); }
      if(makelower(input)=="flagatrous"){      fin >> bval; par.setFlagATrous(bval); }
      if(makelower(input)=="scalemin"){        fin >> ival; par.setMinScale(ival); }
      if(makelower(input)=="snrrecon"){        fin >> fval; par.setAtrousCut(fval); }
      if(makelower(input)=="flagadjacent"){    fin >> bval; par.setFlagAdjacent(bval); }
      if(makelower(input)=="threshspatial"){   fin >> fval; par.setThreshS(fval); }
      if(makelower(input)=="threshvelocity"){  fin >> fval; par.setThreshV(fval); }
      if(makelower(input)=="minchannels"){     fin >> ival; par.setMinChannels(ival); }
      if(makelower(input)=="flaglog"){         fin >> bval; par.setFlagLog(bval); }
      if(makelower(input)=="logfile"){         fin >> sval; par.setLogFile(sval); }
      if(makelower(input)=="outfile"){         fin >> sval; par.setOutFile(sval); }
      if(makelower(input)=="spectrafile"){     fin >> sval; par.setSpectraFile(sval); }
      if(makelower(input)=="imagefile"){       fin >> sval; par.setImageFile(sval); }
      if(makelower(input)=="flagsubsection"){  fin >> bval; par.setFlagSubsection(bval); }
      if(makelower(input)=="subsection"){      fin >> sval; par.setSubsection(sval); }
      if(makelower(input)=="flagoutputrecon"){ fin >> bval; par.setFlagOutputRecon(bval); }
      if(makelower(input)=="flagoutputresid"){ fin >> bval; par.setFlagOutputResid(bval); }
    }
  }

}

void showParams(Param &par)
{
  cerr.setf(ios::boolalpha);
  cerr<<"---- Parameters ---"<<endl;
  if(par.getFlagSubsection())
    cerr<<"Image to be analysed\t\t\t= "               <<(par.getImageFile()+
                                                          par.getSubsection())    <<endl;
  else 
    cerr<<"Image to be analysed\t\t\t= "               <<par.getImageFile()       <<endl;
  cerr<<"Intermediate Logfile\t\t\t= "                 <<par.getLogFile()         <<endl;
  cerr<<"Final Results file\t\t\t= "                   <<par.getOutFile()         <<endl;
  cerr<<"Spectrum file\t\t\t\t= "                      <<par.getSpectraFile()     <<endl;
  if(par.getFlagATrous()){			       
    cerr<<"Saving reconstructed cube?\t\t= "           <<par.getFlagOutputRecon() <<endl;
    cerr<<"Saving residuals from reconstruction?\t= "  <<par.getFlagOutputResid() <<endl;
  }						       
  cerr<<"-----"<<endl;				       
  cerr<<"Fixing Null Pixels?\t\t\t= "                  <<par.getFlagNullPix()     <<endl;
  cerr<<"Null Pixel Value\t\t\t= "                     <<par.getNullPixVal()      <<endl;
  cerr<<"Removing Milky Way channels?\t\t= "           <<par.getFlagMW()          <<endl;
  cerr<<"Milky Way Channels\t\t\t= "                   <<par.getMinMW()
      <<"-"                                            <<par.getMaxMW()           <<endl;
  cerr<<"Minimum # Pixels in a detection\t\t= "        <<par.getMinPix()          <<endl;
  cerr<<"Growing objects after detection?\t= "         <<par.getFlagGrowth()      <<endl;
  if(par.getFlagGrowth()) 			       
    cerr<<"SNR Threshold for growth\t\t= "             <<par.getGrowthCut()       <<endl;
  cerr<<"Using A Trous reconstruction?\t\t= "          <<par.getFlagATrous()      <<endl;
  if(par.getFlagATrous()){			       
    cerr<<"Minimum scale in reconstruction\t\t= "      <<par.getMinScale()        <<endl;
    cerr<<"SNR Threshold within reconstruction\t= "    <<par.getAtrousCut()       <<endl;
  }						       
  cerr<<"Using FDR analysis?\t\t\t= "                  <<par.getFlagFDR()         <<endl;
  if(par.getFlagFDR()){				       
    cerr<<"Alpha value for FDR analysis\t\t= "         <<par.getAlpha()           <<endl;
    cerr<<"Number of pixels in PSF (used by FDR)\t= "  <<par.getSizePSF()         <<endl;
  }						       
  else cerr<<"SNR Threshold\t\t\t\t= "                 <<par.getCut()             <<endl;
  cerr<<"Using Adjacent-pixel criterion?\t\t= "        <<par.getFlagAdjacent()    <<endl;
  if(!par.getFlagAdjacent()){
    cerr<<"Max. spatial separation for merging\t= "    <<par.getThreshS()         <<endl;
    cerr<<"Max. velocity separation for merging\t= "   <<par.getThreshV()         <<endl;
  }
  cerr<<"Min. # channels for merging\t\t= "            <<par.getMinChannels()     <<endl;
  cerr<<"-------------------"<<endl;
  cerr.unsetf(ios::boolalpha);
}

void showParams(Param &par, std::ostream &stream)
{
  stream.setf(ios::boolalpha);
  stream<<"---- Parameters ---"<<endl;
  if(par.getFlagSubsection())
    stream<<"Image to be analysed\t\t\t= "               <<(par.getImageFile()+
                                                            par.getSubsection())    <<endl;
  else 
    stream<<"Image to be analysed\t\t\t= "               <<par.getImageFile()       <<endl;
  stream<<"Intermediate Logfile\t\t\t= "                 <<par.getLogFile()         <<endl;
  stream<<"Final Results file\t\t\t= "                   <<par.getOutFile()         <<endl;
  stream<<"Spectrum file\t\t\t\t= "                      <<par.getSpectraFile()     <<endl;
  if(par.getFlagATrous()){			       
    stream<<"Saving reconstructed cube?\t\t= "           <<par.getFlagOutputRecon() <<endl;
    stream<<"Saving residuals from reconstruction?\t= "  <<par.getFlagOutputResid() <<endl;
  }						       
  stream<<"-----"<<endl;				       
  stream<<"Fixing Null Pixels?\t\t\t= "                  <<par.getFlagNullPix()     <<endl;
  stream<<"Null Pixel Value\t\t\t= "                     <<par.getNullPixVal()      <<endl;
  stream<<"Removing Milky Way channels?\t\t= "           <<par.getFlagMW()          <<endl;
  stream<<"Milky Way Channels\t\t\t= "                   <<par.getMinMW()
	<<"-"                                            <<par.getMaxMW()           <<endl;
  stream<<"Minimum # Pixels in a detection\t\t= "        <<par.getMinPix()          <<endl;
  stream<<"Growing objects after detection?\t= "         <<par.getFlagGrowth()      <<endl;
  if(par.getFlagGrowth()) 			       
    stream<<"SNR Threshold for growth\t\t= "             <<par.getGrowthCut()       <<endl;
  stream<<"Using A Trous reconstruction?\t\t= "          <<par.getFlagATrous()      <<endl;
  if(par.getFlagATrous()){			       
    stream<<"Minimum scale in reconstruction\t\t= "      <<par.getMinScale()        <<endl;
    stream<<"SNR Threshold within reconstruction\t= "    <<par.getAtrousCut()       <<endl;
  }						       
  stream<<"Using FDR analysis?\t\t\t= "                  <<par.getFlagFDR()         <<endl;
  if(par.getFlagFDR()){				       
    stream<<"Alpha value for FDR analysis\t\t= "         <<par.getAlpha()           <<endl;
    stream<<"Number of pixels in PSF (used by FDR)\t= "  <<par.getSizePSF()         <<endl;
  }						       
  else stream<<"SNR Threshold\t\t\t\t= "                 <<par.getCut()             <<endl;
  stream<<"Using Adjacent-pixel criterion?\t\t= "        <<par.getFlagAdjacent()    <<endl;
  if(!par.getFlagAdjacent()){
    stream<<"Max. spatial separation for merging\t= "    <<par.getThreshS()         <<endl;
    stream<<"Max. velocity separation for merging\t= "   <<par.getThreshV()         <<endl;
  }
  stream<<"Min. # channels for merging\t\t= "            <<par.getMinChannels()     <<endl;
  stream<<"-------------------"<<endl;
  stream.unsetf(ios::boolalpha);
}

std::ostream& operator<< ( std::ostream& theStream, Param& par)
{
  theStream.setf(ios::boolalpha);
  theStream<<"---- Parameters ---"<<endl;
  if(par.getFlagSubsection())
    theStream<<"Image to be analysed\t\t\t= "               <<(par.getImageFile()+
							       par.getSubsection())    <<endl;
  else 
    theStream<<"Image to be analysed\t\t\t= "               <<par.getImageFile()       <<endl;
  theStream<<"Intermediate Logfile\t\t\t= "                 <<par.getLogFile()         <<endl;
  theStream<<"Final Results file\t\t\t= "                   <<par.getOutFile()         <<endl;
  theStream<<"Spectrum file\t\t\t\t= "                      <<par.getSpectraFile()     <<endl;
  if(par.getFlagATrous()){			       
    theStream<<"Saving reconstructed cube?\t\t= "           <<par.getFlagOutputRecon() <<endl;
    theStream<<"Saving residuals from reconstruction?\t= "  <<par.getFlagOutputResid() <<endl;
  }						       
  theStream<<"-----"<<endl;				       
  theStream<<"Fixing Null Pixels?\t\t\t= "                  <<par.getFlagNullPix()     <<endl;
  theStream<<"Null Pixel Value\t\t\t= "                     <<par.getNullPixVal()      <<endl;
  theStream<<"Removing Milky Way channels?\t\t= "           <<par.getFlagMW()          <<endl;
  theStream<<"Milky Way Channels\t\t\t= "                   <<par.getMinMW()
	   <<"-"                                            <<par.getMaxMW()           <<endl;
  theStream<<"Minimum # Pixels in a detection\t\t= "        <<par.getMinPix()          <<endl;
  theStream<<"Growing objects after detection?\t= "         <<par.getFlagGrowth()      <<endl;
  if(par.getFlagGrowth()) 			       
    theStream<<"SNR Threshold for growth\t\t= "             <<par.getGrowthCut()       <<endl;
  theStream<<"Using A Trous reconstruction?\t\t= "          <<par.getFlagATrous()      <<endl;
  if(par.getFlagATrous()){			       
    theStream<<"Minimum scale in reconstruction\t\t= "      <<par.getMinScale()        <<endl;
    theStream<<"SNR Threshold within reconstruction\t= "    <<par.getAtrousCut()       <<endl;
  }						       
  theStream<<"Using FDR analysis?\t\t\t= "                  <<par.getFlagFDR()         <<endl;
  if(par.getFlagFDR()){				       
    theStream<<"Alpha value for FDR analysis\t\t= "         <<par.getAlpha()           <<endl;
    theStream<<"Number of pixels in PSF (used by FDR)\t= "  <<par.getSizePSF()         <<endl;
  }						       
  else theStream<<"SNR Threshold\t\t\t\t= "                 <<par.getCut()             <<endl;
  theStream<<"Using Adjacent-pixel criterion?\t\t= "        <<par.getFlagAdjacent()    <<endl;
  if(!par.getFlagAdjacent()){
    theStream<<"Max. spatial separation for merging\t= "    <<par.getThreshS()         <<endl;
    theStream<<"Max. velocity separation for merging\t= "   <<par.getThreshV()         <<endl;
  }
  theStream<<"Min. # channels for merging\t\t= "            <<par.getMinChannels()     <<endl;
  theStream<<"-------------------"<<endl;
  theStream.unsetf(ios::boolalpha);
}

