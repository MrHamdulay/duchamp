// -----------------------------------------------------------------------
// sort.cc: Alternative sorting functions, including one that takes a
//          pair of arrays, sorts one but keeps pairs associated.
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
#include <functional>
#include <Utils/utils.hh>
// #include <algorithm>
// #include <iterator>
// #include <iostream>

template <class T> void sort(T *array, int begin, int end) 
{
  T pivot = array[begin];
  int i = begin + 1, j = end, k = end;
  T t;
  
  while (i < j) {
    if (array[i] < pivot) i++;
    else if (array[i] > pivot) {
      j--; k--;
      t = array[i];
      array[i] = array[j];
      array[j] = array[k];
      array[k] = t; }
    else {
      j--;
      std::swap(array[i], array[j]);
    }  }
  i--;
  std::swap(array[begin], array[i]);        
  if (i - begin > 1)
    sort(array, begin, i);
  if (end - k   > 1)
    sort(array, k, end);                      
}
template void sort<int>(int *array, int begin, int end);
template void sort<long>(long *array, int begin, int end);
template void sort<float>(float *array, int begin, int end);
template void sort<double>(double *array, int begin, int end);
//--------------------------------------------------------------------
 
template <class T1,class T2> 
void sort(T1 *array, T2 *matchingArray, int begin, int end) 
{
  T1 pivot = array[begin];
  int i = begin + 1, j = end, k = end;
  T1 t;
  T2 tt;

  while (i < j) {
    if (array[i] < pivot) i++;
    else if (array[i] > pivot) {
      j--; k--;
      t = array[i];
      array[i] = array[j];
      array[j] = array[k];
      array[k] = t; 
      tt = matchingArray[i];
      matchingArray[i] = matchingArray[j];
      matchingArray[j] = matchingArray[k];
      matchingArray[k] = tt; 
    }
    else {
      j--;
      std::swap(array[i], array[j]);
      std::swap(matchingArray[i], matchingArray[j]);
    }  
  }
  i--;
  std::swap(array[begin], array[i]);        
  std::swap(matchingArray[begin], matchingArray[i]);
  if (i - begin > 1)
    sort(array, matchingArray, begin, i);
  if (end - k   > 1)
    sort(array, matchingArray, k, end);                      
}
template void sort<int,int>(int *array, int *arrayB, int begin, int end);
template void sort<int,float>(int *arrayA, float *arrayB, int begin, int end);
template void sort<float,int>(float *array, int *arrayB, int begin, int end);
template void sort<float,float>(float *array, float *arrayB, int begin, int end);
//--------------------------------------------------------------------
 



// template< typename BidirectionalIterator, typename Compare >
// void quick_sort( BidirectionalIterator first, BidirectionalIterator last, Compare cmp ) {
//   if( first != last ) {
//     BidirectionalIterator left  = first;
//     BidirectionalIterator right = last;
//     BidirectionalIterator pivot = left++;

//     while( left != right ) {
//       if( cmp( *left, *pivot ) ) {
//          ++left;
//       } else {
//          while( (left != --right) && cmp( *pivot, *right ) )
//            ;
//          std::iter_swap( left, right );
//       }
//     }

//     --left;
//     std::iter_swap( first, left );

//     quick_sort( first, left, cmp );
//     quick_sort( right, last, cmp );
//   }
// }

// template< typename BidirectionalIterator >
// inline void quick_sort( BidirectionalIterator first, BidirectionalIterator last ) {
//   quick_sort( first, last,
//     std::less_equal< typename std::iterator_traits< BidirectionalIterator >::value_type >()
//   );
// }




// void sort(float *arr, int beg, int end) 
// {
//   if (end > beg + 1) 
//   {
//     float piv = arr[beg];
//     int l = beg + 1, r = end;
//     while (l < r) 
//     {
//       if (arr[l] <= piv) 
//         l++;
//       else 
//         swap(arr[l], arr[--r]);
//     }
//     swap(arr[--l], arr[beg]);
//     sort(arr, beg, l);
//     sort(arr, r, end);
//   }
// }

