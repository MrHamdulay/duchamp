// -----------------------------------------------------------------------
// mainDuchamp.cc: Main program for Duchamp source finder.
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
#include <fstream>
#include <string>
#include <math.h>
#include <unistd.h>
#include <time.h>

#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/pgheader.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/ATrous/atrous.hh>

using namespace duchamp;

int main(int argc, char * argv[])
{

  std::string paramFile,fitsfile;
  Cube *cube = new Cube;

  if(cube->getopts(argc,argv)==FAILURE) return FAILURE;

  if(cube->pars().getImageFile().empty()){
    std::stringstream errmsg;
    errmsg << "No input image has been given!\n"
	   << "Use the imageFile parameter in " 
	   << paramFile << " to specify the FITS file.\nExiting...\n";
    duchampError("Duchamp", errmsg.str());
    return FAILURE;
  }

  if(cube->pars().getFlagSubsection() || cube->pars().getFlagStatSec()){
    // make sure the subsection is OK.
    if(cube->pars().verifySubsection() == FAILURE){
      duchampError("Duchamp", 
		   "Unable to use the subsection provided.\nExiting...\n");
      return FAILURE;
    }
  }      

  std::cout << "Opening image: " 
	    << cube->pars().getFullImageFile() << std::endl;

  if( cube->getCube() == FAILURE){
    std::stringstream errmsg;
    errmsg << "Unable to open image file "
	   << cube->pars().getFullImageFile() 
	   << "\nExiting...\n";
    duchampError("Duchamp", errmsg.str());
    return FAILURE;
  }
  else std::cout << "Opened successfully." << std::endl; 

  // Read in any saved arrays that are in FITS files on disk.
  if(!cube->pars().getFlagUsePrevious()) 
    cube->readSavedArrays();

  // special case for 2D images -- ignore the minChannels requirement
  if(cube->getDimZ()==1) cube->pars().setMinChannels(0);  

  // Write the parameters to screen.
  std::cout << cube->pars();

  if(cube->pars().getFlagUsePrevious()){
    std::cout << "Reading detections from existing log file... \n";
    if(cube->getExistingDetections() == FAILURE){
      duchampError("Duchamp", 
		   "Could not read detections from log file\nExiting...\n");
      return FAILURE;
    }
    else std::cout << "Done.\n";
  }
  else {

    if(cube->pars().getFlagLog()){
      // Prepare the log file.
      cube->prepareLogFile(argc, argv);
    }

    //if(cube->pars().getFlagBlankPix()){
    if(cube->pars().getFlagTrim()){
      // Trim any blank pixels from the edges, and report the new size
      // of the cube
      std::cout<<"Trimming the Blank Pixels from the edges...  "<<std::flush;
      cube->trimCube();
      std::cout<<"Done."<<std::endl;
      std::cout << "New dimensions:  " << cube->getDimX();
      if(cube->getNumDim()>1) std::cout << "x" << cube->getDimY();
      if(cube->getNumDim()>2) std::cout << "x" << cube->getDimZ();
      std::cout << std::endl;
    }

    if(cube->pars().getFlagBaseline()){
      std::cout<<"Removing the baselines... "<<std::flush;
      cube->removeBaseline();
      std::cout<<" Done.                 \n";
    }

    if(cube->pars().getFlagNegative()){
      std::cout<<"Inverting the Cube... "<<std::flush;
      cube->invert();
      std::cout<<" Done.\n";
    }

    cube->Search(true);
    std::cout << "Done. Intermediate list has " << cube->getNumObj();
    if(cube->getNumObj()==1) std::cout << " object.\n";
    else std::cout << " objects.\n";

    if(cube->getNumObj() > 0){
      if(cube->pars().getFlagGrowth())
	std::cout<<"Merging, Growing and Rejecting...  "<<std::flush;
      else
	std::cout<<"Merging and Rejecting...  "<<std::flush;
      cube->ObjectMerger();
      std::cout<<"Done.                      "<<std::endl;
    }
    std::cout<<"Final object count = "<<cube->getNumObj()<<std::endl; 

    if(cube->pars().getFlagNegative()){
      std::cout<<"Un-Inverting the Cube... "<<std::flush;
      cube->reInvert();
      std::cout<<" Done."<<std::endl;
    }

    if(cube->pars().getFlagBaseline()){
      std::cout<<"Replacing the baselines...  "<<std::flush;
      cube->replaceBaseline();
      std::cout<<"Done."<<std::endl;
    }

    if(cube->pars().getFlagCubeTrimmed()){
      std::cout<<"Replacing the trimmed pixels on the edges...  "<<std::flush;
      cube->unTrimCube();
      std::cout<<"Done."<<std::endl;
    }

  }

  cube->prepareOutputFile();

  if(cube->getNumObj()>0){

    cube->calcObjectWCSparams();

    cube->setObjectFlags();
    
    cube->sortDetections();

  }
  
  cube->outputDetectionList();

  if(USE_PGPLOT){
    std::cout<<"Creating the maps...  "<<std::flush;
    std::vector<std::string> devices;
    if(cube->pars().getFlagXOutput()) devices.push_back("/xs");
    if(cube->pars().getFlagMaps()) 
      devices.push_back(cube->pars().getMomentMap()+"/vcps");
    cube->plotMomentMap(devices);
    if(cube->pars().getFlagMaps())
      cube->plotDetectionMap(cube->pars().getDetectionMap()+"/vcps");
    std::cout << "Done.\n";
    
    if(cube->getNumObj()>0){
      std::cout << "Plotting the individual spectra... " << std::flush;
      cube->outputSpectra();
      std::cout << "Done.\n";
    }
  }
  else{
    std::cout << "PGPLOT has not been enabled, so no graphical output.\n";
  }

  if(!cube->pars().getFlagUsePrevious()){

    if(cube->pars().getFlagATrous()&&
       (cube->pars().getFlagOutputRecon()||cube->pars().getFlagOutputResid()) ){
      std::cout << "Saving reconstructed cube to "
		<< cube->pars().outputReconFile() << "... "<<std::flush;
      if(cube->saveReconstructedCube() == FAILURE)
	std::cout << "Failed!\n";
      else 
	std::cout << "done.\n";
    }
    if(cube->pars().getFlagSmooth()&& cube->pars().getFlagOutputSmooth()){
      std::cout << "Saving smoothed cube to "
		<< cube->pars().outputSmoothFile() << "... " <<std::flush;
      if(cube->saveSmoothedCube() == FAILURE)
	std::cout << "Failed!\n";
      else 
	std::cout << "done.\n";
    }
    if(cube->pars().getFlagOutputMask()){
      std::cout << "Saving mask cube to "
		<< cube->pars().outputMaskFile() << "... " <<std::flush;
      if(cube->saveMaskCube() == FAILURE)
	std::cout << "Failed!\n";
      else 
	std::cout << "done.\n";
    }
    if(cube->pars().getFlagOutputMomentMap()){
      std::cout << "Saving 0th moment map image to "
		<< cube->pars().outputMomentMapFile() << "... " <<std::flush;
      if(cube->saveMomentMapImage() == FAILURE)
	std::cout << "Failed!\n";
      else 
	std::cout << "done.\n";
    }

    if(cube->pars().getFlagLog() && (cube->getNumObj()>0)){
      std::ofstream logfile(cube->pars().getLogFile().c_str(),std::ios::app);
      logfile << "=-=-=-=-=-=-=-\nCube summary\n=-=-=-=-=-=-=-\n";
      logfile << *cube;
      logfile.close();
    }

  }

  if(cube->pars().getFlagVOT()){
    std::ofstream votfile(cube->pars().getVOTFile().c_str());
    cube->outputDetectionsVOTable(votfile);
    votfile.close();
  }

  if(cube->pars().getFlagKarma()){
    std::ofstream karmafile(cube->pars().getKarmaFile().c_str());
    cube->outputDetectionsKarma(karmafile);
    karmafile.close();
  }

  if(!cube->pars().getFlagUsePrevious() && cube->pars().getFlagLog()){
    // Open the logfile and write the time on the first line
    std::ofstream logfile(cube->pars().getLogFile().c_str(),std::ios::app);
    logfile << "Duchamp completed: ";
    time_t now = time(NULL);
    logfile << asctime( localtime(&now) );
    logfile.close();
  }


  delete cube;

  return SUCCESS;
}

