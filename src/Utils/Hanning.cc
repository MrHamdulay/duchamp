#include <iostream>
#include <math.h>
#include <Utils/Hanning.hh>

Hanning::Hanning(){
  allocated=false;
};
Hanning::~Hanning(){
  if(allocated) delete [] coeffs;
};

Hanning::Hanning(int size){
  /**
   * Constructor that sets the Hanning width and calculates the coefficients.
   * \param size The full width of the filter. The parameter \f$a\f$ is 
   *  defined as (size+1)/2.
   */ 
  if(size%2==0){ 
    std::cerr << "Hanning: need an odd number for the size. "
	      << "Changing "<< size << " to " << ++size<<".\n";
  }
  hanningSize = size;
  coeffs = new float[size];
  allocated = true;
  float a = (size+1.)/2.;
  for(int i=0;i<size;i++){
    float x = i-(size-1)/2.;
    coeffs[i] = 0.5 + 0.5*cos(x * M_PI / a);
  }
};


float *Hanning::smooth(float *array, int npts){
  /**
   * This smooths an array of float by doing a discrete convolution of
   * the input array with the filter coefficients.
   * 
   * Currently only works for C arrays of floats, as that is all I
   * need it for.  Could be made more general if needs be.
   *
   * \param array The input array. Needs to be defined -- no memory
   * checks are done!  \param npts The size of the input array.
   * \return Returns an array of the same size. If filter coefficients
   * have not been allocated, the input array is returned.
   */
  if(!this->allocated) return array;
  else{
    float *newarray = new float[npts];
    float scale = (hanningSize+1.)/2.;
    for(int i=0;i<npts;i++){
      newarray[i] = 0.;
      for(int j=0;j<hanningSize;j++){
	int x = j-(hanningSize-1)/2;
	if((i+x>0)&&(i+x<npts)) newarray[i]+=coeffs[j] * array[i+x];
      }
      newarray[i] /= scale;
    }
    return newarray;
  }
};
