// -----------------------------------------------------------------------
// Section.cc: Member functions for the Section class, particularly
//             how to interpret the subsection string.
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
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <duchamp/Utils/Section.hh>
#include <duchamp/duchamp.hh>

namespace duchamp
{

  Section::Section()
  {
    this->subsection = "";
    this->numSections = 0;
    this->flagParsed = false;
  }

  Section::Section(const Section& s)
  {
    operator=(s);
  }
  //--------------------------------------------
 
  Section& Section::operator= (const Section& s)
  {
    if(this == &s) return *this;
    this->flagParsed    = s.flagParsed;
    this->subsection  = s.subsection;
    this->sections    = s.sections;
    this->numSections = s.numSections;
    this->starts      = s.starts;
    this->dims        = s.dims;
    return *this;
  }
  //--------------------------------------------

  OUTCOME Section::parse(long *dimAxes, int size)
  {
    std::vector<size_t> vecDim(size);
    for(int i=0;i<size;i++) vecDim[i] = dimAxes[i];
    return this->parse(vecDim);
  }

  OUTCOME Section::parse(size_t *dimAxes, int size)
  {
    std::vector<size_t> vecDim(size);
    for(int i=0;i<size;i++) vecDim[i] = dimAxes[i];
    return this->parse(vecDim);
  }

  OUTCOME Section::parse(std::vector<int> dimAxes)
  {
    std::vector<size_t> vecDim(dimAxes.size());
    for(size_t i=0;i<dimAxes.size();i++) vecDim[i] = long(dimAxes[i]);
    return this->parse(vecDim);
  }

  OUTCOME Section::parse(std::vector<long> dimAxes)
  {
    std::vector<size_t> vecDim(dimAxes.size());
    for(size_t i=0;i<dimAxes.size();i++) vecDim[i] = long(dimAxes[i]);
    return this->parse(vecDim);
  }

  OUTCOME Section::parse(std::vector<size_t> dimAxes)
  {
    /// @details
    /// This function reads the subsection string, and registers the
    /// starting values and lengths for each dimension. The array of axis
    /// dimensions is needed to know how long each axis really is, and
    /// whether we have strayed over the limit or not.
    /// 
    /// Note that steps in the subsection string are not dealt with -- a
    /// warning message is written to the screen, and the step values are
    /// removed from the subsection string.
    /// 
    /// The function also does basic checks to make sure it is of the
    /// correct format (ie. has square brackets delimiting it) and the
    /// correct number of sections, returning a FAILURE if either of
    /// these checks fail.
    /// 
    /// \param dimAxes The array of axis dimensions, of which the
    ///                 Section is a subsection.
    /// \return SUCCESS/FAILURE (from duchamp.hh)

    std::stringstream errmsg;

    // Return an error if we haven't set the subsection yet.
    if(this->subsection == ""){
	DUCHAMPERROR("Section parsing", "Subsection has not yet been defined.");
	return FAILURE;
    }

    // First Make sure subsection has [ and ] at ends
    if((this->subsection[0]!='[') || 
       (this->subsection[this->subsection.size()-1]!=']')){
      DUCHAMPERROR("Section parsing","Subsection needs to be delimited by square brackets -- You provided: " << this->subsection);
      return FAILURE;
    }

    this->starts.clear();
    this->dims.clear();
  
    this->numSections=1;
    for(size_t i=0;i<this->subsection.size();i++)
      if(this->subsection[i]==',') this->numSections++;

    if(numSections!=dimAxes.size()){
      DUCHAMPERROR("Section parsing","Subsection " << this->subsection << " has "<<numSections <<" sections, compared to a cube with " << dimAxes.size() << " axes");
      return FAILURE;
    }

    this->starts.resize(this->numSections);
    this->dims.resize(this->numSections);
    this->sections.resize(this->numSections);

    std::vector<std::string> tempsections(numSections); 
    // this will hold the section strings for each dimension
    std::stringstream ss;
    ss.str(this->subsection);
    bool removeStep = false;
    bool doingBorders = false;
    std::string temp;

    getline(ss,temp,'[');
    for(size_t i=0;i<numSections-1;i++){
      getline(ss,temp,',');
      tempsections[i]=temp;
    }
    getline(ss,temp,']');
    tempsections[numSections-1]=temp;

    for(size_t str=0;str<numSections;str++){
      if(tempsections[str]=="*"){
	this->starts[str] = 0;
	this->dims[str]= dimAxes[str];
	this->sections[str] = tempsections[str];
      }
      else{
	int numColon=0;
	for(size_t i=0;i<tempsections[str].size();i++){
	  if(tempsections[str][i]==':'){
	    tempsections[str][i]=' ';
	    numColon++;
	  }
	}
	int a,b,c;
	std::stringstream readString,fixedString;
	readString.str(tempsections[str]);
	switch(numColon){
	case 1: // usual case
	  readString >> a >> b;
	  this->starts[str] = a-1;
	  this->dims[str] = b-a+1;
	  fixedString << a << ":" << b;
	  this->sections[str] = fixedString.str();
	  break;
	case 0: // borders case -- so many off each border
	  readString >> a;
	  if(a>=int(dimAxes[str])/2){
	    DUCHAMPERROR("Section parsing", "You requested the subsection " << this->subsection << " but axis #" << str+1 <<" has zero size, since its dimension is " << dimAxes[str]);
	    return FAILURE;
	  }
	  this->starts[str] = a;
	  this->dims[str] = dimAxes[str]-2*a;
	  fixedString << this->starts[str]+1 << ":" 
		      << this->getEnd(str)+1;
	  this->sections[str] = fixedString.str();
	  doingBorders=true;
	  break;
	case 2: // subsection involves a step
	default:
	  readString>> a >> b >> c;
	  this->starts[str] = a-1;
	  this->dims[str] = b-a+1;
	  fixedString << a << ":" << b;
	  this->sections[str] = fixedString.str();
	  removeStep=true;
	  break;
	}

      }
    }

    if(removeStep){  // if there was a step present
      errmsg.str("");
      errmsg << "The subsection given is " << this->subsection <<".\n"
	     << "Duchamp is currently unable to deal with pixel steps"
	     << " in the subsection.\n"
	     << "These have been ignored, and so the subection used is ";
    }

    if(removeStep || doingBorders){
      // rewrite subsection without any step sizes and with correct borders.
      this->subsection = "[" + this->sections[0];
      for(size_t str=1;str<numSections;str++) 
	this->subsection += ',' + this->sections[str];
      this->subsection += "]";
    }

    if(removeStep){
      errmsg << this->subsection;
      DUCHAMPWARN("Section parsing", errmsg);
    }

    this->flagParsed = true;
    return SUCCESS;
  
  }

  std::string nullSection(int ndim)
  {
    std::stringstream ss;
    ss << "[";
    for(int i=0;i<ndim-1;i++) ss << "*,";
    ss << "*]";
    return ss.str();
  }

  bool Section::isValid()
  {
    /// @details A test to see whether each of the axis sections have
    /// a positive length. If at least one of them has zero or
    /// negative length, or if the section has not yet been parsed,
    /// then false is returned. If everything is OK, then we get true.

    bool valid = (this->numSections>0);
    for(size_t i=0;i<this->dims.size()&&valid;i++){
      valid = valid && dims[i]>0;
    }
    return valid;
  }

  Section Section::intersect(Section &other)
  {
    /// @details Return a Section object that is the intersection
    /// between the current section and another. The returned section
    /// only has its section string set, so it will need to be parsed
    /// afterwards.
    
    if(this->numSections != other.numSections){
      DUCHAMPERROR("Section::intersect", "Sizes of sections not equal - returning initial section");
      return *this;
    }

    std::vector<std::string> outputsections(this->numSections);
    for(size_t i=0;i<this->numSections;i++){
      std::stringstream ss;
      ss << std::max(this->starts[i],other.starts[i])+1 << ":" << std::min(this->getEnd(i),other.getEnd(i))+1;
      outputsections[i] = ss.str();
    }
	  
    std::stringstream section;
    section << "[" << outputsections[0];
    for(size_t i=1;i<this->numSections;i++) section<<"," << outputsections[i];
    section << "]";
    duchamp::Section output;
    output.setSection(section.str());
    return output;

  }

  Section Section::intersect(long *dimAxes, int size)
  {
    std::vector<long> vecDim(size);
    for(int i=0;i<size;i++) vecDim[i] = dimAxes[i];
    return this->intersect(vecDim);
  }

  Section Section::intersect(std::vector<int> dimAxes)
  {
    std::vector<long> vecDim(dimAxes.size());
    for(size_t i=0;i<dimAxes.size();i++) vecDim[i] = long(dimAxes[i]);
    return this->intersect(vecDim);
  }

  Section Section::intersect(std::vector<long> dimAxes)
  {
    
    std::string nullSec = nullSection(dimAxes.size());
    Section null(nullSec);
    null.parse(dimAxes);
    return this->intersect(null);

  }

  Section operator* (Section &lhs, Section &rhs)
  {
    /// @details Return a Section object that is the combination of two subsections. The second subsection indicates the pixel ranges *within* the ranges of the first, and the output is in the same global units as the first (this).
    /// So, [10:20,20:40] * [4:7,12:15] = [13:16,31:34]
    /// Note this is not commutative!
    
    if(lhs.numSections != rhs.numSections){
      std::stringstream ss;
      ss << "Sizes of sections not equal - you requested " << lhs.subsection << " * " << rhs.subsection << ", with " << lhs.numSections << " and " << rhs.numSections << " sections each.\n";
      DUCHAMPERROR("Section::operator*", "Sizes of sections not equal - you requested " << lhs.subsection << " * " << rhs.subsection << ", with " << lhs.numSections << " and " << rhs.numSections << " sections each.");
      return lhs;
    }

    std::vector<std::string> outputsections(lhs.numSections);
    for(size_t i=0;i<lhs.numSections;i++){
      std::stringstream ss;
      int minval=std::min(lhs.starts[i]+rhs.starts[i],lhs.starts[i]+lhs.dims[i])+1;
      int maxval=std::min(lhs.starts[i]+rhs.starts[i]+rhs.dims[i],lhs.starts[i]+lhs.dims[i]);
      //      ss << lhs.starts[i]+rhs.starts[i]+1 << ":" << lhs.starts[i]+rhs.starts[i]+rhs.dims[i];
      ss << minval << ":"<<maxval;
      outputsections[i] = ss.str();
    }
	  
    std::stringstream section;
    section << "[" << outputsections[0];
    for(size_t i=1;i<lhs.numSections;i++) section<<"," << outputsections[i];
    section << "]";
    duchamp::Section output;
    output.setSection(section.str());
    return output;

  }


}
