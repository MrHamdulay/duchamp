#include <iostream>
#include <fstream>
#include <string>
#include <cpgplot.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#include <duchamp.hh>
#include <Detection/detection.hh>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>
#include <ATrous/atrous.hh>

using std::ofstream;
using std::string;

Filter reconFilter;

int main(int argc, char * argv[])
{

  string paramFile,fitsfile;
  Cube *cube = new Cube;
  Param *par = new Param;

  if(argc==1){
    std::cout << ERR_USAGE_MSG;
    return 1;
  }
  else{
    char c;
    while( ( c = getopt(argc,argv,"p:f:hv") )!=-1){
      switch(c) {
      case 'p':
	paramFile = optarg;
	cube->readParam(paramFile);
	break;
      case 'f':
	fitsfile = optarg;
	par->setImageFile(fitsfile);
	cube->saveParam(*par);
	break;
      case 'v':
	std::cout << PROGNAME << " " << VERSION << std::endl;
	return 1;
	break;
      case 'h':
      default :
	std::cout << ERR_USAGE_MSG;
	return 1;
	break;
      }
    }
  }

  delete par;

  if(cube->pars().getImageFile().empty()){
    std::cerr << "ERROR : No input image has been given!\n";
    std::cerr << "        Use the imageFile parameter in "<< paramFile << " to specify the FITS file.\n";
    std::cerr << "Exiting...\n\n";
    return 1;
  }

  string fname = cube->pars().getImageFile();
  if(cube->pars().getFlagSubsection()){
    cube->pars().parseSubsection();
    fname+=cube->pars().getSubsection();
  }
  std::cout << "Opening image: " << fname << std::endl;
  if( cube->getCube(fname) == FAILURE){
    std::cerr << "ERROR : Unable to open image file : " << fname << std::endl;
    std::cerr << "Exiting...\n\n";
    return 1;
  }
  else std::cout << "Opened successfully." << std::endl; 

  // If the reconstructed array is to be read in from disk
  if( cube->pars().getFlagReconExists() && cube->pars().getFlagATrous() ){
    std::cout << "Reading reconstructed array: "<<std::endl;
    if( cube->readReconCube() == FAILURE){
      std::cerr << "WARNING : Could not read in existing reconstructed array.\n"
		<< "          Will perform reconstruction using assigned parameters.\n";
      cube->pars().setFlagReconExists(false);
    }
    else std::cout << "Reconstructed array available.\n";
  }

  // Filter needed for baseline removal and a trous reconstruction
  if(cube->pars().getFlagATrous() || cube->pars().getFlagBaseline()){
    reconFilter.define(cube->pars().getFilterCode());
    cube->pars().setFilterName(reconFilter.getName());
  }

  // Write the parameters to screen.
  std::cout << cube->pars();

  if(cube->pars().getFlagLog()){
    ofstream logfile(cube->pars().getLogFile().c_str());
    logfile << "New run of the Duchamp sourcefinder: ";
    time_t now = time(NULL);
    logfile << asctime( localtime(&now) );
    logfile << cube->pars();
    logfile.close();
  }

  if(cube->pars().getFlagBlankPix()){
    std::cout<<"Trimming the Blank Pixels from the edges...  "<<std::flush;
    cube->trimCube();
    std::cout<<"Done."<<std::endl;
    std::cout << "New dimensions:  " << cube->getDimX();
    if(cube->getNumDim()>1) std::cout << "x" << cube->getDimY();
    if(cube->getNumDim()>2) std::cout << "x" << cube->getDimZ();
    std::cout << std::endl;
  }

//   if(cube->pars().getFlagMW()){
//     std::cout<<"Removing the Milky Way channels...  "<<std::flush;
//     cube->removeMW();
//     std::cout<<"Done."<<std::endl;
//   }

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

  if(cube->pars().getFlagATrous()){
    std::cout<<"Commencing search in reconstructed cube..."<<std::endl;
    cube->ReconSearch();
    std::cout<<"Done. Found "<<cube->getNumObj()<<" objects."<<std::endl;
  }
  else{
    std::cout<<"Commencing search in cube..."<<std::endl;
    cube->CubicSearch();
    std::cout<<"Done. Found "<<cube->getNumObj()<<" objects."<<std::endl;
  }

  std::cout<<"Merging lists...  "<<std::flush;
  cube->ObjectMerger();
  std::cout<<"Done.                      "<<std::endl;
  std::cout<<"Final object count = "<<cube->getNumObj()<<std::endl; 

  if(cube->pars().getFlagNegative()){
    std::cout<<"Un-Inverting the Cube... "<<std::flush;
    cube->reInvert();
    std::cout<<" Done."<<std::endl;
  }

  cube->replaceBaseline();

  if(cube->pars().getFlagCubeTrimmed()){
    std::cout<<"Replacing the trimmed pixels on the edges...  "<<std::flush;
    cube->unTrimCube();
    std::cout<<"Done."<<std::endl;
  }

  if(cube->getNumObj()>0){

    cube->calcObjectWCSparams();

    cube->setObjectFlags();
    
    cube->sortDetections();
    
    cube->outputDetectionList();
  }

  std::cout<<"Creating the maps...  "<<std::flush;
  cube->plotMomentMap("/xs");
  if(cube->pars().getFlagMaps()){
    cube->plotMomentMap(cube->pars().getMomentMap()+"/vcps");
    cube->plotDetectionMap(cube->pars().getDetectionMap()+"/vcps");
  }
  std::cout << "done.\n";

  if((cube->getDimZ()>1) && (cube->getNumObj()>0)){
    std::cout << "Plotting the individual spectra... " << std::flush;
    cube->outputSpectra();
    std::cout << "done.\n";
  }
  else if(cube->getDimZ()<=1) std::cerr << "WARNING : Not plotting any spectra  -- no third dimension.\n";

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

