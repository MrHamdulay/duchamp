#include <duchamp/Outputs/ASCIICatalogueWriter.hh>
#include <duchamp/Outputs/CatalogueWriter.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/param.hh>
#include <duchamp/fitsHeader.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Detection/columns.hh>
#include <duchamp/Cubes/cubes.hh>
#include <duchamp/Utils/Statistics.hh>
#include <iostream>
#include <fstream>
#include <vector>

namespace duchamp {

  ASCIICatalogueWriter::ASCIICatalogueWriter():
    CatalogueWriter()
  {
    this->itsStream=0;
    this->itsFileStream=0;
  }

  ASCIICatalogueWriter::ASCIICatalogueWriter(std::string name):
    CatalogueWriter(name)
  {
    this->itsStream=0;
    this->itsFileStream=0;
  }

  ASCIICatalogueWriter::ASCIICatalogueWriter(Column::DESTINATION dest):
    itsDestination(dest)
  {
    this->itsStream=0;
    this->itsFileStream=0;
  }

  ASCIICatalogueWriter::ASCIICatalogueWriter(std::string name, Column::DESTINATION dest):
    CatalogueWriter(name), itsDestination(dest)
  {
    this->itsStream=0;
    this->itsFileStream=0;
  }

  ASCIICatalogueWriter::ASCIICatalogueWriter(const ASCIICatalogueWriter& other)
  {
    this->operator=(other);
  }

  ASCIICatalogueWriter& ASCIICatalogueWriter::operator= (const ASCIICatalogueWriter& other)
  {
    if(this==&other) return *this;
    ((CatalogueWriter &) *this) = other;
    this->itsStream = other.itsStream;
    this->itsFileStream = other.itsFileStream;
    this->itsDestination = other.itsDestination;
    return *this;
  }
  
  bool ASCIICatalogueWriter::openCatalogue()
  {
    if(this->itsName == ""){
      DUCHAMPERROR("ASCIICatalogueWriter","No catalogue name provided");
      this->itsOpenFlag = false;
    }
    else {
      if(this->itsDestination==Column::SCREEN){
	this->itsStream = &std::cout;
	this->itsOpenFlag = true;
      }
      else{
	this->itsFileStream->open(this->itsName.c_str());
	this->itsStream = this->itsFileStream;
	this->itsOpenFlag = this->itsFileStream->is_open();
      }
      if(!this->itsOpenFlag) 
	DUCHAMPERROR("ASCIICatalogueWriter","Could not open file \""<<this->itsName<<"\"");
    }
    return this->itsOpenFlag;
  }

  void ASCIICatalogueWriter::writeHeader()
  {
    if(this->itsOpenFlag){
      *this->itsStream <<"Results of the Duchamp source finder v."<<VERSION<<": ";
      time_t now = time(NULL);
      *this->itsStream << asctime( localtime(&now) );
    }
  }

  void ASCIICatalogueWriter::writeParameters()
  {
    if(this->itsOpenFlag){
      *this->itsStream << *this->itsParam;
      *this->itsStream <<"--------------------\n";
    }
  }

  void ASCIICatalogueWriter::writeStats()
  {
    if(this->itsOpenFlag){

      *this->itsStream<<"Summary of statistics:\n";
      *this->itsStream<<"Detection threshold = " << this->itsStats->getThreshold()
		      <<" " << this->itsHead->getFluxUnits();
      if(this->itsParam->getFlagFDR())
	*this->itsStream<<" (or S/N=" << this->itsStats->getThresholdSNR()<<")";
      if(this->itsParam->getFlagSmooth()){
	*this->itsStream << " in smoothed cube.";
	if(!this->itsParam->getFlagUserThreshold())
	  *this->itsStream<<"\nNoise level = " << this->itsStats->getMiddle()
			  <<", Noise spread = " << this->itsStats->getSpread()
			  <<" in smoothed cube.";
      
	// // calculate the stats for the original array, so that we can
	// // quote S/N values correctly.
	// this->itsParam->setFlagSmooth(false);
	// bool verb=this->itsParam->isVerbose();
	// bool fdrflag=this->itsParam->getFlagFDR();
	// this->itsParam->setVerbosity(false);
	// this->itsParam->setFlagFDR(false);
	// this->setCubeStats();
	// this->itsParam->setVerbosity(verb);
	// this->itsParam->setFlagFDR(fdrflag);
	// this->itsParam->setFlagSmooth(true);
      
	// *this->itsStream << "\nNoise properties for the original cube are:";
      }
     
      if(!this->itsParam->getFlagUserThreshold())
	*this->itsStream<<"\nNoise level = " << this->itsStats->getMiddle()
			<<", Noise spread = " << this->itsStats->getSpread();

      if(this->itsParam->getFlagGrowth()){
	Statistics::StatsContainer<float> growthStats = *this->itsStats;
	if(this->itsParam->getFlagUserGrowthThreshold())
	  growthStats.setThreshold(this->itsParam->getGrowthThreshold());
	else
	  growthStats.setThresholdSNR(this->itsParam->getGrowthCut());
	growthStats.setUseFDR(false);
	*this->itsStream<<"\nDetections grown down to threshold of " 
			<< growthStats.getThreshold() << " " 
			<< this->itsHead->getFluxUnits();
      }

      if(!this->itsParam->getFlagUserThreshold())
	*this->itsStream << "\nFull stats:\n" << *this->itsStats;
      else
	*this->itsStream << "\n\nNot calculating full stats since threshold was provided directly.\n";

      *this->itsStream<<"--------------------\n";
 
      *this->itsStream<<"Total number of detections = "<<this->itsObjectList->size()<<"\n";
      *this->itsStream<<"--------------------\n";

    }
  }

  void ASCIICatalogueWriter::writeTableHeader()
  {
    if(this->itsOpenFlag){
      outputTableHeader(*this->itsStream,*this->itsColumnSpecification,this->itsDestination,this->itsHead->isWCS());
    }
  }

  void ASCIICatalogueWriter::writeEntries()
  {
    if(this->itsOpenFlag){
      for(std::vector<Detection>::iterator obj=this->itsObjectList->begin();obj<this->itsObjectList->end();obj++)
	this->writeEntry(&*obj);
    }
  }

  void ASCIICatalogueWriter::writeEntry(Detection *object)
  {
    if(this->itsOpenFlag){
      object->printTableRow(*this->itsStream, *this->itsColumnSpecification, this->itsDestination);
    }
  }

  bool ASCIICatalogueWriter::closeCatalogue()
  {
    bool returnval=true;
    if(this->itsDestination!=Column::SCREEN){
      this->itsFileStream->close();
      returnval=!this->itsFileStream->fail();
    }
    return returnval;
  }


}
