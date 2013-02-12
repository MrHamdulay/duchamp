#include <duchamp/FitsIO/ReadExistingRecon.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp {

  ReadExistingRecon::ReadExistingRecon():
    ReadExisting()
  {
  }
  
  ReadExistingRecon::ReadExistingRecon(Cube *cube):
    ReadExisting(cube)
  {
  }

  ReadExistingRecon::ReadExistingRecon(const ReadExistingRecon &other)
  {
    this->operator=(other);
  }

  ReadExistingRecon::ReadExistingRecon(const ReadExisting &base)
  {
    this->operator=(base);
  }

  ReadExistingRecon& ReadExistingRecon::operator= (const ReadExistingRecon& other)
  {
    if(this==&other) return *this;
    ((ReadExisting &) *this) = other;
    return *this;
  }

  ReadExistingRecon& ReadExistingRecon::operator= (const ReadExisting& base)
  {
    if(this==&base) return *this;
    ((ReadExisting &) *this) = base;
    return *this;
  }

  OUTCOME ReadExistingRecon::checkPars()
  {
    if(!this->itsCube->pars().getFlagReconExists()){
      DUCHAMPWARN("readReconCube", 
		  "reconExists flag is not set. Not reading anything in!");
      return FAILURE;
    }
    else if(!this->itsCube->pars().getFlagATrous()){
      DUCHAMPWARN("readReconCube",
		  "flagATrous is not set. Don't need to read in recon array!");
      return FAILURE;
    }
    return SUCCESS;
  }

  OUTCOME ReadExistingRecon::checkFile()
  {
    OUTCOME result=FAILURE;
    int exists,status=0;
    std::string reconFile = this->itsCube->pars().outputReconFile();

    if(this->itsCube->pars().getReconFile() == "")
      DUCHAMPWARN("readReconCube", "ReconFile not specified. Working out name from parameters.");

    fits_file_exists(reconFile.c_str(),&exists,&status);
    if(exists<=0){
      fits_report_error(stderr, status);
      DUCHAMPERROR("readReconCube","ReconFile not present.");
      this->itsCube->pars().setFlagReconExists(false);
      result = FAILURE;
    }
    else{
      result=SUCCESS;
      this->itsCube->pars().setReconFile(reconFile);
    }

    this->itsFilename = reconFile;
    return result;

  }

  OUTCOME ReadExistingRecon::checkHeaders()
  {
    OUTCOME result=SUCCESS;
    if(this->itsFptr==0){
      DUCHAMPERROR("readReconCube","FITS file not open");
      result=FAILURE;
    }
    else{
      unsigned int scaleMin;
      int filterCode,reconDim;
      float snrRecon;
      char *comment = new char[80];
      int status = 0;
      fits_read_key(this->itsFptr, TINT, (char *)keyword_reconDim.c_str(), 
		    &reconDim, comment, &status);
      if(reconDim != this->itsCube->pars().getReconDim()){
	DUCHAMPERROR("readReconCube", "reconDim keyword in reconFile (" << reconDim << ") does not match that requested (" << this->itsCube->pars().getReconDim() << ").");
	result = FAILURE;
      }
      status = 0;
      fits_read_key(this->itsFptr, TINT, (char *)keyword_filterCode.c_str(), 
		    &filterCode, comment, &status);
      if(filterCode != this->itsCube->pars().getFilterCode()){
	DUCHAMPERROR("readReconCube", "filterCode keyword in reconFile (" << filterCode << ") does not match that requested (" << this->itsCube->pars().getFilterCode() << ").");
	result = FAILURE;
      }
      status = 0;
      fits_read_key(this->itsFptr, TFLOAT, (char *)keyword_snrRecon.c_str(), 
		    &snrRecon, comment, &status);
      if(snrRecon != this->itsCube->pars().getAtrousCut()){
	DUCHAMPERROR("readReconCube", "snrRecon keyword in reconFile (" << snrRecon << ") does not match that requested (" << this->itsCube->pars().getAtrousCut() << ").");
	result = FAILURE;
      }
      status = 0;
      fits_read_key(this->itsFptr, TINT, (char *)keyword_scaleMin.c_str(), 
		    &scaleMin, comment, &status);
      if(scaleMin != this->itsCube->pars().getMinScale()){
	DUCHAMPERROR("readReconCube", "scaleMin keyword in reconFile (" << scaleMin << ") does not match that requested (" << this->itsCube->pars().getMinScale() << ").");
	result = FAILURE;
      }

    }

    return result;

  }

  OUTCOME ReadExistingRecon::readFromFile()
  {
    OUTCOME result = this->ReadExisting::readFromFile();
    if(result == SUCCESS){
      // We don't want to write out the recon or resid files at the end
      this->itsCube->pars().setFlagOutputRecon(false);
      this->itsCube->pars().setFlagOutputResid(false);
    }
    return result;
  }


}
