// -----------------------------------------------------------------------
// detectionIO.cc: Screen and File output of the detected objects.
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
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Cubes/cubes.hh> 
#include <duchamp/PixelMap/Object3D.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Detection/columns.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/Statistics.hh>
#include <duchamp/Outputs/ASCIICatalogueWriter.hh>
 
using std::endl;
using std::setw;
using std::setprecision;
using namespace PixelInfo;
using namespace Statistics;

namespace duchamp
{

  void Cube::outputDetectionsKarma(std::ostream &stream)
  {
    /// @details
    ///  Prints to a stream (provided) the list of detected objects in the cube
    ///   in the format of an annotation file for the Karma suite of programs.
    ///  Annotation file draws a box enclosing the detection, and writes the 
    ///   ID number of the detection to the right of the box.

    std::string fname = this->par.getImageFile();
    if(this->par.getFlagSubsection()) fname+=this->par.getSubsection();
    stream << "# Duchamp Source Finder v."<< VERSION << endl;
    stream << "# Results for FITS file: " << fname << endl;
    if(this->par.getFlagFDR())
      stream<<"# FDR Significance = " << this->par.getAlpha() << endl;
    else
      stream<<"# Threshold = " << this->par.getCut() << endl;
    if(this->par.getFlagATrous()){
      stream<<"# The a trous reconstruction method was used, with the following parameters." << endl;
      stream<<"#  Dimension = " << this->par.getReconDim() << endl;
      stream<<"#  Threshold = " << this->par.getAtrousCut() << endl;
      stream<<"#  Minimum Scale =" << this->par.getMinScale() << endl;
      stream<<"#  Filter = " << this->par.getFilterName() << endl;
    }
    else if(this->par.getFlagSmooth()){
      stream<<"# The data was smoothed prior to searching, with the following parameters." << endl;
      stream<<"#  Smoothing type = " << this->par.getSmoothType() << endl;
      if(this->par.getSmoothType()=="spectral"){
	stream << "#  Hanning width = " << this->par.getHanningWidth() << endl;
      }
      else{
	stream << "#  Kernel Major axis = " << this->par.getKernMaj() << endl;
	if(this->par.getKernMin()>0) 
	  stream << "#  Kernel Minor axis = " << this->par.getKernMin() << endl;
	else
	  stream << "#  Kernel Minor axis = " << this->par.getKernMaj() << endl;
	stream << "#  Kernel Major axis = " << this->par.getKernPA() << endl;
      }
    }
    stream << "#\n";
    stream << "COLOR RED" << endl;
    if(this->head.isWCS()) stream << "COORD W" << endl;
    else stream << "COORD P" << endl;
    stream << std::setprecision(6);
    stream.setf(std::ios::fixed);
    double *pix = new double[3];
    double *wld = new double[3];
    std::vector<Detection>::iterator obj;
    for(obj=this->objectList->begin();obj<this->objectList->end();obj++){

      if(this->par.getAnnotationType()=="borders"){
	std::vector<int> vertexSet = obj->getVertexSet();
	for(size_t i=0;i<vertexSet.size()/4;i++){
	  pix[0] = vertexSet[i*4]-0.5;
	  pix[1] = vertexSet[i*4+1]-0.5;
	  pix[2] = obj->getZcentre();
	  if(this->head.isWCS()){
	    this->head.pixToWCS(pix,wld);
	    stream << "LINE " << wld[0] << " " << wld[1];
	  }
	  else
	    stream << "LINE " << pix[0] << " " << pix[1];
	  pix[0] = vertexSet[i*4+2]-0.5;
	  pix[1] = vertexSet[i*4+3]-0.5;
	  if(this->head.isWCS()){
	    this->head.pixToWCS(pix,wld);
	    stream << " " << wld[0] << " " << wld[1] << "\n";
	  }
	  else
	    stream << " " << pix[0] << " " << pix[1] << "\n";
	}
      }
      else if(this->par.getAnnotationType()=="circles"){
        float radius = obj->getRAWidth()/120.;
        if(obj->getDecWidth()/120.>radius)
          radius = obj->getDecWidth()/120.;
        stream << "CIRCLE " 
               << obj->getRA() << " " 
               << obj->getDec() << " " 
               << radius << "\n";
      }

      stream << "TEXT " 
	     << obj->getRA() << " " 
	     << obj->getDec() << " " 
	     << obj->getID() << "\n\n";
    }

    delete [] pix;
    delete [] wld;

  }

  void Cube::outputCatalogue()
  {
    this->setupColumns();

    ASCIICatalogueWriter catWriter(this->par.getOutFile(),Column::FILE);
    catWriter.setup(this);
    ASCIICatalogueWriter headerWriter(this->par.getHeaderFile(),Column::FILE);
    headerWriter.setup(this);
    ASCIICatalogueWriter screenWriter(Column::SCREEN);
    screenWriter.setup(this);
    ASCIICatalogueWriter *writer;

    // write the header information
    if(this->par.getFlagSeparateHeader()) writer = &headerWriter;
    else writer=&catWriter;

    writer->openCatalogue();
    writer->writeHeader();
    writer->writeParameters();
    writer->writeStats();

    if(this->par.getFlagSeparateHeader()){
      writer->closeCatalogue();
      writer = &catWriter;
    }

    // write the catalogue
    writer->writeTableHeader();
    writer->writeEntries();
    writer->closeCatalogue();

    screenWriter.openCatalogue();
    screenWriter.writeTableHeader();
    screenWriter.writeEntries();
    screenWriter.closeCatalogue();

  }

  void Cube::prepareOutputFile()
  {
    ///  @details
    ///  A function to write the paramters, time of execution, and
    ///  statistical information to the output file.

    std::string outfile;
    if(this->par.getFlagSeparateHeader()) outfile = this->par.getHeaderFile();
    else outfile = this->par.getOutFile();
    std::ofstream output(outfile.c_str());
    output<<"Results of the Duchamp source finder v."<<VERSION<<": ";
    time_t now = time(NULL);
    output << asctime( localtime(&now) );
    this->showParam(output);
    output<<"--------------------\n";
    output.close();
    this->outputStats();
  
  }

  void Cube::outputStats()
  {
    ///  @details
    ///  A function to write the statistical information to the output
    ///  file. This writes the threshold, its equivalent S/N ratio, and
    ///  the noise level and spread.
    /// 
    ///  If smoothing has been done, the noise level & spread for the
    ///  original array are calculated and printed as well.

    std::string outfile;
    if(this->par.getFlagSeparateHeader()) outfile = this->par.getHeaderFile();
    else outfile = this->par.getOutFile();
    std::ofstream output(outfile.c_str(),std::ios::app);
    output<<"Summary of statistics:\n";
    output<<"Detection threshold = " << this->Stats.getThreshold()
	  <<" " << this->head.getFluxUnits();
    if(this->par.getFlagFDR())
      output<<" (or S/N=" << this->Stats.getThresholdSNR()<<")";
    if(this->par.getFlagSmooth()){
      output << " in smoothed cube.";
      if(!this->par.getFlagUserThreshold())
	output<<"\nNoise level = " << this->Stats.getMiddle()
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
      
      output << "\nNoise properties for the original cube are:";
    }
     
    if(!this->par.getFlagUserThreshold())
      output<<"\nNoise level = " << this->Stats.getMiddle()
	    <<", Noise spread = " << this->Stats.getSpread();

    if(this->par.getFlagGrowth()){
      StatsContainer<float> growthStats = this->Stats;
      if(this->par.getFlagUserGrowthThreshold())
	growthStats.setThreshold(this->par.getGrowthThreshold());
      else
	growthStats.setThresholdSNR(this->par.getGrowthCut());
      growthStats.setUseFDR(false);
      output<<"\nDetections grown down to threshold of " 
	    << growthStats.getThreshold() << " " 
	    << this->head.getFluxUnits();
    }

    if(!this->par.getFlagUserThreshold())
      output << "\nFull stats:\n" << this->Stats;
    else
      output << "\n\nNot calculating full stats since threshold was provided directly.\n";

    output<<"--------------------\n";
    output.close();
  }

  void Cube::outputDetectionList()
  {
    ///  @details
    ///  A front-end to writing the full list of detected objects to a results 
    ///   file and to cout.
    ///  Leaves the testing of whether the WCS parameters for each object 
    ///   have been calculated to the printing function.

    std::string outfile;
    if(this->par.getFlagSeparateHeader()) outfile = this->par.getHeaderFile();
    else outfile = this->par.getOutFile();
    std::ofstream output(outfile.c_str(),std::ios::app);
    output<<"Total number of detections = "<<this->objectList->size()<<endl;
    output<<"--------------------\n";
    output.close();

    if(this->par.getFlagSeparateHeader()) 
      output.open(this->par.getOutFile().c_str());
    else 
      output.open(this->par.getOutFile().c_str(),std::ios::app);

    if(this->objectList->size()>0){
      this->setupColumns();
      outputTableHeader(output,this->fullCols,Column::FILE,this->head.isWCS());
      outputTableHeader(std::cout,this->fullCols,Column::SCREEN,this->head.isWCS());
      std::vector<Detection>::iterator obj;
      for(obj=this->objectList->begin();obj<this->objectList->end();obj++){
	obj->printTableRow(output,this->fullCols,Column::FILE);
	obj->printTableRow(std::cout,this->fullCols,Column::SCREEN);
      }
    }

    output.close();
  }

  void Cube::prepareLogFile(int argc, char *argv[])
  {
    /// @details
    ///  Opens the log file so that it can be written to, and writes
    ///  the parameter summary as well as the time of execution to the
    ///  file.
    /// 
    ///  It also writes the command-line statement, hence the need for
    ///  argv and argc.

    // // Open the logfile and write the time on the first line
    // std::ofstream logfile(this->par.getLogFile().c_str());
    // logfile << "New run of the Duchamp source finder v."<<VERSION<<": ";
    // time_t now = time(NULL);
    // logfile << asctime( localtime(&now) );
    // // Write out the command-line statement
    // logfile << "Executing statement : ";
    // for(int i=0;i<argc;i++) logfile << argv[i] << " ";
    // logfile << std::endl;
    // logfile << this->par;
    // logfile.close();

    ASCIICatalogueWriter logwriter(this->par.getLogFile(),Column::LOG);
    logwriter.setup(this);
    logwriter.openCatalogue();
    logwriter.writeHeader();
    logwriter.writeCommandLineEntry(argc,argv);
    logwriter.writeParameters();
    logwriter.closeCatalogue();
  }


  void Cube::logDetectionList(bool calcFluxes)
  {
    /// @details
    ///  A front-end to writing a list of detected objects to the log file.
    ///  Does not assume WCS is present.
    ///  Designed to be used by searching routines before returning their 
    ///   final list.
    ///  @param[in] calcFluxes If true (the default), calculate the various flux parameters for each object.

    if(this->objectList->size()>0){

      ASCIICatalogueWriter logwriter(this->par.getLogFile(),Column::LOG);
      logwriter.setup(this);
      logwriter.openCatalogue(std::ios::app);

      long left = this->par.getBorderLeft();
      long bottom = this->par.getBorderBottom();

      // std::ofstream fout(this->par.getLogFile().c_str(),std::ios::app);
      if(calcFluxes) this->calcObjectFluxes();
      this->setupColumns();
      // outputTableHeader(fout,this->fullCols,Column::LOG,this->head.isWCS());
      logwriter.writeTableHeader();

      if(this->par.getFlagBaseline()){
	for(size_t i=0;i<this->axisDim[0]*this->axisDim[1]*this->axisDim[2];i++)
	  this->array[i] += this->baseline[i];
      }

      for(size_t objCtr=0;objCtr<this->objectList->size();objCtr++){
	Detection obj = this->objectList->at(objCtr);
	obj.setOffsets(par);
	if(this->par.getFlagCubeTrimmed()){
	  obj.addOffsets(left,bottom,0);
	}
	if(calcFluxes) obj.calcFluxes(this->array, this->axisDim);
	obj.setID(objCtr+1);
	//	obj.printTableRow(fout,this->fullCols,Column::LOG);
	logwriter.writeEntry(&obj);
      }

      if(this->par.getFlagBaseline()){
	for(size_t i=0;i<this->axisDim[0]*this->axisDim[1]*this->axisDim[2];i++)
	  this->array[i] -= this->baseline[i];
      }
      //      fout.close();
      logwriter.closeCatalogue();
 
    }

  }

  void Cube::logDetection(Detection obj, int counter)
  {
    /// @details
    ///  A front-end to writing a detected object to the log file.
    ///  Does not assume WCS is present.
    ///  Corrects for changes to positions of pixels and removal of baselines.
    ///  Designed to be used by searching routines before returning their final 
    ///   list.
    ///  \param obj Detection object to be written : passed by value, as we want 
    ///    to potentially change positions etc, but not for the object in the 
    ///    calling function.
    ///  \param counter The number to assign to the object : ideally its number
    ///    in a list of some kind.

    std::ofstream fout(this->par.getLogFile().c_str(),std::ios::app);
    // Need to deal with possibility of trimmed array
    size_t left = this->par.getBorderLeft();
    size_t right = this->par.getBorderRight();
    size_t top = this->par.getBorderTop();
    size_t bottom = this->par.getBorderBottom();
    size_t *tempDim = new size_t[3];
    tempDim[0] = (this->axisDim[0] + left + right);
    tempDim[1] = (this->axisDim[1] + bottom + top);
    tempDim[2] = this->axisDim[2];
    size_t tempsize = tempDim[0] * tempDim[1] * tempDim[2];
    float *temparray = new float[tempsize];
    //  for(int i=0;i<this->numPixels;i++){ // loop over this->array
    for(size_t z=0;z<tempDim[2];z++){
      for(size_t y=0;y<tempDim[1];y++){
	for(size_t x=0;x<tempDim[0];x++){

	  bool isDud = (x<left) || (x>=this->axisDim[0]+left) || 
	    (y<bottom) || (y>=this->axisDim[1]+bottom);
	
	  size_t temppos = x + tempDim[0]*y + tempDim[1]*tempDim[0]*z;

	  size_t pos = (x-left) + (y-bottom)*this->axisDim[0] + 
	    z*this->axisDim[0]*this->axisDim[1];

	  if(isDud) temparray[temppos] = this->par.getBlankPixVal();
	  else temparray[temppos] = this->array[pos];
  
	  if(this->par.getFlagBaseline() && !isDud) 
	    temparray[temppos] += this->baseline[pos];

	}
      }
    }

    if(this->par.getFlagCubeTrimmed()){
      obj.addOffsets(left,bottom,0);
    }
    obj.calcFluxes(temparray, this->axisDim);
    obj.printTableRow(fout,this->fullCols,Column::LOG);
    delete [] temparray;
    delete [] tempDim;
    fout.close();
  }

  void Cube::logSummary()
  {
    ASCIICatalogueWriter logwriter(this->par.getLogFile(),Column::LOG);
    logwriter.setup(this);
    logwriter.openCatalogue(std::ios::app);
    logwriter.writeCubeSummary();
    logwriter.closeCatalogue();
  }

  void Cube::writeSpectralData()
  {
    /// @details
    ///  A function to write, in ascii form, the spectra of each
    ///  detected object to a file. The file consists of a column for
    ///  the spectral coordinates, and one column for each object
    ///  showing the flux at that spectral position. The units are the
    ///  same as those shown in the graphical output. The filename is
    ///  given by the Param::spectraTextFile parameter in the Cube::par
    ///  parameter set.

    const int zdim = this->axisDim[2];
    const int numObj = this->objectList->size();
    float *specxOut = new float[zdim];
    float *spectra = new float[numObj*zdim];
    
    for(int obj=0; obj<numObj; obj++){
      float *temp = new float[zdim];
      float *specx = new float[zdim];
      float *recon = new float[zdim];
      float *base = new float[zdim];
      this->getSpectralArrays(obj, specx, temp, recon, base);
      for(int z=0;z<zdim;z++) spectra[obj*zdim+z] = temp[z];
      if(obj==0) for(int z=0;z<zdim;z++) specxOut[z] = specx[z];
      delete [] specx;
      delete [] recon;
      delete [] base;
      delete [] temp;
    }
    
    std::ofstream fspec(this->par.getSpectraTextFile().c_str());
    fspec.setf(std::ios::fixed);

    for(int z=0;z<zdim;z++){
      
      fspec << std::setprecision(8);
      fspec << specxOut[z] << "  ";
      for(int obj=0;obj<numObj; obj++) {
	fspec << spectra[obj*zdim+z] << "  ";
      }
      fspec << "\n";

    }
    fspec.close();

    delete [] spectra;
    delete [] specxOut;

  }
  //--------------------------------------------------------------------


}
