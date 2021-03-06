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

  try {
    

  std::string paramFile,fitsfile;
  Cube *cube = new Cube;

  if(cube->getopts(argc,argv)==FAILURE) return FAILURE;

  if(cube->pars().getImageFile().empty()){
    DUCHAMPTHROW("Duchamp", "No input image has been given!. Use the imageFile parameter in the parameter file to specify the FITS file.");
    return FAILURE;
  }

  if(cube->pars().getFlagSubsection() || cube->pars().getFlagStatSec()){
    // make sure the subsection is OK.
    if(cube->pars().verifySubsection() == FAILURE){
      DUCHAMPTHROW("Duchamp", "Unable to use the subsection provided.");
      return FAILURE;
    }
  }      

  std::cout << "Opening image: " 
	    << cube->pars().getFullImageFile() << std::endl;

  if( cube->getCube() == FAILURE){
    DUCHAMPTHROW("Duchamp", "Unable to open image file "<< cube->pars().getFullImageFile());
    return FAILURE;
  }
  else std::cout << "Opened successfully." << std::endl; 

  // Read in any saved arrays that are in FITS files on disk.
  if(!cube->pars().getFlagUsePrevious()) 
    cube->readSavedArrays();

  // Write the parameters to screen.
  std::cout << cube->pars() << "\n";

  if(cube->pars().getFlagUsePrevious()){
    std::cout << "Reading detections from binary catalogue... \n";
    if(cube->readBinaryCatalogue() == FAILURE){
      DUCHAMPTHROW("Duchamp", "Could not read detections from log file");
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

    cube->Search();
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
      cube->invert(!cube->pars().getFlagBaseline(),true);
      std::cout<<" Done."<<std::endl;
    }

    if(cube->pars().getFlagBaseline() && !cube->pars().getFlagNegative()){
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

  if(cube->getNumObj()>0){

    std::cout << "Calculating final parameters and setting flags...  "<<std::flush;
    cube->calcObjectWCSparams();

    cube->setObjectFlags();
    
    cube->sortDetections();

    std::cout << "Done." << std::endl;

  }
  
  if(cube->pars().getFlagNegative() && cube->pars().getFlagBaseline()){
    std::cout<<"Un-Inverting the Cube... "<<std::flush;
    cube->invert(true,true);
    std::cout<<" Done."<<std::endl;
  }

  if(cube->pars().getFlagBaseline() && cube->pars().getFlagNegative()){
    std::cout<<"Replacing the baselines...  "<<std::flush;
    cube->replaceBaseline(false);
    std::cout<<"Done."<<std::endl;
  }

  cube->outputCatalogue();

  if(USE_PGPLOT){
    std::cout<<"Creating the maps...  "<<std::flush;
    std::vector<std::string> devices;
    if(cube->nondegDim()>1){
      if(cube->pars().getFlagXOutput()) devices.push_back("/xs");
      if(cube->pars().getFlagMaps()) 
	devices.push_back(cube->pars().getMomentMap()+"/vcps");
      cube->plotMomentMap(devices);
    }
    if(cube->pars().getFlagMaps()){
      if(cube->nondegDim()==1) cube->plotDetectionMap("/xs");
      cube->plotDetectionMap(cube->pars().getDetectionMap()+"/vcps");
    }
    std::cout << "Done.\n";
    
    if(cube->getNumObj()>0 && cube->pars().getFlagPlotSpectra()){
      std::cout << "Plotting the spectra of detected objects... " << std::flush;
      cube->outputSpectra();
      std::cout << "Done.\n";
    }
  }
  else{
    std::cout << "PGPLOT has not been enabled, so no graphical output.\n";
  }

  if(!cube->pars().getFlagUsePrevious()){
    
    if(cube->pars().getFlagWriteBinaryCatalogue() && (cube->getNumObj()>0)){
      cube->writeBinaryCatalogue();
    }

  }

  std::cout << "Writing to FITS files... \n";
  cube->writeToFITS();
  std::cout << "Done.\n";
  
  if(cube->pars().getFlagVOT()){
    cube->outputDetectionsVOTable();
  }

  // write annotation files, if required.
  cube->outputAnnotations();

  if(cube->pars().getFlagTextSpectra()){
    if(cube->pars().isVerbose()) std::cout << "Saving spectra to text file ... ";
    cube->writeSpectralData();
    if(cube->pars().isVerbose()) std::cout << "done.\n";
  }

  if(!cube->pars().getFlagUsePrevious() && cube->pars().getFlagLog()){
    // Open the logfile and write the time on the first line
    std::ofstream logfile(cube->pars().getLogFile().c_str(),std::ios::app);
    logfile << "# Duchamp completed: ";
    time_t now = time(NULL);
    logfile << asctime( localtime(&now) );
    logfile.close();
  }


  delete cube;

  } catch (const DuchampError &err) {
    std::cout << "\nDuchamp failed with the following error:\n" << err.what() << "\n";
    exit(1);
  }

  return SUCCESS;

}

