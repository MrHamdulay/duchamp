#include <duchamp/FitsIO/WriteSmoothArray.hh>
#include <duchamp/FitsIO/WriteArray.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>
#include <fitsio.h>

namespace duchamp {

  WriteSmoothArray::WriteSmoothArray():
    WriteArray()
  {
    this->itsBitpix=FLOAT_IMG;
  }
  
  WriteSmoothArray::WriteSmoothArray(Cube *cube):
    WriteArray(cube,FLOAT_IMG)
  {
  }

  WriteSmoothArray::WriteSmoothArray(const WriteSmoothArray &other)
  {
    this->operator=(other);
  }

  WriteSmoothArray::WriteSmoothArray(const WriteArray &base)
  {
    this->operator=(base);
  }

  WriteSmoothArray& WriteSmoothArray::operator= (const WriteSmoothArray& other)
  {
    if(this==&other) return *this;
    ((WriteArray &) *this) = other;
    return *this;
  }

  WriteSmoothArray& WriteSmoothArray::operator= (const WriteArray& base)
  {
    if(this==&base) return *this;
    ((WriteArray &) *this) = base;
    return *this;
  }

  OUTCOME WriteSmoothArray::writeHeader()
  {
    /// @details
    ///   A simple function that writes all the necessary keywords and comments
    ///    to the FITS header pointed to by this->itsFptr.
    ///   The keyword names and comments are taken from duchamp.hh

    OUTCOME result=SUCCESS;
    int status = 0;
    if(fits_write_history(this->itsFptr, (char *)header_smoothHistory.c_str(), &status)){
      duchampFITSerror(status,"writeSmoothArray","Error : header I/O");
      result=FAILURE;
    }
    status = 0;
    if(fits_write_history(this->itsFptr, (char *)header_smoothHistory_input.c_str(),&status)){
      duchampFITSerror(status,"writeSmoothArray","Error : header I/O");
      result=FAILURE;
    }
    status = 0;
    if(fits_write_history(this->itsFptr, (char *)this->itsCube->pars().getImageFile().c_str(), &status)){
      duchampFITSerror(status,"writeSmoothArray","Error : header I/O");
      result=FAILURE;
    }

    if(this->itsCube->pars().getFlagSubsection()){
      status = 0;
      if(fits_write_comment(this->itsFptr,(char *)header_smoothSubsection_comment.c_str(),&status)){
	duchampFITSerror(status,"writeSmoothArray","Error : header I/O");
	result=FAILURE;
      }
      status = 0;
      if(fits_write_key(this->itsFptr, TSTRING, (char *)keyword_subsection.c_str(), 
			(char *)this->itsCube->pars().getSubsection().c_str(),
			(char *)comment_subsection.c_str(), &status)){
	duchampFITSerror(status,"writeSmoothArray","Error : header I/O");
	result=FAILURE;
      }
    }
    
    if(this->itsCube->pars().getSmoothType()=="spatial"){
      // if kernMin is negative (not defined), make it equal to kernMaj
      float kernMaj=this->itsCube->pars().getKernMaj();
      float kernMin=this->itsCube->pars().getKernMin();
      float kernPA=this->itsCube->pars().getKernPA();
      if(kernMin<0) kernMin=kernMaj;

      status=0;
      if(fits_write_key(this->itsFptr, TSTRING, (char *)keyword_smoothtype.c_str(),
			(char *)header_smoothSpatial.c_str(),
			(char *)comment_smoothtype.c_str(), &status)){
	duchampFITSerror(status,"writeSmoothArray","Error : header I/O");
	result=FAILURE;
      }
      status=0;
      if(fits_write_key(this->itsFptr, TFLOAT, (char *)keyword_kernmaj.c_str(), &kernMaj,
			(char *)comment_kernmaj.c_str(), &status)){
	duchampFITSerror(status,"writeSmoothArray","Error : header I/O");
	result=FAILURE;
      }
      status=0;
      if(fits_write_key(this->itsFptr, TFLOAT, (char *)keyword_kernmin.c_str(), &kernMin,
			(char *)comment_kernmin.c_str(), &status)){
	duchampFITSerror(status,"writeSmoothArray","Error : header I/O");
	result=FAILURE;
      }
      status=0;
      if(fits_write_key(this->itsFptr, TFLOAT, (char *)keyword_kernpa.c_str(), &kernPA,
			(char *)comment_kernpa.c_str(), &status)){
	duchampFITSerror(status,"writeSmoothArray","Error : header I/O");
	result=FAILURE;
      }
    }
    else if(this->itsCube->pars().getSmoothType()=="spectral"){
      status=0;
      if(fits_write_key(this->itsFptr, TSTRING, (char *)keyword_smoothtype.c_str(),
			(char *)header_smoothSpectral.c_str(),
			(char *)comment_smoothtype.c_str(), &status)){
	duchampFITSerror(status,"writeSmoothArray","Error : header I/O");
	result=FAILURE;
      }
      int width = this->itsCube->pars().getHanningWidth();
      status=0;
      if(fits_write_key(this->itsFptr, TINT, (char *)keyword_hanningwidth.c_str(), &width,
			(char *)comment_hanningwidth.c_str(), &status)){
	duchampFITSerror(status,"writeSmoothArray","Error : header I/O");
	result=FAILURE;
      }
    }

    return result;

  }

  OUTCOME WriteSmoothArray::writeData()
  {
    OUTCOME result = SUCCESS;

    long group=0;
    int status=0;
    if(this->itsCube->pars().getFlagBlankPix())
      fits_write_imgnull_flt(this->itsFptr, group, 1, this->itsCube->getSize(), this->itsCube->getRecon(), this->itsCube->pars().getBlankPixVal(), &status);
    else 
      fits_write_img_flt(this->itsFptr, group, 1, this->itsCube->getSize(), this->itsCube->getRecon(), &status);
    if(status){
      duchampFITSerror(status,"writeSmoothArray","Error writing smoothed array:");
      result = FAILURE;
    }

    return result;

  }

}

