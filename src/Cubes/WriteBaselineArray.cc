#include <duchamp/Cubes/WriteBaselineArray.hh>
#include <duchamp/Cubes/WriteArray.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>
#include <fitsio.h>

namespace duchamp {

  WriteBaselineArray::WriteBaselineArray():
    WriteArray()
  {
    this->itsBitpix=FLOAT_IMG;
  }
  
  WriteBaselineArray::WriteBaselineArray(Cube *cube):
    WriteArray(cube,FLOAT_IMG)
  {
    this->itsFlag2D=true;
  }

  WriteBaselineArray::WriteBaselineArray(const WriteBaselineArray &other)
  {
    this->operator=(other);
  }

  WriteBaselineArray::WriteBaselineArray(const WriteArray &base)
  {
    this->operator=(base);
  }

  WriteBaselineArray& WriteBaselineArray::operator= (const WriteBaselineArray& other)
  {
    if(this==&other) return *this;
    ((WriteArray &) *this) = other;
    return *this;
  }

  WriteBaselineArray& WriteBaselineArray::operator= (const WriteArray& base)
  {
    if(this==&base) return *this;
    ((WriteArray &) *this) = base;
    return *this;
  }

  OUTCOME WriteBaselineArray::writeHeader()
  {
    /// @details
    ///   A simple function that writes all the necessary keywords and comments
    ///    to the FITS header pointed to by this->itsFptr.
    ///   The keyword names and comments are taken from duchamp.hh

    OUTCOME result=SUCCESS;
    int status = 0;
    if(fits_write_history(this->itsFptr, (char *)header_baselineHistory.c_str(), &status)){
      duchampFITSerror(status,"writeBaselineArray","Error : header I/O");
      result=FAILURE;
    }
    status = 0;
    if(fits_write_history(this->itsFptr, (char *)header_baselineHistory_input.c_str(),&status)){
      duchampFITSerror(status,"writeBaselineArray","Error : header I/O");
      result=FAILURE;
    }
    status = 0;
    if(fits_write_history(this->itsFptr, (char *)this->itsCube->pars().getImageFile().c_str(), &status)){
      duchampFITSerror(status,"writeBaselineArray","Error : header I/O");
      result=FAILURE;
    }

    if(this->itsCube->pars().getFlagSubsection()){
      status = 0;
      if(fits_write_comment(this->itsFptr,(char *)header_baselineSubsection_comment.c_str(),&status)){
	duchampFITSerror(status,"writeBaselineArray","Error : header I/O");
	result=FAILURE;
      }
      status = 0;
      if(fits_write_key(this->itsFptr, TSTRING, (char *)keyword_subsection.c_str(), 
			(char *)this->itsCube->pars().getSubsection().c_str(),
			(char *)comment_subsection.c_str(), &status)){
	duchampFITSerror(status,"writeBaselineArray","Error : header I/O");
	result=FAILURE;
      }
    }
 
    return result;

  }

  OUTCOME WriteBaselineArray::writeData()
  {
    OUTCOME result = SUCCESS;

    long group=0;
    int status=0;
    if(this->itsCube->pars().getFlagBlankPix())
      fits_write_imgnull_flt(this->itsFptr, group, 1, this->itsCube->getSize(), this->itsCube->getBaseline(), this->itsCube->pars().getBlankPixVal(), &status);
    else 
      fits_write_img_flt(this->itsFptr, group, 1, this->itsCube->getSize(), this->itsCube->getBaseline(), &status);
    if(status){
      duchampFITSerror(status,"writeBaselineArray","Error writing baseline array:");
      result = FAILURE;
    }

    return result;

  }

}

