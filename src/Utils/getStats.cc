#include <cpgplot.h>
#include <iostream>
#include <math.h>
#include <Utils/utils.hh>

template <class T> T absval(T value)
{
  if( value > 0) return value;
  else return 0-value;
}
//--------------------------------------------------------------------

template <class T> void findMinMax(const T *array, const int size, 
				   T &min, T &max)
{
  min = max = array[0];
  for(int i=1;i<size;i++) {
    if(array[i]<min) min=array[i];
    if(array[i]>max) max=array[i];
  }
}
template void findMinMax<int>(const int *array, const int size, 
			      int &min, int &max);
template void findMinMax<float>(const float *array, const int size, 
				float &min, float &max);
//--------------------------------------------------------------------

template <class T> float findMean(T *array, int size)
{
  float mean = array[0];
  for(int i=1;i<size;i++) mean += array[i];
  mean /= float(size);
  return mean;
}
template float findMean<int>(int *array, int size);
template float findMean<float>(float *array, int size);
//--------------------------------------------------------------------

template <class T> float findStddev(T *array, int size)
{
  float mean = findMean(array,size);
  float stddev = (array[0]-mean) * (array[0]-mean);
  for(int i=1;i<size;i++) stddev += (array[i]-mean)*(array[i]-mean);
  stddev = sqrt(stddev/float(size-1));
  return stddev;
}
template float findStddev<int>(int *array, int size);
template float findStddev<float>(float *array, int size);
//--------------------------------------------------------------------

template <class T> T findMedian(T *array, int size)
{
  // NOTE: madfm = median absolute deviation from median
  T *newarray = new T[size];
  T median;
  for(int i=0;i<size;i++) newarray[i] = array[i];
  sort(newarray,0,size);
  if((size%2)==0) median = (newarray[size/2-1]+newarray[size/2])/2;
  else median = newarray[size/2];
  delete [] newarray;
  return median;
}
template int findMedian<int>(int *array, int size);
template float findMedian<float>(float *array, int size);
//--------------------------------------------------------------------

template <class T> T findMADFM(T *array, int size)
{
  // NOTE: madfm = median absolute deviation from median
  T *newarray = new T[size];
  T median = findMedian(array,size);
  T madfm;
  for(int i=0;i<size;i++) newarray[i] = absval(array[i]-median);
  sort(newarray,0,size);
  if((size%2)==0) madfm = (newarray[size/2-1]+newarray[size/2])/2;
  else madfm = newarray[size/2];
  delete [] newarray;
  return madfm;
}
template int findMADFM<int>(int *array, int size);
template float findMADFM<float>(float *array, int size);
//--------------------------------------------------------------------

template <class T> void findMedianStats(T *array, int size, 
					T &median, T &madfm)
{
  // NOTE: madfm = median absolute deviation from median
  if(size==0){
    std::cerr << "Error in findMedianStats: zero sized array!\n";
    return;
  }
  T *newarray = new T[size];

  for(int i=0;i<size;i++) newarray[i] = array[i];
  sort(newarray,0,size);
  if((size%2)==0) median = (newarray[size/2-1]+newarray[size/2])/2;
  else median = newarray[size/2];

  for(int i=0;i<size;i++) newarray[i] = absval(array[i]-median);
  sort(newarray,0,size);
  if((size%2)==0) madfm = (newarray[size/2-1]+newarray[size/2])/2;
  else madfm = newarray[size/2];

  delete [] newarray;
}
template void findMedianStats<int>(int *array, int size, 
				   int &median, int &madfm);
template void findMedianStats<long>(long *array, int size, 
				    long &median, long &madfm);
template void findMedianStats<float>(float *array, int size, 
				     float &median, float &madfm);
template void findMedianStats<double>(double *array, int size, 
				      double &median, double &madfm);
//--------------------------------------------------------------------

template <class T> void findMedianStats(T *array, int size, bool *isGood, 
					T &median, T &madfm)
{
  // NOTE: madfm = median absolute deviation from median
  int goodSize=0;
  for(int i=0;i<size;i++) if(isGood[i]) goodSize++;
  if(goodSize==0){
    std::cerr << "Error in findMedianStats: no good values!\n";
    return;
  }
  T *newarray = new T[goodSize];
  for(int i=0;i<size;i++) if(isGood[i]) newarray[goodSize++] = array[i];
  sort(newarray,0,goodSize);
  if((goodSize%2)==0) 
    median = (newarray[goodSize/2-1]+newarray[goodSize/2])/2;
  else 
    median = newarray[goodSize/2];

  for(int i=0;i<goodSize;i++) newarray[i] = absval(newarray[i]-median);
  sort(newarray,0,goodSize);
  if((goodSize%2)==0) 
    madfm = (newarray[goodSize/2-1]+newarray[goodSize/2])/2;
  else 
    madfm = newarray[goodSize/2];

  delete [] newarray;
}
template void findMedianStats<int>(int *array, int size, bool *isGood, 
				   int &median, int &madfm);
template void findMedianStats<long>(long *array, int size, bool *isGood, 
				    long &median, long &madfm);
template void findMedianStats<float>(float *array, int size, bool *isGood, 
				     float &median, float &madfm);
template void findMedianStats<double>(double *array, int size, bool *isGood, 
				      double &median, double &madfm);
//--------------------------------------------------------------------
  

template <class T> void findNormalStats(T *array, int size, 
					float &mean, float &stddev)
{
  if(size==0){
    std::cerr << "Error in findNormalStats: zero sized array!\n";
    return;
  }
  mean = array[0];
  for(int i=1;i<size;i++){
    mean += array[i];
  }
  mean /= float(size);

  stddev = (array[0]-mean) * (array[0]-mean);
  for(int i=1;i<size;i++) stddev += (array[i]-mean)*(array[i]-mean);
  stddev = sqrt(stddev/float(size-1));

}
template void findNormalStats<int>(int *array, int size, 
				   float &mean, float &stddev);
template void findNormalStats<long>(long *array, int size, 
				    float &mean, float &stddev);
template void findNormalStats<float>(float *array, int size, 
				     float &mean, float &stddev);
template void findNormalStats<double>(double *array, int size, 
				      float &mean, float &stddev);
//--------------------------------------------------------------------

template <class T> void findNormalStats(T *array, int size, bool *isGood, 
					float &mean, float &stddev)
{
  int goodSize=0;
  for(int i=0;i<size;i++) if(isGood[i]) goodSize++;
  if(goodSize==0){
    std::cerr << "Error in findNormalStats: no good values!\n";
    return;
  }
  int start=0;
  while(!isGood[start]){start++;}
  mean = array[start];
  for(int i=start+1;i<size;i++){
    if(isGood[i]) mean += array[i];
  }
  mean /= float(goodSize);

  stddev = (array[start]-mean) * (array[start]-mean);
  for(int i=1;i<size;i++){
    if(isGood[i]) stddev += (array[i]-mean)*(array[i]-mean);
  }
  stddev = sqrt(stddev/float(goodSize-1));

}
template void findNormalStats<int>(int *array, int size, bool *isGood, 
				   float &mean, float &stddev);
template void findNormalStats<long>(long *array, int size, bool *isGood, 
				    float &mean, float &stddev);
template void findNormalStats<float>(float *array, int size, bool *isGood, 
				     float &mean, float &stddev);
template void findNormalStats<double>(double *array, int size, bool *isGood, 
				      float &mean, float &stddev);
//--------------------------------------------------------------------

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
  delete [] num,keep,newarray;

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
  sort(sorted,0,size);
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

//   std::cerr << "npts = " << size << ", start = " << start << ", finish = " << finish << std::endl;

  int trimSize=0;
  float *newarray = new float[finish-start+1];
  for(int i=0;i<finish-start+1;i++) newarray[i] = sorted[i+start]*(datamax-datamin);

  findNormalStats(newarray,finish-start+1,tmean,tsigma);
  
}
