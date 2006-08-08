#include <stdio.h>
#include <stdlib.h>
#include <cpgplot.h>
#include <math.h>

void _swap(float *a, float *b)
{ 
  float t;
  t=*a; *a=*b; *b=t; 
}

void _sort(float *array, int begin, int end) 
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
      array[k] = t; }
    else {
      j--;
      _swap(&array[i], &array[j]);
    }  }
  i--;
  _swap(&array[begin], &array[i]);        
  if (i - begin > 1)
    _sort(array, begin, i);
  if (end - k   > 1)
    _sort(array, k, end);                      
}


void cpgcumul(int npts, float *data, float datamin, float datamax, int pgflag)
{
  /** 
   *  cpgcumul(npts, data, datamin, datamax, pgflag)
   *    A new pgplot routine that draws a cumulative distribution.
   *   The use of pgflag is similar to cpghist & cpghist_log:
   *    0 --> draw a new graph using cpgenv, going from 0 to 1 on the y-axis.
   *    2 --> draw the plot on the current graph, without re-drawing any axes.
   */

  int i;
  float *sorted;
  float MINCOUNT=0.;

  sorted = (float *)calloc(npts,sizeof(float));
  for(i=0;i<npts;i++) sorted[i] = data[i];
  _sort(sorted,0,npts);

  cpgbbuf();

  // DEFINE ENVIRONMENT IF NECESSARY
  if(pgflag == 0) cpgenv(datamin,datamax,MINCOUNT,1.0001,0,0);
  if(pgflag == 2) cpgswin(datamin,datamax,MINCOUNT,1.);

  // DRAW LINE
  cpgmove(datamin,MINCOUNT);
  for(i=0;i<npts;i++) cpgdraw(sorted[i],(float)(i+1)/(float)(npts));
  cpgdraw(datamax,1.);

  cpgebuf();
  
  free(sorted);

}


