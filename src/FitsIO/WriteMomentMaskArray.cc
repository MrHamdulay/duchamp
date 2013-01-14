#include <duchamp/FitsIO/WriteMomentMaskArray.hh>
#include <duchamp/FitsIO/WriteArray.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>
#include <fitsio.h>
#include <string.h>

namespace duchamp {

  WriteMomentMaskArray::WriteMomentMaskArray():
    WriteArray()
  {
    this->itsBitpix=LONG_IMG;
    this->itsFlag2D=true;
  }
  
  WriteMomentMaskArray::WriteMomentMaskArray(Cube *cube):
    WriteArray(cube,LONG_IMG)
  {
    this->itsFlag2D=true;
  }

  WriteMomentMaskArray::WriteMomentMaskArray(const WriteMomentMaskArray &other)
  {
    this->operator=(other);
  }

  WriteMomentMaskArray::WriteMomentMaskArray(const WriteArray &base)
  {
    this->operator=(base);
  }

  WriteMomentMaskArray& WriteMomentMaskArray::operator= (const WriteMomentMaskArray& other)
  {
    if(this==&other) return *this;
    ((WriteArray &) *this) = other;
    return *this;
  }

  WriteMomentMaskArray& WriteMomentMaskArray::operator= (const WriteArray& base)
  {
    if(this==&base) return *this;
    ((WriteArray &) *this) = base;
    return *this;
  }

  OUTCOME WriteMomentMaskArray::writeHeader()
  {
    /// @details
    ///   A simple function that writes all the necessary keywords and comments
    ///    to the FITS header pointed to by fptr.
    ///   The keyword names and comments are taken from duchamp.hh

    OUTCOME result=SUCCESS;

    int status = 0;
    if(fits_write_history(this->itsFptr, (char *)header_moment0History.c_str(), &status)){
      duchampFITSerror(status,"writeMomentMaskArray","Error : header I/O");
      result=FAILURE;
    }
    status = 0;
    if(fits_write_history(this->itsFptr, (char *)header_moment0History_input.c_str(),&status)){
      duchampFITSerror(status,"writeMomentMaskArray","Error : header I/O");
      result=FAILURE;
    }
    status = 0;
    if(fits_write_history(this->itsFptr, (char *)this->itsCube->pars().getImageFile().c_str(), &status)){
      duchampFITSerror(status,"writeMomentMaskArray","Error : header I/O");
      result=FAILURE;
    }
    
    if(this->itsCube->pars().getFlagSubsection()){
      status = 0;
      if( fits_write_comment(this->itsFptr,(char *)header_moment0Subsection_comment.c_str(), &status)){
	duchampFITSerror(status,"writeMomentMaskArray","Error : header I/O");
	result=FAILURE;
      }
      status = 0;
      if( fits_write_key(this->itsFptr, TSTRING, (char *)keyword_subsection.c_str(), 
			 (char *)this->itsCube->pars().getSubsection().c_str(),
			 (char *)comment_subsection.c_str(), &status) ){
	duchampFITSerror(status,"writeMomentMaskArray","Error : header I/O");
	result=FAILURE;
      }
    }

    char *comment = new char[FLEN_COMMENT];
    strcpy(comment,"");
    char *keyword = new char[FLEN_KEYWORD];
    std::string newunits = "Detection flag";
    strcpy(keyword,"BUNIT");
    if(fits_update_key(this->itsFptr, TSTRING, keyword, (char *)newunits.c_str(), comment, &status)){
      duchampFITSerror(status,"writeMomentMaskArray","Error writing BUNIT header:");
      result = FAILURE;
    }
    delete [] comment;
    delete [] keyword;

    return result;

  }

  OUTCOME WriteMomentMaskArray::writeData()
  {
    OUTCOME result = SUCCESS;
    int *mask = new int[this->itsCube->getSpatialSize()];
    for(size_t i=0;i<this->itsCube->getSpatialSize();i++) mask[i]=0;
    std::vector<Detection>::iterator obj;
    for(obj=this->itsCube->pObjectList()->begin();obj<this->itsCube->pObjectList()->end();obj++){
      std::vector<PixelInfo::Voxel> voxlist = obj->getPixelSet();
      std::vector<PixelInfo::Voxel>::iterator vox;
      for(vox=voxlist.begin();vox<voxlist.end();vox++){
	size_t pixelpos = vox->getX() + this->itsCube->getDimX()*vox->getY();
	mask[pixelpos] = 1;
      }
    }
    int status=0;
    long group=0;
    LONGLONG first=1;
    LONGLONG nelem=LONGLONG(this->itsCube->getSpatialSize());
    if(fits_write_img_int(this->itsFptr, group, first, nelem, mask, &status)){
      duchampFITSerror(status,"writeMomentMaskArray","Error writing mask array:");
      result = FAILURE;
    }

    return result;

  }

}

