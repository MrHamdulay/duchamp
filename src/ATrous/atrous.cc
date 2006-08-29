// ATROUS.CC
//  Functions necessary for the reconstruction routines.
#include <iostream>
#include <sstream>
#include <duchamp.hh>
#include <ATrous/atrous.hh>
#include <math.h>

int Filter::getNumScales(long length)
{
  switch(this->filter1D.size()){
  case 5: 
    return int(log(double(length-1))/M_LN2) - 1;
    break;
  case 3:
    return int(log(double(length-1))/M_LN2);
    break;
  default:
    return 1 + int(log(double(length-1)/double(this->filter1D.size()-1))/M_LN2);
    break;
  }
}

int Filter::getMaxSize(int scale)
{
  switch(this->filter1D.size()){
  case 5:
    return int(pow(2,scale+1)) + 1;
    break;
  case 3:
    return int(pow(2,scale)) + 1;
    break;
  default:
    return int(pow(2,scale-1))*(this->filter1D.size()-1) + 1;
    break;
  }
}

Filter::Filter()
{
  this->sigmaFactors.resize(3);
  for(int i=0;i<3;i++) this->sigmaFactors[i] = new vector<double>(20);
}


void Filter::define(int filtercode)
{
  switch(filtercode)
    {
    case 2:
      this->loadTriangle();
      break;
    case 3:
      this->loadHaar();
      break;
    case 4:
      //       this->loadTopHat();
    case 1:
    default:
      if(filtercode!=1){
	if(filtercode==4) {
	  std::stringstream errmsg;
	  errmsg << "TopHat Wavelet not being used currently."
		 << "Using B3 spline instead.\n";
	  duchampWarning("Filter::define", errmsg.str());
	}
	else {
	  std::stringstream errmsg;
	  errmsg << "Filter code " << filtercode << " undefined. Using B3 spline.\n";
	  duchampWarning("Filter::define", errmsg.str());
	}
      }
      this->loadSpline();
      break;
    }
  
}

void Filter::loadSpline()
{
  double filter[5] = {0.0625, 0.25, 0.375, 0.25, 0.0625};
  this->name = "B3 spline function";
  this->filter1D.resize(5);
  for(int i=0;i<5;i++) this->filter1D[i] = filter[i];
  this->sigmaFactors.resize(3);
  this->maxNumScales.resize(3);

  this->maxNumScales[0] = 18;
  double sigmaFactors1D[19] = {1.00000000000,7.23489806e-1,2.85450405e-1,1.77947535e-1,
			       1.22223156e-1,8.58113122e-2,6.05703043e-2,4.28107206e-2,
			       3.02684024e-2,2.14024008e-2,1.51336781e-2,1.07011079e-2,
			       7.56682272e-3,5.35055108e-3,3.78341085e-3,2.67527545e-3,
			       1.89170541e-3,1.33763772e-3,9.45852704e-4};
  this->sigmaFactors[0]->resize(19);
  for(int i=0;i<19;i++)(*this->sigmaFactors[0])[i] = sigmaFactors1D[i]; 

  this->maxNumScales[1] = 11;
  double sigmaFactors2D[12] = {1.00000000000,8.90796310e-1,2.00663851e-1,8.55075048e-2,
			       4.12474444e-2,2.04249666e-2,1.01897592e-2,5.09204670e-3,
			       2.54566946e-3,1.27279050e-3,6.36389722e-4,3.18194170e-4};
  this->sigmaFactors[1]->resize(12);
  for(int i=0;i<12;i++)(*this->sigmaFactors[1])[i] = sigmaFactors2D[i]; 

  this->maxNumScales[2] = 7;
  double sigmaFactors3D[8] = {1.00000000000,9.56543592e-1,1.20336499e-1,3.49500154e-2,
			      1.18164242e-2,4.13233507e-3,1.45703714e-3,5.14791120e-4};
  this->sigmaFactors[2]->resize(8);
  for(int i=0;i<12;i++)(*this->sigmaFactors[2])[i] = sigmaFactors3D[i]; 
}

void Filter::loadTriangle()
{
  double filter[3] = {1./4., 1./2., 1./4.};
  this->filter1D.resize(3);
  for(int i=0;i<3;i++) this->filter1D[i] = filter[i];
  this->name = "Triangle function";
  this->sigmaFactors.resize(3);
  this->maxNumScales.resize(3);

  this->maxNumScales[0] = 18;
  double sigmaFactors1D[19] = {1.00000000000,6.12372436e-1,3.30718914e-1,2.11947812e-1,
			       1.45740298e-1,1.02310944e-1,7.22128185e-2,5.10388224e-2,
			       3.60857673e-2,2.55157615e-2,1.80422389e-2,1.27577667e-2,
			       9.02109930e-3,6.37887978e-3,4.51054902e-3,3.18942978e-3,
			       2.25527449e-3,1.59471988e-3,1.12763724e-4};
  this->sigmaFactors[0]->resize(19);
  for(int i=0;i<19;i++)(*this->sigmaFactors[0])[i] = sigmaFactors1D[i]; 

  this->maxNumScales[1] = 12;
  double sigmaFactors2D[13] = {1.00000000000,8.00390530e-1,2.72878894e-1,1.19779282e-1,
			       5.77664785e-2,2.86163283e-2,1.42747506e-2,7.13319703e-3,
			       3.56607618e-3,1.78297280e-3,8.91478237e-4,4.45738098e-4,
			       2.22868922e-4};
  this->sigmaFactors[1]->resize(13);
  for(int i=0;i<12;i++)(*this->sigmaFactors[1])[i] = sigmaFactors2D[i]; 

  this->maxNumScales[2] = 8;
  double sigmaFactors3D[9] = {1.00000000000,8.959544490e-1,1.92033014e-1,5.76484078e-2,
			      1.94912393e-2,6.812783870e-3,2.40175885e-3,8.48538128e-4,
			      2.99949455e-4};
  this->sigmaFactors[2]->resize(9);
  for(int i=0;i<12;i++)(*this->sigmaFactors[2])[i] = sigmaFactors3D[i]; 
}

void Filter::loadHaar()
{
  double filter[3] = {0., 1./2., 1./2.};
  this->name = "Haar wavelet";
  this->filter1D.resize(3);
  for(int i=0;i<3;i++) this->filter1D[i] = filter[i];
  this->sigmaFactors.resize(3);
  this->maxNumScales.resize(3);

  this->maxNumScales[0] = 6;
  double sigmaFactors1D[7] = {1.00000000000,7.07167810e-1,5.00000000e-1,3.53553391e-1,
			      2.50000000e-1,1.76776695e-1,1.25000000e-1};
  this->sigmaFactors[0]->resize(7);
  for(int i=0;i<19;i++)(*this->sigmaFactors[0])[i] = sigmaFactors1D[i]; 

  this->maxNumScales[1] = 6;
  double sigmaFactors2D[7] = {1.00000000000,4.33012702e-1,2.16506351e-1,1.08253175e-1,
			      5.41265877e-2,2.70632939e-2,1.35316469e-2};
  this->sigmaFactors[1]->resize(7);
  for(int i=0;i<12;i++)(*this->sigmaFactors[1])[i] = sigmaFactors2D[i]; 

  this->maxNumScales[2] = 8;
  double sigmaFactors3D[9] = {1.00000000000,9.35414347e-1,3.30718914e-1,1.16926793e-1,
			      4.13398642e-2,1.46158492e-2,5.16748303e-3,1.82698115e-3,
			      6.45935379e-4};
  this->sigmaFactors[2]->resize(9);
  for(int i=0;i<12;i++)(*this->sigmaFactors[2])[i] = sigmaFactors3D[i]; 
}
