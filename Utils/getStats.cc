#include <cpgplot.h>
#include <iostream>
#include <math.h>
#include <Utils/utils.hh>

void findMinMax(const float *array, const int size, float &min, float &max)
{
  min = max = array[0];
  for(int i=1;i<size;i++) {
    if(array[i]<min) min=array[i];
    if(array[i]>max) max=array[i];
  }
}

float findMean(float *&array, int size)
{
  float mean = array[0];
  for(int i=1;i<size;i++) mean += array[i];
  mean /= float(size);
  return mean;
}

float findSigma(float *&array, int size)
{
  float mean = findMean(array,size);
  float sig = (array[0]-mean) * (array[0]-mean);
  for(int i=1;i<size;i++) sig += (array[i]-mean)*(array[i]-mean);
  sig = sqrt(sig/float(size-1));
  return sig;
}

float findMedian(float *&array, int size)
{
  // NOTE: madfm = median absolute deviation from median
  float *newarray = new float[size];
  float median;
  for(int i=0;i<size;i++) newarray[i] = array[i];
  sort(newarray,0,size);
  if((size%2)==0) median = 0.5*(newarray[size/2-1]+newarray[size/2]);
  else median = newarray[size/2];
  delete [] newarray;
  return median;
}

float findMADFM(float *&array, int size)
{
  // NOTE: madfm = median absolute deviation from median
  float *newarray = new float[size];
  float median = findMedian(array,size);
  float madfm;
  for(int i=0;i<size;i++) newarray[i] = fabs(array[i]-median);
  sort(newarray,0,size);
  if((size%2)==0) madfm = 0.5*(newarray[size/2-1]+newarray[size/2]);
  else madfm = newarray[size/2];
  delete [] newarray;
  return madfm;
}
  
void findMedianStats(float *&array, int size, float &median, float &madfm)
{
  // NOTE: madfm = median absolute deviation from median
  float *newarray = new float[size];

  for(int i=0;i<size;i++) newarray[i] = array[i];
  sort(newarray,0,size);
  if((size%2)==0) median = 0.5*(newarray[size/2-1]+newarray[size/2]);
  else median = newarray[size/2];

  for(int i=0;i<size;i++) newarray[i] = fabs(array[i]-median);
  sort(newarray,0,size);
  if((size%2)==0) madfm = 0.5*(newarray[size/2-1]+newarray[size/2]);
  else madfm = newarray[size/2];

  delete [] newarray;
}
  
void findMedianStats(float *&array, long size, float &median, float &madfm)
{
  // NOTE: madfm = median absolute deviation from median
  float *newarray = new float[size];

  for(int i=0;i<size;i++) newarray[i] = array[i];
  sort(newarray,0,size);
  if((size%2)==0) median = 0.5*(newarray[size/2-1]+newarray[size/2]);
  else median = newarray[size/2];

  for(int i=0;i<size;i++) newarray[i] = fabs(array[i]-median);
  sort(newarray,0,size);
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

void findTrimmedHistStatsOLD(float *array, const int size, float &tmean, float &tsigma)
{

  const int nbin = 100;
  float *num = new float[nbin];
  bool *keep = new bool[nbin];

  // HOW MANY VALUES IN EACH BIN?
  float min,max;
  findMinMax(array,size,min,max);
  for(int i=0; i<nbin; i++) num[i]=0;
  for(int i=0; i<size; i++){
    float fraction = (array[i] - min) / (max - min);
    int bin = (int)( floor(fraction*nbin) );
    if((bin>=0)&&(bin<nbin)) num[bin]++;
  }
  int binmax = 0;
  for(int i=1; i<nbin; i++)  if(num[i]>num[binmax]) binmax = i;
  for(int i=0; i<nbin; i++) keep[i] = (num[i]>=num[binmax]/2);
  float *newarray = new float[size];
  int newsize = 0;
  for(int i=0; i<size; i++){
    float fraction = (array[i] - min) / (max - min);
    int bin = (int)( floor(fraction*nbin) );
    if(keep[bin]) newarray[newsize++] = array[i];
  }
  // if(newsize==0)
    std::cerr << size << "<->" << newsize << std::endl;

  findNormalStats(newarray,newsize,tmean,tsigma);
//   cpgopen("tmp.ps/vps");
//   cpghistlog(newsize,newarray,min,max,100,0);
//   cpghist(newsize,newarray,min,max,100,0);
//   cpgend();
  delete [] num,keep,newarray;

//   tsigma *= trimToNormal;

}
void findTrimmedHistStats2(float *array, const int size, float &tmean, float &tsigma)
{

  const int nbin = 200;
  float *num = new float[nbin];
  bool *keep = new bool[nbin];

  // HOW MANY VALUES IN EACH BIN?
  float min,max;
  findMinMax(array,size,min,max);
  for(int i=0; i<nbin; i++) num[i]=0;
  for(int i=0; i<size; i++){
    float fraction = (array[i] - min) / (max - min);
    int bin = (int)( floor(fraction*nbin) );
    if((bin>=0)&&(bin<nbin)) num[bin]++;
  }
  int binmax = 0;
  for(int i=1; i<nbin; i++)  if(num[i]>num[binmax]) binmax = i;
  for(int i=0; i<nbin; i++) keep[i] = (num[i]>=num[binmax]/2);
  float *newarray = new float[size];
  int newsize = 0;
  for(int i=0; i<size; i++){
    float fraction = (array[i] - min) / (max - min);
    int bin = (int)( floor(fraction*nbin) );
    if(keep[bin]) newarray[newsize++] = array[i];
  }

  tmean = findMean(newarray,newsize);

  tsigma = newsize * (max-min)/float(nbin) / 
    (num[binmax] * erf(sqrt(M_LN2)) * sqrt(2.*M_PI));

}

void findTrimmedHistStats(float *array, const int size, float &tmean, float &tsigma)
{
  float datamin,datamax;
  findMinMax(array,size,datamin,datamax);
  float *sorted = new float[size];
  float *cumul  = new float[size];
  float *angle  = new float[size];
  for(int i=0;i<size;i++) sorted[i] = array[i]/(datamax-datamin);
  sort(sorted,0,size);
  for(int i=0;i<size;i++) cumul[i] = (float)i/(float)size;
  int width =(int)( 20. * log10((float)size));
  for(int i=0;i<size;i++){
    int beg,end;
    float slope,eSlope,inter,eInter,r;
    if(i<width/2) beg=0; else beg=i-width/2;
    if(i>=size-width/2) end=size-1; else end=i+width/2;
    if(linear_regression(size,sorted,cumul,beg,end,slope,eSlope,inter,eInter,r)==0)
      angle[i] = atanf( fabs(slope) ) * 180. / M_PI;
    else angle[i] = 90.;
  }

//   int start = 0;
//   while(angle[start] < 45.) start++;
//   int finish = size-1;
//   while(angle[finish] < 45.) finish--;

  int maxpt = 0;
  for(int i = 1; i<size; i++) if(angle[i]>angle[maxpt]) maxpt=i;
  int start = maxpt;
  while((start>0)&&(angle[start]>45.)) start--;
  int finish = maxpt;
  while((finish < size-1)&&(angle[finish]>45.)) finish++;

  std::cerr << "npts = " << size << ", start = " << start << ", finish = " << finish << std::endl;

  int trimSize=0;
  float *newarray = new float[finish-start+1];
  for(int i=0;i<finish-start+1;i++) newarray[i] = sorted[i+start]*(datamax-datamin);

  findNormalStats(newarray,finish-start+1,tmean,tsigma);
  
}
