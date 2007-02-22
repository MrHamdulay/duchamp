#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <time.h>
#include <Cubes/cubes.hh> 
#include <Detection/detection.hh>
#include <Detection/columns.hh>
#include <Utils/utils.hh>
 
using std::endl;
using std::setw;
using std::setprecision;
using namespace Column;

void Cube::outputDetectionsKarma(std::ostream &stream)
{
  /**
   *  Prints to a stream (provided) the list of detected objects in the cube
   *   in the format of an annotation file for the Karma suite of programs.
   *  Annotation file draws a box enclosing the detection, and writes the 
   *   ID number of the detection to the right of the box.
   */

  std::string fname = this->par.getImageFile();
  if(this->par.getFlagSubsection()) fname+=this->par.getSubsection();
  stream << "# Duchamp Source Finder results for FITS file: " << fname << endl;
  if(this->par.getFlagFDR())
    stream<<"# FDR Significance = " << this->par.getAlpha() << endl;
  else
    stream<<"# Threshold = " << this->par.getCut() << endl;
  if(this->par.getFlagATrous()){
    stream<<"# The a trous reconstruction method was used, with the following parameters." << endl;
    stream<<"#  ATrous Dimension = " << this->par.getReconDim() << endl;
    stream<<"#  ATrous Threshold = " << this->par.getAtrousCut() << endl;
    stream<<"#  ATrous Minimum Scale =" << this->par.getMinScale() << endl;
    stream<<"#  ATrous Filter = " << this->par.getFilterName() << endl;
  }
  else stream << "# No ATrous reconstruction done." << endl;
  stream << "#\n";
  stream << "COLOR RED" << endl;
  stream << "COORD W" << endl;
  for(int i=0;i<this->objectList.size();i++){
    if(this->head.isWCS()){
      float radius = this->objectList[i].getRAWidth()/120.;
      if(this->objectList[i].getDecWidth()/120.>radius)
	radius = this->objectList[i].getDecWidth()/120.;
      stream << "CIRCLE " 
	     << this->objectList[i].getRA() << " " 
	     << this->objectList[i].getDec() << " " 
	     << radius << endl;
      stream << "TEXT " 
	     << this->objectList[i].getRA() << " " 
	     << this->objectList[i].getDec() << " " 
	     << this->objectList[i].getID() << endl;
    }
    else{
      float radius = this->objectList[i].getXmax() - 
	this->objectList[i].getXmin() + 1;
      if(this->objectList[i].getYmax()-this->objectList[i].getYmin() + 1 
	 > radius)
	radius = this->objectList[i].getYmax() - 
	  this->objectList[i].getYmin() + 1;
      stream << "CIRCLE " 
	     << this->objectList[i].getXcentre() << " " 
	     << this->objectList[i].getYcentre() << " " 
	     << radius << endl;
      stream << "TEXT " 
	     << this->objectList[i].getXcentre() << " " 
	     << this->objectList[i].getYcentre() << " " 
	     << this->objectList[i].getID() << endl;
    }
  }      

}

std::string fixUnitsVOT(std::string oldstring)
{
  /** Fix a string containing units to make it acceptable for a VOTable.
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

void Cube::outputDetectionsVOTable(std::ostream &stream)
{
  /**
   *  Prints to a stream (provided) the list of detected objects in the cube
   *   in a VOTable format.
   *  Uses WCS information and assumes WCS parameters have been calculated for each
   *   detected object.
   *  If they have not (given by the isWCS() function), then those objects are not written...
   */

  // Set up Column definitions here
  std::vector<Col> localCol;
  std::string posUCD[4];
  localCol.push_back(this->fullCols[NUM]);  // objID
  localCol.push_back(this->fullCols[NAME]);  // name
  localCol.push_back(this->fullCols[RA]);  // ra
  if(makelower(localCol[2].getName())=="ra"){
    posUCD[0] = "pos.eq.ra;meta.main";
    posUCD[2] = "phys.angSize;pos.eq.ra";
  }
  else{
    posUCD[0] = "pos.galactic.lat;meta.main";
    posUCD[2] = "phys.angSize;pos.galactic.lat";
  }
  localCol.push_back(this->fullCols[DEC]);  // dec
  if(makelower(localCol[2].getName())=="dec"){
    posUCD[1] = "pos.eq.dec;meta.main";
    posUCD[3] = "phys.angSize;pos.eq.dec";
  }
  else{
    posUCD[1] = "pos.galactic.lon;meta.main";
    posUCD[3] = "phys.angSize;pos.galactic.lon";
  }
  localCol.push_back(this->fullCols[WRA]);  // w_ra
  localCol.push_back(this->fullCols[WDEC]);  // w_dec
  localCol.push_back(this->fullCols[VEL]);  // vel
  localCol.push_back(this->fullCols[WVEL]); // w_vel
  localCol.push_back(this->fullCols[FINT]); // f_int
  localCol.push_back(this->fullCols[FPEAK]); // f_peak
  localCol.push_back(this->fullCols[FLAG]); // flag

  stream<<"<?xml version=\"1.0\"?>\n";
  stream<<"<VOTABLE version=\"1.1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
  stream<<" xsi:noNamespaceSchemaLocation=\"http://www.ivoa.net/xml/VOTable/VOTable/v1.1\">\n";

  stream<<"  <COOSYS ID=\"J2000\" equinox=\"J2000.\" epoch=\"J2000.\" system=\"eq_FK5\"/>\n";
  stream<<"  <RESOURCE name=\"Duchamp Output\">\n";
  stream<<"    <TABLE name=\"Detections\">\n";
  stream<<"      <DESCRIPTION>Detected sources and parameters from running the Duchamp source finder.</DESCRIPTION>\n";

  // PARAM section -- parts that are not entry-specific ie. apply to whole dataset
  std::string fname = this->par.getImageFile();
  if(this->par.getFlagSubsection()) fname+=this->par.getSubsection();
  stream<<"      <PARAM name=\"FITS file\" datatype=\"char\" ucd=\"meta.file;meta.fits\" value=\"" 
	<< fname << "\" arraysize=\""<<fname.size()<<"\"/>\n";
  if(this->par.getFlagFDR())
    stream<<"      <PARAM name=\"FDR Significance\" datatype=\"float\" ucd=\"stat.param\" value=\"" 
	  << this->par.getAlpha() << "\"/>\n";
  else
    stream<<"      <PARAM name=\"Threshold\" datatype=\"float\" ucd=\"stat.snr\" value=\"" 
	  << this->par.getCut() << "\"/>\n";
  if(this->par.getFlagATrous()){
    std::string note = "The a trous reconstruction method was used, with the following parameters.";
    stream<<"      <PARAM name=\"ATrous note\" datatype=\"char\" ucd=\"meta.note\" value=\""
	  <<note << "\" arraysize=\"" << note.size() << "\"/>\n";
    stream<<"      <PARAM name=\"ATrous Dimension\" datatype=\"int\" ucd=\"meta.code;stat\" value=\"" 
	  << this->par.getReconDim() << "\"/>\n";
    stream<<"      <PARAM name=\"ATrous Threshold\" datatype=\"float\" ucd=\"stat.snr\" value=\"" 
	  << this->par.getAtrousCut() << "\"/>\n";
    stream<<"      <PARAM name=\"ATrous Minimum Scale\" datatype=\"int\" ucd=\"stat.param\" value=\"" 
	  << this->par.getMinScale() << "\"/>\n";
    stream<<"      <PARAM name=\"ATrous Filter\" datatype=\"char\" ucd=\"meta.code;stat\" value=\""
	  << this->par.getFilterName() << "\" arraysize=\"" << this->par.getFilterName().size() << "\"/>\n";
  }
  // FIELD section -- names, titles and info for each column.
  stream<<"      <FIELD name=\"ID\" ID=\"col01\" ucd=\"meta.id\" datatype=\"int\" width=\""
	<<localCol[0].getWidth()<<"\" unit=\"--\"/>\n";
  stream<<"      <FIELD name=\"Name\" ID=\"col02\" ucd=\"meta.id;meta.main\" datatype=\"char\" arraysize=\""
	<<localCol[1].getWidth()<<"\" unit=\"--\"/>\n";
  stream<<"      <FIELD name=\""<<localCol[2].getName()<<"\" ID=\"col03\" ucd=\""
	<<posUCD[0]<<"\" ref=\"J2000\" datatype=\"float\" width=\""
	<<localCol[2].getWidth()<<"\" precision=\""<<prec[prPOS]<<"\" unit=\"deg\"/>\n";
  stream<<"      <FIELD name=\""<<localCol[3].getName()<<"\" ID=\"col04\" ucd=\""
	<<posUCD[1]<<"\" ref=\"J2000\" datatype=\"float\" width=\""
	<<localCol[3].getWidth()<<"\" precision=\""<<prec[prPOS]<<"\" unit=\"deg\"/>\n";
  stream<<"      <FIELD name=\""<<localCol[4].getName()<<"\" ID=\"col05\" ucd=\""
	<<posUCD[2]<<"\" ref=\"J2000\" datatype=\"float\" width=\""
	<<localCol[4].getWidth()<<"\" precision=\""<<prec[prWPOS]<<"\" unit=\""
	<<fixUnitsVOT(localCol[4].getUnits())<<"\"/>\n";
  stream<<"      <FIELD name=\""<<localCol[5].getName()<<"\" ID=\"col06\" ucd=\""
	<<posUCD[2]<<"\" ref=\"J2000\" datatype=\"float\" width=\""
	<<localCol[5].getWidth()<<"\" precision=\""<<prec[prWPOS]<<"\" unit=\""
	<<fixUnitsVOT(localCol[5].getUnits())<<"\"/>\n";
  stream<<"      <FIELD name=\"Vel\" ID=\"col07\" ucd=\"phys.veloc;src.dopplerVeloc\" datatype=\"float\" width=\""
	<<localCol[6].getWidth()<<"\" precision=\""<<prec[prVEL]<<"\" unit=\""
	<<fixUnitsVOT(localCol[6].getUnits())<<"\"/>\n";
  stream<<"      <FIELD name=\"w_Vel\" ID=\"col08\" ucd=\"phys.veloc;src.dopplerVeloc;spect.line.width\""
	<<" datatype=\"float\" width=\""<<localCol[7].getWidth()<<"\" precision=\""
	<<prec[prVEL]<<"\" unit=\""<<fixUnitsVOT(localCol[7].getUnits())<<"\"/>\n";
  stream<<"      <FIELD name=\"Integrated_Flux\" ID=\"col09\" ucd=\"phot.flux;spect.line.intensity\""
	<<" datatype=\"float\" width=\""<<localCol[8].getWidth()<<"\" precision=\""
	<<prec[prFLUX]<<"\" unit=\""<<fixUnitsVOT(localCol[8].getUnits())<<"\"/>\n";
  stream<<"      <FIELD name=\"Peak_Flux\" ID=\"col10\" ucd=\"phot.flux;spect.line.intensity\""
	<<" datatype=\"float\" width=\""<<localCol[9].getWidth()<<"\" precision=\""
	<<prec[prFLUX]<<"\" unit=\""<<fixUnitsVOT(localCol[9].getUnits())<<"\"/>\n";
  stream<<"      <FIELD name=\"Flag\" ID=\"col11\" ucd=\"meta.code.qual\" datatype=\"char\" arraysize=\"3\" unit=\"--\"/>\n";


  stream<<"      <DATA>\n"
	<<"        <TABLEDATA>\n";

  stream.setf(std::ios::fixed);  
  for(int i=0;i<this->objectList.size();i++){
    if(this->objectList[i].isWCS()){
      stream<<"        <TR>\n";
      stream<<"          <TD>"<<setw(localCol[0].getWidth())<<i+1<<"</TD>";
      stream<<"<TD>" << setw(localCol[1].getWidth()) << this->objectList[i].getName()       <<"</TD>";
      stream<<setprecision(prec[prPOS]);
      stream<<"<TD>" << setw(localCol[2].getWidth()) << this->objectList[i].getRA()         <<"</TD>";
      stream<<"<TD>" << setw(localCol[3].getWidth()) << this->objectList[i].getDec()        <<"</TD>";
      stream<<setprecision(prec[prWPOS]);
      stream<<"<TD>" << setw(localCol[4].getWidth()) << this->objectList[i].getRAWidth()    <<"</TD>";
      stream<<"<TD>" << setw(localCol[5].getWidth()) << this->objectList[i].getDecWidth()   <<"</TD>";
      stream<<setprecision(prec[prVEL]);
      stream<<"<TD>" << setw(localCol[6].getWidth()) << this->objectList[i].getVel()        <<"</TD>";
      stream<<"<TD>" << setw(localCol[7].getWidth()) << this->objectList[i].getVelWidth()   <<"</TD>";
      stream<<setprecision(prec[prFLUX]);
      stream<<"<TD>" << setw(localCol[8].getWidth()) << this->objectList[i].getIntegFlux()  <<"</TD>";
      stream<<"<TD>" << setw(localCol[9].getWidth()) << this->objectList[i].getPeakFlux()   <<"</TD>";
      stream<<"<TD>" << setw(localCol[10].getWidth())<< this->objectList[i].getFlagText()   <<"</TD>";
      
      stream<<endl;
      stream<<"        </TR>\n";
    }
  }
  stream<<"        </TABLEDATA>\n";
  stream<<"      </DATA>\n";
  stream<<"    </TABLE>\n";
  stream<<"  </RESOURCE>\n";
  stream<<"</VOTABLE>\n";
  resetiosflags(std::ios::fixed);
  
}


void Cube::outputDetectionList()
{
  /** 
   *  A front-end to writing the full list of detected objects to a results 
   *   file and to cout.
   *  Uses outputDetectionTextWCS for each objects.
   *  Leaves the testing of whether the WCS parameters for each object 
   *   have been calculated to outputDetectionTextWCS.
   */

  int count=0;
  int flag;
  std::ofstream output(this->par.getOutFile().c_str());
  output<<"Results of the Duchamp source finder: ";
  time_t now = time(NULL);
  output << asctime( localtime(&now) );
  this->showParam(output);
  output<<"--------------------\n";
  output<<"Detection threshold = " << this->Stats.getThreshold()
	<<" " << this->head.getFluxUnits();
  if(this->par.getFlagFDR())
    output<<" (or S/N=" << this->Stats.getThresholdSNR()<<")";
  output<<endl;
  output<<"  Noise level = " << this->Stats.getMiddle()
	<<", Noise spread = " << this->Stats.getSpread() << endl;
  output<<"Total number of detections = "<<this->objectList.size()<<endl;
  output<<"--------------------\n";
  this->setupColumns();
  this->objectList[0].outputDetectionTextHeader(output,this->fullCols);
  this->objectList[0].outputDetectionTextHeader(std::cout,this->fullCols);
  for(int i=0;i<this->objectList.size();i++){
    this->objectList[i].outputDetectionTextWCS(output,this->fullCols);
    this->objectList[i].outputDetectionTextWCS(std::cout,this->fullCols);
  }
}

void Cube::logDetectionList()
{
  /**
   * logDetectionList
   *  A front-end to writing a list of detected objects to the log file.
   *  Does not assume WCS, so uses outputDetectionText.
   *  Designed to be used by searching routines before returning their final list.
   */

  std::ofstream fout(this->par.getLogFile().c_str(),std::ios::app);
  this->setupColumns();
  this->objectList[0].outputDetectionTextHeader(fout,this->logCols);
  long pos;
  bool baselineFlag = this->par.getFlagBaseline();
  for(int objCtr=0;objCtr<this->objectList.size();objCtr++){
    Detection *obj = new Detection;
    *obj = objectList[objCtr];
    if(this->par.getFlagCubeTrimmed()){
      for(int pix=0;pix<obj->getSize();pix++){
	// Need to correct the pixels first, as this hasn't been done yet.
	// Corrections needed for positions (to account for trimmed region)
	//  and for the baseline removal, if it has happened.
	// Don't want to keep these changes, so just do it on a dummy variable.
	pos = obj->getX(pix) + obj->getY(pix)*this->axisDim[0]
	  + obj->getZ(pix)*this->axisDim[0]*this->axisDim[1];
	obj->setX(pix, obj->getX(pix) + this->par.getBorderLeft() );
	obj->setY(pix, obj->getY(pix) + this->par.getBorderBottom() );
	if(baselineFlag) obj->setF(pix, obj->getF(pix) + this->baseline[pos]);   // add in baseline
      }
      obj->calcParams();
    }
    obj->outputDetectionText(fout,this->logCols,objCtr+1);
    delete obj;
  }
  fout.close();
}

void Cube::logDetection(Detection obj, int counter)
{
  /**
   *  A front-end to writing a detected object to the log file.
   *  Does not assume WCS, so uses outputDetectionText.
   *  Corrects for changes to positions of pixels and removal of baselines.
   *  Designed to be used by searching routines before returning their final 
   *   list.
   *  \param obj Detection object to be written : passed by value, as we want 
   *    to potentially change positions etc, but not for the object in the 
   *    calling function.
   *  \param counter The number to assign to the object : ideally its number
   *    in a list of some kind.
   */

  std::ofstream fout(this->par.getLogFile().c_str(),std::ios::app);
  bool baselineFlag = this->par.getFlagBaseline();
  if(this->par.getFlagCubeTrimmed()){
    for(int pix=0;pix<obj.getSize();pix++){
      // Need to correct the pixels first, as this hasn't been done yet.
      // Corrections needed for positions (to account for trimmed region)
      //  and for the baseline removal, if it has happened.
      // Don't want to keep these changes, so just do it on a dummy variable.
      long pos = obj.getX(pix) + obj.getY(pix)*this->axisDim[0]
	+ obj.getZ(pix)*this->axisDim[0]*this->axisDim[1];
      obj.setX(pix, obj.getX(pix) + this->par.getBorderLeft() );
      obj.setY(pix, obj.getY(pix) + this->par.getBorderBottom() );
      if(baselineFlag) obj.setF(pix, obj.getF(pix) + this->baseline[pos]);   // add in baseline
    }
    obj.calcParams();
  }
  obj.outputDetectionText(fout,this->logCols,counter);
  fout.close();
}
