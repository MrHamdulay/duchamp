// -----------------------------------------------------------------------
// mainRrose: A program to re-analyse detected objects from a Duchamp run.
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
#include <cpgplot.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#include <duchamp.hh>
#include <param.hh>
#include <PixelMap/Voxel.hh>
#include <Detection/detection.hh>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>
#include <ATrous/atrous.hh>

using namespace PixelInfo;

int main(int argc, char * argv[])
{

  std::string paramFile,fitsfile,temp;
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

  if(cube->pars().getFlagSubsection()){
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
  cube->readSavedArrays();


  std::ifstream logfile(cube->pars().getLogFile().c_str());
  
  if(!logfile.is_open()){
    std::cerr << "\aUnable to open file " 
	      << cube->pars().getLogFile() << "\nExiting...\n";
    return 1;
  }

  // read down until first Detection # line
  cube->setCubeStats();
  std::cerr << "Reading from logfile : " << cube->pars().getLogFile() << "\n";
  while(getline(logfile,temp), temp.substr(0,11)!="Detection #"){ }
  int xpix, ypix, zpix;
  float fpix;
  while(!logfile.eof()){
    Detection obj;
    while(getline(logfile,temp), temp.substr(0,3)!="---"){
      std::stringstream ss;
      ss.str(temp);
      ss >> xpix >> ypix >> zpix >> fpix;
      Voxel vox(xpix,ypix,zpix,fpix);
      obj.addPixel(vox);
    }
    obj.setOffsets(cube->pars());
    obj.calcParams();
    if(obj.getSize()>0) cube->addObject(obj);
    getline(logfile,temp); // reads next line -- should be Detection #...
    if(temp.substr(0,4)=="----") getline(logfile,temp);
  }

  std::cout<<"Final object count = "<<cube->getNumObj()<<std::endl; 

  cube->calcObjectWCSparams();
  cube->setObjectFlags();

  if((cube->getDimZ()>1) && (cube->getNumObj()>0)){
    std::cout << "Plotting the individual spectra... " << std::flush;
    cube->outputSpectra();
    std::cout << "done.\n";
  }

}
