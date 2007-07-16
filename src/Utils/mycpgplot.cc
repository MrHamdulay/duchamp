// -----------------------------------------------------------------------
// mycpgplot.cc: Various useful functions for use with PGPLOT.
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
#include <string>
#include <cpgplot.h>
#include <mycpgplot.hh>

namespace mycpgplot
{
  
  int mycpgopen(std::string device)
  {
    /** 
     * This function opens the requested device using cpgopen.
     *
     * It also defines both the new colours DARKGREEN and WCSGREEN,
     * for later use so that they don't have to be defined manually.
     *
     * \param device The PGPLOT device to be opened.
     * \return The return value of cpgopen.
     */
    int status = cpgopen(device.c_str());
    if(status>0){
      setDarkGreen();
      setWCSGreen();
    }
    return status;
  }

  void setDarkGreen(){
    cpgscr(DARKGREEN,0.,0.7,0.);
  }

  void setWCSGreen(){
    cpgscr(WCSGREEN,0.3,0.5,0.3);
  }

  void setWhite(){
  /**
   * Uses cpgIsPS() to determine whether a device is a postscript
   * one. If so, we use the BACKGND colour (ie. 0), otherwise use
   * FOREGND (1).
   */
    if(cpgIsPS()) cpgsci(BACKGND);
    else cpgsci(FOREGND);
  }

  
  void setBlack(){
  /**
   * Uses cpgIsPS() to determine whether a device is a postscript
   * one. If so, we use the FOREGND colour (ie. 1), otherwise use
   * BACKGND (0).
   */
    if(cpgIsPS()) cpgsci(FOREGND);
    else cpgsci(BACKGND);
  }

}
