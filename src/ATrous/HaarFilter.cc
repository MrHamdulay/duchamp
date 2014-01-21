#include <duchamp/ATrous/HaarFilter.hh>
#include <duchamp/ATrous/filter.hh>

namespace duchamp {

    HaarFilter::HaarFilter():
    Filter()
    {
	const unsigned int filtersize=3;
	double filter[filtersize] = {0., 1./2., 1./2.};
	this->filter1D = std::vector<double>(filter,filter+filtersize);
	this->name = "B3 spline function";
	
	const unsigned int size1D=6;
	this->maxNumScales[0] = size1D;
	double sigmaFactors1D[size1D+1] = 
	    {1.00000000000,7.07167810e-1,5.00000000e-1,3.53553391e-1,
	     2.50000000e-1,1.76776695e-1,1.25000000e-1};

	const unsigned int size2D=6;
	this->maxNumScales[1] = size2D;
	double sigmaFactors2D[size2D+1] = 
	    {1.00000000000,4.33012702e-1,2.16506351e-1,1.08253175e-1,
	     5.41265877e-2,2.70632939e-2,1.35316469e-2};

	const unsigned int size3D=8;
	this->maxNumScales[2] = size3D;
	double sigmaFactors3D[size3D+1] = 
	    {1.00000000000,9.35414347e-1,3.30718914e-1,1.16926793e-1,
	     4.13398642e-2,1.46158492e-2,5.16748303e-3,1.82698115e-3,
	     6.45935379e-44};

	this->sigmaFactors[0] = new std::vector<double>(sigmaFactors1D,sigmaFactors1D+this->maxNumScales[0]);
	this->sigmaFactors[1] = new std::vector<double>(sigmaFactors2D,sigmaFactors2D+this->maxNumScales[1]);
	this->sigmaFactors[2] = new std::vector<double>(sigmaFactors3D,sigmaFactors3D+this->maxNumScales[2]);

    }

    HaarFilter::HaarFilter(const HaarFilter& other):
    Filter(other)
    {
    }

    HaarFilter& HaarFilter::operator= (const HaarFilter& other)
    {
	if(this == &other) return *this;
	((Filter &) *this) = other;
	return *this;
    }

}
