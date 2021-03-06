// -----------------------------------------------------------------------
// existingDetections.cc: Functions to read in previously-made
//                         detections from an existing log file
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
#include <sstream>
#include <string>

#include <duchamp/Cubes/cubes.hh>
#include <duchamp/param.hh>
#include <duchamp/PixelMap/Scan.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Utils/feedback.hh>

using namespace PixelInfo;

namespace duchamp 
{

  OUTCOME Cube::getExistingDetections()
  {

    std::ifstream logfile(this->par.getLogFile().c_str());
  
    if(!logfile.is_open()){
      DUCHAMPERROR("getExistingDetections","Unable to open log file " << this->par.getLogFile());
      return FAILURE;
    }

    if(this->par.isVerbose()) std::cout << "  Reading from logfile : " << this->par.getLogFile() << "\n";

    std::string temp,filename;
    std::stringstream ss;

    // first check filename, just to be sure
    while(getline(logfile,temp), (temp.size()<11 || temp.substr(0,13)!="# Image to be")){}
    ss.str(temp);
    ss >> temp >> temp >> temp >> temp >> temp>> temp >> filename;
    if(filename != this->par.getFullImageFile()){
      DUCHAMPERROR("getExistingDetections", "The image file given the log file (" << filename <<") is different to that in the parameter file (" << this->par.getFullImageFile()<<").");
      return FAILURE;
    }

    // read down until first Detection # line
    while(getline(logfile,temp), (temp.size()<9 || temp.substr(0,9)!="Threshold")){
    }
    float threshold,middle,spread;
    bool robust;
    getline(logfile,temp);
    std::stringstream dataline;
    dataline.str(temp);
    dataline >> threshold >> middle >> spread >> robust;
    if(this->par.isVerbose()) std::cout << "  Detection threshold used was " << threshold << "\n";
    this->Stats.setRobust(robust);
    this->Stats.setThreshold(threshold);
    this->Stats.setMiddle(middle);
    this->Stats.setSpread(spread);
    getline(logfile,temp);
    std::stringstream numline;
    numline.str(temp);
    int numDets;
    std::string null;
    numline >> numDets >> null;
    if(this->par.isVerbose()) std::cout << "  Number of detections in logfile = " << numDets << "\n";
    ProgressBar bar;
    if(this->par.isVerbose()){
      std::cout << "  Reading detections... ";// << std::flush;
      bar.init(numDets);
    }
    getline(logfile,temp);
    int x1,x2, ypix, zpix;
    for(int d=0; d<numDets; d++){
      getline(logfile,temp);   // This should be Detection #X:
      if(this->par.isVerbose()) bar.update(d+1);
      if(temp.substr(0,11)!="Detection #") DUCHAMPTHROW("existingDetections","Format of Log file is wrong - \""<<temp.substr(0,11)<<"\" should be \"Detection #\"!");
      Detection obj;
      getline(logfile,temp);
      while(temp.size()>0){
	for(size_t i=0;i<temp.size();i++)
	  if(temp[i]=='-' || temp[i]==',') temp[i] = ' ';
	std::stringstream ss;
	ss.str(temp);
	ss >> x1 >> x2 >> ypix >> zpix;
	Scan scn(ypix-this->par.getYOffset(),x1-this->par.getXOffset(),x2-x1+1);
	obj.addScan(scn,zpix-this->par.getZOffset());
	getline(logfile,temp);
      }
      
      obj.setOffsets(this->par);
      if(obj.getSize()>0){
	obj.calcParams();
	this->addObject(obj);
      }

    }
    logfile.close();

    this->updateDetectMap();

    if(this->par.isVerbose()){
      bar.remove();
      std::cout << "Done.\n";
    }

    return SUCCESS;

  }


}
