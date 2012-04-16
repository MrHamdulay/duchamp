// -----------------------------------------------------------------------
// getStats.cc: Basic functions to calculate statistical parameters
//              such as mean, median, rms, madfm, min, max.
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
#include <algorithm>
#include <math.h>
#include <duchamp/Utils/utils.hh>

template <class T> T findMedian(T *array, size_t size, bool changeArray)
{
  /// @details
  /// Find the median value of an array of numbers. Type independent.
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param changeArray [false] Whether to use the provided array in calculations. If true, the input array will be altered (ie. the order of elements will be changed).
  /// \return The median value of the array, returned as the same type as the array.
  T *newarray;
  if(changeArray) newarray = array;
  else{
    newarray = new T[size];
    for(size_t i=0;i<size;i++) newarray[i] = array[i];
  }
  T median;
  bool isEven = ((size%2)==0);
  std::nth_element(newarray,newarray+size/2,newarray+size);
  median = newarray[size/2];
  if(isEven){
    std::nth_element(newarray,newarray+size/2-1,newarray+size);
    median += newarray[size/2-1];
    median /= T(2);
  }
  if(!changeArray) delete [] newarray;
  return median;
}
template int findMedian<int>(int *array, size_t size, bool changeArray);
template long findMedian<long>(long *array, size_t size, bool changeArray);
template float findMedian<float>(float *array, size_t size, bool changeArray);
template double findMedian<double>(double *array, size_t size, bool changeArray);
//--------------------------------------------------------------------

template <class T> T findMedianDiff(T *first, T *second, size_t size)
{
  /// @details
  /// Find the median value of an array of numbers. Type independent.
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param changeArray [false] Whether to use the provided array in calculations. If true, the input array will be altered (ie. the order of elements will be changed).
  /// \return The median value of the array, returned as the same type as the array.
  T *newarray = new T[size];
  for(size_t i=0;i<size;i++) newarray[i] = first[i]-second[i];
  
  T median;
  bool isEven = ((size%2)==0);
  std::nth_element(newarray,newarray+size/2,newarray+size);
  median = newarray[size/2];
  if(isEven){
    std::nth_element(newarray,newarray+size/2-1,newarray+size);
    median += newarray[size/2-1];
    median /= T(2);
  }
  delete [] newarray;
  return median;
}
template int findMedianDiff<int>(int *first, int *second, size_t size);
template long findMedianDiff<long>(long *first, long *second, size_t size);
template float findMedianDiff<float>(float *first, float *second, size_t size);
template double findMedianDiff<double>(double *first, double *second, size_t size);
//--------------------------------------------------------------------

template <class T> T findMedian(T *array, bool *mask, size_t size)
{
  /// @details
  /// Find the median value of an array of numbers. Type independent.
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param changeArray [false] Whether to use the provided array in calculations. If true, the input array will be altered (ie. the order of elements will be changed).
  /// \return The median value of the array, returned as the same type as the array.

  int goodSize=0,ct=0;
  for(size_t i=0;i<size;i++) if(mask[i]) goodSize++;
  T *newarray = new T[goodSize];
  for(size_t i=0;i<size;i++) {
    if(mask[i]) newarray[ct++] = array[i];
  }
  T median;
  bool isEven = ((goodSize%2)==0);
  std::nth_element(newarray,newarray+goodSize/2,newarray+goodSize);
  median = newarray[goodSize/2];
  if(isEven){
    std::nth_element(newarray,newarray+goodSize/2-1,newarray+goodSize);
    median += newarray[goodSize/2-1];
    median /= T(2);
  }
  delete [] newarray;
  return median;
}
template int findMedian<int>(int *array, bool *mask, size_t size);
template long findMedian<long>(long *array, bool *mask, size_t size);
template float findMedian<float>(float *array, bool *mask, size_t size);
template double findMedian<double>(double *array, bool *mask, size_t size);
//--------------------------------------------------------------------

template <class T> T findMedianDiff(T *first, T *second, bool *mask, size_t size)
{
  /// @details
  /// Find the median value of an array of numbers. Type independent.
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param changeArray [false] Whether to use the provided array in calculations. If true, the input array will be altered (ie. the order of elements will be changed).
  /// \return The median value of the array, returned as the same type as the array.
  int goodSize=0,ct=0;
  for(size_t i=0;i<size;i++) if(mask[i]) goodSize++;
  T *newarray = new T[goodSize];
  for(size_t i=0;i<size;i++)
    if(mask[i]) newarray[ct++] = first[i]-second[i];
  
  T median=0;
  bool isEven = ((goodSize%2)==0);
  std::nth_element(newarray,newarray+goodSize/2,newarray+goodSize);
  median = newarray[goodSize/2];
  if(isEven){
    std::nth_element(newarray,newarray+goodSize/2-1,newarray+goodSize);
    median += newarray[goodSize/2-1];
    median /= T(2);
  }
  delete [] newarray;
  return median;
}
template int findMedianDiff<int>(int *first, int *second, bool *mask, size_t size);
template long findMedianDiff<long>(long *first, long *second, bool *mask, size_t size);
template float findMedianDiff<float>(float *first, float *second, bool *mask, size_t size);
template double findMedianDiff<double>(double *first, double *second, bool *mask, size_t size);
//--------------------------------------------------------------------

template <class T> T findMADFM(T *array, size_t size, bool changeArray)
{
  /// @details
  /// Find the median absolute deviation from the median value of an
  /// array of numbers. Type independent.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param changeArray [false] Whether to use the provided array in calculations. If true, the input array will be altered - both the order and values of the elements will be changed.
  /// \return The median absolute deviation from the median value of
  /// the array, returned as the same type as the array.
  T *newarray;
  if(changeArray) newarray = array;
  else newarray = new T[size];

  T median = findMedian<T>(array,size,changeArray);
  T madfm;
  bool isEven = ((size%2)==0);
  for(size_t i=0;i<size;i++) newarray[i] = absval(array[i]-median);
  std::nth_element(newarray,newarray+size/2,newarray+size);
  madfm = newarray[size/2];
  if(isEven){
    std::nth_element(newarray,newarray+size/2-1,newarray+size);
    madfm += newarray[size/2-1];
    madfm /= T(2);
  }
  if(!changeArray) delete [] newarray;
  return madfm;
}
template int findMADFM<int>(int *array, size_t size, bool changeArray);
template long findMADFM<long>(long *array, size_t size, bool changeArray);
template float findMADFM<float>(float *array, size_t size, bool changeArray);
template double findMADFM<double>(double *array, size_t size, bool changeArray);
//--------------------------------------------------------------------

template <class T> T findMADFMDiff(T *first, T *second, size_t size)
{
  /// @details
  /// Find the median absolute deviation from the median value of an
  /// array of numbers. Type independent.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param changeArray [false] Whether to use the provided array in calculations. If true, the input array will be altered - both the order and values of the elements will be changed.
  /// \return The median absolute deviation from the median value of
  /// the array, returned as the same type as the array.
  T *newarray = new T[size];
  T median = findMedianDiff<T>(first,second,size);
  T madfm;
  bool isEven = ((size%2)==0);
  for(size_t i=0;i<size;i++) newarray[i] = absval(first[i]-second[i]-median);
  std::nth_element(newarray,newarray+size/2,newarray+size);
  madfm = newarray[size/2];
  if(isEven){
    std::nth_element(newarray,newarray+size/2-1,newarray+size);
    madfm += newarray[size/2-1];
    madfm /= T(2);
  }
  delete [] newarray;
  return madfm;
}
template int findMADFMDiff<int>(int *first, int *second, size_t size);
template long findMADFMDiff<long>(long *first, long *second, size_t size);
template float findMADFMDiff<float>(float *first, float *second, size_t size);
template double findMADFMDiff<double>(double *first, double *second, size_t size);
//--------------------------------------------------------------------

template <class T> T findMADFM(T *array, bool *mask, size_t size)
{
  /// @details
  /// Find the median absolute deviation from the median value of an
  /// array of numbers. Type independent.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param changeArray [false] Whether to use the provided array in calculations. If true, the input array will be altered - both the order and values of the elements will be changed.
  /// \return The median absolute deviation from the median value of
  /// the array, returned as the same type as the array.
  T median = findMedian<T>(array,mask,size);
  int goodSize=0,ct=0;
  for(size_t i=0;i<size;i++) if(mask[i]) goodSize++;
  T *newarray = new T[goodSize];
  for(size_t i=0;i<size;i++)
    if(mask[i]) newarray[ct++] = absval(array[i]-median);

  T madfm;
  bool isEven = ((goodSize%2)==0);
  std::nth_element(newarray,newarray+goodSize/2,newarray+goodSize);
  madfm = newarray[goodSize/2];
  if(isEven){
    std::nth_element(newarray,newarray+goodSize/2-1,newarray+goodSize);
    madfm += newarray[goodSize/2-1];
    madfm /= T(2);
  }
  delete [] newarray;
  return madfm;
}
template int findMADFM<int>(int *array, bool *mask, size_t size);
template long findMADFM<long>(long *array, bool *mask, size_t size);
template float findMADFM<float>(float *array, bool *mask, size_t size);
template double findMADFM<double>(double *array, bool *mask, size_t size);
//--------------------------------------------------------------------

template <class T> T findMADFMDiff(T *first, T *second, bool *mask, size_t size)
{
  /// @details
  /// Find the median absolute deviation from the median value of an
  /// array of numbers. Type independent.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param changeArray [false] Whether to use the provided array in calculations. If true, the input array will be altered - both the order and values of the elements will be changed.
  /// \return The median absolute deviation from the median value of
  /// the array, returned as the same type as the array.
  T median = findMedianDiff<T>(first,second,mask,size);
  int goodSize=0,ct=0;
  for(size_t i=0;i<size;i++) if(mask[i]) goodSize++;
  T *newarray = new T[goodSize];
  for(size_t i=0;i<size;i++)
    if(mask[i]) newarray[ct++] = absval(first[i]-second[i]-median);

  T madfm;
  bool isEven = ((goodSize%2)==0);
  std::nth_element(newarray,newarray+goodSize/2,newarray+goodSize);
  madfm = newarray[goodSize/2];
  if(isEven){
    std::nth_element(newarray,newarray+goodSize/2-1,newarray+goodSize);
    madfm += newarray[goodSize/2-1];
    madfm /= T(2);
  }
  delete [] newarray;
  return madfm;
}
template int findMADFMDiff<int>(int *first, int *second, bool *mask, size_t size);
template long findMADFMDiff<long>(long *first, long *second, bool *mask, size_t size);
template float findMADFMDiff<float>(float *first, float *second, bool *mask, size_t size);
template double findMADFMDiff<double>(double *first, double *second, bool *mask, size_t size);
//--------------------------------------------------------------------

template <class T> T findMADFM(T *array, size_t size, T median, bool changeArray)
{
  /// @details
  /// Find the median absolute deviation from the median value of an
  /// array of numbers. Type independent. This version accepts a previously-
  /// calculated median value.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param median The median of the array.
  /// \param changeArray [false] Whether to use the provided array in calculations. If true, the input array will be altered - both the order and values of the elements will be changed.
  /// \return The median absolute deviation from the median value of
  /// the array, returned as the same type as the array.
  T *newarray;
  if(changeArray) newarray = array;
  else newarray = new T[size];

  T madfm;
  bool isEven = ((size%2)==0);
  for(size_t i=0;i<size;i++) newarray[i] = absval(array[i]-median);
  std::nth_element(newarray,newarray+size/2,newarray+size);
  madfm = newarray[size/2];
  if(isEven){
    std::nth_element(newarray,newarray+size/2-1,newarray+size);
    madfm += newarray[size/2-1];
    madfm /= T(2);
  }
  if(!changeArray) delete [] newarray;
  return madfm;
}
template int findMADFM<int>(int *array, size_t size, int median, bool changeArray);
template long findMADFM<long>(long *array, size_t size, long median, bool changeArray);
template float findMADFM<float>(float *array, size_t size, float median, bool changeArray);
template double findMADFM<double>(double *array, size_t size, double median, bool changeArray);
//--------------------------------------------------------------------

template <class T> T findMADFMDiff(T *first, T *second, size_t size, T median)
{
  /// @details
  /// Find the median absolute deviation from the median value of an
  /// array of numbers. Type independent. This version accepts a previously-
  /// calculated median value.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param median The median of the array.
  /// \param changeArray [false] Whether to use the provided array in calculations. If true, the input array will be altered - both the order and values of the elements will be changed.
  /// \return The median absolute deviation from the median value of
  /// the array, returned as the same type as the array.
  T *newarray = new T[size];

  T madfm;
  bool isEven = ((size%2)==0);
  for(size_t i=0;i<size;i++) newarray[i] = absval(first[i]-second[i]-median);
  std::nth_element(newarray,newarray+size/2,newarray+size);
  madfm = newarray[size/2];
  if(isEven){
    std::nth_element(newarray,newarray+size/2-1,newarray+size);
    madfm += newarray[size/2-1];
    madfm /= T(2);
  }
  delete [] newarray;
  return madfm;
}
template int findMADFMDiff<int>(int *first, int *second, size_t size, int median);
template long findMADFMDiff<long>(long *first, long *second, size_t size, long median);
template float findMADFMDiff<float>(float *first, float *second, size_t size, float median);
template double findMADFMDiff<double>(double *first, double *second, size_t size, double median);
//--------------------------------------------------------------------

template <class T> T findMADFM(T *array, bool *mask, size_t size, T median)
{
  /// @details
  /// Find the median absolute deviation from the median value of an
  /// array of numbers. Type independent. This version accepts a previously-
  /// calculated median value.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param median The median of the array.
  /// \param changeArray [false] Whether to use the provided array in calculations. If true, the input array will be altered - both the order and values of the elements will be changed.
  /// \return The median absolute deviation from the median value of
  /// the array, returned as the same type as the array.
  int goodSize=0,ct=0;
  for(size_t i=0;i<size;i++) if(mask[i]) goodSize++;
  T *newarray = new T[goodSize];
  for(size_t i=0;i<size;i++){
    if(mask[i]) newarray[ct++] = absval(array[i]-median);
  }
  T madfm;
  bool isEven = ((goodSize%2)==0);
  std::nth_element(newarray,newarray+goodSize/2,newarray+goodSize);
  madfm = newarray[goodSize/2];
  if(isEven){
    std::nth_element(newarray,newarray+goodSize/2-1,newarray+goodSize);
    madfm += newarray[goodSize/2-1];
    madfm /= T(2);
  }
  delete [] newarray;
  return madfm;
}
template int findMADFM<int>(int *array, bool *mask, size_t size, int median);
template long findMADFM<long>(long *array, bool *mask, size_t size, long median);
template float findMADFM<float>(float *array, bool *mask, size_t size, float median);
template double findMADFM<double>(double *array, bool *mask, size_t size, double median);
//--------------------------------------------------------------------

template <class T> T findMADFMDiff(T *first, T *second, bool *mask, size_t size, T median)
{
  /// @details
  /// Find the median absolute deviation from the median value of an
  /// array of numbers. Type independent. This version accepts a previously-
  /// calculated median value.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param median The median of the array.
  /// \param changeArray [false] Whether to use the provided array in calculations. If true, the input array will be altered - both the order and values of the elements will be changed.
  /// \return The median absolute deviation from the median value of
  /// the array, returned as the same type as the array.
  int goodSize=0,ct=0;
  for(size_t i=0;i<size;i++) if(mask[i]) goodSize++;
  T *newarray = new T[goodSize];
  for(size_t i=0;i<size;i++){
    if(mask[i]) newarray[ct++] = absval(first[i]-second[i]-median);
  }
  T madfm;
  bool isEven = ((goodSize%2)==0);
  std::nth_element(newarray,newarray+goodSize/2,newarray+goodSize);
  madfm = newarray[goodSize/2];
  if(isEven){
    std::nth_element(newarray,newarray+goodSize/2-1,newarray+goodSize);
    madfm += newarray[goodSize/2-1];
    madfm /= T(2);
  }
  delete [] newarray;
  return madfm;
}
template int findMADFMDiff<int>(int *first, int *second, bool *mask, size_t size, int median);
template long findMADFMDiff<long>(long *first, long *second, bool *mask, size_t size, long median);
template float findMADFMDiff<float>(float *first, float *second, bool *mask, size_t size, float median);
template double findMADFMDiff<double>(double *first, double *second, bool *mask, size_t size, double median);
//--------------------------------------------------------------------

template <class T> void findMedianStats(T *array, size_t size, 
					T &median, T &madfm)
{
  /// @details
  /// Find the median and the median absolute deviation from the median
  /// value of an array of numbers. Type independent.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param median The median value of the array, returned as the same
  /// type as the array.
  /// \param madfm The median absolute deviation from the median value
  /// of the array, returned as the same type as the array.
  if(size==0){
    std::cerr << "Error in findMedianStats: zero sized array!\n";
    return;
  }
  T *newarray = new T[size];
   for(size_t i=0;i<size;i++) newarray[i] = array[i];

   median = findMedian<T>(newarray,size,true);
   //   madfm = findMADFM<T>(newarray,size,true);
   madfm = findMADFM<T>(newarray,size,median,true);

  delete [] newarray;
}
template void findMedianStats<int>(int *array, size_t size, 
				      int &median, int &madfm);
template void findMedianStats<long>(long *array, size_t size, 
				       long &median, long &madfm);
template void findMedianStats<float>(float *array, size_t size, 
					float &median, float &madfm);
template void findMedianStats<double>(double *array, size_t size, 
					 double &median, double &madfm);
//--------------------------------------------------------------------

template <class T> void findMedianStats(T *array, size_t size, bool *mask, 
					T &median, T &madfm)
{
  /// @details
  /// Find the median and the median absolute deviation from the median
  /// value of a subset of an array of numbers. The subset is defined
  /// by an array of bool variables. Type independent.
  /// 
  /// \param array The array of numbers.
  /// \param size The length of the array.
  /// \param mask An array of the same length that says whether to
  /// include each member of the array in the calculations. Only use
  /// values where mask=true.
  /// \param median The median value of the array, returned as the same
  /// type as the array.
  /// \param madfm The median absolute deviation from the median value
  /// of the array, returned as the same type as the array.

  int goodSize=0;
  for(size_t i=0;i<size;i++) if(mask[i]) goodSize++;
  if(goodSize==0){
    std::cerr << "Error in findMedianStats: no good values!\n";
    return;
  }
  T *newarray = new T[goodSize];

  goodSize=0;
  for(size_t i=0;i<size;i++) if(mask[i]) newarray[goodSize++] = array[i];
  median = findMedian<T>(newarray,goodSize,true);
  //  madfm = findMADFM<T>(newarray,goodSize,true);
  madfm = findMADFM<T>(newarray,goodSize,median,true);

  delete [] newarray;
}
template void findMedianStats<int>(int *array, size_t size, bool *mask, 
				      int &median, int &madfm);
template void findMedianStats<long>(long *array, size_t size, bool *mask, 
				       long &median, long &madfm);
template void findMedianStats<float>(float *array, size_t size, bool *mask, 
					float &median, float &madfm);
template void findMedianStats<double>(double *array, size_t size, bool *mask, 
					 double &median, double &madfm);
//--------------------------------------------------------------------

template <class T> void findMedianStatsDiff(T *first, T *second, size_t size, 
					    T &median, T &madfm)
{
  /// @details Find the median and the median absolute deviation from
  /// the median value of the difference between two arrays of
  /// numbers. Type independent. The difference is defined as first -
  /// second.
  /// 
  /// \param first The first array
  /// \param second The second array
  /// \param size The length of the array.
  /// \param median The median value of the array, returned as the same
  /// type as the array.
  /// \param madfm The median absolute deviation from the median value
  /// of the array, returned as the same type as the array.
  if(size==0){
    std::cerr << "Error in findMedianStats: zero sized array!\n";
    return;
  }
  T *newarray = new T[size];
   for(size_t i=0;i<size;i++) newarray[i] = first[i]-second[i];

   median = findMedian<T>(newarray,size,true);
   //   madfm = findMADFM<T>(newarray,size,true);
   madfm = findMADFM<T>(newarray,size,median,true);

  delete [] newarray;
}
template void findMedianStatsDiff<int>(int *first, int *second, size_t size, 
				       int &median, int &madfm);
template void findMedianStatsDiff<long>(long *first, long *second, size_t size, 
					long &median, long &madfm);
template void findMedianStatsDiff<float>(float *first, float *second, size_t size, 
					 float &median, float &madfm);
template void findMedianStatsDiff<double>(double *first, double *second, size_t size, 
					  double &median, double &madfm);
//--------------------------------------------------------------------

template <class T> void findMedianStatsDiff(T *first, T *second, size_t size, bool *mask, 
					    T &median, T &madfm)
{
  /// @details Find the median and the median absolute deviation from
  /// the median value of the difference between two arrays of
  /// numbers, where some elements are masked out. The mask is defined
  /// by an array of bool variables. Type independent. The difference
  /// is defined as first - second.
  /// 
  /// \param first The first array
  /// \param second The second array
  /// \param size The length of the array.
  /// \param mask An array of the same length that says whether to
  /// include each member of the array in the calculations. Only use
  /// values where mask=true.
  /// \param median The median value of the array, returned as the same
  /// type as the array.
  /// \param madfm The median absolute deviation from the median value
  /// of the array, returned as the same type as the array.

  int goodSize=0;
  for(size_t i=0;i<size;i++) if(mask[i]) goodSize++;
  if(goodSize==0){
    std::cerr << "Error in findMedianStats: no good values!\n";
    return;
  }
  T *newarray = new T[goodSize];

  goodSize=0;
  for(size_t i=0;i<size;i++) if(mask[i]) newarray[goodSize++] = first[i]-second[i];
  median = findMedian<T>(newarray,goodSize,true);
  //  madfm = findMADFM<T>(newarray,goodSize,true);
  madfm = findMADFM<T>(newarray,goodSize,median,true);

  delete [] newarray;
}
template void findMedianStatsDiff<int>(int *first, int *second, size_t size, bool *mask, 
				       int &median, int &madfm);
template void findMedianStatsDiff<long>(long *first, long *second, size_t size, bool *mask, 
					long &median, long &madfm);
template void findMedianStatsDiff<float>(float *first, float *second, size_t size, bool *mask, 
					 float &median, float &madfm);
template void findMedianStatsDiff<double>(double *first, double *second, size_t size, bool *mask, 
					  double &median, double &madfm);
//--------------------------------------------------------------------
  
