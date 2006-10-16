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
      swap(array[i], array[j]);
    }  }
  i--;
  swap(array[begin], array[i]);        
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
      swap(array[i], array[j]);
      swap(matchingArray[i], matchingArray[j]);
    }  
  }
  i--;
  swap(array[begin], array[i]);        
  swap(matchingArray[begin], matchingArray[i]);
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

