#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <time.h>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>

using std::endl;
using std::setw;
using std::setprecision;

void Cube::outputDetectionsKarma(std::ostream &stream)
{
  /**
   * outputDetectionsKarma
   *  Prints to a stream (provided) the list of detected objects in the cube
   *   in the format of an annotation file for the Karma suite of programs.
   *  Annotation file draws a box enclosing the detection, and writes the ID number
   *   of the detection to the right of the box.
   */

  stream << "# Duchamp Source Finder results for cube " << this->par.getImageFile() << endl;
  stream << "COLOR RED" << endl;
  stream << "COORD W" << endl;
  for(int i=0;i<this->objectList.size();i++){
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

}

void Cube::outputDetectionsVOTable(std::ostream &stream)
{
  /**
   * outputDetectionsVOTable
   *  Prints to a stream (provided) the list of detected objects in the cube
   *   in a VOTable format.
   *  Uses WCS information and assumes WCS parameters have been calculated for each
   *   detected object.
   *  If they have not (given by the isWCS() function), then those objects are not written...
   */

  stream<<"<?xml version=\"1.0\"?>"<<endl;
  stream<<"<VOTABLE version=\"1.1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""<<endl;
  stream<<" xsi:noNamespaceSchemaLocation=\"http://www.ivoa.net/xml/VOTable/VOTable/v1.1\">"<<endl;

  stream<<"  <COOSYS ID=\"J2000\" equinox=\"J2000.\" epoch=\"J2000.\" system=\"eq_FK5\"/>"<<endl;
  stream<<"  <RESOURCE name=\"Duchamp Output\">"<<endl;
  stream<<"    <TABLE name=\"Detections\">"<<endl;
  stream<<"      <DESCRIPTION>Detected sources and parameters from running the Duchamp source finder.</DESCRIPTION>"<<endl;

  // PARAM section -- parts that are not entry-specific ie. apply to whole dataset
  string fname = this->pars().getImageFile();
  if(this->pars().getFlagSubsection()) fname+=this->pars().getSubsection();
  stream<<"      <PARAM name=\"FITS file\" datatype=\"char\" ucd=\"meta.file;meta.fits\" value=\"" << fname << "\"/>"<<endl;
  if(this->pars().getFlagFDR())
    stream<<"      <PARAM name=\"FDR Significance\" datatype=\"float\" ucd=\"stat.param\" value=\"" << this->pars().getAlpha() << "\">" << endl;
  else
    stream<<"      <PARAM name=\"Threshold\" datatype=\"float\" ucd=\"stat.snr\" value=\"" << this->pars().getCut() << "\">" << endl;
  if(this->pars().getFlagATrous()){
    stream<<"      <PARAM name=\"ATrous note\" datatype=\"char\" ucd=\"meta.note\" value=\"The a trous reconstruction method was used, with the following parameters.\">" << endl;
    stream<<"      <PARAM name=\"ATrous Cut\" datatype=\"float\" ucd=\"stat.snr\" value=\"" << this->pars().getAtrousCut() << "\">" << endl;
    stream<<"      <PARAM name=\"ATrous Minimum Scale\" datatype=\"int\" ucd=\"stat.param\" value=\"" << this->pars().getMinScale() << "\">" << endl;
    stream<<"      <PARAM name=\"ATrous Filter\" datatype=\"char\" ucd=\"meta.code;stat\" value=\"" << this->pars().getFilterName() << "\">" << endl;
  }
  // FIELD section -- names, titles and info for each column.
  stream<<"      <FIELD name=\"ID\" ID=\"col1\" ucd=\"meta.id\" datatype=\"int\" width=\"4\"/>"<<endl;
  stream<<"      <FIELD name=\"Name\" ID=\"col2\" ucd=\"meta.id;meta.main\" datatype=\"char\" arraysize=\"14\"/>"<<endl;
  stream<<"      <FIELD name=\"RA\" ID=\"col3\" ucd=\"pos.eq.ra;meta.main\" ref=\"J2000\" datatype=\"float\" width=\"10\" precision=\"6\" unit=\"deg\"/>"<<endl;
  stream<<"      <FIELD name=\"Dec\" ID=\"col4\" ucd=\"pos.eq.dec;meta.main\" ref=\"J2000\" datatype=\"float\" width=\"10\" precision=\"6\" unit=\"deg\"/>"<<endl;
  stream<<"      <FIELD name=\"w_RA\" ID=\"col3\" ucd=\"phys.angSize;pos.eq.ra\" ref=\"J2000\" datatype=\"float\" width=\"7\" precision=\"2\" unit=\"arcmin\"/>"<<endl;
  stream<<"      <FIELD name=\"w_Dec\" ID=\"col4\" ucd=\"phys.angSize;pos.eq.dec\" ref=\"J2000\" datatype=\"float\" width=\"7\" precision=\"2\" unit=\"arcmin\"/>"<<endl;
  stream<<"      <FIELD name=\"Vel\" ID=\"col4\" ucd=\"phys.veloc;src.dopplerVeloc\" datatype=\"float\" width=\"9\" precision=\"3\" unit=\"km/s\"/>"<<endl;
  stream<<"      <FIELD name=\"w_Vel\" ID=\"col4\" ucd=\"phys.veloc;src.dopplerVeloc;spect.line.width\" datatype=\"float\" width=\"8\" precision=\"3\" unit=\"km/s\"/>"<<endl;
  stream<<"      <FIELD name=\"Integrated_Flux\" ID=\"col4\" ucd=\"phys.flux;spect.line.intensity\" datatype=\"float\" width=\"10\" precision=\"4\" unit=\"km/s\"/>"<<endl;


  stream<<"      <DATA>"<<endl
	<<"        <TABLEDATA>"<<endl;

  stream.setf(std::ios::fixed);  
  for(int i=0;i<this->objectList.size();i++){
    if(this->objectList[i].isWCS()){
      stream<<"        <TR>"<<endl;
      stream<<"          <TD>"<<setw(4)<<i+1<<"</TD>";
      stream<<"<TD>" << setw(14) << this->objectList[i].getName()       <<"</TD>";
      stream<<setprecision(6);					    
      stream<<"<TD>" << setw(10) << this->objectList[i].getRA()         <<"</TD>";
      stream<<"<TD>" << setw(10) << this->objectList[i].getDec()        <<"</TD>";
      stream<<setprecision(2);					    
      stream<<"<TD>" << setw(7)  << this->objectList[i].getRAWidth()    <<"</TD>";
      stream<<"<TD>" << setw(7)  << this->objectList[i].getDecWidth()   <<"</TD>";
      stream<<setprecision(3);					    
      stream<<"<TD>" << setw(9)  << this->objectList[i].getVel()        <<"</TD>";
      stream<<"<TD>" << setw(8)  << this->objectList[i].getVelWidth()   <<"</TD>";
      stream<<setprecision(3);
      stream<<"<TD>" << setw(10) << this->objectList[i].getIntegFlux()  <<"</TD>";
      
      stream<<endl;
      stream<<"        </TR>"<<endl;
    }
  }
  stream<<"        </TABLEDATA>"<<endl;
  stream<<"      </DATA>"<<endl;
  stream<<"    </TABLE>"<<endl;
  stream<<"  </RESOURCE>"<<endl;
  stream<<"</VOTABLE>"<<endl;
  resetiosflags(std::ios::fixed);
  
}


void Cube::outputDetectionList()
{
  /** 
   * outputDetectionList
   *  A front-end to writing the full list of detected objects to a results file and to cout.
   *  Uses outputDetectionTextWCS for each objects.
   *  Leaves the testing of whether the WCS parameters for each object have been calculated
   *   to outputDetectionTextWCS.
   */

  int count=0;
  int flag;
  std::ofstream output(this->par.getOutFile().c_str());
  output<<"Results of the Duchamp source finder: ";
  time_t now = time(NULL);
  output << asctime( localtime(&now) );
  this->showParam(output);
  output<<"Total number of detections = "<<this->objectList.size()<<endl;
  output<<"--------------------"<<endl;
  if(flag=wcsset(wcs)){
    std::cerr<<"outputDetectionList():: WCSSET failed! Code="<<flag <<": "<<wcs_errmsg[flag]<<endl;
  }
  if(this->flagWCS){
    outputDetectionTextWCSHeader(output,this->wcs);
    outputDetectionTextWCSHeader(std::cout,this->wcs);
  }
  else{
    outputDetectionTextHeader(output);
    outputDetectionTextHeader(std::cout);
  }
  for(int i=0;i<this->objectList.size();i++){
    if(this->flagWCS){
      this->objectList[i].outputDetectionTextWCS(output);
      this->objectList[i].outputDetectionTextWCS(std::cout);
    }
    else{
      this->objectList[i].outputDetectionText(output,i+1);
      this->objectList[i].outputDetectionText(std::cout,i+1);
    }
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
  outputDetectionTextHeader(fout);
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
    obj->outputDetectionText(fout,objCtr);
    delete obj;
  }
  fout.close();
}

void Cube::logDetection(Detection obj, int counter)
{
  /**
   * logDetection
   *  A front-end to writing a detected object to the log file.
   *  Does not assume WCS, so uses outputDetectionText.
   *  obj is passed by value, as we want to potentially change positions etc, but not for 
   *  object in the calling function.
   *  Corrects for changes to positions of pixels and removal of baselines.
   *  Designed to be used by searching routines before returning their final list.
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
  obj.outputDetectionText(fout,counter);
  fout.close();
}
