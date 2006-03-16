#include <functional>
// #include <algorithm>
// #include <iterator>
// #include <iostream>

void swap(float &a, float &b)
{ 
  float t=a; a=b; b=t; 
}

void sort(float *array, int begin, int end) {
   float pivot = array[begin];
   int i = begin + 1, j = end, k = end;
   float t;

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
 
void sort(float *array, float *matchingArray, int begin, int end) 
{
   float pivot = array[begin];
   int i = begin + 1, j = end, k = end;
   float t;

   while (i < j) {
      if (array[i] < pivot) i++;
      else if (array[i] > pivot) {
         j--; k--;
         t = array[i];
         array[i] = array[j];
         array[j] = array[k];
         array[k] = t; 
         t = matchingArray[i];
         matchingArray[i] = matchingArray[j];
         matchingArray[j] = matchingArray[k];
         matchingArray[k] = t; 
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
//   cerr<<"!";
//     swap(arr[--l], arr[beg]);
//   cerr<<"!";
//     sort(arr, beg, l);
//     sort(arr, r, end);
//   }
// }

