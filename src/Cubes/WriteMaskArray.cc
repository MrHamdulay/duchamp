#include <duchamp/Cubes/WriteMaskArray.hh>
#include <duchamp/Cubes/WriteArray.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>
#include <fitsio.h>

namespace duchamp {

  WriteMaskArray::WriteMaskArray():
    WriteArray()
  {
    this->itsBitpix=LONG_IMG;
  }
  
  WriteMaskArray::WriteMaskArray(Cube *cube):
    WriteArray(cube,LONG_IMG)
  {
  }

  WriteMaskArray::WriteMaskArray(const WriteMaskArray &other)
  {
    this->operator=(other);
  }

  WriteMaskArray::WriteMaskArray(const WriteArray &base)
  {
    this->operator=(base);
  }

  WriteMaskArray& WriteMaskArray::operator= (const WriteMaskArray& other)
  {
    if(this==&other) return *this;
    ((WriteArray &) *this) = other;
    return *this;
  }

  WriteMaskArray& WriteMaskArray::operator= (const WriteArray& base)
  {
    if(this==&base) return *this;
    ((WriteArray &) *this) = base;
    return *this;
  }

  OUTCOME WriteMaskArray::writeHeader()
  {
    /// @details
    ///   A simple function that writes all the necessary keywords and comments
    ///    to the FITS header pointed to by fptr.
    ///   The keyword names and comments are taken from duchamp.hh

    OUTCOME result=SUCCESS;
    int status = 0;
    if(fits_write_history(this->itsFptr, (char *)header_maskHistory.c_str(), &status)){
      duchampFITSerror(status,"writeMaskArray","Error : header I/O");
      result=FAILURE;
    }
    status = 0;
    if(fits_write_history(this->itsFptr, (char *)header_maskHistory_input.c_str(),&status)){
      duchampFITSerror(status,"writeMaskArray","Error : header I/O");
      result=FAILURE;
    }
    status = 0;
    if(fits_write_history(this->itsFptr, (char *)this->itsCube->pars().getImageFile().c_str(), &status)){
      duchampFITSerror(status,"writeMaskArray","Error : header I/O");
      result=FAILURE;
    }
    
    if(this->itsCube->pars().getFlagSubsection()){
      status = 0;
      if( fits_write_comment(this->itsFptr,(char *)header_maskSubsection_comment.c_str(), &status)){
	duchampFITSerror(status,"writeMaskArray","Error : header I/O");
	result=FAILURE;
      }
      status = 0;
      if( fits_write_key(this->itsFptr, TSTRING, (char *)keyword_subsection.c_str(), 
			 (char *)this->itsCube->pars().getSubsection().c_str(),
			 (char *)comment_subsection.c_str(), &status) ){
	duchampFITSerror(status,"writeMaskArray","Error : header I/O");
	result=FAILURE;
      }
    }

    char *comment = new char[FLEN_COMMENT];
    strcpy(comment,"");
    char *keyword = new char[FLEN_KEYWORD];
    std::string newunits = (this->itsCube->pars().getFlagMaskWithObjectNum()) ? "Object ID" : "Detection flag";
    strcpy(keyword,"BUNIT");
    if(fits_update_key(this->itsFptr, TSTRING, keyword, (char *)newunits.c_str(), comment, &status)){
      duchampFITSerror(status,"writeMaskArray","Error writing BUNIT header:");
      result = FAILURE;
    }
    delete [] comment;
    delete [] keyword;

    return result;

  }

  OUTCOME WriteMaskArray::writeData()
  {
    OUTCOME result = SUCCESS;
    int *mask = new int[this->itsCube->getSize()];
    for(size_t i=0;i<this->itsCube->getSize();i++) mask[i]=0;
    std::vector<Detection>::iterator obj;
    for(obj=this->itsCube->pObjectList()->begin();obj<this->itsCube->pObjectList()->end();obj++){
      std::vector<PixelInfo::Voxel> voxlist = obj->getPixelSet();
      std::vector<PixelInfo::Voxel>::iterator vox;
      for(vox=voxlist.begin();vox<voxlist.end();vox++){
	size_t pixelpos = vox->getX() + this->itsCube->getDimX()*vox->getY() + 
	  this->itsCube->getDimX()*this->itsCube->getDimY()*vox->getZ();
	if(this->itsCube->pars().getFlagMaskWithObjectNum()) mask[pixelpos] = obj->getID();
	else mask[pixelpos] = 1;
      }
    }
    int status=0;
    long group=0;
    LONGLONG first=1;
    LONGLONG nelem=LONGLONG(this->itsCube->getSize());
    if(fits_write_img_int(this->itsFptr, group, first, nelem, mask, &status)){
      duchampFITSerror(status,"writeMaskArray","Error writing mask array:");
      result = FAILURE;
    }

    return result;

  }

}

