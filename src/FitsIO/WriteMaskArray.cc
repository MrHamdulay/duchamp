#include <duchamp/FitsIO/WriteMaskArray.hh>
#include <duchamp/FitsIO/WriteArray.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>
#include <fitsio.h>
#include <string.h>

namespace duchamp {

  WriteMaskArray::WriteMaskArray():
    WriteArray(), itsBitpix(SHORT_IMG)
  {
  }
  
  WriteMaskArray::WriteMaskArray(Cube *cube):
    WriteArray(cube, SHORT_IMG)
  {
  }

  void WriteMaskArray::setCorrectBitpix()
    {
        if(this->itsCube->pars().getFlagMaskWithObjectNum()){
            long shortImgMax=32768;
            if(this->itsCube->getNumObj()>=shortImgMax) this->itsBitpix=LONG_IMG;
        }
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
      
      if(this->itsCube->pars().getFlagMaskWithObjectNum()){
          if(this->itsBitpix==SHORT_IMG)
            result = writeDataObjNumMask_int();
          else
            result = writeDataObjNumMask_long();
      }
      else
        result = writeDataSimpleMask();
      
      return result;
  }

    OUTCOME WriteMaskArray::writeDataSimpleMask()
    {
        OUTCOME result = SUCCESS;
        
        size_t size=this->itsCube->getSize();
        int *mask = new int[size];
        for(size_t i=0;i<size;i++) mask[i]=0;
        std::vector<Detection>::iterator obj;
        for(obj=this->itsCube->pObjectList()->begin();obj<this->itsCube->pObjectList()->end();obj++){
            std::vector<PixelInfo::Voxel> voxlist = obj->getPixelSet();
            std::vector<PixelInfo::Voxel>::iterator vox;
            for(vox=voxlist.begin();vox<voxlist.end();vox++){
                size_t pixelpos = vox->getX() + this->itsCube->getDimX()*vox->getY() + this->itsCube->getDimX()*this->itsCube->getDimY()*vox->getZ();
                mask[pixelpos] = 1;
            }
        }

        result = this->writeToFITS_int(size,mask);
        
        delete [] mask;
        
        return result;

    }

    OUTCOME WriteMaskArray::writeDataObjNumMask_int()
    {
        OUTCOME result = SUCCESS;
        
        size_t size=this->itsCube->getSize();
        int *mask = new int[size];
        for(size_t i=0;i<size;i++) mask[i]=0;
        std::vector<Detection>::iterator obj;
        for(obj=this->itsCube->pObjectList()->begin();obj<this->itsCube->pObjectList()->end();obj++){
            std::vector<PixelInfo::Voxel> voxlist = obj->getPixelSet();
            std::vector<PixelInfo::Voxel>::iterator vox;
            for(vox=voxlist.begin();vox<voxlist.end();vox++){
                size_t pixelpos = vox->getX() + this->itsCube->getDimX()*vox->getY() + this->itsCube->getDimX()*this->itsCube->getDimY()*vox->getZ();
                mask[pixelpos] = obj->getID();
            }
        }
        
        result = this->writeToFITS_int(size,mask);
        
        delete [] mask;
        
        return result;
        
    }
    
    OUTCOME WriteMaskArray::writeDataObjNumMask_long()
    {
        OUTCOME result = SUCCESS;
        
        size_t size=this->itsCube->getSize();
        long *mask = new long[size];
        for(size_t i=0;i<size;i++) mask[i]=0;
        std::vector<Detection>::iterator obj;
        for(obj=this->itsCube->pObjectList()->begin();obj<this->itsCube->pObjectList()->end();obj++){
            std::vector<PixelInfo::Voxel> voxlist = obj->getPixelSet();
            std::vector<PixelInfo::Voxel>::iterator vox;
            for(vox=voxlist.begin();vox<voxlist.end();vox++){
                size_t pixelpos = vox->getX() + this->itsCube->getDimX()*vox->getY() + this->itsCube->getDimX()*this->itsCube->getDimY()*vox->getZ();
                mask[pixelpos] = obj->getID();
            }
        }
        
        result = this->writeToFITS_long(size,mask);
        
        delete [] mask;
        
        return result;
        
    }


}

