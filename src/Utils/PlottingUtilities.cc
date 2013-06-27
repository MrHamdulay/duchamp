// -----------------------------------------------------------------------
// PlottingUtilities.cc : Utility functions to help plotting,
//                        particularly for using cpgsbox.
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
#include <duchamp/duchamp.hh>
#include <duchamp/Utils/mycpgplot.hh>
#include <wcslib/cpgsbox.h>
#include <wcslib/pgwcsl.h>
#include <wcslib/wcs.h>
#include <cpgplot.h>

void wcsAxes(struct wcsprm *wcs, size_t *axes, int textColour, int axisColour)
{

    /// @details
    ///  A front-end to the cpgsbox command, to draw the gridlines for the WCS 
    ///    over the current plot.
    ///  Lines are drawn in dark green over the full plot area, and the axis 
    ///    labels are written on the top and on the right hand sides, so as not 
    ///    to conflict with other labels.
    ///  \param textColour The colour index to use for the text labels --
    ///  defaults to duchamp::DUCHAMP_ID_TEXT_COLOUR 
    ///  \param axisColour The colour index to use for the axes --
    ///  defaults to duchamp::DUCHAMP_WCS_AXIS_COLOUR

    float boxXmin=0,boxYmin=0;

    char idents[3][80], opt[2], nlcprm[1];

    strcpy(idents[0], wcs->lngtyp);
    strcpy(idents[1], wcs->lattyp);
    strcpy(idents[2], "");
    if(strcmp(wcs->lngtyp,"RA")==0) opt[0] = 'G';
    else opt[0] = 'D';
    opt[1] = 'E';

    float  blc[2], trc[2];
    //   float  scl; // --> unused here.
    blc[0] = boxXmin + 0.5;
    blc[1] = boxYmin + 0.5;
    trc[0] = boxXmin + axes[0]+0.5;
    trc[1] = boxYmin + axes[1]+0.5;
  
    int existingLineWidth;
    cpgqlw(&existingLineWidth);
    int existingColour;
    cpgqci(&existingColour);
    float existingSize;
    cpgqch(&existingSize);
    cpgsci(textColour);
    cpgsch(0.8);
    int    c0[7], ci[7], gcode[2], ic, ierr;
    for(int i=0;i<7;i++) c0[i] = -1;
    /* define the WCS axes colour */
    mycpgplot::setWCSGreen();

    gcode[0] = 2;  // type of grid to draw: 0=none, 1=ticks only, 2=full grid
    gcode[1] = 2;

    double cache[257][4], grid1[9], grid2[9], nldprm[8];
    grid1[0] = 0.0;  
    grid2[0] = 0.0;

    // Draw the celestial grid with no intermediate tick marks.
    // Set LABCTL=2100 to write 1st coord on top, and 2nd on right

    //Colour indices used by cpgsbox: make it all the same colour for thin 
    // line case.
    ci[0] = axisColour; // grid lines, coord 1
    ci[1] = axisColour; // grid lines, coord 2
    ci[2] = axisColour; // numeric labels, coord 1
    ci[3] = axisColour; // numeric labels, coord 2
    ci[4] = axisColour; // axis annotation, coord 1
    ci[5] = axisColour; // axis annotation, coord 2
    ci[6] = axisColour; // title

    cpgsbox(blc, trc, idents, opt, 2100, 0, ci, gcode, 0.0, 0, grid1, 0, grid2,
	    0, pgwcsl_, 1, WCSLEN, 1, nlcprm, (int *)wcs, 
	    nldprm, 256, &ic, cache, &ierr);

    cpgsci(existingColour);
    cpgsch(existingSize);
    cpgslw(existingLineWidth);
}


