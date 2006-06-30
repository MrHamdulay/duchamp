#include <iostream>
#include <math.h>
#include <ATrous/atrous.hh>
//#include <Cubes/cubes.hh>
#include <Utils/utils.hh>

using namespace std;
void atrous1DReconstructOLD(long &size, float *input,float *output, Param &par)
{
  const float SNR_THRESH=par.getAtrousCut();
  const int MIN_SCALE=par.getMinScale();

  int numScales = getNumScales(size);
  /*
  if(numScales>maxNumScales1D){
    cerr<<"Error in atrous1DReconstruct:: numScales ("<<numScales<<") > "<<maxNumScales1D<<"\n";
    cerr<<"Don't have correction factors for this many scales...\n";
    cerr<<"Exiting...\n";
    exit(1);
  }
  */
  double *sigmaFactors = new double[numScales+1];
  for(int i=0;i<=numScales;i++){
    if(i<=maxNumScales1D) sigmaFactors[i] = sigmaFactors1D[i];
    else sigmaFactors[i] = sigmaFactors[i-1] / sqrt(2.);
  }


  double *coeffs = new double[(numScales+1)*size];
  double *wavelet = new double[(numScales+1)*size];

  atrousTransform(size,numScales,input,coeffs,wavelet);

  for(int pos=0;pos<size;pos++) output[pos]=0.;

  float *array = new float[size];
  float mean,sigma,originalSigma,originalMean;
  findMedianStats(input,size,originalMean,originalSigma);
  for(int scale=MIN_SCALE;scale<numScales;scale++){
    for(int pos=0;pos<size;pos++) array[pos] = wavelet[scale*size + pos];
    findMedianStats(array,size,mean,sigma);
    for(int pos=0;pos<size;pos++){
      if( fabs(wavelet[scale*size+pos])>(mean+SNR_THRESH*originalSigma*sigmaFactors[scale]))
	output[pos] += wavelet[scale*size+pos];
    }
  }
  for(int pos=0;pos<size;pos++) 
    output[pos] += coeffs[numScales*size+pos];
  float *residual = new float[size];
  for(int pos=0;pos<size;pos++) 
    residual[pos] = input[pos] - output[pos];
  float oldsigma,newsigma;
  do{
    findMedianStats(residual,size,mean,oldsigma);
    atrousTransform(size,numScales,residual,coeffs,wavelet);
    for(int scale=MIN_SCALE;scale<numScales;scale++){
      for(int pos=0;pos<size;pos++) array[pos] = wavelet[scale*size+pos];
      findMedianStats(array,size,mean,sigma);
      for(int pos=0;pos<size;pos++){
	if(fabs(wavelet[scale*size+pos]) >
	   (mean+SNR_THRESH*originalSigma*sigmaFactors[scale]))
	  output[pos] += wavelet[scale*size+pos];
      }
    }
    for(int pos=0;pos<size;pos++) output[pos] += coeffs[numScales*size+pos];
    for(int pos=0;pos<size;pos++) residual[pos] = input[pos] - output[pos];
    findMedianStats(residual,size,mean,newsigma);
  }while( fabsf(oldsigma-newsigma)/newsigma > reconTolerance);

  delete [] coeffs;
  delete [] wavelet;
  delete [] residual;
  delete [] array;
}



void atrous2DReconstructOLD(long &xdim, long &ydim, float *input,float *output, Param &par)
{
  const float SNR_THRESH=par.getAtrousCut();
  const int MIN_SCALE=par.getMinScale();
  bool flagBlank=par.getFlagBlankPix();
  float blankPixValue = par.getBlankPixVal();
  long size = xdim * ydim;
  long mindim = xdim;
  if (ydim<mindim) mindim = ydim;
  int numScales = getNumScales(mindim);
  if(numScales>maxNumScales2D){
    cerr<<"Error in atrous2DReconstruct:: numScales ("<<numScales<<") > "<<maxNumScales2D<<"\n";
    cerr<<"Don't have correction factors for this many scales...\n";
    cerr<<"XDIM = "<<xdim<<", YDIM = "<<ydim<<endl;
    cerr<<"Exiting...\n";
    exit(1);
  }

  double *coeffs = new double[size];
  double *wavelet = new double[(numScales+1)*size];

  atrousTransform2D(xdim,ydim,numScales,input,coeffs,wavelet,par);

  for(int pos=0;pos<size;pos++) output[pos]=0.;

  bool *isGood = new bool[size];
  for(int pos=0;pos<size;pos++)// isGood[pos] = (!flagBlank) || (input[pos]!=blankPixValue);
    isGood[pos] = !par.isBlank(input[pos]);

  float mean,sigma,originalSigma,originalMean;
  // Only get stats for the non-blank pixels.
  float *array = new float[size];
  int goodSize=0;
  for(int i=0;i<size;i++) if(isGood[i]) array[goodSize++] = input[i];
  findMedianStats(array,goodSize,originalMean,originalSigma);
  delete [] array;

  for(int scale=MIN_SCALE;scale<=numScales;scale++){
    array = new float[size];
    goodSize=0;
    for(int pos=0;pos<size;pos++) 
      if(isGood[pos]) array[goodSize++] = wavelet[scale*size + pos];
    findMedianStats(array,goodSize,mean,sigma);
    delete [] array;

    for(int pos=0;pos<size;pos++){
      // preserve the Blank pixel values in the output.
      if(!isGood[pos]) output[pos] = blankPixValue;
      else if( fabs(wavelet[scale*size+pos]) > 
	       (mean+SNR_THRESH*originalSigma*sigmaFactors2D[scale]) ){
	output[pos] += wavelet[scale*size+pos];
      }
    }
  }
  for(int pos=0;pos<size;pos++) if(isGood[pos]) output[pos] += coeffs[pos];

  float *residual = new float[size];
  for(int pos=0;pos<size;pos++) 
    residual[pos] = input[pos] - output[pos];

  float oldsigma,newsigma;
  do{
    array = new float[size];
    goodSize=0;
    for(int i=0;i<size;i++) if(isGood[i]) array[goodSize++] = residual[i];
    findMedianStats(array,goodSize,mean,oldsigma);
    delete [] array;

    cerr<<"\nIn atrous2DReconstruct, setting bad bits to BLANK in residual before transform.\n";
    for(int i=0;i<size;i++) if(!isGood[i]) residual[i]=blankPixValue;
    atrousTransform2D(xdim,ydim,numScales,residual,coeffs,wavelet,par);
    for(int i=0;i<size;i++) if(!isGood[i]) residual[i]=0;
    
    for(int scale=MIN_SCALE;scale<=numScales;scale++){

      array = new float[size];
      goodSize=0;
      for(int pos=0;pos<size;pos++) 
	if(isGood[pos]) array[goodSize++] = wavelet[scale*size+pos];
      findMedianStats(array,goodSize,mean,sigma);
      delete [] array;

      for(int pos=0;pos<size;pos++){
	if(!isGood[pos]) output[pos] = blankPixValue;
	else if(fabs(wavelet[scale*size+pos]) >
		(mean+SNR_THRESH*originalSigma*sigmaFactors2D[scale]))
	  output[pos] += wavelet[scale*size+pos];
      }
    }
    for(int pos=0;pos<size;pos++) if(isGood[pos]) output[pos] += coeffs[pos];
    for(int pos=0;pos<size;pos++) residual[pos] = input[pos] - output[pos];

    array = new float[size];
    goodSize=0;
    for(int i=0;i<size;i++) if(isGood[i]) array[goodSize++] = residual[i];
    findMedianStats(array,goodSize,mean,newsigma);
    delete [] array;
  }while( fabsf(oldsigma-newsigma)/newsigma > reconTolerance);


  delete [] coeffs;
  delete [] wavelet;
  delete [] residual;
}


void atrous3DReconstructOLD(long &xdim, long &ydim, long &zdim, float *&input,float *&output, Param &par)
{
  const float SNR_THRESH=par.getAtrousCut();
  const int MIN_SCALE=par.getMinScale();
  bool flagBlank=par.getFlagBlankPix();
  float blankPixValue = par.getBlankPixVal();
  long size = xdim * ydim * zdim;
  long mindim = xdim;
  if (ydim<mindim) mindim = ydim;
  if (zdim<mindim) mindim = zdim;
  int numScales = getNumScales(mindim);
  if(numScales>maxNumScales3D){
    cerr<<"Error in atrous3DReconstruct:: numScales ("<<numScales<<") > "<<maxNumScales3D<<"\n";
    cerr<<"Don't have correction factors for this many scales...\n";
    cerr<<"Exiting...\n";
    exit(1);
  }

  float mean,sigma,originalSigma,originalMean;
  bool *isGood = new bool[size];
  for(int pos=0;pos<size;pos++)// isGood[pos] = (!flagBlank) || (input[pos]!=blankPixValue);
    isGood[pos] = !par.isBlank(input[pos]);
 
  float *array = new float[size];
  int goodSize=0;
  for(int i=0;i<size;i++) if(isGood[i]) array[goodSize++] = input[i];
  findMedianStats(array,goodSize,originalMean,originalSigma);
  delete [] array;

  float *coeffs = new float[size];
  float *wavelet = new float[(numScales+1)*size];
  float *residual = new float[size];

  cerr << size<<" ";
  atrousTransform3D(xdim,ydim,zdim,numScales,input,coeffs,wavelet,par);
  cerr <<",";
  for(int pos=0;pos<size;pos++) output[pos]=0.;
  cerr <<",";

  for(int scale=MIN_SCALE;scale<=numScales;scale++){
    cerr<<",";
    array = new float[size];
    goodSize=0;
    for(int pos=0;pos<size;pos++) 
      if(isGood[pos]) array[goodSize++] = wavelet[scale*size + pos];
    findMedianStats(array,goodSize,mean,sigma);
    delete [] array;

    for(int pos=0;pos<size;pos++){
      // preserve the Blank pixel values in the output.
      if(!isGood[pos]) output[pos] = blankPixValue;
      else if( fabs(wavelet[scale*size+pos])>
	       (mean+SNR_THRESH*originalSigma*sigmaFactors3D[scale]))
	output[pos] += wavelet[scale*size+pos];
    }
  }
  for(int pos=0;pos<size;pos++) if(isGood[pos]) output[pos] += coeffs[pos];

  for(int pos=0;pos<size;pos++) residual[pos] = input[pos] - output[pos];

  float oldsigma,newsigma;
  cerr<<"!";
  array = new float[size];
  goodSize=0;
  for(int i=0;i<size;i++) if(isGood[i]) array[goodSize++] = residual[i];
  findMedianStats(array,goodSize,mean,newsigma);
  delete [] array;
  do{
    oldsigma = newsigma;
    cerr<<"!"<<oldsigma;
    atrousTransform3D(xdim,ydim,zdim,numScales,residual,coeffs,wavelet,par);
    cerr<<"!";
    for(int scale=MIN_SCALE;scale<numScales;scale++){

      array = new float[size];
      goodSize=0;
      for(int pos=0;pos<size;pos++) 
	if(isGood[pos]) array[goodSize++] = wavelet[scale*size+pos];
      findMedianStats(array,goodSize,mean,sigma);
      delete [] array;

      for(int pos=0;pos<size;pos++){
	if(!isGood[pos]) output[pos] = blankPixValue;
	else if( fabs(wavelet[scale*size+pos]) >
		 (mean+SNR_THRESH*originalSigma*sigmaFactors3D[scale]))
	  output[pos] += wavelet[scale*size+pos];
      }
    }
    for(int pos=0;pos<size;pos++) if(isGood[pos]) output[pos] += coeffs[pos];
    for(int pos=0;pos<size;pos++) residual[pos] = input[pos] - output[pos];
    array = new float[size];
    goodSize=0;
    for(int i=0;i<size;i++) if(isGood[i]) array[goodSize++] = residual[i];
    findMedianStats(array,goodSize,mean,newsigma);
    delete [] array;
    cerr<<"|"<<newsigma<<"|"<<fabsf(oldsigma-newsigma)/newsigma;

  }while( fabsf(oldsigma-newsigma)/newsigma > reconTolerance);

  delete [] coeffs;
  delete [] wavelet;
  delete [] residual;
}
