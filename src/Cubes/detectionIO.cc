#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <time.h>
#include <param.hh>
#include <fitsHeader.hh>
#include <Cubes/cubes.hh> 
#include <PixelMap/Object3D.hh>
#include <Detection/detection.hh>
#include <Detection/columns.hh>
#include <Utils/utils.hh>
 
using std::endl;
using std::setw;
using std::setprecision;
using namespace Column;
using namespace PixelInfo;

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
  localCol.push_back(this->fullCols[NUM]);  // 0 = objID
  localCol.push_back(this->fullCols[NAME]);  // 1 = name
  localCol.push_back(this->fullCols[RA]);  // 2 = ra
  if(makelower(localCol[2].getName())=="ra"){
    posUCD[0] = "pos.eq.ra;meta.main";
    posUCD[2] = "phys.angSize;pos.eq.ra";
  }
  else{
    posUCD[0] = "pos.galactic.lat;meta.main";
    posUCD[2] = "phys.angSize;pos.galactic.lat";
  }
  localCol.push_back(this->fullCols[DEC]);  // 3 = dec
  if(makelower(localCol[2].getName())=="dec"){
    posUCD[1] = "pos.eq.dec;meta.main";
    posUCD[3] = "phys.angSize;pos.eq.dec";
  }
  else{
    posUCD[1] = "pos.galactic.lon;meta.main";
    posUCD[3] = "phys.angSize;pos.galactic.lon";
  }
  localCol.push_back(this->fullCols[WRA]);  // 4 = w_ra
  localCol.push_back(this->fullCols[WDEC]);  // 5 = w_dec
  localCol.push_back(this->fullCols[VEL]);  // 6 = vel
  localCol.push_back(this->fullCols[WVEL]); // 7 = w_vel
  if(this->head.getNumAxes()>2)
    localCol.push_back(this->fullCols[FINT]); // 8 = f_int
  else
    localCol.push_back(this->fullCols[FTOT]); // 8 = f_tot
  localCol.push_back(this->fullCols[FPEAK]); // 9 = f_peak
  localCol.push_back(this->fullCols[FLAG]); // 10 = flag
  localCol.push_back(this->fullCols[XCENT]); // 11 = x_cent
  localCol.push_back(this->fullCols[YCENT]); // 12 = y_cent
  localCol.push_back(this->fullCols[ZCENT]); // 13 = z_cent
  localCol.push_back(this->fullCols[XAV]); // 14 = x_av
  localCol.push_back(this->fullCols[YAV]); // 15 = y_av
  localCol.push_back(this->fullCols[ZAV]); // 16 = z_av
  localCol.push_back(this->fullCols[XPEAK]); // 17 = x_peak
  localCol.push_back(this->fullCols[YPEAK]); // 18 = y_peak
  localCol.push_back(this->fullCols[ZPEAK]); // 19 = z_peak

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
  if(this->par.getFlagSmooth()){
    if(this->par.getSmoothType()=="spectral"){
      std::string note = "The cube was smoothed spectrally with a Hanning filter, with the following parameters.";
      stream<<"      <PARAM name=\"Smoothing note\" datatype=\"char\" ucd=\"meta.note\" value=\""
	    <<note << "\" arraysize=\"" << note.size() << "\"/>\n";
      stream<<"      <PARAM name=\"Hanning filter width\" datatype=\"int\" ucd=\"meta.code;stat\" value=\"" 
	    << this->par.getHanningWidth() << "\"/>\n";
    }
    else if(this->par.getSmoothType()=="spatial"){
      std::string note = "The cube was smoothed spatially with a Gaussian kernel, with the following parameters.";
      stream<<"      <PARAM name=\"Smoothing note\" datatype=\"char\" ucd=\"meta.note\" value=\""
	    <<note << "\" arraysize=\"" << note.size() << "\"/>\n";
      stream<<"      <PARAM name=\"Gaussian kernel major-axis FWHM\" datatype=\"int\" ucd=\"meta.code;stat\" value=\"" 
	    << this->par.getKernMaj() << "\"/>\n";
      stream<<"      <PARAM name=\"Gaussian kernel minor-axis FWHM\" datatype=\"int\" ucd=\"meta.code;stat\" value=\"" 
	    << this->par.getKernMin() << "\"/>\n";
      stream<<"      <PARAM name=\"Gaussian kernel position angle\" datatype=\"int\" ucd=\"meta.code;stat\" value=\"" 
	    << this->par.getKernPA() << "\"/>\n";
    }    
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
  if(this->head.getNumAxes()>2)
    stream<<"      <FIELD name=\"Integrated_Flux\" ID=\"col09\" ucd=\"phot.flux;spect.line.intensity\""
	  <<" datatype=\"float\" width=\""<<localCol[8].getWidth()<<"\" precision=\""
	  <<prec[prFLUX]<<"\" unit=\""<<fixUnitsVOT(localCol[8].getUnits())<<"\"/>\n";
  else
    stream<<"      <FIELD name=\"Total_Flux\" ID=\"col09\" ucd=\"phot.flux;spect.line.intensity\""
	  <<" datatype=\"float\" width=\""<<localCol[8].getWidth()<<"\" precision=\""
	  <<prec[prFLUX]<<"\" unit=\""<<fixUnitsVOT(localCol[8].getUnits())<<"\"/>\n";
  stream<<"      <FIELD name=\"Peak_Flux\" ID=\"col10\" ucd=\"phot.flux;spect.line.intensity\""
	<<" datatype=\"float\" width=\""<<localCol[9].getWidth()<<"\" precision=\""
	<<prec[prFLUX]<<"\" unit=\""<<fixUnitsVOT(localCol[9].getUnits())<<"\"/>\n";
  stream<<"      <FIELD name=\"Flag\" ID=\"col11\" ucd=\"meta.code.qual\" datatype=\"char\" arraysize=\"3\" unit=\"--\"/>\n";
  stream<<"      <FIELD name=\"X_Centroid\" ID=\"col12\" ucd=\"\""
	<<" datatype=\"float\" width=\""<<localCol[11].getWidth()<<"\" precision=\""<<localCol[11].getPrecision()<<"\" unit=\"\"/>\n";
  stream<<"      <FIELD name=\"Y_Centroid\" ID=\"col13\" ucd=\"\""
	<<" datatype=\"float\" width=\""<<localCol[12].getWidth()<<"\" precision=\""<<localCol[12].getPrecision()<<"\" unit=\"\"/>\n";
  stream<<"      <FIELD name=\"Z_Centroid\" ID=\"col14\" ucd=\"\""
	<<" datatype=\"float\" width=\""<<localCol[13].getWidth()<<"\" precision=\""<<localCol[13].getPrecision()<<"\" unit=\"\"/>\n";
  stream<<"      <FIELD name=\"X_Av\" ID=\"col15\" ucd=\"\""
	<<" datatype=\"float\" width=\""<<localCol[14].getWidth()<<"\" precision=\""<<localCol[14].getPrecision()<<"\" unit=\"\"/>\n";
  stream<<"      <FIELD name=\"Y_Av\" ID=\"col16\" ucd=\"\""
	<<" datatype=\"float\" width=\""<<localCol[15].getWidth()<<"\" precision=\""<<localCol[15].getPrecision()<<"\" unit=\"\"/>\n";
  stream<<"      <FIELD name=\"Z_Av\" ID=\"col17\" ucd=\"\""
	<<" datatype=\"float\" width=\""<<localCol[16].getWidth()<<"\" precision=\""<<localCol[16].getPrecision()<<"\" unit=\"\"/>\n";
  stream<<"      <FIELD name=\"X_Peak\" ID=\"col18\" ucd=\"\""
	<<" datatype=\"int\" width=\""<<localCol[17].getWidth()<<"\" precision=\""<<localCol[17].getPrecision()<<"\" unit=\"\"/>\n";
  stream<<"      <FIELD name=\"Y_Peak\" ID=\"col19\" ucd=\"\""
	<<" datatype=\"int\" width=\""<<localCol[18].getWidth()<<"\" precision=\""<<localCol[18].getPrecision()<<"\" unit=\"\"/>\n";
  stream<<"      <FIELD name=\"Z_Peak\" ID=\"col20\" ucd=\"\""
	<<" datatype=\"int\" width=\""<<localCol[19].getWidth()<<"\" precision=\""<<localCol[19].getPrecision()<<"\" unit=\"\"/>\n";


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
      if(this->head.getNumAxes()>2)
	stream<<"<TD>" << setw(localCol[8].getWidth()) << this->objectList[i].getIntegFlux() <<"</TD>";
      else
	stream<<"<TD>" << setw(localCol[8].getWidth()) << this->objectList[i].getTotalFlux() <<"</TD>";
      stream<<"<TD>" << setw(localCol[9].getWidth()) << this->objectList[i].getPeakFlux()   <<"</TD>";
      stream<<"<TD>" << setw(localCol[10].getWidth())<< this->objectList[i].getFlagText()   <<"</TD>";
      stream<<"<TD>" << setw(localCol[11].getWidth())<< this->objectList[i].getXCentroid()  <<"</TD>";
      stream<<"<TD>" << setw(localCol[12].getWidth())<< this->objectList[i].getYCentroid()  <<"</TD>";
      stream<<"<TD>" << setw(localCol[13].getWidth())<< this->objectList[i].getZCentroid()  <<"</TD>";
      stream<<"<TD>" << setw(localCol[14].getWidth())<< this->objectList[i].getXAverage()   <<"</TD>";
      stream<<"<TD>" << setw(localCol[15].getWidth())<< this->objectList[i].getYAverage()   <<"</TD>";
      stream<<"<TD>" << setw(localCol[16].getWidth())<< this->objectList[i].getZAverage()   <<"</TD>";
      stream<<"<TD>" << setw(localCol[17].getWidth())<< this->objectList[i].getXPeak()      <<"</TD>";
      stream<<"<TD>" << setw(localCol[18].getWidth())<< this->objectList[i].getYPeak()      <<"</TD>";
      stream<<"<TD>" << setw(localCol[19].getWidth())<< this->objectList[i].getZPeak()      <<"</TD>";
      
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

void Cube::prepareOutputFile()
{
  /** 
   *  A function to write the paramters, time of execution, and
   *  statistical information to the output file.
   */

  std::ofstream output(this->par.getOutFile().c_str());
  output<<"Results of the Duchamp source finder: ";
  time_t now = time(NULL);
  output << asctime( localtime(&now) );
  this->showParam(output);
  output<<"--------------------\n";
  output.close();
  this->outputStats();
  output<<"--------------------\n";
  
}

void Cube::outputStats()
{
  /** 
   *  A function to write the statistical information to the output
   *  file. This writes the threshold, its equivalent S/N ratio, and
   *  the noise level and spread.
   *
   *  If smoothing has been done, the noise level & spread for the
   *  original array are calculated and printed as well.
   */

  std::ofstream output(this->par.getOutFile().c_str(),std::ios::app);
  output<<"Summary of statistics:\n";
  output<<"  Detection threshold = " << this->Stats.getThreshold()
	<<" " << this->head.getFluxUnits();
  if(this->par.getFlagFDR())
    output<<" (or S/N=" << this->Stats.getThresholdSNR()<<")";
  if(this->par.getFlagSmooth()){
    output << " in smoothed cube.";
    if(!this->par.getFlagUserThreshold())
      output<<"\n  Noise level = " << this->Stats.getMiddle()
	    <<", Noise spread = " << this->Stats.getSpread()
	    <<" in smoothed cube.";
    
    // calculate the stats for the original array, so that we can
    // quote S/N values correctly.
    this->par.setFlagSmooth(false);
    bool verb=this->par.isVerbose();
    bool fdrflag=this->par.getFlagFDR();
    this->par.setVerbosity(false);
    this->par.setFlagFDR(false);
    this->setCubeStats();
    this->par.setVerbosity(verb);
    this->par.setFlagFDR(fdrflag);
    this->par.setFlagSmooth(true);
      
    output << "\n  Noise properties for the original cube are:";
  }
     
  if(!this->par.getFlagUserThreshold())
    output<<"\n  Noise level = " << this->Stats.getMiddle()
	  <<", Noise spread = " << this->Stats.getSpread()
	  <<"\n";

  output.close();
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

  std::ofstream output(this->par.getOutFile().c_str(),std::ios::app);
  output<<"Total number of detections = "<<this->objectList.size()<<endl;
  output<<"--------------------\n";
  this->setupColumns();
  this->objectList[0].outputDetectionTextHeaderFull(output,this->fullCols);
  this->objectList[0].outputDetectionTextHeader(std::cout,this->fullCols);
  for(int i=0;i<this->objectList.size();i++){
    this->objectList[i].outputDetectionTextWCSFull(output,this->fullCols);
    this->objectList[i].outputDetectionTextWCS(std::cout,this->fullCols);
  }

  output.close();
}

void Cube::logDetectionList()
{
  /**
   *  A front-end to writing a list of detected objects to the log file.
   *  Does not assume WCS, so uses outputDetectionText.
   *  Designed to be used by searching routines before returning their 
   *   final list.
   */

  long left = this->par.getBorderLeft();
  long bottom = this->par.getBorderBottom();

  std::ofstream fout(this->par.getLogFile().c_str(),std::ios::app);
  this->setupColumns();
  this->objectList[0].outputDetectionTextHeader(fout,this->logCols);

  if(this->par.getFlagBaseline()){
    for(int i=0;i<this->axisDim[0]*this->axisDim[1]*this->axisDim[2];i++)
      this->array[i] += this->baseline[i];
  }

  for(int objCtr=0;objCtr<this->objectList.size();objCtr++){
    Detection *obj = new Detection;
    *obj = objectList[objCtr];
    obj->setOffsets(par);
    obj->calcFluxes(this->array, this->axisDim);
    if(this->par.getFlagCubeTrimmed()){
      obj->pixels().addOffsets(left,bottom,0);
    }
    obj->outputDetectionText(fout,this->logCols,objCtr+1);
    delete obj;
  }

  if(this->par.getFlagBaseline()){
    for(int i=0;i<this->axisDim[0]*this->axisDim[1]*this->axisDim[2];i++)
      this->array[i] -= this->baseline[i];
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
  // Need to deal with possibility of trimmed array
  long left = this->par.getBorderLeft();
  long right = this->par.getBorderRight();
  long top = this->par.getBorderTop();
  long bottom = this->par.getBorderBottom();
  long *tempDim = new long[3];
  tempDim[0] = (this->axisDim[0] + left + right);
  tempDim[1] = (this->axisDim[1] + bottom + top);
  tempDim[2] = this->axisDim[2];
  long tempsize = tempDim[0] * tempDim[1] * tempDim[2];
  float *temparray = new float[tempsize];
  //  for(int i=0;i<this->numPixels;i++){ // loop over this->array
  for(int z=0;z<tempDim[2];z++){
    for(int y=0;y<tempDim[1];y++){
      for(int x=0;x<tempDim[0];x++){

	bool isDud = (x<left) || (x>=this->axisDim[0]+left) || 
	  (y<bottom) || (y>=this->axisDim[1]+bottom);
	
	int temppos = x + tempDim[0]*y + tempDim[1]*tempDim[0]*z;

	int pos = (x-left) + (y-bottom)*this->axisDim[0] + 
	  z*this->axisDim[0]*this->axisDim[1];

	if(isDud) temparray[temppos] = this->par.getBlankPixVal();
	else temparray[temppos] = this->array[pos];
  
	if(this->par.getFlagBaseline() && !isDud) 
	  temparray[temppos] += this->baseline[pos];

      }
    }
  }

  if(this->par.getFlagCubeTrimmed()){
    obj.pixels().addOffsets(left,bottom,0);
  }
  obj.calcFluxes(temparray, this->axisDim);
  obj.outputDetectionText(fout,this->logCols,counter);
  delete [] temparray;
  delete [] tempDim;
  fout.close();
}
