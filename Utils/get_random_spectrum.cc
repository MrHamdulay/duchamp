#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>

float getNormalRV()
{
  float v1,v2,s;
  do{
    v1 = 2.*(1.*rand())/(RAND_MAX+1.0) - 1.;
    v2 = 2.*(1.*rand())/(RAND_MAX+1.0) - 1.;
    s = v1*v1+v2*v2;
  }while(s>1);
  return sqrt(-2.*log(s)/s)*v1;

}

float getNormalRVtrunc()
{
  float v1,v2,s;
  do{
    v1 = 2.*(1.*rand())/(RAND_MAX+1.0) - 1.;
    v2 = 2.*(1.*rand())/(RAND_MAX+1.0) - 1.;
    s = v1*v1+v2*v2;
  }while((s>1)||(fabsf(sqrt(-2.*log(s)/s)*v1)>sqrt(2*M_LN2)));
  return sqrt(-2.*log(s)/s)*v1;

}

float getNormalRV(float mean, float sigma)
{
  float v1,v2,s;
  do{
    v1 = 2.*(1.*rand())/(RAND_MAX+1.0) - 1.;
    v2 = 2.*(1.*rand())/(RAND_MAX+1.0) - 1.;
    s = v1*v1+v2*v2;
  }while(s>1);
  float z=sqrt(-2.*log(s)/s)*v1;
  return z*sigma + mean;
}

void getRandomSpectrum(int length, float *x, float *y)
{
  srandom(time(0));
  rand();

  for(int i=0;i<length;i++){
    x[i] = i;
    // simulate a standard normal RV via polar method
    float v1,v2,s;
    do{
      v1 = 2.*(1.*rand())/(RAND_MAX+1.0) - 1.;
      v2 = 2.*(1.*rand())/(RAND_MAX+1.0) - 1.;
      s = v1*v1+v2*v2;
    }while(s>1);
    y[i] = sqrt(-2.*log(s)/s)*v1;
  }
}

void getRandomSpectrum(int length, float *x, double *y)
{
  srandom(time(0));
  rand();
  for(int i=0;i<length;i++){
    x[i] = (float)i;
    // simulate a standard normal RV via polar method
    double v1,v2,s;
    do{
      v1 = 2.*(1.*rand())/(RAND_MAX+1.0) - 1.;
      v2 = 2.*(1.*rand())/(RAND_MAX+1.0) - 1.;
      s = v1*v1+v2*v2;
    }while(s>1);
    y[i] = sqrt(-2.*log(s)/s)*v1;
//     cerr<< x[i]<<" " << y[i]<<endl;
  }
}


void getRandomSpectrum(int length, float mean, float sigma, float *x, double *y)
{
  srandom(time(0));
  rand();
  for(int i=0;i<length;i++){
    x[i] = (float)i;
    // simulate a standard normal RV via polar method
    double v1,v2,s;
    do{
      v1 = 2.*(1.*rand())/(RAND_MAX+1.0) - 1.;
      v2 = 2.*(1.*rand())/(RAND_MAX+1.0) - 1.;
      s = v1*v1+v2*v2;
    }while(s>1);
    float z = sqrt(-2.*log(s)/s)*v1;
    y[i] = z * sigma + mean;
//     cerr<< x[i]<<" " << y[i]<<endl;
  }
}

void getRandomSpectrum(int length, float mean, float sigma, float *x, float *y)
{
  srandom(time(0));
  rand();
  for(int i=0;i<length;i++){
    x[i] = (float)i;
    // simulate a standard normal RV via polar method
    float v1,v2,s;
    do{
      v1 = 2.*(1.*rand())/(RAND_MAX+1.0) - 1.;
      v2 = 2.*(1.*rand())/(RAND_MAX+1.0) - 1.;
      s = v1*v1+v2*v2;
    }while(s>1);
    float z = sqrt(-2.*log(s)/s)*v1;
    y[i] = z * sigma + mean;
//     cerr<< x[i]<<" " << y[i]<<endl;
  }
}

