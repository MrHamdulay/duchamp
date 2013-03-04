// -----------------------------------------------------------------------
// outputDetection.cc: Different ways of printing inforamtion about
//                     the Detections.
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
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <duchamp/duchamp.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Outputs/columns.hh>
#include <duchamp/Outputs/CatalogueSpecification.hh>
#include <duchamp/Utils/feedback.hh>

namespace duchamp
{

  void Detection::printTableRow(std::ostream &stream, Catalogues::CatalogueSpecification columns, Catalogues::DESTINATION tableType)
  {
    /// @details
    ///  Print a row of values for the current Detection into an output
    ///  table. Columns are printed according to the tableType string,
    ///  using the Column::doCol() function as a determinant.
    /// \param stream Where the output is written
    /// \param columns The vector list of Column objects
    /// \param tableType A Catalogues::DESTINATION label saying what format to use: one of
    /// FILE, LOG, SCREEN or VOTABLE (although the latter
    /// shouldn't be used with this function).

    stream.setf(std::ios::fixed);  
    for(size_t i=0;i<columns.size();i++){
      if(columns.column(i).doCol(tableType,this->flagWCS)) this->printTableEntry(stream, columns.column(i));
    }
    stream << "\n";

  }
  //--------------------------------------------------------------------

  void Detection::printTableEntry(std::ostream &stream, Catalogues::Column column)
  {
    /// @details
    ///  Print a single value into an output table. The Detection's
    ///  correct value is extracted according to the Catalogues::COLNAME
    ///  key in the column given.
    /// \param stream Where the output is written
    /// \param column The Column object defining the formatting.

    std::string type=column.type();

    if(type=="NUM") column.printEntry(stream,this->id);
    else if(type=="NAME") column.printEntry(stream,this->name);
    else if(type=="X") column.printEntry(stream,this->getXcentre() + this->xSubOffset);
    else if(type=="Y") column.printEntry(stream,this->getYcentre() + this->ySubOffset);
    else if(type=="Z") column.printEntry(stream,this->getZcentre() + this->zSubOffset);
    else if(type=="RA"){
      if(this->flagWCS) column.printEntry(stream,this->raS);
      else column.printBlank(stream);
    }
    else if(type=="DEC"){
      if(this->flagWCS) column.printEntry(stream,this->decS);
      else column.printBlank(stream);
    }
    else if(type=="RAJD"){
      if(this->flagWCS) column.printEntry(stream,this->ra);
      else column.printBlank(stream);
    }
    else if(type=="DECJD"){
      if(this->flagWCS) column.printEntry(stream,this->dec);
      else column.printBlank(stream);
    }
    else if(type=="VEL"){
      if(this->flagWCS) {
	if(this->specOK) column.printEntry(stream,this->vel);
	else column.printEntry(stream,0.);
      }
      else column.printBlank(stream);
    }
    else if(type=="MAJ") column.printEntry(stream,this->majorAxis);
    else if(type=="MIN") column.printEntry(stream,this->minorAxis);
    else if(type=="PA") column.printEntry(stream,this->posang);
    else if(type=="WRA"){
      if(this->flagWCS) column.printEntry(stream,this->raWidth);
      else column.printBlank(stream);
    }
    else if(type=="WDEC"){
      if(this->flagWCS) column.printEntry(stream,this->decWidth);
      else column.printBlank(stream);
    }
    else if(type=="W50"){
      if(this->specOK) column.printEntry(stream,this->w50);
      else column.printEntry(stream,0.);
    }
    else if(type=="W20"){
      if(this->specOK) column.printEntry(stream,this->w20);
      else column.printEntry(stream,0.);
    }
    else if(type=="WVEL"){
      if(this->specOK) column.printEntry(stream,this->velWidth);
      else column.printEntry(stream,0.);
    }
    else if(type=="FINT") column.printEntry(stream,this->intFlux);
    else if(type=="FINTERR") column.printEntry(stream,this->eIntFlux);
    else if(type=="FTOT") column.printEntry(stream,this->totalFlux);
    else if(type=="FTOTERR") column.printEntry(stream,this->eTotalFlux);
    else if(type=="FPEAK") column.printEntry(stream,this->peakFlux);
    else if(type=="SNRPEAK") column.printEntry(stream,this->peakSNR);
    else if(type=="X1") column.printEntry(stream,this->getXmin() + this->xSubOffset);
    else if(type=="X2") column.printEntry(stream,this->getXmax() + this->xSubOffset);
    else if(type=="Y1") column.printEntry(stream,this->getYmin() + this->ySubOffset);
    else if(type=="Y2") column.printEntry(stream,this->getYmax() + this->ySubOffset);
    else if(type=="Z1") column.printEntry(stream,this->getZmin() + this->zSubOffset);
    else if(type=="Z2") column.printEntry(stream,this->getZmax() + this->zSubOffset);
    else if(type=="NPIX") column.printEntry(stream,int(this->getSize()));
    else if(type=="FLAG") column.printEntry(stream,this->flagText);
    else if(type=="XAV") column.printEntry(stream,this->getXaverage() + this->xSubOffset);
    else if(type=="YAV") column.printEntry(stream,this->getYaverage() + this->ySubOffset);
    else if(type=="ZAV") column.printEntry(stream,this->getZaverage() + this->zSubOffset);
    else if(type=="XCENT") column.printEntry(stream,this->getXCentroid() + this->xSubOffset);
    else if(type=="YCENT") column.printEntry(stream,this->getYCentroid() + this->ySubOffset);
    else if(type=="ZCENT") column.printEntry(stream,this->getZCentroid() + this->zSubOffset);
    else if(type=="XPEAK") column.printEntry(stream,this->getXPeak() + this->xSubOffset);
    else if(type=="YPEAK") column.printEntry(stream,this->getYPeak() + this->ySubOffset);
    else if(type=="ZPEAK") column.printEntry(stream,this->getZPeak() + this->zSubOffset);
    else if(type=="NUMCH") column.printEntry(stream,this->getMaxAdjacentChannels());
    else if(type=="SPATSIZE") column.printEntry(stream,int(this->getSpatialSize()));

  }
  //--------------------------------------------------------------------

  std::string Detection::outputLabelWCS()
  {
    /// @details
    ///  Prints to a std::string the WCS position and velocity
    ///  information of the Detection, as well as the ID number and any
    ///  flags.
    ///  Assumes the WCS parameters of the object have been calculated.
    ///  If they have not (given by the isWCS() function), then the
    ///  WCS-related outputs are left blank.
    ///  Returns the string.

    std::stringstream ss;
    ss << "#" << std::setfill('0') << std::setw(3) << this->id << ": ";
    ss << this->name ;
    if(this->getFlagText()!="-") 
      ss << " [" << this->getFlagText() << "]   ";
    else ss<< "   ";
    ss << std::setfill(' ');
    ss << this->raS << ", ";
    ss << this->decS;
    if(this->specOK){
      ss << std::setprecision(this->velPrec);
      ss.setf(std::ios::fixed);
      ss << ", " << this->vel << " " << this->specUnits;
    }

    return ss.str();

  }
  //--------------------------------------------------------------------

  std::string Detection::outputLabelFluxes()
  {
    /// @details
    ///  Prints to a std::string the fluxes of the object, both
    ///  integrated/total and peak, as well as the peak S/N value.
    ///  Assumes the WCS parameters of the object have been calculated.
    ///  If they have not (given by the isWCS() function), then the ID
    ///  number and the total/peak/SNR values are returned.
    ///  /return The string.

    std::stringstream ss;
    ss.setf(std::ios::fixed);
    if(this->flagWCS){
      ss << std::setprecision(this->fintPrec);
      ss << "F\\dint\\u=" << this->intFlux << " " << this->intFluxUnits;
      ss << std::setprecision(this->fpeakPrec);
      ss << ", F\\dpeak\\u=" << this->peakFlux << " " << this->fluxUnits;
      if(this->peakSNR>0.){
	  ss << std::setprecision(this->snrPrec);
	  ss << ", S/N\\dmax\\u=" << this->peakSNR;
      }
    }
    else{ 
      ss << "#" << std::setfill('0') << std::setw(3) << this->id << ": ";
      ss << std::setprecision(this->fintPrec);
      ss << "F\\dtot\\u=" << this->totalFlux << this->fluxUnits;
      ss << std::setprecision(this->fpeakPrec);
      ss << ", F\\dpeak\\u=" << this->peakFlux << this->fluxUnits;
      if(this->peakSNR>0.){
	  ss << std::setprecision(this->snrPrec);
	  ss << ", S/N\\dmax\\u=" << this->peakSNR;
      }
    }
    std::string output = ss.str();

    return output;
  }
  //--------------------------------------------------------------------

  std::string Detection::outputLabelWidths(FitsHeader &head)
  {
    /// @details
    ///  Prints to a std::string the widths of the object in position
    ///  and velocity.
    ///  Assumes the WCS parameters of the object have been calculated.
    ///  If they have not (given by the isWCS() function), then a string of 
    ///   length 0 is returned.
    ///  \returns The string.

    std::string output;
    if(this->flagWCS){
      std::stringstream ss;
      ss.setf(std::ios::fixed);
      ss << std::setprecision(this->posPrec);
      // ss << "W\\d"              << this->lngtype  <<"\\u=" << this->raWidth;
      // ss << ", W\\d"            << this->lattype  <<"\\u=" << this->decWidth;
      std::string pgunits,units=head.getShapeUnits();
      if(units=="deg") pgunits="\\(0718)";
      else if(units=="arcmin") pgunits="\\(0716)";
      else if(units=="arcsec") pgunits="\\(0717)";
      else pgunits="";
      ss << this->majorAxis<<pgunits<<char(215)<<this->minorAxis<<pgunits;
      ss <<", PA="<<this->posang<<"\\(0718)";
      if(this->specOK){
	ss << std::setprecision(this->velPrec);
	ss << ", W\\d50\\u="    << this->w50      << " "   << this->specUnits;
	ss << ", W\\d20\\u="    << this->w20      << " "   << this->specUnits;
	ss << ", W\\d"<<this->specType<<"\\u="   << this->velWidth << " "   << this->specUnits;
      }
      output = ss.str();
    }
    else output = this->outputLabelFluxes();

    return output;
  }
  //--------------------------------------------------------------------

  std::string Detection::outputLabelPix()
  {
    /// @details
    ///  Prints to a std::string the pixel centres and extents of a
    ///  detected object.  Returns the string.

    std::stringstream ss;
    ss.setf(std::ios::fixed);
    ss << "Centre: ";
    ss << std::setprecision(this->xyzPrec) << std::setfill(' ');
    ss <<"("       << this->getXcentre() + this->xSubOffset;
    ss <<", "      << this->getYcentre() + this->ySubOffset;
    ss <<", "      << this->getZcentre() + this->zSubOffset << ")";
    ss <<", Size: "<< this->getSize() << " voxels,  ";
    ss <<"Range: [";
    ss <<             this->getXmin() + this->xSubOffset 
       <<":"       << this->getXmax() + this->xSubOffset;
    ss <<", "      << this->getYmin() + this->ySubOffset 
       <<":"       << this->getYmax() + this->ySubOffset;
    if(this->specOK){
      ss <<", "      << this->getZmin() + this->zSubOffset 
	 <<":"       << this->getZmax() + this->zSubOffset;
    }
    ss<< "]";
  
    return ss.str();
  }


}
