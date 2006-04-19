#include <iostream>
#include <math.h>
#include <Cubes/cubes.hh>

bool Image::isDetection(long x, long y)
{
  if(this->par.getFlagFDR()) 
    return ( this->pValue[y*axisDim[0]+x] < this->pCutLevel );
  else 
    return ( ((this->array[y*axisDim[0]+x]-this->mean)/this->sigma) > this->cutLevel );
}

bool Image::isDetection(float value)
{
  return ( ((value - this->mean) / this->sigma) > this->cutLevel ) ;
}

bool Image::isDetectionFDR(float pvalue)
{
  return (  pvalue < this->pCutLevel ) ;

}

bool isDetection(float value, float mean, float sigma, float cut)
{
  return ( ((value - mean) / sigma) > cut ) ;
}

int Image::setupFDR()
{
  this->alpha = this->par.alphaFDR;

  // first calculate p-value for each pixel, using mean and sigma
  // assume Gaussian for now.
  bool *isGood = new bool[this->numPixels];
  int *loopCtr = new int;
  for(*loopCtr=0;*loopCtr<this->numPixels;(*loopCtr)++) 
    isGood[*loopCtr] = !(this->par.isBlank(this->array[*loopCtr]));

  float *rootTwo = new float;
  *rootTwo=sqrt(2.);
  for(int j=0;j<this->numPixels;j++){
    if(isGood[j])
      this->pValue[j] = 0.5*erfc((this->array[j] - this->mean) / (*rootTwo * this->sigma));
    else
      this->pValue[j] = 1.;  //need to make this high so that it won't be below the P cut level.
  }
  delete rootTwo;

  // now order them
  float *orderedP = new float[this->numPixels];
  int *count = new int;
  *count = 0;
  for(*loopCtr=0;*loopCtr<this->numPixels;(*loopCtr)++) 
    if(isGood[*loopCtr])
      orderedP[(*count)++] = this->pValue[*loopCtr];

  sort(orderedP,0,(*count-1));
  
  // now find the maximum P value.
  int *max = new int;
  *max = -1;
  float *cN  = new float;
  *cN = 0.;
  int *psfCtr = new int;
  int *numPix = new int;
  *numPix = int(this->par.getBeamSize());
  for(*psfCtr=1;*psfCtr<=*numPix;(*psfCtr)++) 
    *cN += 1./float(*psfCtr);
  delete psfCtr;
  delete numPix;

  for(*loopCtr=0;*loopCtr<*count;(*loopCtr)++) {
    if( orderedP[*loopCtr] < (double(*loopCtr+1)*this->alpha/(*cN * double(*count))) )
      *max = *loopCtr;
  }

  this->pCutLevel = orderedP[*max];

  delete [] orderedP;
  delete [] isGood;
  delete max;
  delete cN;
  delete count;
  delete loopCtr;
}

