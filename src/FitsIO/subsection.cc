#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
#include <wcs.h>
#define WCSLIB_GETWCSTAB // define this so that we don't try and redefine 
                         //  wtbarr (this is a problem when using gcc v.4+
#include <fitsio.h>
#include <param.hh>
#include <duchamp.hh>

void Param::setOffsets(struct wcsprm *wcs)
{
  if(this->flagSubsection){ // if so, then the offsets array is defined.
    this->xSubOffset = this->offsets[wcs->lng];
    this->ySubOffset = this->offsets[wcs->lat];
    this->zSubOffset = this->offsets[wcs->spec];
  }
  else{// else they should be 0
    this->xSubOffset = this->ySubOffset = this->zSubOffset = 0;
  }
}

int Param::verifySubsection()
{
  /**
   *  Param::verifySubsection()
   *
   *   Checks that the subsection string is in the appropriate format, with
   *    the correct number of entries (one for each axis).
   *   This involves reading the individual substrings and converting to
   *    integers, and storing in the offsets array.
   *   Steps in the subsection string are not dealt with -- a warning message
   *    is written to the screen, and the step values are removed from the 
   *    subsection string.
   */

  // First open the requested FITS file and check its existence and 
  //  number of axes.
  int numAxes,status = 0;  /* MUST initialize status */
  fitsfile *fptr;         

  // Make sure the FITS file exists
  int exists;
  fits_file_exists(this->imageFile.c_str(),&exists,&status);
  if(exists<=0){
    fits_report_error(stderr, status);
    duchampWarning("verifySubsection", "Requested image does not exist!\n");
    return FAILURE;
  }
  // Open the FITS file
  if( fits_open_file(&fptr,this->imageFile.c_str(),READONLY,&status) ){
    fits_report_error(stderr, status);
    return FAILURE;
  }
  // Read the size information -- number of axes.
  status = 0;
  if(fits_get_img_dim(fptr, &numAxes, &status)){
    fits_report_error(stderr, status);
    return FAILURE;
  }
  // Close the FITS file.
  status = 0;
  fits_close_file(fptr, &status);
  if (status){
    duchampWarning("verifySubsection","Error closing file: ");
    fits_report_error(stderr, status);
  }

  ///////////////////

  // Make sure subsection has [ and ] at ends
  if((this->subsection[0]!='[') || 
     (this->subsection[this->subsection.size()-1]!=']')){
    std::stringstream errmsg;
    errmsg << "Subsection needs to be delimited by square brackets\n"
	   << "You provided: " << this->subsection << std::endl;
    duchampError("verifySubsection",errmsg.str());
    return FAILURE;
  }

  ///////////////////

  // Make sure subsection has number of sections matching number of axes

  int numSections=1;
  for(int i=0;i<this->subsection.size();i++)
    if(this->subsection[i]==',') numSections++;

  if(numSections!=numAxes){
    std::stringstream errmsg;
    errmsg << "Subsection has "<<numSections
	   <<" sections, whereas the FITS file has " << numAxes << " axes\n"
	   << "Subsection provided was: " << this->subsection << std::endl;
    duchampError("verifySubsection",errmsg.str());
    return FAILURE;
  }
    
  ///////////////////

  // If we get to here, should have correct number of entries.

  this->offsets = new long[numAxes];
  this->sizeOffsets = numAxes;

  vector<string> sections(numSections); // this will hold the individual 
                                        //   section strings

  std::stringstream ss;
  ss.str(this->subsection);
  bool removeStep = false;
  string temp;

  bool atEnd = false;
  getline(ss,temp,'[');
  for(int i=0;i<numSections-1;i++){
    getline(ss,temp,',');
    sections[i]=temp;
  }
  getline(ss,temp,']');
  sections[numSections-1]=temp;

  for(int str=0;str<numSections;str++){
    if(sections[str]=="*") this->offsets[str] = 0;
    else{
      // if it is a genuine subsection and not everything.
      int a = sections[str].find(':');     // first occurence of ':' in section
      int b = sections[str].find(':',a+1); // location of second ':' - will be 
                                           //  -1 if there is no second ':'
      this->offsets[str] = atoi( sections[str].substr(0,a).c_str() ) - 1;
      // store the minimum pixel value in offsets array
      if(b>0){  
	// if there is a step component, rewrite section string without 
	//  the step part.
	sections[str] = sections[str].substr(0,b); 
	removeStep = true;
      }
    }
  }

  if(removeStep){  // if there was a step present
    std::stringstream errmsg;
    errmsg << "The subsection given is " << this->subsection <<".\n"
	   << "Duchamp is currently unable to deal with pixel steps"
	   << " in the subsection.\n"
	   << "These have been ignored, and so the subection used is "; 
    // rewrite subsection without any step sizes. 
   this->subsection = "[" + sections[0];
   for(int str=1;str<numSections;str++) 
     this->subsection += ',' + sections[str];
   this->subsection += "]";
   errmsg << this->subsection << std::endl;
   duchampWarning("verifySubsection", errmsg.str());
  }

}
