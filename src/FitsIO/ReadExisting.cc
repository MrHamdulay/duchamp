#include <duchamp/FitsIO/ReadExisting.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>
#include <fitsio.h>
#include <wcslib/wcsunits.h>

namespace duchamp {

  ReadExisting::ReadExisting()
  {
    itsCube = 0;
    itsArray = 0;
    itsFptr = 0;
  }

  ReadExisting::ReadExisting(Cube *cube):
    itsCube(cube)
  {
    itsArray = this->itsCube->getRecon();
    itsFptr = 0;
  }

  ReadExisting::ReadExisting(const ReadExisting &other)
  {
    this->operator=(other);
  }

  ReadExisting& ReadExisting::operator= (const ReadExisting& other)
  {
    if(this==&other) return *this;
    this->itsCube = other.itsCube;
    this->itsArray = other.itsArray;
    this->itsFilename = other.itsFilename;
    this->itsFptr = other.itsFptr;
    return *this;
  }

  OUTCOME ReadExisting::openFile()
  {
    int status=0;
    fits_open_file(&this->itsFptr,this->itsFilename.c_str(),READONLY,&status);
    if(status){
      DUCHAMPWARN("Reading Cube", "Error opening file " << this->itsFilename);
      fits_report_error(stderr, status);
      return FAILURE;
    }
    return SUCCESS;
  }

  OUTCOME ReadExisting::checkDim()
  {
    OUTCOME result=SUCCESS;
    if(this->itsFptr==0){
      DUCHAMPERROR("Reading Cube","FITS file not open");
      result = FAILURE;
    }
    else{
      
      int status = 0, maxdim = 3;
      int bitpix,numAxesNew;
      long *dimAxesNew = new long[maxdim];
      for(int i=0;i<maxdim;i++) dimAxesNew[i]=1;
      fits_get_img_param(this->itsFptr, maxdim, &bitpix, &numAxesNew, dimAxesNew, &status);
      if(status){
	fits_report_error(stderr, status);
	result = FAILURE;
      }

      if(numAxesNew != this->itsCube->getNumDim()){
	DUCHAMPERROR("Reading Cube", "Input cube " << this->itsFilename << " has a different number of axes to original!" 
		     << " (" << numAxesNew << " cf. " << this->itsCube->getNumDim() << ")");
	result = FAILURE;
      }

      for(int i=0;i<numAxesNew;i++){
	if(dimAxesNew[i]!=int(this->itsCube->getDimArray()[i])){
	  DUCHAMPERROR("Reading Cube", "Input cube " << this->itsFilename << " has different axis dimensions to original! Axis #" 
		       << i+1 << " has size " << dimAxesNew[i] << " cf. " << this->itsCube->getDimArray()[i] <<" in original.");
	  result = FAILURE;
	}
      }


      char *comment = new char[80];
      if(this->itsCube->pars().getFlagSubsection()){
	char *subsection = new char[80];
	status = 0;
	fits_read_key(this->itsFptr, TSTRING, (char *)keyword_subsection.c_str(), subsection, comment, &status);
	if(status){
	  DUCHAMPERROR("Reading Cube", "subsection keyword not present in reconFile.");
	  result = FAILURE;
	}
	else{
	  if(this->itsCube->pars().getSubsection() != subsection){
	    DUCHAMPERROR("Reading Cube", "subsection keyword in reconFile (" << subsection 
			 << ") does not match that requested (" << this->itsCube->pars().getSubsection() << ").");
	    result = FAILURE;
	  }
	}
	delete subsection;
      }

    }

    return result;

  }

  OUTCOME ReadExisting::readFromFile()
  {
    OUTCOME result=SUCCESS;
    if(this->itsFptr==0){
      DUCHAMPERROR("Reading Cube","FITS file not open");
      result = FAILURE;
    }
    else{

      short int maxdim=3;
      long *fpixel = new long[maxdim];
      for(int i=0;i<maxdim;i++) fpixel[i]=1;
      long *lpixel = new long[maxdim];
      for(int i=0;i<maxdim;i++) lpixel[i]=this->itsCube->getDimArray()[i];
      long *inc = new long[maxdim];
      for(int i=0;i<maxdim;i++) inc[i]=1;
      long *dimAxesNew = new long[maxdim];
      for(int i=0;i<maxdim;i++) dimAxesNew[i]=this->itsCube->getDimArray()[i];
      int anynul;
      int colnum = 0;  // want the first dataset in the FITS file
      int status = 0;

      if(fits_read_subset_flt(this->itsFptr, colnum, 3, dimAxesNew, fpixel, lpixel, inc, 
			      this->itsCube->pars().getBlankPixVal(), this->itsArray, &anynul, &status)){
	DUCHAMPERROR("Recon Reader", "There was an error reading in the data array:");
	fits_report_error(stderr, status);
	result = FAILURE;
      }
      else{
	this->itsCube->setReconFlag(true);

	std::string header("BUNIT");
	char *unit = new char[FLEN_VALUE];
	char *comment = new char[80];
	std::string fluxunits;
	fits_read_key(this->itsFptr, TSTRING, (char *)header.c_str(), unit, comment, &status);
	if (status){
	  DUCHAMPWARN("Cube Reader","Error reading BUNIT keyword: ");
	  fits_report_error(stderr, status);
	  return FAILURE;
	}
	else{
	  wcsutrn(0,unit);
	  fluxunits = unit;
	}
	this->itsCube->convertFluxUnits(fluxunits,this->itsCube->pars().getNewFluxUnits(),RECON);
      }

    }
    return result;
  }

  OUTCOME ReadExisting::read()
  {
    OUTCOME result = this->checkPars();
    if(result==SUCCESS) result=this->checkFile();
    if(result==SUCCESS) result=this->openFile();
    if(result==SUCCESS) result=this->checkDim();
    if(result==SUCCESS) result=this->checkHeaders();
    if(result==SUCCESS) result=this->readFromFile();
    if(result==SUCCESS) result=this->closeFile();
    return result;
  }

  OUTCOME ReadExisting::closeFile()
  {
    OUTCOME result=SUCCESS;
    int status = 0;
    if(this->itsFptr==0){
      fits_close_file(this->itsFptr, &status);
      if (status){
	DUCHAMPWARN("Reading Cube", "Error closing file " << this->itsFilename);
	fits_report_error(stderr, status);
	result=FAILURE;
      }
    }
    return result;
  }

}
