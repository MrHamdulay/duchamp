#include <stdio.h>
#include <stdlib.h>
#include <cpgplot.h>
#include <math.h>
void cpghistlog(int npts, float *data, float datamin, float datamax, int nbin, int pgflag)
{
  int i,bin;
  float *num;
  float maxNum,binSize,x1,x2,y1,y2,older,newer,fraction;
  float MINCOUNT=-0.2;

/*   if(npts<1) grwarn( */
/* ||(datamax<datamin)||(nbin<1)||(nbin>MAXBIN)) */

  num = (float *)calloc(nbin,sizeof(float));

  cpgbbuf();

  // HOW MANY VALUES IN EACH BIN?
/*   for(i=0; i<nbin; i++) num[i] = 0.; */
  for(i=0; i<npts; i++){
    fraction = (data[i] - datamin) / (datamax - datamin);
    bin = (int)( floor(fraction*nbin) );
    if((bin>=0)&&(bin<nbin)) num[bin]+=1.;
  }
  for(i=0; i<nbin;i++){
    if(num[i]>0) num[i] = log10(num[i]);
    else num[i] = MINCOUNT;
  }
  maxNum = num[0];
  for(i=1; i<nbin; i++) if(num[i]>maxNum) maxNum = num[i];
  binSize = (datamax - datamin) / (float)nbin;

  // BOUNDARIES OF PLOT
  x1 = datamin;
  x2 = datamax;
  y1 = MINCOUNT;
  y2 = ceil(maxNum);

  // DEFINE ENVIRONMENT IF NECESSARY
  if(pgflag%2 == 0) cpgenv(x1,x2,y1,y2,0,20);

  // DRAW HISTOGRAM
  if(pgflag/2 == 0){
    older = 0.;
    x2 = datamin;
    cpgmove(datamin,MINCOUNT);
    for(i=0;i<nbin;i++){
      newer = num[i];
      x1 = x2;
      x2 = datamin + i*binSize;
      if(newer!=MINCOUNT){
	if(newer<=older){
	  cpgmove(x1,newer);
	  cpgdraw(x2,newer);
	}
	else{
	  cpgmove(x1,older);
	  cpgdraw(x1,newer);
	  cpgdraw(x2,newer);
	}
      }
      cpgdraw(x2,MINCOUNT);
      older=newer;
    }
  }
  else if(pgflag/2 == 1){
    older = MINCOUNT;
    x2 = datamin;
    for(i=0;i<nbin;i++){
      newer = num[i];
      x1 = x2;
      x2 = datamin + i*binSize;
      if(newer!=MINCOUNT) cpgrect(x1,x2,0.,newer);	
    }
  }
  else if(pgflag/2 == 2){
    older = 0.;
    cpgmove(datamin,MINCOUNT);
    x2 = datamin;
    for(i=0;i<nbin;i++){
      newer = num[i];
      x1 = x2;
      x2 = datamin + i*binSize;
      if((newer==MINCOUNT)&&(older==MINCOUNT)) cpgmove(x2,MINCOUNT);
      else {
	cpgdraw(x1,newer);
	if(newer==MINCOUNT) cpgmove(x2,newer);
	else cpgdraw(x2,newer);
      }
      older = newer;
    }
  }

  cpgebuf();
    
}
