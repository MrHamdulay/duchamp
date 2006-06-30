#include <iostream>
#include <Utils/utils.hh>
const int nsample=1000;
// const int width=300;
const int width=150;
const float contrast=0.25;

void zscale(long imagesize, float *image, float &z1, float &z2)
{

  float *smallarray = new float[nsample];
  float *ct = new float[nsample];
  long size=0;

  float mean=0.,sd=0.;
  for(int i=0;i<imagesize;i++) mean+=image[i];
  mean /= float(imagesize);
  for(int i=0;i<imagesize;i++) sd+=(image[i]-mean)*(image[i]-mean);
  sd /= float(imagesize);
  

  if(imagesize>nsample){
    int step = (imagesize / nsample) + 1;
    for(int i=0;i<imagesize;i++){
      if((i%step)==0){
	smallarray[size] = image[i];
	ct[size]=(float)size+1.;
	size++;
      }
    }
  }
  else{
    for(int i=0;i<imagesize;i++){
      smallarray[i] = image[i];
      ct[i] = float(i+1);
    }
    size=imagesize;
  }

  sort(smallarray,0,size);
  
  /* fit a linear slope to the centre of the cumulative distribution */
  long midpt = size/2;
  long imin = midpt - width;
  float slope,intercept,errSlope,errIntercept,r;
  if(size<2*width) 
    linear_regression(size,ct,smallarray,0,size-1,slope,errSlope,
		      intercept,errIntercept,r);
  else linear_regression(size,ct,smallarray,imin,imin+2*width,slope,errSlope,
			 intercept,errIntercept,r);

  z1 = smallarray[midpt] + (slope/contrast)*(float)(1-midpt);
  z2 = smallarray[midpt] + (slope/contrast)*(float)(nsample-midpt);

  if(z1==z2){
 
    if(z1!=0) z2 = z1 * 1.05;
    else z2 = z1+0.01;

  }

  delete [] smallarray;
  delete [] ct;
	   
}


void zscale(long imagesize, float *image, float &z1, float &z2, float blankVal)
{
  float *newimage = new float[imagesize];
  int newsize=0;
  for(int i=0;i<imagesize;i++) if(image[i]!=blankVal) newimage[newsize++] = image[i];

//   cerr<<"Sizes: "<<imagesize<<"   "<<newsize<<endl;

  float *smallarray = new float[nsample];
  float *ct = new float[nsample];
  long size=0;

  float mean=0.,sd=0.;
  for(int i=0;i<newsize;i++) mean+=newimage[i];
  mean /= float(newsize);
  for(int i=0;i<newsize;i++) sd+=(newimage[i]-mean)*(newimage[i]-mean);
  sd /= float(newsize);
  

  if(newsize>nsample){
    int step = (newsize / nsample) + 1;
    for(int i=0;i<newsize;i++){
      if((i%step)==0){
	smallarray[size] = newimage[i];
	ct[size]=(float)size+1.;
	size++;
      }
    }
  }
  else{
    for(int i=0;i<newsize;i++){
      smallarray[i] = newimage[i];
      ct[i] = float(i+1);
    }
    size=newsize;
  }

  sort(smallarray,0,size);
  
  /* fit a linear slope to the centre of the cumulative distribution */
  long midpt = size/2;
  long imin = midpt - width;
  float slope,intercept,errSlope,errIntercept,r;
  if(size<2*width) 
    linear_regression(size,ct,smallarray,0,size-1,slope,errSlope,
		      intercept,errIntercept,r);
  else linear_regression(size,ct,smallarray,imin,imin+2*width,slope,errSlope,
		    intercept,errIntercept,r);

  z1 = smallarray[midpt] + (slope/contrast)*(float)(1-midpt);
  z2 = smallarray[midpt] + (slope/contrast)*(float)(nsample-midpt);

  if(z1==z2){
 
    if(z1!=0) z2 = z1 * 1.05;
    else z2 = z1+0.01;

  }

  delete [] newimage;
  delete [] smallarray;
  delete [] ct;

	   
}


