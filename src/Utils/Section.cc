#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <Utils/Section.hh>
#include <duchamp.hh>

Section::Section(const Section& s)
{
  this->subsection  = s.subsection;
  this->numSections = s.numSections;
  this->starts 	    = s.starts;
  this->dims        = s.dims;
}
//--------------------------------------------
 
Section& Section::operator= (const Section& s)
{
  if(this == &s) return *this;
  this->subsection  = s.subsection;
  this->numSections = s.numSections;
  this->starts 	    = s.starts;
  this->dims        = s.dims;
  return *this;
}
//--------------------------------------------

int Section::parse(std::vector<long> dimAxes)
{
  /**
   * This function reads the subsection string, and registers the
   * starting values and lengths for each dimension. The array of axis
   * dimensions is needed to know how long each axis really is, and
   * whether we have strayed over the limit or not.
   *
   * Note that steps in the subsection string are not dealt with -- a
   * warning message is written to the screen, and the step values are
   * removed from the subsection string.
   *
   * The function also does basic checks to make sure it is of the
   * correct format (ie. has square brackets delimiting it) and the
   * correct number of sections, returning a FAILURE if either of
   * these checks fail.
   *
   * \param dimAxes The array of axis dimensions, of which the
   *                 Section is a subsection.
   * \return SUCCESS/FAILURE (from duchamp.hh)
   */

  std::stringstream errmsg;

  // First Make sure subsection has [ and ] at ends
  if((this->subsection[0]!='[') || 
     (this->subsection[this->subsection.size()-1]!=']')){
    errmsg.str();
    errmsg << "Subsection needs to be delimited by square brackets\n"
	   << "You provided: " << this->subsection << std::endl;
    duchampError("Section::parse",errmsg.str());
    return FAILURE;
  }

  this->starts.clear();
  this->dims.clear();
  
  this->numSections=1;
  for(int i=0;i<this->subsection.size();i++)
    if(this->subsection[i]==',') this->numSections++;

  if(numSections!=dimAxes.size()){
    errmsg.str();
    errmsg << "Subsection has "<<numSections
	   <<" sections, compared to a cube with "
	   << dimAxes.size() << " axes\n"
	   << "Subsection provided was: " << this->subsection << std::endl;
    duchampError("Section::parse",errmsg.str());
    return FAILURE;
  }

  this->starts.resize(this->numSections);
  this->dims.resize(this->numSections);

  std::vector<std::string> sections(numSections); 
  // this will hold the section strings for each dimension
  std::stringstream ss;
  ss.str(this->subsection);
  bool removeStep = false;
  bool doingBorders = false;
  std::string temp;

  getline(ss,temp,'[');
  for(int i=0;i<numSections-1;i++){
    getline(ss,temp,',');
    sections[i]=temp;
  }
  getline(ss,temp,']');
  sections[numSections-1]=temp;

  for(int str=0;str<numSections;str++){
    if(sections[str]=="*"){
      this->starts[str] = 0;
      this->dims[str]= dimAxes[str];
    }
    else{
      int numColon=0;
      for(int i=0;i<sections[str].size();i++){
	if(sections[str][i]==':'){
	  sections[str][i]=' ';
	  numColon++;
	}
      }
      int a,b,c;
      std::stringstream temp;
      temp.str(sections[str]);
      switch(numColon){
      case 1: // usual case
	temp >> a >> b;
	this->starts[str] = a-1;
	this->dims[str] = b-a+1;
	break;
      case 0: // borders case -- so many off each border
	temp >> a;
	this->starts[str] = a-1;
	this->dims[str] = dimAxes[str]-2*a;
	temp.str();
	temp << a << ":" << dimAxes[str]-a;
	sections[str] = temp.str();
	doingBorders=true;
	break;
      case 2:
      default:
	temp >> a >> b >> c;
	this->starts[str] = a-1;
	this->dims[str] = b-a+1;
	temp.str();
	temp << a << ":" << b;
	sections[str] = temp.str();
	removeStep=true;
	break;
      }

    }
  }

  if(removeStep){  // if there was a step present
    errmsg.str();
    errmsg << "The subsection given is " << this->subsection <<".\n"
	   << "Duchamp is currently unable to deal with pixel steps"
	   << " in the subsection.\n"
	   << "These have been ignored, and so the subection used is ";
  }

  if(removeStep || doingBorders){
    // rewrite subsection without any step sizes and with correct borders.
   this->subsection = "[" + sections[0];
   for(int str=1;str<numSections;str++) 
     this->subsection += ',' + sections[str];
   this->subsection += "]";
  }

  if(removeStep){
    errmsg << this->subsection << std::endl;
    duchampWarning("Section::parse", errmsg.str());
  }

  return SUCCESS;
  
}
//--------------------------------------------
