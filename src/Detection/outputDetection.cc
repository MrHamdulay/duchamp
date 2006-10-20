#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <duchamp.hh>
#include <param.hh>
#include <Detection/detection.hh>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>
#include <Detection/columns.hh>

using namespace Column;

void Detection::outputDetectionTextHeader(std::ostream &stream, 
					  vector<Col> columns)
{
  /**
   * outputDetectionTextHeader
   *  Prints to a stream the column headers to match the output 
   *  generated by outputDetectionText or outputDetectionTextWCS
   *  The exact columns depend on the vector<Col>.
   */

  vector<Col> local = columns;
  if(local.size()==Column::numColumns){
    vector <Col>::iterator iter;
    if(this->flagWCS) iter = local.begin() + FTOT;
    else iter = local.begin() + FINT;
    local.erase(iter);
  }

  stream << std::setfill(' ');
  for(int i=0;i<local.size();i++) local[i].printDash(stream);
  stream << std::endl;
  for(int i=0;i<local.size();i++) local[i].printTitle(stream);
  stream << std::endl;
  for(int i=0;i<local.size();i++) local[i].printUnits(stream);
  stream << std::endl;
  for(int i=0;i<local.size();i++) local[i].printDash(stream);
  stream << std::endl;
}
//--------------------------------------------------------------------

void Detection::outputDetectionTextWCS(std::ostream &stream, 
				       vector<Col> columns)
{
  /**
   * outputDetectionTextWCS
   *  Prints to a stream the relevant details of a detected object,
   *  including the WCS parameters, which need to have been calculated.
   *  If they have not (given by the isWCS() function), then the 
   *  WCS-related outputs are left blank.
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
    columns[NAME].printEntry(stream,this->name.c_str());
    columns[X].printEntry(stream,this->xcentre + this->xSubOffset);
    columns[Y].printEntry(stream,this->ycentre + this->ySubOffset);
    columns[Z].printEntry(stream,this->zcentre + this->zSubOffset);
    if(this->flagWCS){
      columns[RA].printEntry(stream,this->raS);
      columns[DEC].printEntry(stream,this->decS);
      columns[VEL].printEntry(stream,this->vel);
      columns[WRA].printEntry(stream,this->raWidth);
      columns[WDEC].printEntry(stream,this->decWidth);
      columns[WVEL].printEntry(stream,this->velWidth);
    }
    else for(int i=RA;i<=WVEL;i++) columns[i].printBlank(stream);
    if(this->flagWCS) columns[FINT].printEntry(stream,this->intFlux);
    else columns[FTOT].printEntry(stream,this->totalFlux);
    columns[FPEAK].printEntry(stream,this->peakFlux);
    columns[SNRPEAK].printEntry(stream,this->peakSNR);
    columns[X1].printEntry(stream,this->xmin + this->xSubOffset);
    columns[X2].printEntry(stream,this->xmax + this->xSubOffset);
    columns[Y1].printEntry(stream,this->ymin + this->ySubOffset);
    columns[Y2].printEntry(stream,this->ymax + this->ySubOffset);
    columns[Z1].printEntry(stream,this->zmin + this->zSubOffset);
    columns[Z2].printEntry(stream,this->zmax + this->zSubOffset);
    columns[NPIX].printEntry(stream,int(this->pix.size()));
    columns[FLAG].printEntry(stream,this->flagText);
    stream << std::endl;
  }
}
//--------------------------------------------------------------------

void Detection::outputDetectionText(std::ostream &stream, 
				    vector<Col> columns, int idNumber)
{
  /**
   * outputDetectionText
   *  Print to a stream the relevant details of a detected object.
   *  This does not include any WCS parameters, only pixel positions & extent, 
   *    and flux info (including peak SNR).
   *  Also prints a counter, provided as an input.
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
   columns[lX].printEntry(stream,this->xcentre + this->xSubOffset);
   columns[lY].printEntry(stream,this->ycentre + this->ySubOffset);
   columns[lZ].printEntry(stream,this->zcentre + this->zSubOffset);
   columns[lFTOT].printEntry(stream,this->totalFlux);
   columns[lFPEAK].printEntry(stream,this->peakFlux);
   columns[lSNRPEAK].printEntry(stream,this->peakSNR);
   columns[lX1].printEntry(stream,this->xmin + this->xSubOffset);
   columns[lX2].printEntry(stream,this->xmax + this->xSubOffset);
   columns[lY1].printEntry(stream,this->ymin + this->ySubOffset);
   columns[lY2].printEntry(stream,this->ymax + this->ySubOffset);
   columns[lZ1].printEntry(stream,this->zmin + this->zSubOffset);
   columns[lZ2].printEntry(stream,this->zmax + this->zSubOffset);
   columns[lNPIX].printEntry(stream,this->pix.size());
   stream<<std::endl;
  }
}
//--------------------------------------------------------------------

string Detection::outputLabelWCS()
{

  std::stringstream ss;
  ss << "#" << std::setfill('0') << std::setw(3) << this->id << ": ";
  ss << this->name ;
  if(this->getFlagText()!="") 
    ss << " [" << this->getFlagText() << "]   ";
  else ss<< "   ";
  ss << std::setfill(' ');
  ss << this->raS << ", ";
  ss << this->decS;
  ss << std::setprecision(this->velPrec);
  ss.setf(std::ios::fixed);
  ss << ", " << this->vel << " " << this->specUnits;

  return ss.str();

}
//--------------------------------------------------------------------

string Detection::outputLabelInfo()
{
  /**
   * outputLabelInfo
   *  Prints to a string the widths of the object (in position and velocity), 
   *  as well as the flux information.
   *  Assumes the WCS parameters of the object have been calculated.
   *  If they have not (given by the isWCS() function), then the WCS-related 
   *   outputs are left blank.
   *  Returns the string.
   */

  std::stringstream ss;
  ss.setf(std::ios::fixed);
  if(this->flagWCS){
    ss << std::setprecision(this->posPrec);
    ss << "w_"          << this->lngtype  <<"="    << this->raWidth;
    ss << ", w_"        << this->lattype  <<"="    << this->decWidth;
    ss << std::setprecision(this->velPrec);
    ss << ", w_Vel="    << this->velWidth << " " << this->specUnits;
    ss << std::setprecision(this->fintPrec);
    ss << ", F\\dint\\u=" << this->intFlux << " " << this->intFluxUnits;
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
  string output = ss.str();

  return output;
}
//--------------------------------------------------------------------

string Detection::outputLabelPix()
{
  /**
   * outputLabelPix
   *  Prints to a string the pixel centres and extents of a detected object.
   *  Returns the string.
   */

  std::stringstream ss;
  ss.setf(std::ios::fixed);
  ss << "Centre: ";
  ss << std::setprecision(this->xyzPrec) << std::setfill(' ');
  ss <<"("  << this->xcentre + this->xSubOffset;
  ss <<", " << this->ycentre + this->ySubOffset;
  ss <<", " << this->zcentre + this->zSubOffset << ")";
  ss <<", Size: " << this->pix.size() << " voxels,  ";
  ss <<"Range: ["<< this->xmin + this->xSubOffset 
     <<":"<< this->xmax + this->xSubOffset;
  ss <<", "      << this->ymin + this->ySubOffset 
     <<":"<< this->ymax + this->ySubOffset;
  ss <<", "      << this->zmin + this->zSubOffset 
     <<":"<< this->zmax + this->zSubOffset << "]";
  
  return ss.str();
}


