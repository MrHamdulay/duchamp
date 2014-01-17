#include <duchamp/ATrous/B3SplineFilter.hh>
#include <duchamp/ATrous/Filter.hh>

namespace duchamp {

    B3SplineFilter::B3SplineFilter():
    Filter()
    {
	const unsigned int filtersize=5;
	double filter[filtersize] = {0.0625, 0.25, 0.375, 0.25, 0.0625};
	this->filter1D = std::vector<double>(filter,filter+filtersize);
	this->name = "B3 spline function";
	
	const unsigned int size1D=18;
	this->maxNumScales[0] = size1D;
	double sigmaFactors1D[size1D+1] = 
	    {1.00000000000,7.23489806e-1,2.85450405e-1,1.77947535e-1,
	     1.22223156e-1,8.58113122e-2,6.05703043e-2,4.28107206e-2,
	     3.02684024e-2,2.14024008e-2,1.51336781e-2,1.07011079e-2,
	     7.56682272e-3,5.35055108e-3,3.78341085e-3,2.67527545e-3,
	     1.89170541e-3,1.33763772e-3,9.45852704e-4};

	const unsigned int size2D=12;
	this->maxNumScales[1] = size2D;
	double sigmaFactors2D[size2D+1] = 
	    {1.00000000000,8.90796310e-1,2.00663851e-1,8.55075048e-2,
	     4.12474444e-2,2.04249666e-2,1.01897592e-2,5.09204670e-3,
	     2.54566946e-3,1.27279050e-3,6.36389722e-4,3.18194170e-4,
	     1.59096999e-4};

	const unsigned int size3D=7;
	this->maxNumScales[2] = size3D;
	double sigmaFactors3D[size3D+1] = 
	    {1.00000000000,9.56543592e-1,1.20336499e-1,3.49500154e-2,
	     1.18164242e-2,4.13233507e-3,1.45703714e-3,5.14791120e-4};

	this->sigmaFactors[0] = new std::vector<double>(sigmaFactors1D,sigmaFactors1D+this->maxNumScales[0]);
	this->sigmaFactors[1] = new std::vector<double>(sigmaFactors2D,sigmaFactors2D+this->maxNumScales[1]);
	this->sigmaFactors[2] = new std::vector<double>(sigmaFactors3D,sigmaFactors3D+this->maxNumScales[2]);

    }

    B3SplineFilter::B3SplineFilter(const B3SplineFilter& other):
    Filter(other)
    {
    }

    B3SplineFilter& B3SplineFilter::operator= (const B3SplineFilter& other)
    {
	if(this == &other) return *this;
	((Filter &) *this) = other;
	return *this;
    }

}
