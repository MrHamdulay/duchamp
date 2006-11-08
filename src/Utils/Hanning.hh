#ifndef HANNING_H
#define HANNING_H

#include <math.h>

class Hanning
{
public:
  Hanning(){};
  Hanning(int size){
    if(size%2==0){ 
      std::cerr << "Hanning: need an odd number for the size. "
		<< "Changing "<< size << " to " << ++size<<".\n";
    }
    hanningSize = size;
    coeffs = new float[size];
    float a = (size+1.)/2.;
    for(int i=0;i<size;i++){
      float x = i-(size-1)/2.;
      coeffs[i] = 0.5 + 0.5*cos(x * M_PI / a);
    }
  };
  virtual ~Hanning(){delete [] coeffs;};

  float *smooth(float *array, int npts){
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
  };
    

private:
  int hanningSize;
  float *coeffs;

};

#endif  // HANNING_H
