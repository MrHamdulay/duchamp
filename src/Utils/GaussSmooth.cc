#include <iostream>
#include <math.h>
#include <Utils/GaussSmooth.hh>

GaussSmooth::GaussSmooth()
{
  allocated=false;
};
GaussSmooth::~GaussSmooth()
{
  if(allocated) delete [] kernel;
};

GaussSmooth::GaussSmooth(float maj, float min, float pa)
{
  this->allocated=false;
  this->define(maj, min, pa);
};

GaussSmooth::GaussSmooth(float maj)
{
  this->allocated=false;
  this->define(maj, maj, 0);
};

void GaussSmooth::define(float maj, float min, float pa)
{

  this->kernMaj = maj;
  this->kernMin = min;
  this->kernPA  = pa;

  // The parameters kernMaj & kernMin are the FWHM in the major and
  // minor axis directions. We correct these to the sigma_x and
  // sigma_y parameters for the 2D gaussian by halving and dividing by
  // sqrt(2 ln(2)). Actually work with sigma_x^2 to make things
  // easier.
  float sigmaX2 = (this->kernMaj*this->kernMaj/4.) / (2.*M_LN2);
  float sigmaY2 = (this->kernMin*this->kernMin/4.) / (2.*M_LN2);

  // First determine the size of the kernel.
  // For the moment, just calculate the size based on the number of
  // pixels needed to make the exponential drop to less than the
  // stated precision. Use the major axis to get the largest square
  // that includes the ellipse.
  const float precision = 1.e-4;
  int kernelHW = int(ceil(sqrt(-1.*log(precision)*sigmaX2)));
  this->kernWidth = 2*kernelHW + 1;
//   std::cerr << "Making a kernel of width " << this->kernWidth << "\n";

  if(this->allocated) delete [] this->kernel;
  this->kernel = new float[this->kernWidth*this->kernWidth];
  this->allocated = true;
  this->stddevScale=0.;
  float posang = this->kernPA * M_PI/180.;


  for(int i=0;i<this->kernWidth;i++){
    for(int j=0;j<this->kernWidth;j++){
      float xpt = (i-kernelHW)*sin(posang) - (j-kernelHW)*cos(posang);

      float ypt = (i-kernelHW)*cos(posang) + (j-kernelHW)*sin(posang);
      float rsq = (xpt*xpt/sigmaX2) + (ypt*ypt/sigmaY2);
      kernel[i*this->kernWidth+j] = exp( -0.5 * rsq);
      this->stddevScale += 
	kernel[i*this->kernWidth+j]*kernel[i*this->kernWidth+j];
    }
  }
  this->stddevScale = sqrt(this->stddevScale);
//   std::cerr << "Stddev scaling factor = " << this->stddevScale << "\n";
}

float *GaussSmooth::smooth(float *input, int xdim, int ydim)
{
  /**
   * Smooth a given two-dimensional array, of dimensions xdim
   * \f$\times\f$ ydim, with an elliptical gaussian. Simply runs as a
   * front end to GaussSmooth::smooth(float *, int, int, bool *) by
   * defining a mask that allows all pixels in the input array.
   *
   *  \param input The 2D array to be smoothed.
   *  \param xdim  The size of the x-dimension of the array.
   *  \param ydim  The size of the y-dimension of the array.
   *  \return The smoothed array.
   */
  float *smoothed;
  bool *mask = new bool[xdim*ydim];
  for(int i=0;i<xdim*ydim;i++) mask[i]=true;
  smoothed = this->smooth(input,xdim,ydim,mask);
  delete [] mask;
  return smoothed;
}

float *GaussSmooth::smooth(float *input, int xdim, int ydim, bool *mask)
{
  /**
   *  Smooth a given two-dimensional array, of dimensions xdim
   *  \f$\times\f$ ydim, with an elliptical gaussian, where the
   *  boolean array mask defines which values of the array are valid.
   *
   *  This function convolves the input array with the kernel that
   *  needs to have been defined. If it has not, the input array is
   *  returned unchanged.
   *
   *  The mask should be the same size as the input array, and have
   *  values of true for entries that are considered valid, and false
   *  for entries that are not. For instance, arrays from FITS files
   *  should have the mask entries corresponding to BLANK pixels set
   *  to false.
   *
   *  \param input The 2D array to be smoothed.
   *  \param xdim  The size of the x-dimension of the array.
   *  \param ydim  The size of the y-dimension of the array.
   *  \param mask The array showing which pixels in the input array
   *              are valid.
   *  \return The smoothed array.
   */


  if(!this->allocated) return input;
  else{

    float *output = new float[xdim*ydim];

    int pos,comp,xcomp,ycomp,fpos,ct;
    float fsum;
    int kernelHW = this->kernWidth/2;

    for(int ypos = 0; ypos<ydim; ypos++){
      for(int xpos = 0; xpos<xdim; xpos++){
	pos = ypos*xdim + xpos;
      
	if(!mask[pos]) output[pos] = input[pos];
	else{
	
	  ct=0;
	  fsum=0.;
	  output[pos] = 0.;
	
	  for(int yoff = -kernelHW; yoff<=kernelHW; yoff++){
	    ycomp = ypos + yoff;
	    if((ycomp>=0)&&(ycomp<ydim)){

	      for(int xoff = -kernelHW; xoff<=kernelHW; xoff++){
		xcomp = xpos + xoff;	      
		if((xcomp>=0)&&(xcomp<xdim)){

		  fpos = (xoff+kernelHW) + (yoff+kernelHW)*this->kernWidth;
		  comp = ycomp*xdim + xcomp;
		  if(mask[comp]){
		    ct++;
		    fsum += this->kernel[fpos];
		    output[pos] += input[comp]*this->kernel[fpos];
		  }

		}
	      } // xoff loop

	    }
	  }// yoff loop
	  if(ct>0) output[pos] /= fsum;

	} // else{

      } //xpos loop
    }   //ypos loop
   
    return output;
  }

}
