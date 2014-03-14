#include <duchamp/FitsIO/WriteReconArray.hh>
#include <duchamp/FitsIO/WriteArray.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>
#include <fitsio.h>

namespace duchamp {

  WriteReconArray::WriteReconArray():
    WriteArray()
  {
    this->itsBitpix=FLOAT_IMG;
    this->itsIsRecon = true;
  }
  
  WriteReconArray::WriteReconArray(Cube *cube):
    WriteArray(cube,FLOAT_IMG)
  {
    this->itsIsRecon = true;
  }

  WriteReconArray::WriteReconArray(const WriteReconArray &other)
  {
    this->operator=(other);
  }

  WriteReconArray::WriteReconArray(const WriteArray &base)
  {
    this->operator=(base);
  }

  WriteReconArray& WriteReconArray::operator= (const WriteReconArray& other)
  {
    if(this==&other) return *this;
    ((WriteArray &) *this) = other;
    this->itsIsRecon = other.itsIsRecon;
    return *this;
  }

  WriteReconArray& WriteReconArray::operator= (const WriteArray& base)
  {
    if(this==&base) return *this;
    ((WriteArray &) *this) = base;
    this->itsIsRecon = true;
    return *this;
  }

  OUTCOME WriteReconArray::writeHeader()
  {
    /// @details
    ///   A simple function that writes all the necessary keywords and comments
    ///    to the FITS header pointed to by fptr.
    ///   The keyword names and comments are taken from duchamp.hh

    OUTCOME result=SUCCESS;
    int status = 0;
    if(fits_write_history(this->itsFptr, (char *)header_reconHistory1.c_str(), &status)){
      duchampFITSerror(status,"writeReconArray","Error : header I/O");
      result=FAILURE;
    }
    status = 0;
    if(fits_write_history(this->itsFptr, (char *)header_reconHistory2.c_str(), &status)){
      duchampFITSerror(status,"writeReconArray","Error : header I/O");
      result=FAILURE;
    }
    status = 0;
    if(fits_write_history(this->itsFptr, (char *)header_reconHistory_input.c_str(),&status)){
      duchampFITSerror(status,"writeReconArray","Error : header I/O");
      result=FAILURE;
    }
    status = 0;
    if(fits_write_history(this->itsFptr, (char *)this->itsCube->pars().getImageFile().c_str(), &status)){
      duchampFITSerror(status,"writeReconArray","Error : header I/O");
      result=FAILURE;
    }

    if(this->itsCube->pars().getFlagSubsection()){
      status = 0;
      if(fits_write_comment(this->itsFptr,(char *)header_reconSubsection_comment.c_str(),&status)){
	duchampFITSerror(status,"writeReconArray","Error : header I/O");
	result=FAILURE;
      }
      status = 0;
      if(fits_write_key(this->itsFptr, TSTRING, (char *)keyword_subsection.c_str(), 
			(char *)this->itsCube->pars().getSubsection().c_str(),
			(char *)comment_subsection.c_str(), &status)){
	duchampFITSerror(status,"writeReconArray","Error : header I/O");
	result=FAILURE;
      }
    }

    status=0;
    if(fits_write_comment(this->itsFptr, (char *)header_atrous_comment.c_str(), &status)){
      duchampFITSerror(status,"writeReconArray","Error : header I/O");
      result=FAILURE;
    }

    float valf = this->itsCube->pars().getAtrousCut();
    status=0;
    if(fits_write_key(this->itsFptr, TFLOAT, (char *)keyword_snrRecon.c_str(), &valf, 
		      (char *)comment_snrRecon.c_str(), &status)){
      duchampFITSerror(status,"writeReconArray","Error : header I/O");
      result=FAILURE;
    }

    int vali = this->itsCube->pars().getReconDim();
    status=0;
    if(fits_write_key(this->itsFptr, TINT, (char *)keyword_reconDim.c_str(), &vali, 
		      (char *)comment_reconDim.c_str(), &status)){
      duchampFITSerror(status,"writeReconArray","Error : header I/O");
      result=FAILURE;
    }

    vali = this->itsCube->pars().getMinScale();
    status=0;
    if(fits_write_key(this->itsFptr, TINT, (char *)keyword_scaleMin.c_str(), &vali, 
		      (char *)comment_scaleMin.c_str(), &status)){
      duchampFITSerror(status,"writeReconArray","Error : header I/O");
      result=FAILURE;
    }

    vali = this->itsCube->pars().getFilterCode();
    status=0;
    if(fits_write_key(this->itsFptr, TINT, (char *)keyword_filterCode.c_str(), &vali, 
		      (char *)comment_filterCode.c_str(), &status)){
      duchampFITSerror(status,"writeReconArray","Error : header I/O");
      result=FAILURE;
    }

    std::string explanation,ReconResid;
    if(this->itsIsRecon){
      explanation = "Duchamp: This is the RECONSTRUCTED cube";
      ReconResid = "RECON";
    }
    else{
      explanation = "Duchamp: This is the RESIDUAL cube";
      ReconResid = "RESID";
    }
    status=0;
    if(fits_write_comment(this->itsFptr, (char *)explanation.c_str(), &status)){
      duchampFITSerror(status,"writeReconArray","Error : header I/O");
      result=FAILURE;
    }
    status=0;
    if(fits_write_key(this->itsFptr, TSTRING, (char *)keyword_ReconResid.c_str(), 
		      (char *)ReconResid.c_str(), 
		      (char *)comment_ReconResid.c_str(), &status)){
      duchampFITSerror(status,"writeReconArray","Error : header I/O");
      result=FAILURE;
    }
 
    return result;

  }

  OUTCOME WriteReconArray::writeData()
  {
      OUTCOME result = SUCCESS;
      
      float *array;
      
      if(this->itsIsRecon){
          array = this->itsCube->getRecon();
      }
      else{
          
          array = new float[this->itsCube->getSize()];
          for(size_t i=0;i<this->itsCube->getSize();i++)
          array[i] = this->itsCube->getPixValue(i) - this->itsCube->getReconValue(i);
          
      }
      
      result = this->writeToFITS_flt(this->itsCube->getSize(), array);
      
      if(!this->itsIsRecon) delete [] array;
      
      return result;

  }

}

