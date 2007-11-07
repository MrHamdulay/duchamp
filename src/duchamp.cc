// -----------------------------------------------------------------------
// duchamp.cc: Warning and Error messages.
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
#include <iomanip>
#include <string>
#include <duchamp.hh>

namespace duchamp
{

  void duchampWarning(std::string subroutine, std::string warning)
  {
    /**
     * Prints a WARNING message to the standard error stream, quoting
     * the subroutine that the problem occurred in, and a descriptive
     * warning text.
     *
     * Format: WARNING \<function\> : Something happened that you should be aware of.
     */
    std::string preamble = "WARNING <" + subroutine + "> : ";
    std::cerr << preamble;
    for(int i=0;i<warning.size();i++){
      std::cerr << warning[i];
      if((i!=warning.size()-1)&&(warning[i]=='\n')) 
	std::cerr << std::setw(preamble.size()) <<": ";
    }
  }

  void duchampError(std::string subroutine, std::string error)
  {
    /**
     * Prints an ERROR message to the standard error stream, quoting
     * the subroutine that the problem occurred in, a descriptive
     * warning text, and sounding the bell.
     *
     * Format: ERROR \<function\> : Something bad happened.
     */
    std::string preamble = "\aERROR <" + subroutine + "> : ";
    std::cerr << preamble;
    for(int i=0;i<error.size();i++){
      std::cerr << error[i];
      if((i!=error.size()-1)&&(error[i]=='\n'))
	std::cerr << std::setw(preamble.size()-1) <<": ";
    }
  }

}
