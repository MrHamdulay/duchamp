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
    if(this->pars().getFlagKarma()) this->outputDetectionsKarma();
    if(this->pars().getFlagDS9())   this->outputDetectionsDS9();
    if(this->pars().getFlagCasa())  this->outputDetectionsCasa();
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

      ASCIICatalogueWriter logwriter(this->par.getLogFile(),Catalogues::LOG);
      logwriter.setup(this);
      logwriter.openCatalogue(std::ios::app);

      long left = this->par.getBorderLeft();
      long bottom = this->par.getBorderBottom();

      // std::ofstream fout(this->par.getLogFile().c_str(),std::ios::app);
      if(calcFluxes) this->calcObjectFluxes();
      this->setupColumns();
      // outputTableHeader(fout,this->fullCols,Catalogues::LOG,this->head.isWCS());
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
	//	obj.printTableRow(fout,this->fullCols,Catalogues::LOG);
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

  void Cube::logSummary()
  {
    ASCIICatalogueWriter logwriter(this->par.getLogFile(),Catalogues::LOG);
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
