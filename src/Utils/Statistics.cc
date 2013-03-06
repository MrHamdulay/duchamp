// -----------------------------------------------------------------------
// Statistics.cc: Member functions for the templated StatsContainer
//                class.
// -----------------------------------------------------------------------
// Copyright (C) 2006, Matthew Whiting, ATNF
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// Duchamp is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with Duchamp; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
//
// Correspondence concerning Duchamp may be directed to:
//    Internet email: Matthew.Whiting [at] atnf.csiro.au
//    Postal address: Dr. Matthew Whiting
//                    Australia Telescope National Facility, CSIRO
//                    PO Box 76
//                    Epping NSW 1710
//                    AUSTRALIA
// -----------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <duchamp/Utils/Statistics.hh>
#include <duchamp/Utils/utils.hh>

namespace Statistics
{

  template <class T> 
  float madfmToSigma(T madfm){
    return float(madfm)/correctionFactor;
  }
  template float madfmToSigma<int>(int madfm);
  template float madfmToSigma<long>(long madfm);
  template float madfmToSigma<float>(float madfm);
  template float madfmToSigma<double>(double madfm);

  //--------------------------------------------------------------------
  float sigmaToMADFM(float sigma){
    return float(sigma)*correctionFactor;
  }
  //--------------------------------------------------------------------
  //--------------------------------------------------------------------

    template <class Type> 
    StatsContainer<Type>::StatsContainer()
    {
	defined = false;
	mean=Type(0);
	stddev=Type(0);
	median=Type(0);
	madfm=Type(0);
	threshold=Type(0);
	pThreshold=Type(0);
	useRobust=true; 
	useFDR=false; 
	commentString="";
    }
    template StatsContainer<int>::StatsContainer();
    template StatsContainer<long>::StatsContainer();
    template StatsContainer<float>::StatsContainer();
    template StatsContainer<double>::StatsContainer();
  //--------------------------------------------------------------------


  template <class Type> 
  StatsContainer<Type>::StatsContainer(const StatsContainer<Type>& s)
  {
    ///  @details
    ///  The copy constructor for the StatsContainer class. Just uses
    ///  the assignment operator.

    operator=(s);
  }
  template StatsContainer<int>::StatsContainer(const StatsContainer<int>& s);
  template StatsContainer<long>::StatsContainer(const StatsContainer<long>& s);
  template StatsContainer<float>::StatsContainer(const StatsContainer<float>& s);
  template StatsContainer<double>::StatsContainer(const StatsContainer<double>& s);
  //--------------------------------------------------------------------

  template <class Type> 
  StatsContainer<Type>& 
  StatsContainer<Type>::operator= (const StatsContainer<Type>& s)
  {
    ///  The assignment operator for the StatsContainer class.

    if(this == &s) return *this;
    this->defined    = s.defined;
    this->mean       = s.mean;
    this->stddev     = s.stddev;
    this->median     = s.median;
    this->madfm      = s.madfm;
    this->threshold  = s.threshold;
    this->pThreshold = s.pThreshold;
    this->useRobust  = s.useRobust;
    this->useFDR     = s.useFDR;
    this->commentString = s.commentString;
    return *this;
  }
  template StatsContainer<int>& StatsContainer<int>::operator= (const StatsContainer<int>& s);
  template StatsContainer<long>& StatsContainer<long>::operator= (const StatsContainer<long>& s);
  template StatsContainer<float>& StatsContainer<float>::operator= (const StatsContainer<float>& s);
  template StatsContainer<double>& StatsContainer<double>::operator= (const StatsContainer<double>& s);
  //--------------------------------------------------------------------

  template <class Type> 
  float StatsContainer<Type>::getThresholdSNR()
  {
    ///  @details
    /// The SNR is defined in terms of excess over the middle estimator
    /// in units of the spread estimator.
    return (threshold - this->getMiddle())/this->getSpread();
  }
  template float StatsContainer<int>::getThresholdSNR();
  template float StatsContainer<long>::getThresholdSNR();
  template float StatsContainer<float>::getThresholdSNR();
  template float StatsContainer<double>::getThresholdSNR();
  //--------------------------------------------------------------------

  template <class Type> 
  void  StatsContainer<Type>::setThresholdSNR(float snr)
  {
    ///  @details
    /// The SNR is defined in terms of excess over the middle estimator
    /// in units of the spread estimator.
      if(this->defined)
	  this->threshold=this->getMiddle() + snr*this->getSpread();
  }
  template void StatsContainer<int>::setThresholdSNR(float snr);
  template void StatsContainer<long>::setThresholdSNR(float snr);
  template void StatsContainer<float>::setThresholdSNR(float snr);
  template void StatsContainer<double>::setThresholdSNR(float snr);
  //--------------------------------------------------------------------
  
  template <class Type> 
  float StatsContainer<Type>::valueToSNR(float value)
  {
    ///  @details
    /// The SNR is defined in terms of excess over the middle estimator
    /// in units of the spread estimator.
      if(this->defined)
	  return (value - this->getMiddle())/this->getSpread();
      else
	  return Type(0);
  }
  template float StatsContainer<int>::valueToSNR(float value);
  template float StatsContainer<long>::valueToSNR(float value);
  template float StatsContainer<float>::valueToSNR(float value);
  template float StatsContainer<double>::valueToSNR(float value);
  //--------------------------------------------------------------------
  
  template <class Type> 
  float StatsContainer<Type>::snrToValue(float snr)
  {
    ///  @details
    /// The SNR is defined in terms of excess over the middle estimator
    /// in units of the spread estimator.
    return snr * this->getSpread() + this->getMiddle();
  }
  template float StatsContainer<int>::snrToValue(float value);
  template float StatsContainer<long>::snrToValue(float value);
  template float StatsContainer<float>::snrToValue(float value);
  template float StatsContainer<double>::snrToValue(float value);
  //--------------------------------------------------------------------
    
  template <class Type> 
  void StatsContainer<Type>::setMiddle(float middle)
  {
    ///  @details
    /// The middle value is determined by the StatsContainer::useRobust
    /// flag -- it will be either the median (if true), or the mean (if
    /// false).
    if(useRobust) this->median = Type(middle); 
    else this->mean = middle;
  }
  template void StatsContainer<int>::setMiddle(float middle);
  template void StatsContainer<long>::setMiddle(float middle);
  template void StatsContainer<float>::setMiddle(float middle);
  template void StatsContainer<double>::setMiddle(float middle);
  //--------------------------------------------------------------------
    
  template <class Type> 
  float StatsContainer<Type>::getMiddle()
  {
    ///  @details
    /// The middle value is determined by the StatsContainer::useRobust
    /// flag -- it will be either the median (if true), or the mean (if
    /// false).
    if(useRobust) return float(this->median); 
    else return this->mean;
  }
  template float StatsContainer<int>::getMiddle();
  template float StatsContainer<long>::getMiddle();
  template float StatsContainer<float>::getMiddle();
  template float StatsContainer<double>::getMiddle();
  //--------------------------------------------------------------------
    
  template <class Type> 
  void StatsContainer<Type>::setSpread(float spread)
  {
    ///  @details
    /// The spread value is set according to the
    /// StatsContainer::useRobust flag -- it will be either the madfm
    /// (if true), or the rms (if false). If robust, the spread value will be
    /// converted to a madfm from an equivalent rms under the assumption of
    /// Gaussianity, using the Statistics::sigmaToMADFM function.
    if(useRobust) this->madfm = Type(sigmaToMADFM(spread)); 
    else this->stddev = spread;
  }
  template void StatsContainer<int>::setSpread(float spread);
  template void StatsContainer<long>::setSpread(float spread);
  template void StatsContainer<float>::setSpread(float spread);
  template void StatsContainer<double>::setSpread(float spread);
  //--------------------------------------------------------------------
    
  template <class Type> 
  float StatsContainer<Type>::getSpread()
  {
    ///  @details
    /// The spread value returned is determined by the
    /// StatsContainer::useRobust flag -- it will be either the madfm
    /// (if true), or the rms (if false). If robust, the madfm will be
    /// converted to an equivalent rms under the assumption of
    /// Gaussianity, using the Statistics::madfmToSigma function.
    if(useRobust) return madfmToSigma(this->madfm); 
    else return this->stddev;
  }
  template float StatsContainer<int>::getSpread();
  template float StatsContainer<long>::getSpread();
  template float StatsContainer<float>::getSpread();
  template float StatsContainer<double>::getSpread();
  //--------------------------------------------------------------------
    
  template <class Type> 
  void  StatsContainer<Type>::scaleNoise(float scale)
  {
    /// @details
    ///  Multiply the noise parameters (stddev & madfm) by a given
    ///  factor, and adjust the threshold.
    float snr = (threshold - this->getMiddle())/this->getSpread();    
    this->madfm  = Type(this->madfm*scale);
    this->stddev *= scale;
    this->threshold = this->getMiddle() + snr*this->getSpread();
  }
  template void StatsContainer<int>::scaleNoise(float scale);
  template void StatsContainer<long>::scaleNoise(float scale);
  template void StatsContainer<float>::scaleNoise(float scale);
  template void StatsContainer<double>::scaleNoise(float scale);
 //--------------------------------------------------------------------

  template <class Type> 
  float StatsContainer<Type>::getPValue(float value)
  {
    ///  @details
    /// Get the "probability", under the assumption of normality, of a
    /// value occuring.  
    /// 
    /// It is defined by \f$0.5 \operatorname{erfc}(z/\sqrt{2})\f$, where
    /// \f$z=(x-\mu)/\sigma\f$. We need the factor of 0.5 here, as we are
    /// only considering the positive tail of the distribution -- we
    /// don't care about negative detections.
    
    float zStat = (value - this->getMiddle()) / this->getSpread();
    return 0.5 * erfc( zStat / M_SQRT2 );
  }
  template float StatsContainer<int>::getPValue(float value);
  template float StatsContainer<long>::getPValue(float value);
  template float StatsContainer<float>::getPValue(float value);
  template float StatsContainer<double>::getPValue(float value);
  //--------------------------------------------------------------------
    
  template <class Type> 
  bool StatsContainer<Type>::isDetection(float value)
  {
    ///  @details
    /// Compares the value given to the correct threshold, depending on
    /// the value of the StatsContainer::useFDR flag. 
    if(useFDR) return (this->getPValue(value) < this->pThreshold);
    else       return (value > this->threshold);
  }
  template bool StatsContainer<int>::isDetection(float value);
  template bool StatsContainer<long>::isDetection(float value);
  template bool StatsContainer<float>::isDetection(float value);
  template bool StatsContainer<double>::isDetection(float value);
  //--------------------------------------------------------------------

  template <class Type> 
  void StatsContainer<Type>::define(float mean, Type median, float stddev, Type madfm)
  {
    /// @details
    /// Set all four statistics directly
    /// 
      this->mean = mean;
      this->median = median;
      this->stddev = stddev;
      this->madfm = madfm;
      this->defined = true;
  }
  template void StatsContainer<int>::define(float mean, int median, float stddev, int madfm);
  template void StatsContainer<long>::define(float mean, long median, float stddev, long madfm);
  template void StatsContainer<float>::define(float mean, float median, float stddev, float madfm);
  template void StatsContainer<double>::define(float mean, double median, float stddev, double madfm);
  //--------------------------------------------------------------------

  template <class Type> 
  void StatsContainer<Type>::calculate(Type *array, long size)
  {
    /// @details
    /// Calculate all four statistics for all elements of a given
    /// array.
    /// 
    /// \param array The input data array.
    /// \param size The length of the input array
//     findNormalStats(array, size, this->mean, this->stddev);
//     findMedianStats(array, size, this->median, this->madfm);
    findAllStats(array,size,this->mean,this->stddev,this->median,this->madfm);
    this->defined = true;
  }
  template void StatsContainer<int>::calculate(int *array, long size);
  template void StatsContainer<long>::calculate(long *array, long size);
  template void StatsContainer<float>::calculate(float *array, long size);
  template void StatsContainer<double>::calculate(double *array, long size);
  //--------------------------------------------------------------------

  template <class Type> 
  void StatsContainer<Type>::calculate(Type *array, long size, bool *mask)
  {
    /// @details
    /// Calculate all four statistics for a subset of a given
    /// array. The subset is defined by an array of bool 
    /// variables.  
    /// 
    /// \param array The input data array.
    /// \param size The length of the input array
    /// \param mask An array of the same length that says whether to
    /// include each member of the array in the calculations. Use a
    /// value if mask=true.
//     findNormalStats(array, size, mask, this->mean, this->stddev);
//     findMedianStats(array, size, mask, this->median, this->madfm);
    findAllStats(array, size, mask, 
		 this->mean, this->stddev, this->median, this->madfm);
    this->defined = true;
  }
  template void StatsContainer<int>::calculate(int *array, long size, bool *mask);
  template void StatsContainer<long>::calculate(long *array, long size, bool *mask);
  template void StatsContainer<float>::calculate(float *array, long size, bool *mask);
  template void StatsContainer<double>::calculate(double *array, long size, bool *mask);
  //--------------------------------------------------------------------

  template <class Type> 
  std::ostream& operator<< (std::ostream& theStream, StatsContainer<Type> &s)
  {
    /// Prints out the four key statistics to the requested stream.
    theStream << s.commentString << " "
	      << "Mean   = "   << s.mean   << "\t"
	      << "Std.Dev. = " << s.stddev << "\n"
	      << s.commentString << " "
	      << "Median = "   << s.median << "\t"
	      << "MADFM    = " << s.madfm  << " (= " << madfmToSigma(s.madfm) << " as std.dev.)\n";
    return theStream;
  }
  template std::ostream& operator<<<int> (std::ostream& theStream, StatsContainer<int> &s);
  template std::ostream& operator<<<long> (std::ostream& theStream, StatsContainer<long> &s);
  template std::ostream& operator<<<float> (std::ostream& theStream, StatsContainer<float> &s);
  template std::ostream& operator<<<double> (std::ostream& theStream, StatsContainer<double> &s);

  //--------------------------------------------------------------------

  template <class Type>
  void StatsContainer<Type>::writeToBinaryFile(std::string filename)
  {
    std::ofstream outfile(filename.c_str(), std::ios::out | std::ios::binary | std::ios::app);
    outfile.write(reinterpret_cast<const char*>(&this->defined), sizeof this->defined);
    outfile.write(reinterpret_cast<const char*>(&this->mean), sizeof this->mean);
    outfile.write(reinterpret_cast<const char*>(&this->stddev), sizeof this->stddev);
    outfile.write(reinterpret_cast<const char*>(&this->median), sizeof this->median);
    outfile.write(reinterpret_cast<const char*>(&this->madfm), sizeof this->madfm);
    outfile.write(reinterpret_cast<const char*>(&this->threshold), sizeof this->threshold);
    outfile.write(reinterpret_cast<const char*>(&this->pThreshold), sizeof this->pThreshold);
    outfile.write(reinterpret_cast<const char*>(&this->useRobust), sizeof this->useRobust);
    outfile.write(reinterpret_cast<const char*>(&this->useFDR), sizeof this->useFDR);
    writeStringToBinaryFile(outfile,this->commentString);
    outfile.close();
  }
  template void StatsContainer<int>::writeToBinaryFile(std::string filename);
  template void StatsContainer<long>::writeToBinaryFile(std::string filename);
  template void StatsContainer<float>::writeToBinaryFile(std::string filename);
  template void StatsContainer<double>::writeToBinaryFile(std::string filename);

 //--------------------------------------------------------------------

  template <class Type>
  std::streampos StatsContainer<Type>::readFromBinaryFile(std::string filename, std::streampos loc)
  {
    std::ifstream infile(filename.c_str(), std::ios::in | std::ios::binary);
    if(!infile.is_open()){
      // DUCHAMPERROR("read binary stats","Could not open binary catalogue \""<<filename <<"\"");
      std::cerr << "Read binary stats : Could not open binary catalogue \"" << filename << "\"\n";
      return -1;
    }
    infile.seekg(loc);
    infile.read(reinterpret_cast<char*>(&this->defined), sizeof this->defined);
    infile.read(reinterpret_cast<char*>(&this->mean), sizeof this->mean);
    infile.read(reinterpret_cast<char*>(&this->stddev), sizeof this->stddev);
    infile.read(reinterpret_cast<char*>(&this->median), sizeof this->median);
    infile.read(reinterpret_cast<char*>(&this->madfm), sizeof this->madfm);
    infile.read(reinterpret_cast<char*>(&this->threshold), sizeof this->threshold);
    infile.read(reinterpret_cast<char*>(&this->pThreshold), sizeof this->pThreshold);
    infile.read(reinterpret_cast<char*>(&this->useRobust), sizeof this->useRobust);
    infile.read(reinterpret_cast<char*>(&this->useFDR), sizeof this->useFDR);
    this->commentString=readStringFromBinaryFile(infile);
    std::streampos newloc = infile.tellg();
    infile.close();
    return newloc;
  }
  template std::streampos StatsContainer<int>::readFromBinaryFile(std::string filename, std::streampos loc);
  template std::streampos StatsContainer<long>::readFromBinaryFile(std::string filename, std::streampos loc);
  template std::streampos StatsContainer<float>::readFromBinaryFile(std::string filename, std::streampos loc);
  template std::streampos StatsContainer<double>::readFromBinaryFile(std::string filename, std::streampos loc);



}  // matches:  namespace Statistics {
