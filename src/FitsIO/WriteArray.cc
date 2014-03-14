#include <duchamp/FitsIO/WriteArray.hh>
#include <duchamp/duchamp.hh>
#include <duchamp/Cubes/cubes.hh>
#include <fitsio.h>
#include <string.h>
#include <wcslib/wcsunits.h>

namespace duchamp {

  WriteArray::WriteArray():
    itsCube(0),itsFilename(""),itsBitpix(-32),itsFlag2D(false),itsFptr(0)
  {
  }

  WriteArray::WriteArray(Cube *cube):
    itsCube(cube),itsFilename(""),itsBitpix(-32),itsFlag2D(false),itsFptr(0)
  {
  }

  WriteArray::WriteArray(Cube *cube, int bitpix):
    itsCube(cube),itsFilename(""),itsBitpix(bitpix),itsFlag2D(false),itsFptr(0)
  {
  }

  WriteArray::WriteArray(const WriteArray &other)
  {
    this->operator=(other);
  }

  WriteArray& WriteArray::operator= (const WriteArray& other)
  {
    if(this==&other) return *this;
    this->itsCube = other.itsCube;
    this->itsBitpix = other.itsBitpix;
    this->itsFilename = other.itsFilename;
    this->itsFptr = other.itsFptr;
    this->itsFlag2D = other.itsFlag2D;
    return *this;
  }

  OUTCOME WriteArray::write()
  {
    if(this->openFile()==FAILURE) return FAILURE;
    if(this->writeBasicHeader()==FAILURE) return FAILURE;
    if(this->writeHeader()==FAILURE) return FAILURE;
    if(this->writeData()==FAILURE) return FAILURE;
    if(this->closeFile()==FAILURE) return FAILURE;
    return SUCCESS;
  }

  OUTCOME WriteArray::openFile()
  {
    OUTCOME result=SUCCESS;
    int status=0;
    this->itsFilename = "!"+this->itsFilename;
    fits_create_file(&this->itsFptr,this->itsFilename.c_str(),&status);
    if(status){
      DUCHAMPWARN("Reading Cube", "Error creating file " << this->itsFilename);
      fits_report_error(stderr, status);
      result = FAILURE;
    }
    
    return result;
  }

  OUTCOME WriteArray::writeBasicHeader()
  {
    char *header, *hptr, keyname[9];
    int  i, nkeyrec, status = 0;
    
    size_t naxis=this->itsCube->getNumDim();
    long* naxes = new long[this->itsCube->getNumDim()];
    for(size_t i=0;i<naxis;i++) naxes[i]=this->itsCube->getDimArray()[i];
    if(this->itsFlag2D){
      naxes[this->itsCube->header().WCS().spec]=1;
      naxis=2;
    }
    // write the required header keywords 
    fits_write_imghdr(this->itsFptr, this->itsBitpix, naxis, naxes,  &status);

    // Write beam information
    this->itsCube->header().beam().writeToFITS(this->itsFptr);

    // Write bunit information
    status = 0;
    strcpy(keyname,"BUNIT");
    if (fits_update_key(this->itsFptr, TSTRING, keyname, (char *)this->itsCube->header().getFluxUnits().c_str(), NULL, &status)){
      DUCHAMPWARN("saveImage","Error writing bunit info:");
      fits_report_error(stderr, status);
      return FAILURE;
    }

    // convert the wcsprm struct to a set of 80-char keys
    int oldnaxis = this->itsCube->header().WCS().naxis;
    if(this->itsFlag2D) this->itsCube->header().WCS().naxis=2;
    if ((status = wcshdo(WCSHDO_all, this->itsCube->header().getWCS(), &nkeyrec, &header))) {
      DUCHAMPWARN("saveImage","Could not convert WCS information to FITS header. WCS Error Code = "<<status<<": "<<wcs_errmsg[status]);
      return FAILURE;
    }
    if(this->itsFlag2D) this->itsCube->header().WCS().naxis = oldnaxis;

    hptr = header;
    strncpy(keyname,hptr,8);
    for (i = 0; i < nkeyrec; i++, hptr += 80) {
      status=0;
      if(fits_update_card(this->itsFptr,keyname,hptr,&status)){
	DUCHAMPWARN("saveImage","Error writing header card");
	fits_report_error(stderr,status);
	return FAILURE;
      }
    }

    delete [] naxes;

    return SUCCESS;

  }

  OUTCOME WriteArray::closeFile()
  {
    OUTCOME result=SUCCESS;
    int status = 0;
    if(this->itsFptr!=0){
      fits_close_file(this->itsFptr, &status);
      if (status){
	DUCHAMPWARN("Reading Cube", "Error closing file " << this->itsFilename);
	fits_report_error(stderr, status);
	result=FAILURE;
      }
    }
    return result;
  }
    
    OUTCOME WriteArray::writeToFITS_flt(size_t size, float *array)
    {
        OUTCOME result = SUCCESS;
        
        long group=0;
        int status=0;
        LONGLONG first=1;
        LONGLONG nelem = LONGLONG(size);
        if(this->itsCube->pars().getFlagBlankPix()){
            float nulval = this->itsCube->pars().getBlankPixVal();
            fits_write_imgnull_flt(this->itsFptr, group, first, nelem, array, nulval, &status);
        }
        else
        fits_write_img_flt(this->itsFptr, group, first, nelem, array, &status);
        
        if(status){
            duchampFITSerror(status,"writeArray","Error writing array:");
            result = FAILURE;
        }
        
        return result;
        
    }

    OUTCOME WriteArray::writeToFITS_int(size_t size, int *array)
    {
        OUTCOME result = SUCCESS;
        
        long group=0;
        int status=0;
        LONGLONG first=1;
        LONGLONG nelem = LONGLONG(size);
        fits_write_img_int(this->itsFptr, group, first, nelem, array, &status);
        
        if(status){
            duchampFITSerror(status,"writeArray","Error writing array:");
            result = FAILURE;
        }
        
        return result;
        
    }

    OUTCOME WriteArray::writeToFITS_long(size_t size, long *array)
    {
        OUTCOME result = SUCCESS;
        
        long group=0;
        int status=0;
        LONGLONG first=1;
        LONGLONG nelem = LONGLONG(size);
        fits_write_img_lng(this->itsFptr, group, first, nelem, array, &status);
        
        if(status){
            duchampFITSerror(status,"writeArray","Error writing array:");
            result = FAILURE;
        }
        
        return result;
        
    }

}
