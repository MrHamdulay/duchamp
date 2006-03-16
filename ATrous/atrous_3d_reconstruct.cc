#include <iostream>
#include <iomanip>
#include <math.h>
#include <ATrous/atrous.hh>
#include <Utils/utils.hh>

using std::endl;
using std::setw;

void atrous3DReconstruct(long &xdim, long &ydim, long &zdim, float *&input,float *&output, Param &par)
{
  extern Filter reconFilter;
  long size = xdim * ydim * zdim;
  long mindim = xdim;
  if (ydim<mindim) mindim = ydim;
  if (zdim<mindim) mindim = zdim;
  int numScales = reconFilter.getNumScales(mindim);
  /* 
  if(numScales>maxNumScales3D){
    std::cerr<<"Error in atrous3DReconstruct:: numScales ("<<numScales<<") > "<<maxNumScales3D<<"\n";
    std::cerr<<"Don't have correction factors for this many scales...\n";
    std::cerr<<"Exiting...\n";
    exit(1);
  }
  */
  double *sigmaFactors = new double[numScales+1];
//   for(int i=0;i<=numScales;i++){
//     if(i<=maxNumScales3D) sigmaFactors[i] = sigmaFactors3D[i];
//     else sigmaFactors[i] = sigmaFactors[i-1] / sqrt(8.);
//   }
  for(int i=0;i<=numScales;i++){
    if(i<=reconFilter.maxFactor(3)) sigmaFactors[i] = reconFilter.sigmaFactor(3,i);
    else sigmaFactors[i] = sigmaFactors[i-1] / sqrt(8.);
  }

  float mean,sigma,originalSigma,originalMean,oldsigma,newsigma;
  bool *isGood = new bool[size];
  bool flagBlank = par.getFlagBlankPix();
  float blankPixValue = par.getBlankPixVal();
  for(int pos=0;pos<size;pos++) //isGood[pos] = (!flagBlank) || (input[pos]!=blankPixValue);
    isGood[pos] = !par.isBlank(input[pos]);
 
  float *array = new float[size];
  int goodSize=0;
  for(int i=0;i<size;i++) if(isGood[i]) array[goodSize++] = input[i];
  findMedianStats(array,goodSize,originalMean,originalSigma);
  originalSigma /= correctionFactor; // correct from MADFM to sigma estimator.
  delete [] array;

  float *coeffs = new float[size];
  float *wavelet = new float[size];

  for(int pos=0;pos<size;pos++) output[pos]=0.;

/***********************************************************************/
/////  3-DIMENSIONAL TRANSFORM

//   int filterHW = filterwidth/2;
//   int fsize = filterwidth*filterwidth*filterwidth;
//   double *filter = new double[fsize];
//   for(int i=0;i<filterwidth;i++){
//     for(int j=0;j<filterwidth;j++){
//       for(int k=0;k<filterwidth;k++){
//       filter[i +j*filterwidth + k*filterwidth*filterwidth] = 
// 	filter1D[i] * filter1D[j] * filter1D[k];
//       }
//     }
//   }
  int filterwidth = reconFilter.width();
  int filterHW = filterwidth/2;
  int fsize = filterwidth*filterwidth*filterwidth;
  double *filter = new double[fsize];
  for(int i=0;i<filterwidth;i++){
    for(int j=0;j<filterwidth;j++){
      for(int k=0;k<filterwidth;k++){
      filter[i +j*filterwidth + k*filterwidth*filterwidth] = 
	reconFilter.coeff(i) * reconFilter.coeff(j) * reconFilter.coeff(k);
      }
    }
  }


  // locating the borders of the image -- ignoring BLANK pixels
  // HAVE NOT DONE THIS FOR Z --> ASSUMING NO TRIMMING IN SPECTRAL DIRECTION
//   int xLim1 = 0, yLim1 = 0, xLim2 = xdim-1, yLim2 = ydim-1;
//   for(int col=0;col<xdim;col++){
//     while((yLim1<yLim2)&&(input[col+xdim*yLim1]==blankPixValue) ) yLim1++;
//     while((yLim2>yLim1)&&(input[col+xdim*yLim1]==blankPixValue) ) yLim2--;
//   }
//   for(int row=0;row<ydim;row++){
//     while((xLim1<xLim2)&&(input[row*xdim+xLim1]==blankPixValue) ) xLim1++;
//     while((xLim2>xLim1)&&(input[row*xdim+xLim1]==blankPixValue) ) xLim2--;
//   }
  int *xLim1 = new int[ydim];
  int *yLim1 = new int[xdim];
  int *xLim2 = new int[ydim];
  int *yLim2 = new int[xdim];
  float avGapX = 0, avGapY = 0;
  for(int row=0;row<ydim;row++){
    int ct1 = 0;
    int ct2 = xdim - 1;
    while((ct1<ct2)&&(input[row*xdim+ct1]==blankPixValue) ) ct1++;
    while((ct2>ct1)&&(input[row*xdim+ct2]==blankPixValue) ) ct2--;
    xLim1[row] = ct1;
    xLim2[row] = ct2;
    avGapX += ct2 - ct1;
  }
  avGapX /= float(ydim);

  for(int col=0;col<xdim;col++){
    int ct1=0;
    int ct2=ydim-1;
    while((ct1<ct2)&&(input[col+xdim*ct1]==blankPixValue) ) ct1++;
    while((ct2>ct1)&&(input[col+xdim*ct2]==blankPixValue) ) ct2--;
    yLim1[col] = ct1;
    yLim2[col] = ct2;
    avGapY += ct2 - ct1;
  }
  avGapY /= float(ydim);
 
  mindim = int(avGapX);
  if(avGapY < avGapX) mindim = int(avGapY);
  numScales = reconFilter.getNumScales(mindim);

  float threshold;
  int iteration=0;
  newsigma = 1.e9;
  for(int i=0;i<size;i++) output[i] = 0;
  do{
    if(par.isVerbose())  std::cout << "Iteration #"<<setw(2)<<++iteration<<": ";
    // first, get the value of oldsigma, set it to the previous newsigma value
    oldsigma = newsigma;
    // we are transforming the residual array (input array first time around)
    for(int i=0;i<size;i++)  coeffs[i] = input[i] - output[i];

    int spacing = 1;
    for(int scale = 1; scale<=numScales; scale++){

      if(par.isVerbose()){
	std::cout << "Scale ";
	std::cout << setw(2)<<scale<<" / "<<setw(2)<<numScales
		  << "\b\b\b\b\b\b\b\b\b\b\b\b\b"<<std::flush;
      }

      for(int zpos = 0; zpos<zdim; zpos++){
// 	std::cout << setw(4)<<zpos<<"\b\b\b\b";
	for(int ypos = 0; ypos<ydim; ypos++){
	  for(int xpos = 0; xpos<xdim; xpos++){
	    // loops over each pixel in the image
	    int pos = zpos*xdim*ydim + ypos*xdim + xpos;

	    wavelet[pos] = coeffs[pos];
 	    
	    if(!isGood[pos] )  wavelet[pos] = 0.;
	    else{

	      for(int zoffset=-filterHW; zoffset<=filterHW; zoffset++){
		int z = zpos + spacing*zoffset;
		if(z<0) z = -z;                 // boundary conditions are 
		if(z>=zdim) z = 2*(zdim-1) - z; //    reflection.
	  
		for(int yoffset=-filterHW; yoffset<=filterHW; yoffset++){
		  int y = ypos + spacing*yoffset;
		  //if(y<0) y = -y;                 // boundary conditions are 
		  //if(y>=ydim) y = 2*(ydim-1) - y; //    reflection.
// 		  if(y<yLim1) y = 2*yLim1 - y;      // boundary conditions are 
// 		  if(y>yLim2) y = 2*yLim2 - y;      //    reflection.
	  
		  for(int xoffset=-filterHW; xoffset<=filterHW; xoffset++){
		    int x = xpos + spacing*xoffset;
		    //if(x<0) x = -x;                 // boundary conditions are 
		    //if(x>=xdim) x = 2*(xdim-1) - x; //    reflection.
// 		    if(x<xLim1) x = 2*xLim1 - x;      // boundary conditions are 
// 		    if(x>xLim2) x = 2*xLim2 - x;      //    reflection.

		    // boundary conditions are reflection.
// 		    if(y<yLim1[xpos]) newy = 2*yLim1[xpos] - y;      
// 		    else if(y>yLim2[xpos]) newy = 2*yLim2[xpos] - y;      
// 		    else newy = y;

		    if(yLim1[xpos]!=yLim2[xpos]){ 
		      // if these are equal we will get into an infinite loop here 
		      while((y<yLim1[xpos])||(y>yLim2[xpos])){
			if(y<yLim1[xpos]) y = 2*yLim1[xpos] - y;      
			else if(y>yLim2[xpos]) y = 2*yLim2[xpos] - y;      
		      }
		    }

// 		    if(x<xLim1[ypos]) newx = 2*xLim1[ypos] - x;
// 		    else if(x>xLim2[ypos]) newx = 2*xLim2[ypos] - x;      
// 		    else newx=x;      

		    if(xLim1[ypos]!=xLim2[ypos]){
		      // if these are equal we will get into an infinite loop here 
		      while((x<xLim1[ypos])||(x>xLim2[ypos])){
			if(x<xLim1[ypos]) x = 2*xLim1[ypos] - x;      
			else if(x>xLim2[ypos]) x = 2*xLim2[ypos] - x;      
		      }
		    }

// 		    x = newx;
// 		    y = newy;

		    int filterpos = (zoffset+filterHW)*filterwidth*filterwidth + 
		      (yoffset+filterHW)*filterwidth + (xoffset+filterHW);
		    int oldpos = z*xdim*ydim + y*xdim + x;
		    if(oldpos>=size) 
		      std::cerr<<"oldpos ("<<oldpos<<") exceeds array size("<<size<<")!\n"
			  <<"x="<<x<<", y="<<y<<", z="<<z<<endl
			  <<"xpos="<<xpos<<", ypos="<<ypos<<", zpos="<<zpos<<endl
			  <<"cf. xdim="<<xdim<<", ydim="<<ydim<<", zdim="<<zdim<<endl;
		    
		    if(isGood[oldpos]) 
		      wavelet[pos] -= filter[filterpos]*coeffs[oldpos];
		      
		  } //-> end of xoffset loop
		} //-> end of yoffset loop
	      } //-> end of zoffset loop
	    } //-> end of else{ ( from if(!isGood[pos])  )
	    
	  } //-> end of xpos loop
	} //-> end of ypos loop
      } //-> end of zpos loop

      // Need to do this after we've done *all* the convolving
      for(int pos=0;pos<size;pos++) coeffs[pos] = coeffs[pos] - wavelet[pos];

      // Have found wavelet coeffs for this scale -- now threshold
      if(scale>=par.getMinScale()){
	array = new float[size];
	goodSize=0;
	for(int pos=0;pos<size;pos++) if(isGood[pos]) array[goodSize++] = wavelet[pos];
	findMedianStats(array,goodSize,mean,sigma);
	delete [] array;
	
	threshold = mean + par.getAtrousCut() * originalSigma * sigmaFactors[scale];
	for(int pos=0;pos<size;pos++){
	  if(!isGood[pos]) output[pos] = blankPixValue; // preserve the Blank pixel values in the output.
	  else if( fabs(wavelet[pos]) > threshold ) output[pos] += wavelet[pos];
	}
      }
 
      spacing *= 2;

    } //-> end of scale loop 

    for(int pos=0;pos<size;pos++) if(isGood[pos]) output[pos] += coeffs[pos];

    array = new float[size];
    goodSize=0;
    for(int i=0;i<size;i++) if(isGood[i]) array[goodSize++] = input[i] - output[i];
    findNormalStats(array,goodSize,mean,newsigma);
    delete [] array;

    if(par.isVerbose()) std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";

  } while( (iteration==1) || 
	   (fabsf(oldsigma-newsigma)/newsigma > reconTolerance) );

  if(par.isVerbose()) std::cout << "Completed "<<iteration<<" iterations. ";

  delete [] coeffs;
  delete [] wavelet;
  delete [] isGood;
  delete [] filter;
  delete [] sigmaFactors;
}
