// -----------------------------------------------------------------------
// VOTable.cc: Output of the detected objects to a VOTable
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
#include <time.h>
#include <duchamp/Cubes/VOTable.hh>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Cubes/cubes.hh> 
#include <duchamp/Detection/detection.hh>
#include <duchamp/Detection/columns.hh>
 
namespace duchamp
{

  using namespace Column;
  std::string fixUnitsVOT(std::string oldstring)
  {
    /** 
     * Fix a string containing units to make it acceptable for a VOTable.
     *
     * This function makes the provided units string acceptable
     * according to the standard found at
     * http://vizier.u-strasbg.fr/doc/catstd-3.2.htx 
     * This should then be able to convert the units used in the text
     * table to units suitable for putting in a VOTable.
     *
     * Specifically, it removes any square brackets [] from the
     * start/end of the string, and replaces blank spaces (representing
     * multiplication) with a '.' (full stop).
     *
     * \param oldstring Input unit string to be fixed
     * \return String with fixed units
     */

    std::string newstring;
    for(int i=0;i<oldstring.size();i++){
      if((oldstring[i]!='[')&&(oldstring[i]!=']')){
	if(oldstring[i]==' ') newstring += '.';
	else newstring += oldstring[i];
      }
    }
    return newstring;  
  }
 

  void VOField::define(std::string i, std::string n, std::string U, std::string u, std::string d, std::string r, int w, int p)
  {
    /**
     * A basic definition function, defining each parameter individually. 
     * \param i The ID parameter
     * \param n The name parameter
     * \param U The UCD
     * \param u The units (fixed by fixUnits())
     * \param d The datatype
     * \param r The ref
     * \param w The width
     * \param p The precision
     */
    this->ID = i;
    this->name = n;
    this->UCD = U;
    this->unit = fixUnitsVOT(u);
    this->datatype = d;
    this->ref = r;
    this->width = w;
    this->precision = p;
  }

  void VOField::define(Column::Col column, std::string i, std::string U, std::string d, std::string r)
  {
    /**
     * Another definition function, using the information in a Column::Col object for some parameters.
     * \param column A Column::Col object, used for name, unit, width & precision
     * \param i The ID parameter
     * \param U The UCD
     * \param d The datatype
     * \param r The ref
     */
    this->ID = i;
    this->name = column.getName();
    this->UCD = U;
    this->unit = fixUnitsVOT(column.getUnits());
    this->datatype = d;
    this->ref = r;
    this->width = column.getWidth();
    this->precision = column.getPrecision();
  }

  void VOField::define(Column::Col column)
  {
    /**
     * A more useful definition function, using the Column::COLNAME
     * key to specify particular values for each of the parameters for
     * different columns.
     * \param column A Column::Col object of a particular type. The
     * column.getType() function is used to decide which call to
     * VOField::define(Column::Col column, std::string i, std::string
     * U, std::string d, std::string r) to use
     */
    switch(column.getType())
      {
      case NUM:
	this->define(column,"col01","meta.id","int","");
	break;
      case NAME:
	this->define(column,"col02","meta.id;meta.main","char","");
	break;
      case RAJD:
	this->define(column,"col03","","float","J2000");
	break;
      case DECJD:
	this->define(column,"col04","","float","J2000");
	break;
      case VEL:
	this->define(column,"col05","phys.veloc;src.dopplerVeloc","float","");
	break;
      case WRA:
	this->define(column,"col06","","float","J2000");
	break;
      case WDEC:
	this->define(column,"col07","","float","J2000");
	break;
      case W50:
	this->define(column,"col08","phys.veloc;src.dopplerVeloc;spect.line.width","float","");
	break;
      case W20:
	this->define(column,"col09","phys.veloc;src.dopplerVeloc;spect.line.width","float","");
	break;
      case WVEL:
	this->define(column,"col10","phys.veloc;src.dopplerVeloc;spect.line.width","float","");
	break;
      case FINT:
	this->define(column,"col11","phot.flux;spect.line.intensity","float","");
	this->name = "Integrated_Flux";
	break;
      case FTOT:
	this->define(column,"col11","phot.flux;spect.line.intensity","float","");
	this->name = "Total_Flux";
	break;
      case FPEAK:
	this->define(column,"col12","phot.flux;spect.line.intensity","float","");
	this->name = "Peak_Flux";
	break;
      case SNRPEAK:
	this->define(column,"col13","phot.flux;stat.snr","float","");
	break;
      case FLAG:
	this->define(column,"col14","meta.code.qual","char","");
	break;
      case XAV:
	this->define(column,"col15","pos.cartesian.x","float","");
	break;
      case YAV:
	this->define(column,"col16","pos.cartesian.y","float","");
	break;
      case ZAV:
	this->define(column,"col17","pos.cartesian.z","float","");
	break;
      case XCENT:
	this->define(column,"col18","pos.cartesian.x","float","");
	this->name = "X_Centroid";
	break;
      case YCENT:
	this->define(column,"col19","pos.cartesian.y","float","");
	this->name = "Y_Centroid";
	break;
      case ZCENT:
	this->define(column,"col20","pos.cartesian.z","float","");
	this->name = "Z_Centroid";
	break;
      case XPEAK:
	this->define(column,"col21","pos.cartesian.x","int","");
	break;
      case YPEAK:
	this->define(column,"col22","pos.cartesian.y","int","");
	break;
      case ZPEAK:
	this->define(column,"col23","pos.cartesian.z","int","");
	break;
      default:
	break;
      };
  }

  void VOField::printField(std::ostream &stream)
  {
    /**
     * Print the Field entry with appropriate formatting.
     * \param stream The output stream to send the text to.
     */
    stream << "<FIELD name=\"" <<this->name
	   << "\" ID=\"" << this->ID
	   << "\" ucd=\"" << this->UCD;
    if(this->ref!="") stream << "\" ref=\"" << this->ref;
    stream << "\" datatype=\"" << this->datatype;
    stream << "\" unit=\"" << this->unit;
    if(datatype=="char")
      stream << "\" arraysize=\"" << this->width;
    else{
      stream << "\" width=\"" << this->width;
      if(datatype=="float" || datatype=="double")
	stream << "\" precision=\"" << this->precision;
    }
    stream << "\"/>\n";

  }



  //------------------------------------------------


  template <class T> void VOParam::define(std::string n, std::string U, std::string d, T v, int w, std::string u)
  {
    /**
     * A basic definition function, defining each parameter
     * individually. The value (v) is written to a stringstream, and
     * from there stored as a string.
     * \param n The name
     * \param U The UCD
     * \param d The datatype
     * \param v The value
     * \param w The width
     * \param u The units
     */
    this->name = n;
    this->UCD = U;
    this->datatype = d;
    this->width = w;
    std::stringstream ss;
    ss << v;
    this->value = ss.str();
    this->units = u;
  }
  template void VOParam::define<int>(std::string n, std::string U, std::string d, int v, int w, std::string u);
  template void VOParam::define<long>(std::string n, std::string U, std::string d, long v, int w, std::string u);
  template void VOParam::define<float>(std::string n, std::string U, std::string d, float v, int w, std::string u);
  template void VOParam::define<double>(std::string n, std::string U, std::string d, double v, int w, std::string u);
  template void VOParam::define<std::string>(std::string n, std::string U, std::string d, std::string v, int w, std::string u);

  void VOParam::printParam(std::ostream &stream)
  {
    /**
     * Print the Param entry with appropriate formatting.
     * \param stream The output stream to send the text to.
     */
    stream << "<PARAM name=\"" <<this->name
	   << "\" ucd=\"" << this->UCD
	   << "\" datatype=\"" << this->datatype;
    if(this->units!="")
      stream << "\" units=\"" << this->units;
    if(this->width!=0){
      if(datatype=="char")
	stream << "\" arraysize=\"" << this->width;
      else
	stream << "\" width=\"" << this->width;
    }
    stream << "\" value=\"" << this->value
	   << "\"/>\n";
  }

  //------------------------------------------------


  
  void Cube::outputDetectionsVOTable(std::ostream &stream)
  {
    /**
     *  Prints to a stream (provided) the list of detected objects in the cube
     *   in a VOTable format.
     *  Uses WCS information and assumes WCS parameters have been calculated for each
     *   detected object. 
     * \param stream The output stream to send the text to.
     */
    
    stream<<"<?xml version=\"1.0\"?>\n";
    stream<<"<VOTABLE version=\"1.1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
    stream<<" xsi:noNamespaceSchemaLocation=\"http://www.ivoa.net/xml/VOTable/VOTable/v1.1\">\n";

    stream<<"  <COOSYS ID=\"J2000\" equinox=\"J2000.\" epoch=\"J2000.\" system=\"eq_FK5\"/>\n";
    stream<<"  <RESOURCE name=\"Duchamp Output\">\n";
    stream<<"    <TABLE name=\"Detections\">\n";
    stream<<"      <DESCRIPTION>Detected sources and parameters from running the Duchamp source finder.</DESCRIPTION>\n";

    // PARAM section -- parts that are not entry-specific ie. apply to whole dataset
    std::vector<VOParam> paramList;
    std::vector<VOParam>::iterator param;
    VOParam singleParam;

    std::string fname = this->par.getImageFile();
    if(this->par.getFlagSubsection()) fname+=this->par.getSubsection();
    
    singleParam.define("FITS file","meta.file;meta.fits","char",fname,fname.size(),"");
    paramList.push_back(singleParam);
    if(this->par.getFlagFDR())
      singleParam.define("FDR Significance","stat.param","float",this->par.getAlpha(),0,"");
    else
      singleParam.define("Threshold (SNR)","stat.snr","float",this->par.getCut(),0,"");
    paramList.push_back(singleParam);
    singleParam.define("Flux Threshold","phot.flux;stat.min","float",this->Stats.getThreshold(),0,this->head.getFluxUnits());
    paramList.push_back(singleParam);
    
    if(this->par.getFlagATrous()){
      std::string note = "The a trous reconstruction method was used, with the following parameters.";
      singleParam.define("ATrous note","meta.note","char",note,note.size(),"");
      paramList.push_back(singleParam);
      singleParam.define("ATrous Dimension","meta.code;stat","int",this->par.getReconDim(),0,"");
      paramList.push_back(singleParam);
      singleParam.define("ATrous Threshold","stat.snr","float",this->par.getAtrousCut(),0,"");
      paramList.push_back(singleParam);
      singleParam.define("ATrous Minimum Scale","stat.param","int",this->par.getMinScale(),0,"");
      paramList.push_back(singleParam);
      singleParam.define("ATrous Filter","meta.code;stat","char",this->par.getFilterName(),this->par.getFilterName().size(),"");
      paramList.push_back(singleParam);
    }
    if(this->par.getFlagSmooth()){
      if(this->par.getSmoothType()=="spectral"){
	std::string note = "The cube was smoothed spectrally with a Hanning filter, with the following parameters.";
	singleParam.define("Smoothing note","meta.note","char",note,note.size(),"");
	paramList.push_back(singleParam);
	singleParam.define("Hanning filter width","meta.code;stat","int",this->par.getHanningWidth(),0,"");
	paramList.push_back(singleParam);
      }
      else if(this->par.getSmoothType()=="spatial"){
	std::string note = "The cube was smoothed spatially with a Gaussian kernel, with the following parameters.";
	singleParam.define("Smoothing note","meta.note","char",note,note.size(),"");
	paramList.push_back(singleParam);
	singleParam.define("Gaussian kernel major-axis FWHM","meta.code;stat","int",this->par.getKernMaj(),0,"");
	paramList.push_back(singleParam);
	singleParam.define("Gaussian kernel minor-axis FWHM","meta.code;stat","int",this->par.getKernMin(),0,"");
	paramList.push_back(singleParam);
	singleParam.define("Gaussian kernel position angle","meta.code;stat","int",this->par.getKernPA(),0,"");
	paramList.push_back(singleParam);
      }    
    }

    for(param=paramList.begin();param<paramList.end();param++){
      stream << "      ";
      param->printParam(stream);
    }    


    if(this->fullCols.size()==0) this->setupColumns(); 
    // in case cols haven't been set -- need the column names

    std::string posUCD[4];
    if(makelower(this->fullCols[Column::RAJD].getName())=="ra"){
      posUCD[0] = "pos.eq.ra;meta.main";
      posUCD[2] = "phys.angSize;pos.eq.ra";
    }
    else{
      posUCD[0] = "pos.galactic.lat;meta.main";
      posUCD[2] = "phys.angSize;pos.galactic.lat";
    }
    if(makelower(this->fullCols[DECJD].getName())=="dec"){
      posUCD[1] = "pos.eq.dec;meta.main";
      posUCD[3] = "phys.angSize;pos.eq.dec";
    }
    else{
      posUCD[1] = "pos.galactic.lon;meta.main";
      posUCD[3] = "phys.angSize;pos.galactic.lon";
    }

    std::vector<Column::Col>::iterator col;
    for(col=this->fullCols.begin();col<this->fullCols.end();col++){

      if(col->doCol("votable",this->head.isSpecOK())){
	VOField field;
	field.define(*col);
	if(col->getType()==RAJD)  field.setUCD(posUCD[0]);
	if(col->getType()==WRA)   field.setUCD(posUCD[1]);
	if(col->getType()==DECJD) field.setUCD(posUCD[2]);
	if(col->getType()==WDEC)  field.setUCD(posUCD[3]);	
	stream << "      ";
	field.printField(stream);
      }

    }

    stream<<"      <DATA>\n"
	  <<"        <TABLEDATA>\n";

    stream.setf(std::ios::fixed);  

    std::vector<Detection>::iterator obj;
    for(obj=this->objectList->begin();obj<this->objectList->end();obj++){

      stream<<"        <TR>\n";
      stream<<"          ";
      std::vector<Column::Col>::iterator col;
      for(col=this->fullCols.begin();col<this->fullCols.end();col++){
	if(col->doCol("votable",this->head.isSpecOK())){
	  stream<<"<TD>";
	  obj->printTableEntry(stream, *col);
	  stream<<"</TD>";
	}
      }
      stream<<"\n";
      stream<<"        </TR>\n";

    }

    stream<<"        </TABLEDATA>\n";
    stream<<"      </DATA>\n";
    stream<<"    </TABLE>\n";
    stream<<"  </RESOURCE>\n";
    stream<<"</VOTABLE>\n";
    resetiosflags(std::ios::fixed);
  
  }




}
