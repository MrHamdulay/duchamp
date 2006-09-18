#include <iostream>
#include <string>
#define WCSLIB_GETWCSTAB // define this so that we don't try and redefine 
                         //  wtbarr (this is a problem when using gcc v.4+
#include <fitsio.h>
#include <math.h>
#include <duchamp.hh>
#include <Cubes/cubes.hh>

int Cube::getFITSdata(string fname)
{

  long nelements;
  int bitpix,numAxes;
  int status = 0,  nkeys;  /* MUST initialize status */
  fitsfile *fptr;  

  status = 0;
  if( fits_open_file(&fptr,fname.c_str(),READONLY,&status) ){
    fits_report_error(stderr, status);
    return FAILURE;
  }
  status = 0;
  if(fits_get_img_dim(fptr, &numAxes, &status)){
    fits_report_error(stderr, status);
    return FAILURE;
  }
  long *dimAxes = new long[numAxes];
  for(int i=0;i<numAxes;i++) dimAxes[i]=1;
  status = 0;
  if(fits_get_img_size(fptr, numAxes, dimAxes, &status)){
    fits_report_error(stderr, status);
    return FAILURE;
  }

  int lng = this->head.getWCS()->lng;
  int lat = this->head.getWCS()->lat;
  int spc = this->head.getWCS()->spec;

  int anynul;
  int npix = dimAxes[lng]*dimAxes[lat]*dimAxes[spc];
  float *array = new float[npix];
  char *nullarray = new char[npix];
  long *fpixel = new long[numAxes];
  long *lpixel = new long[numAxes];
  long *inc = new long[numAxes];    // the data-sampling increment
  for(int i=0;i<numAxes;i++){
    inc[i] = fpixel[i] = lpixel[i] = 1;
  }
  lpixel[lng] = dimAxes[lng];
  lpixel[lat] = dimAxes[lat];
  lpixel[spc] = dimAxes[spc];
    
  int colnum = 0;
  status = 0;
  if(fits_read_subsetnull_flt(fptr, colnum, numAxes, dimAxes,
			      fpixel, lpixel, inc, 
			      array, nullarray, &anynul, &status)){
    duchampError("getFITSdata",
		 "There was an error reading in the data array:");
    fits_report_error(stderr, status);
    return FAILURE;
  }

  if(anynul==0){    
    // no blank pixels, so don't bother with any trimming or checking...
    if(this->par.getFlagBlankPix()) {  
      // if user requested fixing, inform them of change.
      duchampWarning("getFITSdata",
		     "No blank pixels, so setting flagBlankPix to false.\n");
    }
    this->par.setFlagBlankPix(false); 
  }

  this->initialiseCube(dimAxes);
  this->saveArray(array,npix);
  //-------------------------------------------------------------
  // Once the array is saved, change the value of the blank pixels from
  // 0 (as they are set by fits_read_pixnull) to the correct blank value
  // as determined by the above code.
  for(int i=0; i<npix;i++){
//     if(nullarray[i]==1) this->array[i] = blank*bscale + bzero;  
    if(isnan(array[i])) this->array[i] = par.getBlankPixVal();
    if(nullarray[i]==1) this->array[i] = this->par.getBlankPixVal();  
  }

  return SUCCESS;

}
