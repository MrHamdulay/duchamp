#include <iostream>
#include <math.h>
#include <Utils/utils.hh>

void findTrimmedHistStatsOLD(float *array, const int size, 
			     float &tmean, float &tsigma)
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

  findNormalStats(newarray,newsize,tmean,tsigma);
  delete [] num;
  delete [] keep;
  delete [] newarray;

}
//--------------------------------------------------------------------

void findTrimmedHistStats2(float *array, const int size, 
			   float &tmean, float &tsigma)
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
//--------------------------------------------------------------------

void findTrimmedHistStats(float *array, const int size, 
			  float &tmean, float &tsigma)
{
  float datamin,datamax;
  findMinMax(array,size,datamin,datamax);
  float *sorted = new float[size];
  float *cumul  = new float[size];
  float *angle  = new float[size];
  for(int i=0;i<size;i++) sorted[i] = array[i]/(datamax-datamin);
  std::sort(sorted,sorted+size);
  for(int i=0;i<size;i++) cumul[i] = (float)i/(float)size;
  int width =(int)( 20. * log10((float)size));
  for(int i=0;i<size;i++){
    int beg,end;
    float slope,eSlope,inter,eInter,r;
    if(i<width/2) beg=0; else beg=i-width/2;
    if(i>=size-width/2) end=size-1; else end=i+width/2;
    if(linear_regression(size,sorted,cumul,beg,end,slope,eSlope,inter,eInter,r)==0)
      angle[i] = atan( fabs(slope) ) * 180. / M_PI;
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

  float *newarray = new float[finish-start+1];
  for(int i=0;i<finish-start+1;i++) 
    newarray[i] = sorted[i+start]*(datamax-datamin);

  findNormalStats(newarray,finish-start+1,tmean,tsigma);
  
}
