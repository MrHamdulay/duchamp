// -----------------------------------------------------------------------
// spectralSelection.cc: A text-based menu to select a particular 1D
//                       spectrum.
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
#include <iomanip>
#include <sstream>
#include <string>
#include <param.hh>
#include <Devel/devel.hh>
#include <Cubes/cubes.hh>
#include <vector>

using std::stringstream;

void spectralSelection(std::vector<float> &xvalues, 
		       std::vector<float> &yvalues, 
		       long &zdim)
{
  std::cout << "Do you want:\t1) HI Cubes?\n";
  std::cout << "\t\t2) PHFS quasar spectra?\n";
  std::cout << "\t\t3) Orion Chandra time series?\n";
  std::cout << "\t\t4) 2BL spectra?\n";
  std::cout << "\t\t5) Gaussian noise?\n";
  std::cout << "\t\t6) Gaussian noise with a single Gaussian source?\n";
  std::cout << "\t\t7) A unit pulse at the central pixel?\n";
  std::cout << "\t\t8) To enter your own filename (text file only)?\n";
  int choice=0;
  while((choice<1)||(choice>8)){
    std::cout << "Enter choice (1 -- 8) : ";
    std::cin >> choice;
  }
  
  std::string fname;
  zdim=0;

  std::vector<float> specx,specy;

  if(choice==1){
    fname=menu();
    std::cerr << fname <<std::endl;
    Cube *cube = new Cube;
    Param par;
    par.setImageFile(fname);
    par.setVerbosity(false);
    cube->saveParam(par);
    cube->getCube();
    zdim = cube->getDimZ();
//     if(par.getFlagMW()) cube->removeMW();
    zdim = cube->getDimZ();

    long xpos,ypos;
    std::cout << "Enter x (1--"<<cube->getDimX()
	      << ") and y (1--"<<cube->getDimY()
	      << ") positions: ";
    std::cin >> xpos >> ypos;

    xpos -= cube->pars().getXOffset();
    ypos -= cube->pars().getYOffset();
    float *array = new float[cube->getSize()];
    cube->getArray(array);
    specx.resize(zdim);
    specy.resize(zdim);
    const int offset = 0;
    int ct=0;
    for(int zpos=0;zpos<zdim;zpos++){
      specx[ct] = float(zpos);
      specy[ct] = 0.;
      for(int x=xpos-offset;x<=xpos+offset;x++){
	for(int y=ypos-offset;y<=ypos+offset;y++){
	  int cubepos = y * cube->getDimX() + x 
	    + zpos * cube->getDimX() * cube->getDimY();
	  specy[ct] += array[cubepos];
	}
      }
      specy[ct] /= (2*offset+1)*(2*offset+1);
      ct++;
    }
    delete cube;
    delete [] array;
  }
  else if((choice==6)||(choice==5)){
    zdim=1024;
    specx.resize(zdim);
    specy.resize(zdim);
    float *tempx = new float[zdim];
    float *tempy = new float[zdim];
    getRandomSpectrum(zdim,tempx,tempy);
    for(int i=0;i<zdim;i++) specx[i] = tempx[i];
    for(int i=0;i<zdim;i++) specy[i] = tempy[i];
    delete [] tempx;
    delete [] tempy;
    if(choice==6){
      float src,snr,loc,fwhm;
      std::cout << "Enter signal-to-noise of source: ";
      std::cin >> snr;
      while((loc<0)||(loc>=zdim)){
	std::cout << "Enter location of source (0 -- "<<zdim<<"), and FHWM: ";
	std::cin >> loc >> fwhm;
      }
      for(int i=0;i<zdim;i++){
	src = snr * exp( -(specx[i]-loc)*(specx[i]-loc)/(fwhm*fwhm) );
	specy[i] += src;
      }
    }
  }
  else if(choice==7){
    zdim=1024;
    specx.resize(zdim);
    specy.resize(zdim);
    for(int i=0;i<zdim;i++){
      specx[i] = i;
      specy[i] = 0.;
    }
    specy[zdim/2] = 1.;
  }
  else{
    if(choice==2) fname=specMenu();
    else if(choice==3) fname=orionMenu();
    else if(choice==4) fname=twoblMenu();
    else {
      std::cout << "Enter filename (full path): ";
      std::cin >> fname;
    }
    std::ifstream fin(fname.c_str());
    float a; float b;
    specx.resize(0);
    specy.resize(0);
    while(fin>>a>>b){
      specx.push_back(a);
      specy.push_back(b);
      zdim++;
    }
    
  }

  xvalues = specx;
  yvalues = specy;

}
