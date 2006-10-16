#include <iostream>
#include <math.h>
#include <Cubes/cubes.hh>

bool Image::isDetection(long x, long y)
{
  if(this->par.getFlagFDR()) 
    return ( (!this->par.isBlank(this->array[y*axisDim[0]+x])) &&
	     (this->pValue[y*axisDim[0]+x] < this->pCutLevel)     );
  else 
    return ( (!this->par.isBlank(this->array[y*axisDim[0]+x])) &&
	     ( this->array[y*axisDim[0]+x] > this->par.getThreshold() ) );
// 	     ( ((this->array[y*axisDim[0]+x]-this->mean)/this->sigma) 
// 	       > this->cutLevel ) );
}

bool Image::isDetection(float value)
{
  return ( (!this->par.isBlank(value)) &&
	     ( value > this->par.getThreshold() ) );
// 	   (((value - this->mean) / this->sigma) > this->cutLevel) );
}

bool Image::isDetectionFDR(float pvalue)
{
  return (  (pvalue < this->pCutLevel ) );

}

bool isDetection(float value, float mean, float sigma, float cut)
{
  return ( ((value - mean) / sigma) > cut ) ;
}

bool isDetection(float value, float threshold)
{
  return ( value > threshold ) ;
}

int Cube::setupFDR()
{
  /**
   *  Cube::setupFDR()
   *   Determines the critical Prob value for the False Discovery Rate
   *    detection routine. All pixels with Prob less than this value will
   *    be considered detections.
   *   The Prob here is the probability, assuming a Normal distribution, of
   *    obtaining a value as high or higher than the pixel value (ie. only the
   *    positive tail of the PDF)
   */

  float alpha = this->par.getAlpha();

  // first calculate p-value for each pixel, using mean and sigma
  // assume Gaussian for now.

  float *orderedP = new float[this->numPixels];
  int count = 0;
  for(int pix=0; pix<this->numPixels; pix++){

    if( !(this->par.isBlank(this->array[pix])) ){
//       float zStat = (this->array[pix] - this->mean) / (this->sigma);
      float zStat = (this->array[pix] - this->median) / 
	(this->madfm/correctionFactor);

      this->pValue[pix] = 0.5 * erfc(zStat/M_SQRT2);
      // Need the factor of 0.5 here, as we are only considering the positive 
      //  tail of the distribution. Don't care about negative detections.
      
      orderedP[count++] = this->pValue[pix];
    }
    else this->pValue[pix] = 1.0;
    //need to make this high so that it won't be below the P cut level.
  }

  // now order them 
  sort(orderedP,0,count);
  
  // now find the maximum P value.
  int max = 0;
  float cN = 0.;
  int psfCtr;
  int numPix = int(this->par.getBeamSize());
  for(psfCtr=1;psfCtr<=numPix;(psfCtr)++) 
    cN += 1./float(psfCtr);

  for(int loopCtr=0;loopCtr<count;loopCtr++) {
    if( orderedP[loopCtr] < (double(loopCtr+1)*this->alpha/(cN * double(count))) ) {
      max = loopCtr;
    }
  }

  this->pCutLevel = orderedP[max];

  delete [] orderedP;

}

