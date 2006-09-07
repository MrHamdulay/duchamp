#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <Utils/utils.hh>
#define WCSLIB_GETWCSTAB // define this so that we don't try and redefine 
                         //  wtbarr (this is a problem when using gcc v.4+
#include <fitsio.h>
#include <string>
using std::string;

void addGaussian(float *array, long *dim, float *pos, float *fwhm, float norm);
void write_header_info(fitsfile *fptr);
int main()
{

  srandom(37);  
  long *dim = new long[3];
  dim[0] = 100;
  dim[1] = 100;
  dim[2] = 128;
  int size = dim[0]*dim[1]*dim[2];

  float *array = new float[size];
  for(int i=0;i<size;i++) array[i] = getNormalRV();
  float xpos[5] = {25.,75.,50.,25.,75.};
  float ypos[5] = {25.,25.,50.,75.,75.};
  float zpos[5] = {20.,42.,64.,86.,108.};
  float fwhm[3] = {5.,5.,5.};
  float pos[3];
  for(int i=0;i<5;i++){
    pos[0] = xpos[i];
    pos[1] = ypos[i];
    pos[2] = zpos[i];
    addGaussian(array, dim, pos, fwhm, 10.);
  }
  
  int status = 0,bitpix;
  long *fpixel = new long[3];
  for(int i=0;i<3;i++) fpixel[i]=1;
  fitsfile *fptr;         
  string cubeName = "!verification/verificationCube.fits";
  if( fits_create_file(&fptr,cubeName.c_str(),&status) ) 
    fits_report_error(stderr, status);

  status = 0;
  if( fits_create_img(fptr, FLOAT_IMG, 3, dim, &status) ) 
    fits_report_error(stderr, status);

  write_header_info(fptr);
  
  status = 0;
  if( fits_write_pix(fptr, TFLOAT, fpixel, size, array, &status) )
    fits_report_error(stderr, status);

  status = 0;
  fits_close_file(fptr, &status);
  if (status){
    std::cerr << "Error closing file: ";
    fits_report_error(stderr, status);
  }
  

  
}


template <class T> T min(T x1, T x2)
{
  if(x1<x2) return x1;
  else return x2;
}

template <class T> T max(T x1, T x2)
{
  if(x1>x2) return x1;
  else return x2;
}

void addGaussian(float *array, long *dim, float *pos, float *fwhm, float norm)
{
  long size = dim[0] * dim[1] * dim[2];
  float sigsig = fwhm[0]*fwhm[0] + fwhm[1]*fwhm[1] + fwhm[2]*fwhm[2];

  int x1 = int(max(0.,pos[0]-10.));
  int x2 = int(min(float(dim[0]),pos[0]+11));
  int y1 = int(max(0.,pos[1]-10.));
  int y2 = int(min(float(dim[1]),pos[1]+11));
  int z1 = int(max(0.,pos[2]-10.));
  int z2 = int(min(float(dim[2]),pos[2]+11));

  for(int x=x1;x<x2;x++){
    for(int y=y1;y<y2;y++){
      for(int z=z1;z<z2;z++){
	
	float distX = (x-pos[0])/fwhm[0];
	float distY = (y-pos[1])/fwhm[1];
	float distZ = (z-pos[2])/fwhm[2];
	float gaussFlux = norm * exp( -1.*(distX*distX + 
					   distY*distY + 
					   distZ*distZ  ) );
	long loc = x + y*dim[0] + z*dim[0]*dim[1];
	array[loc] += gaussFlux;
      }
    }
  }

}

void write_header_info(fitsfile *fptr)
{
  int status = 0;
  float val;
  string str;
  val = 0.24;
  fits_write_key(fptr, TFLOAT,  "BMAJ"  , &val, "", &status);
  fits_write_key(fptr, TFLOAT,  "BMIN"  , &val, "", &status);
  val = 0.;
  fits_write_key(fptr, TFLOAT,  "BPA"   , &val, "", &status);
  str = "JY/BEAM";
  fits_write_key(fptr, TSTRING, "BUNIT" , (char *)str.c_str(), "", &status);
  val = 2000.0;
  fits_write_key(fptr, TFLOAT,  "EPOCH" , &val, "", &status);
  val = 180.0;
  fits_write_key(fptr, TFLOAT, "LONPOLE", &val, "", &status);

  /* First axis -- RA--SIN */
  str = "RA---SIN";
  fits_write_key(fptr, TSTRING, "CTYPE1", (char *)str.c_str(), "", &status);
  val = 90.5;
  fits_write_key(fptr, TFLOAT,  "CRVAL1", &val, "", &status);
  val = -6.6666701e-2;
  fits_write_key(fptr, TFLOAT,  "CDELT1", &val, "", &status);
  val = 0.;
  fits_write_key(fptr, TFLOAT,  "CROTA1", &val, "", &status);
  val = 50.0;
  fits_write_key(fptr, TFLOAT,  "CRPIX1", &val, "", &status);
  str = "DEG";
  fits_write_key(fptr, TSTRING, "CUNIT1", (char *)str.c_str(), "", &status);
  
  /* Second axis -- DEC--SIN */
  str = "DEC--SIN";
  fits_write_key(fptr, TSTRING, "CTYPE2", (char *)str.c_str(), "", &status);
  val = -26.0;
  fits_write_key(fptr, TFLOAT,  "CRVAL2", &val, "", &status);
  val = 6.6666701e-2;
  fits_write_key(fptr, TFLOAT,  "CDELT2", &val, "", &status);
  val = 0.;
  fits_write_key(fptr, TFLOAT,  "CROTA2", &val, "", &status);
  val = 50.0;
  fits_write_key(fptr, TFLOAT,  "CRPIX2", &val, "", &status);
  str = "DEG";
  fits_write_key(fptr, TSTRING, "CUNIT2", (char *)str.c_str(), "", &status);
  
  /* Third axis -- VELO-HEL */
  str = "VELO-HEL";
  fits_write_key(fptr, TSTRING, "CTYPE3", (char *)str.c_str(), "", &status);
  val = 0.;
  fits_write_key(fptr, TFLOAT,  "CRVAL3", &val, "", &status);
  val = 1.3191321e+4;
  fits_write_key(fptr, TFLOAT,  "CDELT3", &val, "", &status);
  val = 0.;
  fits_write_key(fptr, TFLOAT,  "CROTA3", &val, "", &status);
  val = 64.0;
  fits_write_key(fptr, TFLOAT,  "CRPIX3", &val, "", &status);
  str = "M/S";
  fits_write_key(fptr, TSTRING, "CUNIT3", (char *)str.c_str(), "", &status);

  val = 1.420405751786E+09;
  fits_write_key(fptr, TFLOAT,"RESTFREQ", &val, "", &status);


}
