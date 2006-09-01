#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <time.h>
#include <Cubes/cubes.hh> 
#include <Detection/columns.hh>
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

  // Set up Column definitions here
  vector<Column::Col> localCol;
  string posUCD[4];
  localCol.push_back(this->fullColSet.vec[0]);  // objID
  localCol.push_back(this->fullColSet.vec[1]);  // name
  localCol.push_back(this->fullColSet.vec[5]);  // ra
  if(makelower(localCol[2].getName())=="ra"){
    posUCD[0] = "pos.eq.ra;meta.main";
    posUCD[2] = "phys.angSize;pos.eq.ra";
  }
  else{
    posUCD[0] = "pos.galactic.lat;meta.main";
    posUCD[2] = "phys.angSize;pos.galactic.lat";
  }
  localCol.push_back(this->fullColSet.vec[6]);  // dec
  if(makelower(localCol[2].getName())=="dec"){
    posUCD[1] = "pos.eq.dec;meta.main";
    posUCD[3] = "phys.angSize;pos.eq.dec";
  }
  else{
    posUCD[1] = "pos.galactic.lon;meta.main";
    posUCD[3] = "phys.angSize;pos.galactic.lon";
  }
  localCol.push_back(this->fullColSet.vec[8]);  // w_ra
  localCol.push_back(this->fullColSet.vec[9]);  // w_dec
  localCol.push_back(this->fullColSet.vec[7]);  // vel
  localCol.push_back(this->fullColSet.vec[10]); // w_vel
  localCol.push_back(this->fullColSet.vec[11]); // f_int
  localCol.push_back(this->fullColSet.vec[12]); // f_peak
  localCol.push_back(this->fullColSet.vec[20]); // flag

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
    stream<<"      <PARAM name=\"ATrous Dimension\" datatype=\"int\" ucd=\"meta.code;stat\" value=\"" << this->pars().getReconDim() << "\">" << endl;
    stream<<"      <PARAM name=\"ATrous Threshold\" datatype=\"float\" ucd=\"stat.snr\" value=\"" << this->pars().getAtrousCut() << "\">" << endl;
    stream<<"      <PARAM name=\"ATrous Minimum Scale\" datatype=\"int\" ucd=\"stat.param\" value=\"" << this->pars().getMinScale() << "\">" << endl;
    stream<<"      <PARAM name=\"ATrous Filter\" datatype=\"char\" ucd=\"meta.code;stat\" value=\"" << this->pars().getFilterName() << "\">" << endl;
  }
  // FIELD section -- names, titles and info for each column.
  stream<<"      <FIELD name=\"ID\" ID=\"col01\" ucd=\"meta.id\" datatype=\"int\" width=\""<<localCol[0].getWidth()<<"\"/>"<<endl;
  stream<<"      <FIELD name=\"Name\" ID=\"col02\" ucd=\"meta.id;meta.main\" datatype=\"char\" arraysize=\""<<localCol[1].getWidth()<<"\"/>"<<endl;
  stream<<"      <FIELD name=\""<<localCol[2].getName()<<"\" ID=\"col03\" ucd=\""<<posUCD[0]<<"\" ref=\"J2000\" datatype=\"float\" width=\""<<localCol[2].getWidth()<<"\" precision=\""<<Column::posPrec<<"\" unit=\"[deg]\"/>"<<endl;
  stream<<"      <FIELD name=\""<<localCol[3].getName()<<"\" ID=\"col04\" ucd=\""<<posUCD[1]<<"\" ref=\"J2000\" datatype=\"float\" width=\""<<localCol[3].getWidth()<<"\" precision=\""<<Column::posPrec<<"\" unit=\"[deg]\"/>"<<endl;
  stream<<"      <FIELD name=\""<<localCol[4].getName()<<"\" ID=\"col05\" ucd=\""<<posUCD[2]<<"\" ref=\"J2000\" datatype=\"float\" width=\""<<localCol[4].getWidth()<<"\" precision=\""<<Column::wposPrec<<"\" unit=\""<<localCol[4].getUnit()<<"\"/>"<<endl;
  stream<<"      <FIELD name=\""<<localCol[5].getName()<<"\" ID=\"col06\" ucd=\""<<posUCD[2]<<"\" ref=\"J2000\" datatype=\"float\" width=\""<<localCol[5].getWidth()<<"\" precision=\""<<Column::wposPrec<<"\" unit=\""<<localCol[5].getUnit()<<"\"/>"<<endl;
  stream<<"      <FIELD name=\"Vel\" ID=\"col07\" ucd=\"phys.veloc;src.dopplerVeloc\" datatype=\"float\" width=\""<<localCol[6].getWidth()<<"\" precision=\""<<Column::velPrec<<"\" unit=\""<<localCol[6].getUnit()<<"\"/>"<<endl;
  stream<<"      <FIELD name=\"w_Vel\" ID=\"col08\" ucd=\"phys.veloc;src.dopplerVeloc;spect.line.width\" datatype=\"float\" width=\""<<localCol[7].getWidth()<<"\" precision=\""<<Column::velPrec<<"\" unit=\""<<localCol[7].getUnit()<<"\"/>"<<endl;
  stream<<"      <FIELD name=\"Integrated_Flux\" ID=\"col09\" ucd=\"phot.flux;spect.line.intensity\" datatype=\"float\" width=\""<<localCol[8].getWidth()<<"\" precision=\""<<Column::fluxPrec<<"\" unit=\""<<localCol[8].getUnit()<<"\"/>"<<endl;
  stream<<"      <FIELD name=\"Peak_Flux\" ID=\"col10\" ucd=\"phot.flux;spect.line.intensity\" datatype=\"float\" width=\""<<localCol[9].getWidth()<<"\" precision=\""<<Column::fluxPrec<<"\" unit=\""<<localCol[9].getUnit()<<"\"/>"<<endl;
  stream<<"      <FIELD name=\"Flag\" ID=\"col11\" ucd=\"meta.code.qual\" datatype=\"char\" arraysize=\"3\"/>"<<endl;


  stream<<"      <DATA>"<<endl
	<<"        <TABLEDATA>"<<endl;

  stream.setf(std::ios::fixed);  
  for(int i=0;i<this->objectList.size();i++){
    if(this->objectList[i].isWCS()){
      stream<<"        <TR>"<<endl;
      stream<<"          <TD>"<<setw(localCol[0].getWidth())<<i+1<<"</TD>";
      stream<<"<TD>" << setw(localCol[1].getWidth()) << this->objectList[i].getName()       <<"</TD>";
      stream<<setprecision(Column::posPrec);					    
      stream<<"<TD>" << setw(localCol[2].getWidth()) << this->objectList[i].getRA()         <<"</TD>";
      stream<<"<TD>" << setw(localCol[3].getWidth()) << this->objectList[i].getDec()        <<"</TD>";
      stream<<setprecision(Column::wposPrec);					    
      stream<<"<TD>" << setw(localCol[4].getWidth()) << this->objectList[i].getRAWidth()    <<"</TD>";
      stream<<"<TD>" << setw(localCol[5].getWidth()) << this->objectList[i].getDecWidth()   <<"</TD>";
      stream<<setprecision(Column::velPrec);					    
      stream<<"<TD>" << setw(localCol[6].getWidth()) << this->objectList[i].getVel()        <<"</TD>";
      stream<<"<TD>" << setw(localCol[7].getWidth()) << this->objectList[i].getVelWidth()   <<"</TD>";
      stream<<setprecision(Column::fluxPrec);
      stream<<"<TD>" << setw(localCol[8].getWidth()) << this->objectList[i].getIntegFlux()  <<"</TD>";
      stream<<"<TD>" << setw(localCol[9].getWidth()) << this->objectList[i].getPeakFlux()   <<"</TD>";
      stream<<"<TD>" << setw(localCol[10].getWidth())<< this->objectList[i].getFlagText()   <<"</TD>";
      
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
  output<<"Total number of detections = "<<this->objectList.size()<<endl;
  output<<"--------------------"<<endl;
  this->setupColumns();
  outputDetectionTextHeader(output,this->fullColSet);
  outputDetectionTextHeader(std::cout,this->fullColSet);
  for(int i=0;i<this->objectList.size();i++){
    this->objectList[i].outputDetectionTextWCS(output,this->fullColSet);
    this->objectList[i].outputDetectionTextWCS(std::cout,this->fullColSet);
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
  outputDetectionTextHeader(fout,this->logColSet);
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
    obj->outputDetectionText(fout,this->logColSet,objCtr+1);
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
  obj.outputDetectionText(fout,this->logColSet,counter);
  fout.close();
}
