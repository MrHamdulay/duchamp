#include <duchamp/ATrous/TriangleFilter.hh>
#include <duchamp/ATrous/Filter.hh>

namespace duchamp {

    TriangleFilter::TriangleFilter():
    Filter()
    {
	const unsigned int filtersize=3;
	double filter[filtersize] = {1./4., 1./2., 1./4.};
	this->filter1D = std::vector<double>(filter,filter+filtersize);
	this->name = "Triangle function";
	
	const unsigned int size1D=18;
	this->maxNumScales[0] = size1D;
	double sigmaFactors1D[size1D+1] = 
	    {1.00000000000,6.12372436e-1,3.30718914e-1,2.11947812e-1,
	     1.45740298e-1,1.02310944e-1,7.22128185e-2,5.10388224e-2,
	     3.60857673e-2,2.55157615e-2,1.80422389e-2,1.27577667e-2,
	     9.02109930e-3,6.37887978e-3,4.51054902e-3,3.18942978e-3,
	     2.25527449e-3,1.59471988e-3,1.12763724e-4};

	const unsigned int size2D=12;
	this->maxNumScales[1] = size2D;
	double sigmaFactors2D[size2D+1] = 
	    {1.00000000000,8.00390530e-1,2.72878894e-1,1.19779282e-1,
	     5.77664785e-2,2.86163283e-2,1.42747506e-2,7.13319703e-3,
	     3.56607618e-3,1.78297280e-3,8.91478237e-4,4.45738098e-4,
	     2.22868922e-4};

	const unsigned int size3D=12;
	this->maxNumScales[2] = size3D;
	double sigmaFactors3D[size3D+1] = 
	    {1.00000000000,8.959544490e-1,1.92033014e-1,5.76484078e-2,
	     1.94912393e-2,6.812783870e-3,2.40175885e-3,8.48538128e-4,
	     2.99949455e-4};

	this->sigmaFactors[0] = new std::vector<double>(sigmaFactors1D,sigmaFactors1D+this->maxNumScales[0]);
	this->sigmaFactors[1] = new std::vector<double>(sigmaFactors2D,sigmaFactors2D+this->maxNumScales[1]);
	this->sigmaFactors[2] = new std::vector<double>(sigmaFactors3D,sigmaFactors3D+this->maxNumScales[2]);

    }

    TriangleFilter::TriangleFilter(const TriangleFilter& other):
    Filter(other)
    {
    }

    TriangleFilter& TriangleFilter::operator= (const TriangleFilter& other)
    {
	if(this == &other) return *this;
	((Filter &) *this) = other;
	return *this;
    }

}
