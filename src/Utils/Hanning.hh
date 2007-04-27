#ifndef HANNING_H
#define HANNING_H

/**
 *  Define a Hanning filter.
 *
 *  A simple class to define a Hanning filter. It is characterised by
 *  a full width \f$2a-1\f$ (this is the number of coefficients and a
 *  set of coefficients that follow the functional form of \f$c(x) =
 *  0.5 + 0.5\cos(\pi x / a)\f$.
 */

class Hanning
{
public:
  Hanning();          ///< Basic constructor -- no filter width set.
  virtual ~Hanning(); ///< Destructor
  Hanning(int size);  ///< Specific constructor that sets width and
		      ///   coefficients

  void define(int size); ///< Define the size and the array of
			 ///   coefficients.

  float *smooth(float *array, int npts);  ///< Smooth an array with
					  ///the Hanning filter.
    
private:
  int hanningSize; ///< The full width of the filter (number of coefficients)
  float *coeffs;   ///< The coefficients of the filter
  bool allocated;  ///< Have the coefficients been allocated in memory?

};

#endif  // HANNING_H
