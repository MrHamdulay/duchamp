#include <duchamp/FitsIO/ReadExistingSmooth.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>

namespace duchamp {

  ReadExistingSmooth::ReadExistingSmooth():
    ReadExisting()
  {
  }
  
  ReadExistingSmooth::ReadExistingSmooth(Cube *cube):
    ReadExisting(cube)
  {
  }

  ReadExistingSmooth::ReadExistingSmooth(const ReadExistingSmooth &other)
  {
    this->operator=(other);
  }

  ReadExistingSmooth::ReadExistingSmooth(const ReadExisting &base)
  {
    this->operator=(base);
  }

  ReadExistingSmooth& ReadExistingSmooth::operator= (const ReadExistingSmooth& other)
  {
    if(this==&other) return *this;
    ((ReadExisting &) *this) = other;
    return *this;
  }

  ReadExistingSmooth& ReadExistingSmooth::operator= (const ReadExisting& base)
  {
    if(this==&base) return *this;
    ((ReadExisting &) *this) = base;
    return *this;
  }

  OUTCOME ReadExistingSmooth::checkPars()
  {
    if(!this->itsCube->pars().getFlagSmoothExists()){
      DUCHAMPWARN("readSmoothCube", 
		  "smoothExists flag is not set. Not reading anything in!");
      return FAILURE;
    }
    else if(!this->itsCube->pars().getFlagSmooth()){
      DUCHAMPWARN("readSmoothCube",
		  "flagSmooth is not set. Don't need to read in recon array!");
      return FAILURE;
    }
    return SUCCESS;
  }

  OUTCOME ReadExistingSmooth::checkFile()
  {
    OUTCOME result=FAILURE;
    int exists,status=0;
    std::string smoothFile = this->itsCube->pars().outputSmoothFile();

    if(this->itsCube->pars().getSmoothFile() == "")
      DUCHAMPWARN("readSmoothCube", "SmoothFile not specified. Working out name from parameters.");

    fits_file_exists(smoothFile.c_str(),&exists,&status);
    if(exists<=0){
      fits_report_error(stderr, status);
      DUCHAMPERROR("readSmoothCube","SmoothFile not present.");
      result = FAILURE;
    }
    else{
      result=SUCCESS;
      this->itsCube->pars().setSmoothFile(smoothFile);
    }

    this->itsFilename = smoothFile;
    return result;
  }

  OUTCOME ReadExistingSmooth::checkHeaders()
  {
    OUTCOME result=SUCCESS;
    if(this->itsFptr==0){
      DUCHAMPERROR("readSmoothCube","FITS file not open");
      result=FAILURE;
    }
    else{

      int status = 0;
      char *comment = new char[80];

      if(this->itsCube->pars().getSmoothType()=="spectral"){

	int hannWidth;
	fits_read_key(this->itsFptr, TINT, (char *)keyword_hanningwidth.c_str(), 
		      &hannWidth, comment, &status);
	if(hannWidth != this->itsCube->pars().getHanningWidth()){
	  DUCHAMPERROR("readSmoothCube", keyword_hanningwidth << " keyword in smoothFile (" << hannWidth << ") does not match the hanningWidth parameter (" << this->itsCube->pars().getHanningWidth() << ").");
	  result = FAILURE;
	}

      }
      else if(this->itsCube->pars().getSmoothType()=="spatial"){

	float maj,min,pa;
	status = 0;
	fits_read_key(this->itsFptr, TFLOAT, (char *)keyword_kernmaj.c_str(), 
		      &maj, comment, &status);
	if(maj != this->itsCube->pars().getKernMaj()){
	  DUCHAMPERROR("readSmoothCube", keyword_kernmaj << " keyword in smoothFile (" << maj << ") does not match the kernMaj parameter (" << this->itsCube->pars().getKernMaj() << ").");
	  result = FAILURE;
	}
	status = 0;
	fits_read_key(this->itsFptr, TFLOAT, (char *)keyword_kernmin.c_str(), 
		      &min, comment, &status);
	if(min != this->itsCube->pars().getKernMin()){
	  DUCHAMPERROR("readSmoothCube", keyword_kernmin << " keyword in smoothFile (" << maj << ") does not match the kernMin parameter (" << this->itsCube->pars().getKernMin() << ").");
	  result = FAILURE;
	}
	status = 0;
	fits_read_key(this->itsFptr, TFLOAT, (char *)keyword_kernpa.c_str(), 
		      &pa, comment, &status);
	if(pa != this->itsCube->pars().getKernPA()){
	  DUCHAMPERROR("readSmoothCube", keyword_kernpa << " keyword in smoothFile (" << maj << ") does not match the kernPA parameter (" << this->itsCube->pars().getKernPA() << ").");
	  result = FAILURE;
	}

      }

    }

    if(result==FAILURE){
      DUCHAMPERROR("readSmoothCube","Header keyword mismatch - not reading smoothed array from " << this->itsFilename);
    }


    return result;

  }

  OUTCOME ReadExistingSmooth::readFromFile()
  {
    OUTCOME result = this->ReadExisting::readFromFile();
    if(result == SUCCESS){
      // We don't want to write out the smooth file at the end
      this->itsCube->pars().setFlagOutputSmooth(false);
    }
    return result;
  }


}
