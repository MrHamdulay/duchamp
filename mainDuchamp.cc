#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <cpgplot.h>
#include <math.h>
#include <unistd.h>

#include <duchamp.hh>
#include <Detection/detection.hh>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>
#include <ATrous/atrous.hh>

using std::ofstream;
using std::endl;

Filter reconFilter;

int main(int argc, char * argv[])
{

  string paramFile;

  if(argc==1){
    std::cout << ERR_USAGE_MSG;
    exit(1);
  }
  else{
    char c;
    while( ( c = getopt(argc,argv,"p:hv") )!=-1){
      switch(c) {
      case 'p':
	paramFile = optarg;
	break;
      case 'v':
	std::cout << PROGNAME << " " << VERSION << std::endl;
	exit(1);
	break;
      case 'h':
      default :
	std::cout << ERR_USAGE_MSG;
	exit(1);
	break;
      }
    }
  }

  Cube *cube = new Cube;
  cube->readParam(paramFile);
  if(cube->pars().getImageFile().empty()){
    std::cerr << "Error: No input image has been given!"<<endl;
    std::cerr << "Use the imageFile parameter in "<< paramFile << " to specify the FITS file."<<endl;
    std::cerr << "Exiting..." << endl;
    exit(1);
  }

  // Filter needed for baseline removal and a trous reconstruction
  if(cube->pars().getFlagATrous() || cube->pars().getFlagBaseline()){
    reconFilter.define(cube->pars().getFilterCode());
    cube->pars().setFilterName(reconFilter.getName());
  }

  string fname = cube->pars().getImageFile();
  if(cube->pars().getFlagSubsection()){
    cube->pars().parseSubsection();
    fname+=cube->pars().getSubsection();
  }
  std::cout << "Opening image: " << fname << endl;
  if( cube->getCube(fname) == FAILURE){
    std::cerr << "Error opening file : " << fname << endl;
    std::cerr << "Exiting..." << endl;
    exit(1);
  }
  else std::cout << "Opened successfully." << endl; 

  // If the reconstructed array is to be read in from disk
  if( cube->pars().getFlagReconExists() ){
    if( cube->readReconCube() == FAILURE){
      std::cerr << "WARNING : Could not find existing reconstructed array.\n"
		<< "          Will perform reconstruction using assigned parameters.\n";
      cube->pars().setFlagReconExists(false);
    }
  }

  std::cout << cube->pars();

  if(cube->pars().getFlagLog()){
    ofstream logfile(cube->pars().getLogFile().c_str());
    logfile << "New run of the Duchamp sourcefinder: ";
    logfile.close();
    string syscall = "date >> " + cube->pars().getLogFile();
    system(syscall.c_str());
    logfile.open(cube->pars().getLogFile().c_str(),std::ios::app);
    logfile << cube->pars();
    logfile.close();
  }

  if(cube->pars().getFlagBlankPix()){
    std::cout<<"Trimming the Blank Pixels from the edges...  "<<std::flush;
    cube->trimCube();
    std::cout<<"Done."<<endl;
    std::cout << "New dimensions:  " << cube->getDimX();
    if(cube->getNumDim()>1) std::cout << "x" << cube->getDimY();
    if(cube->getNumDim()>2) std::cout << "x" << cube->getDimZ();
    std::cout << endl;
  }

  if(cube->pars().getFlagMW()){
    std::cout<<"Removing the Milky Way channels...  ";
    cube->removeMW();
    std::cout<<"Done."<<endl;
  }

  if(cube->pars().getFlagBaseline()){
    std::cout<<"Removing the baselines... "<<std::flush;
    cube->removeBaseline();
    std::cout<<" Done.                 "<<std::endl;
  }

  if(cube->getDimZ()==1) cube->pars().setMinChannels(0);  // special case for 2D images.

  if(cube->pars().getFlagNegative()){
    std::cout<<"Inverting the Cube... "<<std::flush;
    cube->invert();
    std::cout<<" Done."<<std::endl;
  }

  if(cube->pars().getFlagATrous() || cube->pars().getFlagReconExists()){
    std::cout<<"Commencing search in reconstructed cube..."<<endl;
    cube->ReconSearch3D();
    std::cout<<"Done. Found "<<cube->getNumObj()<<" objects."<<endl;
  }
  else{
    std::cout<<"Commencing search in cube..."<<endl;
    cube->SimpleSearch3D();
    std::cout<<"Done. Found "<<cube->getNumObj()<<" objects."<<endl;
  }

  std::cout<<"Merging lists...  "<<std::flush;
  cube->ObjectMerger();
  std::cout<<"Done.                      "<<endl;
  std::cout<<"Final object count = "<<cube->getNumObj()<<endl; 

  if(cube->pars().getFlagNegative()){
    std::cout<<"Un-Inverting the Cube... "<<std::flush;
    cube->reInvert();
    std::cout<<" Done."<<std::endl;
  }

  cube->replaceBaseline();

  if(cube->pars().getFlagCubeTrimmed()){
    std::cout<<"Replacing the trimmed pixels on the edges...  "<<std::flush;
    cube->unTrimCube();
    std::cout<<"Done."<<endl;
  }

  if(cube->getNumObj()>0){

    cube->calcObjectWCSparams();
    
    cube->sortDetections();
    
    cube->outputDetectionList();
  }

//   cube->plotDetectionMap("/xs");
    cube->plotMomentMap("/xs");

  if(cube->pars().getFlagMaps()){
    std::cout<<"Creating the maps...  "<<std::flush;
    cube->plotMomentMap(cube->pars().getMomentMap()+"/vcps");
    cube->plotDetectionMap(cube->pars().getDetectionMap()+"/vcps");
    std::cout << "done.\n";
  }

//   if(cube->isWCS() && (cube->getNumObj()>0)){
  if((cube->getDimZ()>1) && (cube->getNumObj()>0)){
    std::cout << "Plotting the individual spectra... " << std::flush;
    cube->outputSpectra();
    std::cout << "done.\n";
  }
  else if(cube->getDimZ()<=1) std::cout << "Not plotting any spectra  -- no third dimension.\n";

  cpgend();

  if(cube->pars().getFlagATrous()&&
     (cube->pars().getFlagOutputRecon()||cube->pars().getFlagOutputResid()) ){
    std::cout << "Saving reconstructed cube... "<<std::flush;
    cube->saveReconstructedCube();
    std::cout << "done.\n";
  }

  if(cube->pars().getFlagLog() && (cube->getNumObj()>0)){
    ofstream logfile(cube->pars().getLogFile().c_str(),std::ios::app);
    logfile << "=-=-=-=-=-=-=-\nCube summary\n=-=-=-=-=-=-=-\n";
    logfile << *cube;
    logfile.close();
  }

  if(cube->pars().getFlagVOT()){
    ofstream votfile(cube->pars().getVOTFile().c_str());
    cube->outputDetectionsVOTable(votfile);
    votfile.close();
  }

  if(cube->pars().getFlagKarma()){
    ofstream karmafile(cube->pars().getKarmaFile().c_str());
    cube->outputDetectionsKarma(karmafile);
    karmafile.close();
  }

  delete cube;

  return 0;
}

