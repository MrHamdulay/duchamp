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
#include <duchamp/Outputs/columns.hh>
#include <duchamp/Utils/utils.hh>
#include <duchamp/Utils/Statistics.hh>
#include <duchamp/Outputs/ASCIICatalogueWriter.hh>
#include <duchamp/Outputs/VOTableCatalogueWriter.hh>
#include <duchamp/Outputs/KarmaAnnotationWriter.hh>
#include <duchamp/Outputs/DS9AnnotationWriter.hh>
#include <duchamp/Outputs/CasaAnnotationWriter.hh>
 
using std::endl;
using std::setw;
using std::setprecision;
using namespace PixelInfo;
using namespace Statistics;

namespace duchamp
{

  void Cube::outputAnnotations()
  {
    
    for(int i=0;i<3;i++){
      AnnotationWriter *writer=0;
      switch(i){
      case 0: //Karma
	if(this->par.getFlagKarma())
	  writer = new KarmaAnnotationWriter(this->par.getKarmaFile());
	break;
      case 1://DS9
	if(this->par.getFlagDS9())
	  writer = new DS9AnnotationWriter(this->par.getDS9File());
	break;
      case 2://CASA
	if(this->par.getFlagCasa())
	  writer = new CasaAnnotationWriter(this->par.getCasaFile());
	break;	
      }
      if(writer!=0){
	writer->setup(this);
	writer->openCatalogue();
	writer->writeHeader();
	writer->writeParameters();
	writer->writeStats();
	writer->writeTableHeader();
	writer->writeEntries();
	writer->writeFooter();
	writer->closeCatalogue();
      }

      delete writer;
    }

  }

  void Cube::outputDetectionsKarma()
  {
    KarmaAnnotationWriter writer(this->pars().getKarmaFile());
    writer.setup(this);
    writer.openCatalogue();
    writer.writeHeader();
    writer.writeParameters();
    writer.writeStats();
    writer.writeTableHeader();
    writer.writeEntries();
    writer.writeFooter();
    writer.closeCatalogue();

  }

  void Cube::outputDetectionsDS9()
  {
    DS9AnnotationWriter writer(this->pars().getDS9File());
    writer.setup(this);
    writer.openCatalogue();
    writer.writeHeader();
    writer.writeParameters();
    writer.writeStats();
    writer.writeTableHeader();
    writer.writeEntries();
    writer.writeFooter();
    writer.closeCatalogue();

  }

  void Cube::outputDetectionsCasa()
  {
    CasaAnnotationWriter writer(this->pars().getCasaFile());
    writer.setup(this);
    writer.openCatalogue();
    writer.writeHeader();
    writer.writeParameters();
    writer.writeStats();
    writer.writeTableHeader();
    writer.writeEntries();
    writer.writeFooter();
    writer.closeCatalogue();

  }

  void Cube::outputCatalogue()
  {
    this->setupColumns();

    ASCIICatalogueWriter catWriter(this->par.getOutFile(),Catalogues::FILE);
    catWriter.setup(this);
    ASCIICatalogueWriter headerWriter(this->par.getHeaderFile(),Catalogues::FILE);
    headerWriter.setup(this);
    ASCIICatalogueWriter screenWriter(Catalogues::SCREEN);
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
      writer->openCatalogue();
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


  void Cube::outputDetectionsVOTable()
  {
    VOTableCatalogueWriter writer(this->pars().getVOTFile());
    writer.setup(this);
    writer.setTableName("Detections");
    writer.setTableDescription("Detected sources and parameters from running the Duchamp source finder.");
    writer.openCatalogue();
    writer.writeHeader();
    writer.writeParameters();
    writer.writeStats();
    writer.writeTableHeader();
    writer.writeEntries();
    writer.writeFooter();
    writer.closeCatalogue();

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

  void Cube::prepareLogFile(int argc, char *argv[])
  {
    /// @details
    ///  Opens the log file so that it can be written to, and writes
    ///  the parameter summary as well as the time of execution to the
    ///  file.
    /// 
    ///  It also writes the command-line statement, hence the need for
    ///  argv and argc.

    ASCIICatalogueWriter logwriter(this->par.getLogFile(),Catalogues::LOG);
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

      long left = this->par.getBorderLeft();
      long bottom = this->par.getBorderBottom();

      if(calcFluxes) this->calcObjectFluxes();
      this->setupColumns();

      ASCIICatalogueWriter logwriter(this->par.getLogFile(),Catalogues::LOG);
      logwriter.setup(this);
      logwriter.openCatalogue(std::ios::app);
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
	logwriter.writeEntry(&obj);
      }

      if(this->par.getFlagBaseline()){
	for(size_t i=0;i<this->axisDim[0]*this->axisDim[1]*this->axisDim[2];i++)
	  this->array[i] -= this->baseline[i];
      }

      logwriter.closeCatalogue();
 
    }

  }

  void Cube::logSummary()
  {
    ASCIICatalogueWriter logwriter(this->par.getLogFile(),Catalogues::LOG);
    logwriter.setup(this);
    logwriter.openCatalogue(std::ios::app);
    logwriter.writeCubeSummary();
    logwriter.closeCatalogue();
  }

  void Cube::writeBinaryCatalogue()
  {
    if(this->par.getFlagWriteBinaryCatalogue()){
      
      std::ofstream bincat(this->par.getBinaryCatalogue().c_str(), std::ios::out | std::ios::binary);
      writeStringToBinaryFile(bincat,PACKAGE_VERSION);
      time_t now = time(NULL);
      std::string nowstr(asctime(localtime(&now)));
      writeStringToBinaryFile(bincat,nowstr);
      bincat.close();
      this->par.writeToBinaryFile();

      this->Stats.writeToBinaryFile(this->par.getBinaryCatalogue());
 
      bincat.open(this->par.getBinaryCatalogue().c_str(), std::ios::out | std::ios::binary | std::ios::app);
      size_t numObj=this->objectList->size();
      bincat.write(reinterpret_cast<const char*>(&numObj), sizeof numObj);
      bincat.close();

      for(size_t i=0;i<numObj;i++)
	this->objectList->at(i).write(this->par.getBinaryCatalogue());

    }
  }

  OUTCOME Cube::readBinaryCatalogue()
  {
    std::ifstream bincat(this->par.getBinaryCatalogue().c_str(), std::ios::in | std::ios::binary);
    if(!bincat.is_open()){
      DUCHAMPERROR("read binary catalogue", "Could not open binary catalogue \""<<this->par.getBinaryCatalogue()<<"\".");
      return FAILURE;
    }
    std::string bincatVersion = readStringFromBinaryFile(bincat);
    if(bincatVersion != std::string(PACKAGE_VERSION))
	DUCHAMPWARN("read binary catalogue","Duchamp versions don't match: Binary catalogue has " << bincatVersion << " compared to " << PACKAGE_VERSION<<". Attempting to continue, but beware!");

    std::string nowstr=readStringFromBinaryFile(bincat);
    if(this->par.isVerbose()) std::cout << "  Reading from binary catalogue " << this->par.getBinaryCatalogue() << " made on " << nowstr << std::flush;
    std::streampos loc=bincat.tellg();
    bincat.close();

    loc = this->par.readFromBinaryFile(loc);
    if(loc<0) {
      DUCHAMPERROR("read binary catalogue","Error reading catalogue");
      return FAILURE;
    }

    loc=this->Stats.readFromBinaryFile(this->par.getBinaryCatalogue(),loc);
    if(loc<0) {
      DUCHAMPERROR("read binary catalogue","Error reading catalogue");
      return FAILURE;
    }

    bincat.open(this->par.getBinaryCatalogue().c_str(), std::ios::in | std::ios::binary);
    bincat.seekg(loc);
    size_t numObj;
    bincat.read(reinterpret_cast<char*>(&numObj), sizeof numObj);
    loc=bincat.tellg();
    bincat.close();

    for(size_t i=0;i<numObj;i++){
      Object3D obj;
      loc=obj.read(this->par.getBinaryCatalogue(),loc);
      this->objectList->push_back(obj);
    }

    if(this->par.isVerbose()) std::cout << "  Successfully read " << numObj << " objects from the binary catalogue.\n";

    if(this->arrayAllocated)
    	this->updateDetectMap();

    return SUCCESS;

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
    float *specxOut = new float[zdim];

    std::vector<bool> objectChoice = this->par.getObjectChoices(this->objectList->size());
    size_t numObj = 0;
    for(size_t i=0;i<this->objectList->size();i++) if(objectChoice[i]) numObj++;

    float *spectra = new float[numObj*zdim];
    
    size_t obj=0;
    for(size_t i=0;i<this->objectList->size();i++){
      if(objectChoice[i]){
	float *temp = new float[zdim];
	float *specx = new float[zdim];
	float *recon = new float[zdim];
	float *base = new float[zdim];
	this->getSpectralArrays(i, specx, temp, recon, base);
	for(int z=0;z<zdim;z++) spectra[obj*zdim+z] = temp[z];
	if(obj==0) for(int z=0;z<zdim;z++) specxOut[z] = specx[z];
	obj++;
	delete [] specx;
	delete [] recon;
	delete [] base;
	delete [] temp;
      }
    }
    
    std::ofstream fspec(this->par.getSpectraTextFile().c_str());
    fspec.setf(std::ios::fixed);

    for(int z=0;z<zdim;z++){
      
      fspec << std::setprecision(8);
      fspec << specxOut[z] << "  ";
      obj=0;
      for(size_t i=0;i<this->objectList->size();i++){
	if(objectChoice[i]){
	  fspec << spectra[obj*zdim+z] << "  ";
	  obj++;
	}
      }
      fspec << "\n";

    }
    fspec.close();

    delete [] spectra;
    delete [] specxOut;

  }
  //--------------------------------------------------------------------


}
