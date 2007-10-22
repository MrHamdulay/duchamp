// -----------------------------------------------------------------------
// feedback.hh: Definition of the ProgressBar class, used to indicate
//              progress through a lengthy iteration.
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
#ifndef FEEDBACK_HH
#define FEEDBACK_HH

#include <iostream>
#include <string>

// Simple functions to print a given number of backspaces or spaces
//  to std::cout
inline void printBackSpace(std::ostream &stream, int num);
inline void printBackSpace(int num);
inline void printSpace(std::ostream &stream, int num);
inline void printSpace(int num);
inline void printHash(std::ostream &stream, int num);
inline void printHash(int num);

/**
 *  Controls printing out a progress bar.
 *
 *   A class that prints out a progress bar in the form 
 *    \f$|\#\#\#\ \ \ \ \ \ \ \ \ \ |\f$
 *    that shows how far through a function or loop you are.
 *   The length of it defaults to 20 hashes, but can be set when
 *   declaring the object.
 *   There are five functions: 
 *    <ul><li>init(int)   Prints an empty bar, and defines the increment
 *        <li>update(int) Prints the correct number of hashes, but only when
 *                         num is a multiple of the increment.
 *        <li>rewind()    Prints backspaces to cover the entire bar.
 *        <li>remove()    Does a rewind(), then prints spaces to overwrite
 *                         the bar area -- more clean.
 *        <li>fillSpace(std::string) Does a remove(), then writes
 *                                    the string into the same space.
 * </ul>
 */
class ProgressBar
{
public:
  ProgressBar();                          ///< Default Constructor
  ProgressBar(int newlength);             ///< Alternative constructor
  virtual ~ProgressBar();                 ///< Destructor.
  enum POS {BEG=0,END};                   ///< So that we can record
					  ///   where we are.

  void init(int size);                    ///< Prints empty bar,
					  ///   defines increment.
  void update(int num);                   ///< Prints correct number of hashes
  void rewind();                          ///< Prints backspaces over bar.
  void remove();                          ///< Overwrites bar with blanks
  void fillSpace(std::string someString); ///< Overwrites bar with a string.

private:
  POS loc;                                ///< Are we at the start or end?
  float stepSize;                         ///< What is the interval
					  ///   between hashes?
  int length;                             ///< What's the maximum
					  ///   number of hashes?
  int numVisible;                         ///< How many hashes are
					  ///   there currently visible?
};



#endif // FEEDBACK_HH
