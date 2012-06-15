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
#include <duchamp/Detection/columns.hh>
#include <duchamp/Utils/feedback.hh>

namespace duchamp
{

  using namespace Column;

  void outputTableHeader(std::ostream &stream, std::vector<Column::Col> columns, std::string tableType, bool flagWCS)
  {
    /// @details
    ///  Prints the header row for a table of detections. The columns
    ///  that are included depend on the value of tableType, according
    ///  to the Column::doCol() function. The format is a row of
    ///  dashes, a row with column names, a row with column units, and
    ///  another row of dashes.
    /// \param stream Where the output is written
    /// \param columns The vector list of Column objects
    /// \param tableType A string saying what format to use: one of
    /// "file", "log", "screen" or "votable" (although the latter
    /// shouldn't be used with this function).
    /// \param flagWCS A flag for use with Column::doCol(), specifying
    /// whether to use FINT or FTOT.

    std::vector<Column::Col>::iterator col;
    for(col=columns.begin();col<columns.end();col++)
      if(col->doCol(tableType,flagWCS)) col->printDash(stream);
    stream << "\n";
    for(col=columns.begin();col<columns.end();col++)
      if(col->doCol(tableType,flagWCS)) col->printTitle(stream);
    stream << "\n";
    for(col=columns.begin();col<columns.end();col++)
      if(col->doCol(tableType,flagWCS)) col->printUnits(stream);
    stream << "\n";
    for(col=columns.begin();col<columns.end();col++)
      if(col->doCol(tableType,flagWCS)) col->printDash(stream);
    stream << "\n";

  }
  //--------------------------------------------------------------------

  void Detection::printTableRow(std::ostream &stream, std::vector<Column::Col> columns, std::string tableType)
  {
    /// @details
    ///  Print a row of values for the current Detection into an output
    ///  table. Columns are printed according to the tableType string,
    ///  using the Column::doCol() function as a determinant.
    /// \param stream Where the output is written
    /// \param columns The vector list of Column objects
    /// \param tableType A string saying what format to use: one of
    /// "file", "log", "screen" or "votable" (although the latter
    /// shouldn't be used with this function).

    stream.setf(std::ios::fixed);  
    std::vector<Column::Col>::iterator col;
    for(col=columns.begin();col<columns.end();col++)
      if(col->doCol(tableType,this->flagWCS)) this->printTableEntry(stream, *col);
      
    stream << "\n";

  }
  //--------------------------------------------------------------------

  void Detection::printTableEntry(std::ostream &stream, Column::Col column)
  {
    /// @details
    ///  Print a single value into an output table. The Detection's
    ///  correct value is extracted according to the Column::COLNAME
    ///  key in the column given.
    /// \param stream Where the output is written
    /// \param column The Column object defining the formatting.

    switch(column.getType())
      {
      case NUM:
	column.printEntry(stream,this->id);
	break;
      case NAME:
	column.printEntry(stream,this->name);
	break;
      case X:
	column.printEntry(stream,this->getXcentre() + this->xSubOffset);
	break;
      case Y:
	column.printEntry(stream,this->getYcentre() + this->ySubOffset);
	break;
      case Z:
	column.printEntry(stream,this->getZcentre() + this->zSubOffset);
	break;
      case RA:
	if(this->flagWCS) column.printEntry(stream,this->raS);
	else column.printBlank(stream);
	break;
      case DEC:
	if(this->flagWCS) column.printEntry(stream,this->decS);
	else column.printBlank(stream);
	break;
      case RAJD:
	if(this->flagWCS) column.printEntry(stream,this->ra);
	else column.printBlank(stream);
	break;
      case DECJD:
	if(this->flagWCS) column.printEntry(stream,this->dec);
	else column.printBlank(stream);
	break;
      case VEL:
	if(this->flagWCS) {
	  if(this->specOK) column.printEntry(stream,this->vel);
	  else column.printEntry(stream,0.);
	}
	else column.printBlank(stream);
	break;
      case WRA:
	if(this->flagWCS) column.printEntry(stream,this->raWidth);
	else column.printBlank(stream);
	break;
      case WDEC:
	if(this->flagWCS) column.printEntry(stream,this->decWidth);
	else column.printBlank(stream);
	break;
      case W50:
	if(this->flagWCS) {
	  if(this->specOK) column.printEntry(stream,this->w50);
	  else column.printEntry(stream,0.);
	}
	else column.printBlank(stream);
	break;
      case W20:
	if(this->flagWCS) {
	  if(this->specOK) column.printEntry(stream,this->w20);
	  else column.printEntry(stream,0.);
	}
	else column.printBlank(stream);
	break;
      case WVEL:
	if(this->flagWCS) {
	  if(this->specOK) column.printEntry(stream,this->velWidth);
	  else column.printEntry(stream,0.);
	}
	else column.printBlank(stream);
	break;
      case FINT:
	column.printEntry(stream,this->intFlux);
	break;
      case FTOT:
	column.printEntry(stream,this->totalFlux);
	break;
      case FPEAK:
	column.printEntry(stream,this->peakFlux);
	break;
      case SNRPEAK:
	column.printEntry(stream,this->peakSNR);
	break;
      case X1:
	column.printEntry(stream,this->getXmin() + this->xSubOffset);
	break;
      case X2:
	column.printEntry(stream,this->getXmax() + this->xSubOffset);
	break;
      case Y1:
	column.printEntry(stream,this->getYmin() + this->ySubOffset);
	break;
      case Y2:
	column.printEntry(stream,this->getYmax() + this->ySubOffset);
	break;
      case Z1:
	column.printEntry(stream,this->getZmin() + this->zSubOffset);
	break;
      case Z2:
	column.printEntry(stream,this->getZmax() + this->zSubOffset);
	break;
      case NPIX:
// 	column.printEntry(stream,int(this->pixelArray.getSize()));
	column.printEntry(stream,int(this->getSize()));
	break;
      case FLAG:
	column.printEntry(stream,this->flagText);
	break;
      case XAV:
// 	column.printEntry(stream,this->getXAverage() + this->xSubOffset);
	column.printEntry(stream,this->getXaverage() + this->xSubOffset);
	break;
      case YAV:
// 	column.printEntry(stream,this->getYAverage() + this->ySubOffset);
	column.printEntry(stream,this->getYaverage() + this->ySubOffset);
	break;
      case ZAV:
// 	column.printEntry(stream,this->getZAverage() + this->zSubOffset);
	column.printEntry(stream,this->getZaverage() + this->zSubOffset);
	break;
      case XCENT:
	column.printEntry(stream,this->getXCentroid() + this->xSubOffset);
	break;
      case YCENT:
	column.printEntry(stream,this->getYCentroid() + this->ySubOffset);
	break;
      case ZCENT:
	column.printEntry(stream,this->getZCentroid() + this->zSubOffset);
	break;
      case XPEAK:
	column.printEntry(stream,this->getXPeak() + this->xSubOffset);
	break;
      case YPEAK:
	column.printEntry(stream,this->getYPeak() + this->ySubOffset);
	break;
      case ZPEAK:
	column.printEntry(stream,this->getZPeak() + this->zSubOffset);
	break;
      case NUMCH:
	column.printEntry(stream,this->getMaxAdjacentChannels());
	break;
      case SPATSIZE:
	column.printEntry(stream,int(this->getSpatialSize()));
	break;
      case UNKNOWN:
      default:
	break;
      };
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
      ss << std::setprecision(this->snrPrec);
      ss << ", S/N\\dmax\\u=" << this->peakSNR;
    }
    else{ 
      ss << "#" << std::setfill('0') << std::setw(3) << this->id << ": ";
      ss << std::setprecision(this->fintPrec);
      ss << "F\\dtot\\u=" << this->totalFlux << this->fluxUnits;
      ss << std::setprecision(this->fpeakPrec);
      ss << ", F\\dpeak\\u=" << this->peakFlux << this->fluxUnits;
      ss << std::setprecision(this->snrPrec);
      ss << ", S/N\\dmax\\u=" << this->peakSNR;
    }
    std::string output = ss.str();

    return output;
  }
  //--------------------------------------------------------------------

  std::string Detection::outputLabelWidths()
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
      ss << "W\\d"              << this->lngtype  <<"\\u=" << this->raWidth;
      ss << ", W\\d"            << this->lattype  <<"\\u=" << this->decWidth;
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
//     ss <<", Size: "<< this->pixelArray.getSize() << " voxels,  ";
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
