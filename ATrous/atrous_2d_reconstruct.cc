#include <iostream>
#include <iomanip>
#include <math.h>
#include <ATrous/atrous.hh>
//#include <Cubes/cubes.hh>
#include <Utils/utils.hh>

using std::endl;
using std::setw;

void atrous2DReconstruct(long &xdim, long &ydim, float *&input,float *&output, Param &par)
{
  extern Filter reconFilter;
  bool flagBlank=par.getFlagBlankPix();
  float blankPixValue = par.getBlankPixVal();
  long size = xdim * ydim;
  long mindim = xdim;
  if (ydim<mindim) mindim = ydim;
  int numScales = reconFilter.getNumScales(mindim);
  /*
  if(numScales>maxNumScales2D){
    std::cerr<<"Error in atrous2DReconstruct:: numScales ("<<numScales<<") > "<<maxNumScales2D<<"\n";
    std::cerr<<"Don't have correction factors for this many scales...\n";
    std::cerr<<"XDIM = "<<xdim<<", YDIM = "<<ydim<<endl;
    std::cerr<<"Exiting...\n";
    exit(1);
  }
  */
  double *sigmaFactors = new double[numScales+1];
//   for(int i=0;i<=numScales;i++){
//     if(i<=maxNumScales2D) sigmaFactors[i] = sigmaFactors2D[i];
//     else sigmaFactors[i] = sigmaFactors[i-1] / 2.;
//   }
  for(int i=0;i<=numScales;i++){
    if(i<=reconFilter.maxFactor(2)) sigmaFactors[i] = reconFilter.sigmaFactor(2,i);
    else sigmaFactors[i] = sigmaFactors[i-1] / 2.;
  }

  float mean,sigma,originalSigma,originalMean,oldsigma,newsigma;
  bool *isGood = new bool[size];
  for(int pos=0;pos<size;pos++) //isGood[pos] = (!flagBlank) || (input[pos]!=blankPixValue);
    isGood[pos] = !par.isBlank(input[pos]);
 
  float *array = new float[size];
  int goodSize=0;
  for(int i=0;i<size;i++) if(isGood[i]) array[goodSize++] = input[i];
  findMedianStats(array,goodSize,originalMean,originalSigma);
  delete [] array;
  
  float *coeffs    = new float[size];
  float *wavelet   = new float[size];

  for(int pos=0;pos<size;pos++) output[pos]=0.;

  /***********************************************************************/
  /////  2-DIMENSIONAL TRANSFORM
//   int filterHW = filterwidth/2;
//   double *filter = new double[filterwidth*filterwidth];
//   for(int i=0;i<filterwidth;i++){
//     for(int j=0;j<filterwidth;j++){
//       filter[i*filterwidth+j] = filter1D[i] * filter1D[j];
//     }
//   }
  int filterHW = reconFilter.width()/2;
  double *filter = new double[reconFilter.width()*reconFilter.width()];
  for(int i=0;i<reconFilter.width();i++){
    for(int j=0;j<reconFilter.width();j++){
      filter[i*reconFilter.width()+j] = reconFilter.coeff(i) * reconFilter.coeff(j);
    }
  }

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
  

  /***********************************************************************/
  /***********************************************************************/

  float threshold;
  int iteration=0;
  newsigma = 1.e9;
  for(int i=0;i<size;i++) output[i] = 0;
  do{
    if(par.isVerbose()) std::cout << "Iteration #"<<++iteration<<":             ";
    // first, get the value of oldsigma and set it to the previous newsigma value
    oldsigma = newsigma;
    // we are transforming the residual array
    for(int i=0;i<size;i++)  coeffs[i] = input[i] - output[i];  

    int spacing = 1;
    for(int scale = 1; scale<numScales; scale++){

      if(par.isVerbose()) {
	std::cout << "\b\b\b\b\b\b\b\b\b\b\b\bScale ";
	std::cout << setw(2)<<scale<<" /"<<setw(2)<<numScales<<std::flush;
      }

      for(int ypos = 0; ypos<ydim; ypos++){
	for(int xpos = 0; xpos<xdim; xpos++){
	  // loops over each pixel in the image
	  int pos = ypos*xdim + xpos;
	  
	  wavelet[pos] = coeffs[pos];

	  if(!isGood[pos]) wavelet[pos] = 0.;
	  else{

	    for(int yoffset=-filterHW; yoffset<=filterHW; yoffset++){
	      int y = ypos + spacing*yoffset;
	      int newy;
	      // 	  if(y<0) y = -y;                 // boundary conditions are 
	      // 	  if(y>=ydim) y = 2*(ydim-1) - y; //    reflection.
	      // 	  while((y<yLim1)||(y>yLim2)){
	      // 	    if(y<yLim1) y = 2*yLim1 - y;      // boundary conditions are 
	      // 	    if(y>yLim2) y = 2*yLim2 - y;      //    reflection.
	      // 	  }
	      // boundary conditions are reflection.
	  
	      for(int xoffset=-filterHW; xoffset<=filterHW; xoffset++){
		int x = xpos + spacing*xoffset;
		int newx;
		//if(x<0) x = -x;                 // boundary conditions are 
		// if(x>=xdim) x = 2*(xdim-1) - x; //    reflection.
		//while((x<xLim1)||(x>xLim2)){
		// 	      if(x<xLim1) x = 2*xLim1 - x;      // boundary conditions are 
		// 	      if(x>xLim2) x = 2*xLim2 - x;      //    reflection.
		// 	    }
		// boundary conditions are reflection.
		while((y<yLim1[xpos])||(y>yLim2[xpos])){
		  if(y<yLim1[xpos]) y = 2*yLim1[xpos] - y;      
		  else if(y>yLim2[xpos]) y = 2*yLim2[xpos] - y;      
		}
		while((x<xLim1[ypos])||(x>xLim2[ypos])){
		  if(x<xLim1[ypos]) x = 2*xLim1[ypos] - x;      
		  else if(x>xLim2[ypos]) x = 2*xLim2[ypos] - x;      
		}

// 		if(y<yLim1[xpos]) newy = 2*yLim1[xpos] - y;      
// 		else if(y>yLim2[xpos]) newy = 2*yLim2[xpos] - y;      
// 		else newy = y;
// 		if(x<xLim1[ypos]) newx = 2*xLim1[ypos] - x;
// 		else if(x>xLim2[ypos]) newx = 2*xLim2[ypos] - x;      
// 		else newx=x;      
	      
// 		x = newx;
// 		y = newy;

		int filterpos = (yoffset+filterHW)*reconFilter.width() + (xoffset+filterHW);
		int oldpos = y*xdim + x;

		if(isGood[pos]) 
		  wavelet[pos] -= filter[filterpos] * coeffs[oldpos];

	      } //-> end of xoffset loop
	    } //-> end of yoffset loop
	  } //-> end of else{ ( from if(!isGood[pos])  )
	
	} //-> end of xpos loop
      } //-> end of ypos loop

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

    } // END OF LOOP OVER SCALES

    for(int pos=0;pos<size;pos++) if(isGood[pos]) output[pos] += coeffs[pos];

    array = new float[size];
    goodSize=0;
    for(int i=0;i<size;i++) if(isGood[i]) array[goodSize++] = input[i] - output[i];
    findMedianStats(array,goodSize,mean,newsigma);
    delete [] array;
    
    if(par.isVerbose()) std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";

  } while( (iteration==1) || 
	   (fabsf(oldsigma-newsigma)/newsigma > reconTolerance) );

  if(par.isVerbose()) std::cout << "Completed "<<iteration<<" iterations. ";

  delete [] coeffs;
  delete [] wavelet;
  delete [] isGood;
  delete [] filter;
  delete [] sigmaFactors;
}
    
    
