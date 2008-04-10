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

  void Detection::printTableRow(std::ostream &stream, std::vector<Column::Col> columns, std::string tableType)
  {

    stream.setf(std::ios::fixed);  
    std::vector<Column::Col>::iterator col;
    for(col=columns.begin();col<columns.end();col++)
      if(col->doCol(tableType,this->flagWCS)) this->printTableEntry(stream, *col);
      
    stream << "\n";

  }



  void Detection::printTableEntry(std::ostream &stream, Column::Col column)
  {

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
	column.printEntry(stream,int(this->pixelArray.getSize()));
	break;
      case FLAG:
	column.printEntry(stream,this->flagText);
	break;
      case XAV:
	column.printEntry(stream,this->getXAverage() + this->xSubOffset);
	break;
      case YAV:
	column.printEntry(stream,this->getYAverage() + this->ySubOffset);
	break;
      case ZAV:
	column.printEntry(stream,this->getZAverage() + this->zSubOffset);
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
      case UNKNOWN:
      default:
	break;
      };
  }

  void Detection::outputDetectionTextHeader(std::ostream &stream, 
					    std::vector<Column::Col> columns)
  {
    /**
     *  Prints to a stream the column headers to match the output 
     *  generated by outputDetectionText or outputDetectionTextWCS
     *  The exact columns depend on the std::vector<Col>.
     *
     *  If the WCS is good, it will print the FINT column, otherwise the
     *  FTOT column.
     *
     *  NOTE: It does not print out the 9 columns containing the three
     *  different pixel centre options -- it stops after the FLAG
     *  column.
     *
     * \param stream Where to print.
     * \param columns The set of Columns::Col objects that define the
     *                columns to be printed.
     */

    std::vector<Col> local = columns;
    std::cerr << "++> " << local.size() << " " << int(FLAG) << "\n";    
    if(local.size()==Column::numColumns){
      std::vector <Col>::iterator iter;
      if(this->flagWCS)
	iter = local.begin() + FTOT;
      else 
	iter = local.begin() + FINT;
      local.erase(iter);
      iter = local.begin()+DECJD;
      local.erase(iter);
      iter = local.begin()+RAJD;
      local.erase(iter);
    }
  
    int size = local.size();
    if(int(FLAG)<size) size = int(FLAG);
    std::cerr << "++> " << size << " " << int(FLAG) << "\n";

    stream << std::setfill(' ');
    for(int i=0;i<size;i++) local[i].printDash(stream);
    stream << std::endl;
    for(int i=0;i<size;i++) local[i].printTitle(stream);
    stream << std::endl;
    for(int i=0;i<size;i++) local[i].printUnits(stream);
    stream << std::endl;
    for(int i=0;i<size;i++) local[i].printDash(stream);
    stream << std::endl;
  }
  //--------------------------------------------------------------------

  void Detection::outputDetectionTextWCS(std::ostream &stream, 
					 std::vector<Column::Col> columns)
  {
    /**
     *  Prints to a stream the relevant details of a detected object,
     *  including the WCS parameters, which need to have been
     *  calculated.  If they have not (given by the isWCS() function),
     *  then the WCS-related outputs are left blank.
     *
     *  Also, if the WCS is good, we print the FINT column, else the
     *  FTOT column.
     *
     * \param stream Where to print.
     * \param columns The set of Columns::Col objects that define the
     *                columns to be printed.
     */

    if(columns.size()!=Column::numColumns){
      std::stringstream errmsg;
      errmsg << "columnSet used has wrong number of columns (" 
	     << columns.size() << ")\nshould be " 
	     << Column::numColumns << ".\n";
      duchampError("outputDetectionTextWCS",errmsg.str());
    }
    else{
      stream.setf(std::ios::fixed);  
      columns[NUM].printEntry(stream,this->id);
      columns[NAME].printEntry(stream,this->name);
      columns[X].printEntry(stream,this->getXcentre() + this->xSubOffset);
      columns[Y].printEntry(stream,this->getYcentre() + this->ySubOffset);
      columns[Z].printEntry(stream,this->getZcentre() + this->zSubOffset);
      if(this->flagWCS){
	columns[RA].printEntry(stream,this->raS);
	columns[DEC].printEntry(stream,this->decS);
	if(this->specOK) columns[VEL].printEntry(stream,this->vel);
	else columns[VEL].printEntry(stream,0.);
	columns[WRA].printEntry(stream,this->raWidth);
	columns[WDEC].printEntry(stream,this->decWidth);
	if(this->specOK) columns[WVEL].printEntry(stream,this->velWidth);
	else columns[WVEL].printEntry(stream,0.);
      }
      else for(int i=RA;i<=WVEL;i++) columns[i].printBlank(stream);
      if(this->flagWCS)
	columns[FINT].printEntry(stream,this->intFlux);
      else 
	columns[FTOT].printEntry(stream,this->totalFlux);
      columns[FPEAK].printEntry(stream,this->peakFlux);
      columns[SNRPEAK].printEntry(stream,this->peakSNR);
      columns[X1].printEntry(stream,this->getXmin() + this->xSubOffset);
      columns[X2].printEntry(stream,this->getXmax() + this->xSubOffset);
      columns[Y1].printEntry(stream,this->getYmin() + this->ySubOffset);
      columns[Y2].printEntry(stream,this->getYmax() + this->ySubOffset);
      columns[Z1].printEntry(stream,this->getZmin() + this->zSubOffset);
      columns[Z2].printEntry(stream,this->getZmax() + this->zSubOffset);
      columns[NPIX].printEntry(stream,int(this->pixelArray.getSize()));
      columns[FLAG].printEntry(stream,this->flagText);
      stream << std::endl;
    }
  }
  //--------------------------------------------------------------------

  void Detection::outputDetectionText(std::ostream &stream, 
				      std::vector<Column::Col> columns, 
				      int idNumber)
  {
    /**
     *  Print to a stream the relevant details of a detected object.
     *  This does not include any WCS parameters, only pixel positions & extent, 
     *    and flux info (FTOT, FPEAK, SNRPEAK).
     *  Also prints a counter, provided as an input.
     *
     * \param stream Where to print.
     * \param columns The set of Columns::Col objects that define the
     *    columns to be printed.
     * \param idNumber The number that acts as a counter in the first column.
     */

    if(columns.size()!=Column::numColumnsLog){
      std::stringstream errmsg;
      errmsg << "columnSet used has wrong number of columns (" 
	     << columns.size() << ")\nshould be "
	     << Column::numColumnsLog <<".\n";
      duchampError("outputDetectionText",errmsg.str());
    }
    else{
      stream << std::setfill(' ');
      stream.setf(std::ios::fixed);  
      columns[lNUM].printEntry(stream,idNumber);
      columns[lX].printEntry(stream,this->getXcentre() + this->xSubOffset);
      columns[lY].printEntry(stream,this->getYcentre() + this->ySubOffset);
      columns[lZ].printEntry(stream,this->getZcentre() + this->zSubOffset);
      columns[lFTOT].printEntry(stream,this->totalFlux);
      columns[lFPEAK].printEntry(stream,this->peakFlux);
      columns[lSNRPEAK].printEntry(stream,this->peakSNR);
      columns[lX1].printEntry(stream,this->getXmin() + this->xSubOffset);
      columns[lX2].printEntry(stream,this->getXmax() + this->xSubOffset);
      columns[lY1].printEntry(stream,this->getYmin() + this->ySubOffset);
      columns[lY2].printEntry(stream,this->getYmax() + this->ySubOffset);
      columns[lZ1].printEntry(stream,this->getZmin() + this->zSubOffset);
      columns[lZ2].printEntry(stream,this->getZmax() + this->zSubOffset);
      columns[lNPIX].printEntry(stream,this->pixelArray.getSize());
      stream<<std::endl;
    }
  }
  //--------------------------------------------------------------------

  void Detection::outputDetectionTextHeaderFull(std::ostream &stream, 
						std::vector<Column::Col> columns)
  {
    /**
     *  Prints to a stream the column headers to match the output 
     *  generated by outputDetectionText or outputDetectionTextWCS
     *  The exact columns depend on the std::vector<Col>.
     *
     *  NOTE: It prints all columns, including all the different pixel
     *  centre options and all FLUX columns.
     *
     * \param stream Where to print.
     * \param columns The set of Columns::Col objects that define the
     * columns to be printed.
     */

    stream << std::setfill(' ');
    for(int i=0;i<columns.size();i++) columns[i].printDash(stream);
    stream << std::endl;
    for(int i=0;i<columns.size();i++) columns[i].printTitle(stream);
    stream << std::endl;
    for(int i=0;i<columns.size();i++) columns[i].printUnits(stream);
    stream << std::endl;
    for(int i=0;i<columns.size();i++) columns[i].printDash(stream);
    stream << std::endl;
  }
  //--------------------------------------------------------------------

  void Detection::outputDetectionTextWCSFull(std::ostream &stream, 
					     std::vector<Column::Col> columns)
  {
    /**
     *  Prints to a stream the relevant details of a detected object,
     *  including the WCS parameters, which need to have been calculated.
     *  If they have not (given by the isWCS() function), then the 
     *  WCS-related outputs are left blank.
     *  This function prints all the different versions of the centre
     *  pixel values, and all different flux quantities.
     * \param stream Where to print.
     * \param columns The set of Columns::Col objects that define the
     * columns to be printed.
     */

    if(columns.size()!=Column::numColumns){
      std::stringstream errmsg;
      errmsg << "columnSet used has wrong number of columns (" 
	     << columns.size() << ")\nshould be " 
	     << Column::numColumns << ".\n";
      duchampError("outputDetectionTextWCS",errmsg.str());
    }
    else{
      stream.setf(std::ios::fixed);  
      columns[NUM].printEntry(stream,this->id);
      columns[NAME].printEntry(stream,this->name);
      columns[X].printEntry(stream,this->getXcentre() + this->xSubOffset);
      columns[Y].printEntry(stream,this->getYcentre() + this->ySubOffset);
      columns[Z].printEntry(stream,this->getZcentre() + this->zSubOffset);
      if(this->flagWCS){
	columns[RA].printEntry(stream,this->raS);
	columns[DEC].printEntry(stream,this->decS);
	if(this->specOK) columns[VEL].printEntry(stream,this->vel);
	//       else printSpace(stream,columns[VEL].getWidth());
	else columns[VEL].printEntry(stream,0.);
	columns[WRA].printEntry(stream,this->raWidth);
	columns[WDEC].printEntry(stream,this->decWidth);
	if(this->specOK) columns[WVEL].printEntry(stream,this->velWidth);
	else columns[WVEL].printEntry(stream,0.);
      }
      else for(int i=RA;i<=WVEL;i++) columns[i].printBlank(stream);
      columns[FINT].printEntry(stream,this->intFlux);
      columns[FTOT].printEntry(stream,this->totalFlux);
      columns[FPEAK].printEntry(stream,this->peakFlux);
      columns[SNRPEAK].printEntry(stream,this->peakSNR);
      columns[X1].printEntry(stream,this->getXmin() + this->xSubOffset);
      columns[X2].printEntry(stream,this->getXmax() + this->xSubOffset);
      columns[Y1].printEntry(stream,this->getYmin() + this->ySubOffset);
      columns[Y2].printEntry(stream,this->getYmax() + this->ySubOffset);
      columns[Z1].printEntry(stream,this->getZmin() + this->zSubOffset);
      columns[Z2].printEntry(stream,this->getZmax() + this->zSubOffset);
      columns[NPIX].printEntry(stream,int(this->pixelArray.getSize()));
      columns[FLAG].printEntry(stream,this->flagText);
      columns[XAV].printEntry(stream,this->getXAverage() + this->xSubOffset);
      columns[YAV].printEntry(stream,this->getYAverage() + this->ySubOffset);
      columns[ZAV].printEntry(stream,this->getZAverage() + this->zSubOffset);
      columns[XCENT].printEntry(stream,this->getXCentroid() + this->xSubOffset);
      columns[YCENT].printEntry(stream,this->getYCentroid() + this->ySubOffset);
      columns[ZCENT].printEntry(stream,this->getZCentroid() + this->zSubOffset);
      columns[XPEAK].printEntry(stream,this->getXPeak() + this->xSubOffset);
      columns[YPEAK].printEntry(stream,this->getYPeak() + this->ySubOffset);
      columns[ZPEAK].printEntry(stream,this->getZPeak() + this->zSubOffset);
      stream << std::endl;
    }
  }
  //--------------------------------------------------------------------

  std::string Detection::outputLabelWCS()
  {
    /**
     *  Prints to a std::string the WCS position and velocity
     *  information of the Detection, as well as the ID number and any
     *  flags.
     *  Assumes the WCS parameters of the object have been calculated.
     *  If they have not (given by the isWCS() function), then the
     *  WCS-related outputs are left blank.
     *  Returns the string.
     */
    std::stringstream ss;
    ss << "#" << std::setfill('0') << std::setw(3) << this->id << ": ";
    ss << this->name ;
    if(this->getFlagText()!="") 
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
    /**
     *  Prints to a std::string the fluxes of the object, both
     *  integrated/total and peak, as well as the peak S/N value.
     *  Assumes the WCS parameters of the object have been calculated.
     *  If they have not (given by the isWCS() function), then the ID
     *  number and the total/peak/SNR values are returned.
     *  /return The string.
     */

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
    /**
     *  Prints to a std::string the widths of the object in position
     *  and velocity.
     *  Assumes the WCS parameters of the object have been calculated.
     *  If they have not (given by the isWCS() function), then a string of 
     *   length 0 is returned.
     *  \returns The string.
     */

    std::string output;
    if(this->flagWCS){
      std::stringstream ss;
      ss.setf(std::ios::fixed);
      ss << std::setprecision(this->posPrec);
      ss << "w_"          << this->lngtype  <<"="    << this->raWidth;
      ss << ", w_"        << this->lattype  <<"="    << this->decWidth;
      if(this->specOK){
	ss << std::setprecision(this->velPrec);
	ss << ", w_Vel="    << this->velWidth << " " << this->specUnits;
      }
      output = ss.str();
    }
    else output = this->outputLabelFluxes();

    return output;
  }
  //--------------------------------------------------------------------

  std::string Detection::outputLabelPix()
  {
    /**
     *  Prints to a std::string the pixel centres and extents of a
     *  detected object.  Returns the string.
     */

    std::stringstream ss;
    ss.setf(std::ios::fixed);
    ss << "Centre: ";
    ss << std::setprecision(this->xyzPrec) << std::setfill(' ');
    ss <<"("       << this->getXcentre() + this->xSubOffset;
    ss <<", "      << this->getYcentre() + this->ySubOffset;
    ss <<", "      << this->getZcentre() + this->zSubOffset << ")";
    ss <<", Size: "<< this->pixelArray.getSize() << " voxels,  ";
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
