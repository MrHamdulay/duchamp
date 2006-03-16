#include <iostream>
#include <math.h>
#include <Utils/utils.hh>

void findMedianStats(float *&array, int size, float &median, float &madfm)
{
  // NOTE: madfm = median absolute deviation from median
  float *newarray = new float[size];

  for(int i=0;i<size;i++) newarray[i] = array[i];
  sort(newarray,0,size-1);
  if((size%2)==0) median = 0.5*(newarray[size/2-1]+newarray[size/2]);
  else median = newarray[size/2];

  for(int i=0;i<size;i++) newarray[i] = fabs(array[i]-median);
  sort(newarray,0,size-1);
  if((size%2)==0) madfm = 0.5*(newarray[size/2-1]+newarray[size/2]);
  else madfm = newarray[size/2];

  delete [] newarray;
}
  
void findMedianStats(float *&array, long size, float &median, float &madfm)
{
  // NOTE: madfm = median absolute deviation from median
  float *newarray = new float[size];

  for(int i=0;i<size;i++) newarray[i] = array[i];
  sort(newarray,0,size-1);
  if((size%2)==0) median = 0.5*(newarray[size/2-1]+newarray[size/2]);
  else median = newarray[size/2];

  for(int i=0;i<size;i++) newarray[i] = fabs(array[i]-median);
  sort(newarray,0,size-1);
  if((size%2)==0) madfm = 0.5*(newarray[size/2-1]+newarray[size/2]);
  else madfm = newarray[size/2];

  delete [] newarray;
}
  

void findNormalStats(float *&array, int size, float &mean, float &sig)
{
  mean = array[0];
  for(int i=1;i<size;i++){
    mean += array[i];
  }
  mean /= float(size);

  sig = (array[0]-mean) * (array[0]-mean);
  for(int i=1;i<size;i++) sig += (array[i]-mean)*(array[i]-mean);
  sig = sqrt(sig/float(size-1));

}

