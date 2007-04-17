#include <stdio.h>
#include <stdlib.h>
#include <cpgplot.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define WDGPIX 100  /* used by cpgwedglog 
		       --> number of increments in the wedge. */

/*
 This file contains the following programs, all in C code:

  int cpgtest() --> a front-end to cpgqinf, to test whether a pgplot
                    device is currently open. 
  int cpgIsPS() --> a front-end to cpgqinf, to test whether a postscript
                    device is currently open.
  void cpgwedglog(const char* side, float disp, float width, 
                  float fg, float bg, const char *label)
                --> a C-code version of pgwedg, plotting the wedge 
                    scale in logarithmic units.
  void cpgcumul(int npts, float *data, float datamin, float datamax, 
                       int pgflag)
                --> a new pgplot routine that draws a cumulative 
                    distribution. Uses _swap and _sort.
  void cpghistlog(npts, data, datamin, datamax, nbin, pgflag)
                --> as for cpghist, with the y-axis on a logarithmic scale.

*/


/********************************************************************/
/*   CPGTEST                                                        */
/********************************************************************/

int cpgtest()
{
  /** 
   *     A front-end to cpgqinf, to test whether a pgplot device is 
   *     currently open. Returns 1 if there is, else 0.
   */
  char answer[10];                 /* The PGQINF return string */
  int answer_len = sizeof(answer); /* allocated size of answer[] */
  cpgqinf("STATE", answer, &answer_len);
  return strcmp(answer, "OPEN") == 0;
}

/********************************************************************/
/*   CPGISPS                                                        */
/********************************************************************/

int cpgIsPS()
{
  /** 
   *     A front-end to cpgqinf, that tests whether the device is using a 
   *     postscript (by which we mean "hardcopy") device
   */
  char answer[50];
  int answer_len = sizeof(answer);
  cpgqinf("TYPE", answer, &answer_len);
  return ((answer[answer_len-2]=='P')&&((answer[answer_len-1]=='S')));
}

/********************************************************************/
/*   CPGWEDGLOG                                                     */
/********************************************************************/

void cpgwedglog(const char* side, float disp, float width, float fg, float bg, 
		const char *label)
{
  /** 
   *     A C-code version of PGWEDG that writes the scale of the wedge in 
   *      logarithmic coordinates. All parameters are exactly as for cpgwedg.
   */
  
  float wxa,wxb,wya,wyb, xa,xb,ya,yb; /* Temporary window coord storage.*/
  float vxa,vxb,vya,vyb;              /* Viewport coords of wedge. */
  float oldch, newch;                 /* Original and annotation character 
					 heights. */
  float ndcsiz;                       /* Size of unit character height 
					 (NDC units). */
  int horiz;                          /* Logical: True (=1) if wedge plotted 
					 horizontally. */
  int image;                          /* Logical: Use PGIMAG (T=1) or 
					 PGGRAY (F=0). */

  int nside,i;                        /* nside = symbolic version of side. */
  const int bot=1,top=2,lft=3,rgt=4;
  float wedwid, wdginc, vwidth, vdisp, xch, ych, labwid, fg1, bg1;
  float txtfrc=0.6;                   /* Set the fraction of WIDTH used 
					     for anotation. */
  float txtsep=2.2;                   /* Char separation between numbers 
					     and LABEL. */
  float wdgarr[WDGPIX];               /* Array to draw wedge in. */
  float tr[6] = {0.0,1.0,0.0,0.0,0.0,1.0};

/*   if(pgnoto("pgwedg")) return; */

  /* Get a numeric version of SIDE. */
  if(tolower(side[0])=='b'){
    nside = bot;
    horiz = 1;
  }
  else if(tolower(side[0]=='t')){
    nside = top;
    horiz = 1;
  }
  else if(tolower(side[0]=='l')){
    nside = lft;
    horiz = 0;
  }
  else if(tolower(side[0]=='r')){
    nside = rgt;
    horiz = 0;
  }
  /*   else gwarn("Invalid \"SIDE\" argument in PGWEDG.");  */
  else fprintf(stdout,"%%PGPLOT, Invalid \"SIDE\" argument in CPGWEDGLOG.");

  /* Determine which routine to use. */
  if(strlen(side)<2) image = 0;
  else if(tolower(side[1])=='i') image = 1;
  else if(tolower(side[1])=='g') image = 0;
  else fprintf(stdout,"%%PGPLOT, Invalid \"SIDE\" argument in CPGWEDGLOG.");

  cpgbbuf();

  /* Store the current world and viewport coords and the character height. */
  cpgqwin(&wxa, &wxb, &wya, &wyb);
  cpgqvp(0, &xa, &xb, &ya, &yb);
  cpgqch(&oldch);

  /* Determine the unit character height in NDC coords. */
  cpgsch(1.0);
  cpgqcs(0, &xch, &ych);
  if(horiz == 1)  ndcsiz = ych;
  else ndcsiz = xch;

  /* Convert 'WIDTH' and 'DISP' into viewport units. */
  vwidth = width * ndcsiz * oldch;
  vdisp  = disp * ndcsiz * oldch;

  /* Determine the number of character heights required under the wedge. */
  labwid = txtsep;
  if(strcmp(label," ")!=0) labwid = labwid + 1.0;
  
  /* Determine and set the character height required to fit the wedge
     anotation text within the area allowed for it. */
  newch = txtfrc*vwidth / (labwid*ndcsiz);
  cpgsch(newch);

  /* Determine the width of the wedge part of the plot minus the anotation.
     (NDC units). */
  wedwid = vwidth * (1.0-txtfrc);

  /* Use these to determine viewport coordinates for the wedge + annotation.*/
  vxa = xa;
  vxb = xb;
  vya = ya;
  vyb = yb;
  if(nside==bot){
    vyb = ya - vdisp;
    vya = vyb - wedwid;
  }
  else if(nside==top) {
    vya = yb + vdisp;
    vyb = vya + wedwid;
  }
  else if(nside==lft) {
    vxb = xa - vdisp;
    vxa = vxb - wedwid;
  }
  else if(nside==rgt) {
    vxa = xb + vdisp;
    vxb = vxa + wedwid;
  }

  /* Set the viewport for the wedge. */
  cpgsvp(vxa, vxb, vya, vyb);

  /* Swap FG/BG if necessary to get axis direction right. */
/*   fg1 = max(fg,bg); */
/*   bg1 = min(fg,bg); */
  if(fg>bg) {
    fg1 = fg;
    bg1 = bg;
  }
  else {
    fg1 = bg;
    bg1 = fg;
  }


  /* Create a dummy wedge array to be plotted. */
  wdginc = (fg1-bg1)/(WDGPIX-1);
  for(i=0;i<WDGPIX;i++)  wdgarr[i] = bg1 + (i-1) * wdginc;

  /* Draw the wedge then change the world coordinates for labelling. */
  if (horiz==1) {
    cpgswin(1.0, (float)WDGPIX, 0.9, 1.1);
    if (image==1) cpgimag(wdgarr, WDGPIX,1, 1,WDGPIX, 1,1, fg,bg, tr);
    else          cpggray(wdgarr, WDGPIX,1, 1,WDGPIX, 1,1, fg,bg, tr);
    cpgswin(bg1,fg1,0.0,1.0);
  }
  else{
    cpgswin(0.9, 1.1, 1.0, (float)WDGPIX);
    if (image==1) cpgimag(wdgarr, 1,WDGPIX, 1,1, 1,WDGPIX, fg,bg, tr);
    else          cpggray(wdgarr, 1,WDGPIX, 1,1, 1,WDGPIX, fg,bg, tr);
    cpgswin(0.0, 1.0, bg1, fg1);
  }

  /* Draw a labelled frame around the wedge -- using a logarithmic scale! */
  if(nside==bot)  cpgbox("BCNSTL",0.0,0,"BC",0.0,0);
  else if(nside==top) cpgbox("BCMSTL",0.0,0,"BC",0.0,0);
  else if(nside==lft) cpgbox("BC",0.0,0,"BCNSTL",0.0,0);
  else if(nside==rgt) cpgbox("BC",0.0,0,"BCMSTL",0.0,0);

  /* Write the units label. */
  if((strcmp(label," ")!=0)) cpgmtxt(side,txtsep,1.0,1.0,label);

  /* Reset the original viewport and world coordinates. */
  cpgsvp(xa,xb,ya,yb);
  cpgswin(wxa,wxb,wya,wyb);
  cpgsch(oldch);
  cpgebuf();

}


/********************************************************************/
/*   CPGCUMUL                                                       */
/********************************************************************/

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
   *  A new pgplot routine that draws a cumulative distribution.
   *   The use of pgflag is similar to cpghist & cpghist_log:

   *   <ul><li> 0 --> draw a new graph using cpgenv, going from 0 to 1
   *                  on the y-axis.
   *       <li> 2 --> draw the plot on the current graph, without
   *                  re-drawing any axes.  
   *   </ul>
   */

  int i;
  float *sorted;
  float MINCOUNT=0.;

  sorted = (float *)calloc(npts,sizeof(float));
  for(i=0;i<npts;i++) sorted[i] = data[i];
  _sort(sorted,0,npts);

  cpgbbuf();

  /* DEFINE ENVIRONMENT IF NECESSARY */
  if(pgflag == 0) cpgenv(datamin,datamax,MINCOUNT,1.0001,0,0);
  if(pgflag == 2) cpgswin(datamin,datamax,MINCOUNT,1.);

  /* DRAW LINE */
  cpgmove(datamin,MINCOUNT);
  for(i=0;i<npts;i++) cpgdraw(sorted[i],(float)(i+1)/(float)(npts));
  cpgdraw(datamax,1.);

  cpgebuf();
  
  free(sorted);

}

/********************************************************************/
/*   CPGHISTLOG                                                     */
/********************************************************************/

void cpghistlog(int npts, float *data, float datamin, float datamax, int nbin, 
		int pgflag)
{
  /**
   *  Works exactly as for cpghist, except the y-axis is a logarithmic scale.
   *
   */


  int i,bin;
  float *num;
  float maxNum,binSize,x1,x2,y1,y2,older,newer,fraction;
  float MINCOUNT=-0.2;

  num = (float *)calloc(nbin,sizeof(float));

  cpgbbuf();

  /* HOW MANY VALUES IN EACH BIN? */
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

  /* BOUNDARIES OF PLOT */
  x1 = datamin;
  x2 = datamax;
  y1 = MINCOUNT;
  y2 = ceil(maxNum);

  /* DEFINE ENVIRONMENT IF NECESSARY */
  if(pgflag%2 == 0) cpgenv(x1,x2,y1,y2,0,20);

  /* DRAW HISTOGRAM */
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

