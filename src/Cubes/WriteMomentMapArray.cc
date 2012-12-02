#include <duchamp/Cubes/WriteMomentMapArray.hh>
#include <duchamp/Cubes/WriteArray.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>
#include <fitsio.h>

namespace duchamp {

  WriteMomentMapArray::WriteMomentMapArray():
    WriteArray()
  {
    this->itsBitpix=FLOAT_IMG;
    this->itsFlag2D=true;
  }
  
  WriteMomentMapArray::WriteMomentMapArray(Cube *cube):
    WriteArray(cube,FLOAT_IMG)
  {
    this->itsFlag2D=true;
  }

  WriteMomentMapArray::WriteMomentMapArray(const WriteMomentMapArray &other)
  {
    this->operator=(other);
  }

  WriteMomentMapArray::WriteMomentMapArray(const WriteArray &base)
  {
    this->operator=(base);
  }

  WriteMomentMapArray& WriteMomentMapArray::operator= (const WriteMomentMapArray& other)
  {
    if(this==&other) return *this;
    ((WriteArray &) *this) = other;
    return *this;
  }

  WriteMomentMapArray& WriteMomentMapArray::operator= (const WriteArray& base)
  {
    if(this==&base) return *this;
    ((WriteArray &) *this) = base;
    return *this;
  }

  OUTCOME WriteMomentMapArray::writeHeader()
  {
    /// @details
    ///   A simple function that writes all the necessary keywords and comments
    ///    to the FITS header pointed to by fptr.
    ///   The keyword names and comments are taken from duchamp.hh

    OUTCOME result=SUCCESS;
    int status = 0;
    if(fits_write_history(this->itsFptr, (char *)header_moment0History.c_str(), &status)){
      duchampFITSerror(status,"writeMomentMapArray","Error : header I/O");
      result=FAILURE;
    }
    status = 0;
    if(fits_write_history(this->itsFptr, (char *)header_moment0History_input.c_str(),&status)){
      duchampFITSerror(status,"writeMomentMapArray","Error : header I/O");
      result=FAILURE;
    }
    status = 0;
    if(fits_write_history(this->itsFptr, (char *)this->itsCube->pars().getImageFile().c_str(), &status)){
      duchampFITSerror(status,"writeMomentMapArray","Error : header I/O");
      result=FAILURE;
    }
    
    if(this->itsCube->pars().getFlagSubsection()){
      status = 0;
      if( fits_write_comment(this->itsFptr,(char *)header_moment0Subsection_comment.c_str(), &status)){
	duchampFITSerror(status,"writeMomentMapArray","Error : header I/O");
	result=FAILURE;
      }
      status = 0;
      if( fits_write_key(this->itsFptr, TSTRING, (char *)keyword_subsection.c_str(), 
			 (char *)this->itsCube->pars().getSubsection().c_str(),
			 (char *)comment_subsection.c_str(), &status) ){
	duchampFITSerror(status,"writeMomentMapArray","Error : header I/O");
	result=FAILURE;
      }
    }

    return result;

  }

  OUTCOME WriteMomentMapArray::writeData()
  {
    OUTCOME result = SUCCESS;
    long size = this->itsCube->getSpatialSize();
    float *momentMap = new float[size];
    std::vector<bool> detectionMap = this->itsCube->getMomentMap(momentMap);
    int status=0;
    long group=0;
    if(this->itsCube->pars().getFlagBlankPix())
      fits_write_imgnull_flt(this->itsFptr, group, 1, size, momentMap, this->itsCube->pars().getBlankPixVal(), &status);
    else 
      fits_write_img_flt(this->itsFptr, group, 1, size, momentMap, &status);
    if(status){
      duchampFITSerror(status,"writeMomentMapArray","Error writing data");
      result=FAILURE;
    }

    return result;

  }

}

