#include <iostream>
#include <param.hh>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>

void findSources(Image &image) 
{
  int size = image.getSize();
  float blankPixValue = image.pars().getBlankPixVal();
  bool flagFDR = image.pars().getFlagFDR();

  float *arr = new float[size];
  int goodSize=0;
  for(int i=0;i<size;i++){
    float val = image.getPixValue(i);
    //    if((!image.pars().getFlagBlankPix())||(image.getPixValue(i)!=blankPixValue))
    if(!image.pars().isBlank(val))
      arr[goodSize++] = val;
  }
  if(goodSize>0){
    float mean,sigma;
    findMedianStats(arr,goodSize,mean,sigma);
    image.setStats(mean,sigma,image.pars().getCut());
    if(flagFDR) image.setupFDR();
    image.lutz_detect();
  }
  delete [] arr;
}  

void findSources(Image &image, float mean, float sigma) 
{
  bool flagFDR = image.pars().getFlagFDR();
  int size = image.getSize();
  float *arr = new float[size];
  for(int i=0;i<size;i++) arr[i] = image.getPixValue(i);
  findMedianStats(arr,size,mean,sigma);
  image.setStats(mean,sigma,image.pars().getCut());
  if(flagFDR) image.setupFDR();
  image.lutz_detect();
  delete [] arr;
}  

