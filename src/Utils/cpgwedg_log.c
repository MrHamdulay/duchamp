#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <cpgplot.h>
#include <math.h>
#define WDGPIX 100
void cpgwedglog(const char* side, float disp, float width, float fg, float bg, const char *label)
{
  /** 
   *  cpgwedglog
   *     A C-code version of PGWEDG that writes the scale of the wedge in logarithmic
   *     coordinates. All parameters are exactly as for cpgwedg.
   */
  
  float wxa,wxb,wya,wyb, xa,xb,ya,yb; // Temporary window coord storage.
  float vxa,vxb,vya,vyb;              // Viewport coords of wedge.
  float oldch, newch;                 // Original and anotation character heights.
  float ndcsiz;                       // Size of unit character height (NDC units).
  int horiz;                          // Logical: True (=1) if wedge plotted horizontally.
  int image;                          // Logical: Use PGIMAG (T=1) or PGGRAY (F=0).

  int nside,i;                        // nside = symbolic version of side.
  const int bot=1,top=2,lft=3,rgt=4;
  float wedwid, wdginc, vwidth, vdisp, xch, ych, labwid, fg1, bg1;
  float txtfrc=0.6;                       // Set the fraction of WIDTH used for anotation.
  float txtsep=2.2;                       // Char separation between numbers and LABEL.
  float wdgarr[WDGPIX];                   // Array to draw wedge in.
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
  //   else gwarn("Invalid \"SIDE\" argument in PGWEDG."); 
  else fprintf(stdout,"%PGPLOT, Invalid \"SIDE\" argument in CPGWEDGLOG.");

  /* Determine which routine to use. */
  if(strlen(side)<2) image = 0;
  else if(tolower(side[1])=='i') image = 1;
  else if(tolower(side[1])=='g') image = 0;
  else fprintf(stdout,"%PGPLOT, Invalid \"SIDE\" argument in CPGWEDGLOG.");

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

  /* Use these to determine viewport coordinates for the wedge + annotation. */
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
