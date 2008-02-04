// -----------------------------------------------------------------------
// feedback.cc: Member functions for the ProgressBar class.
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
#include <string>
#include <duchamp/Utils/feedback.hh>

void printBackSpace(std::ostream &stream, int num)
{
  for(int i=0;i<num;i++) stream << '\b';
}
void printBackSpace(int num)
{
  printBackSpace(std::cout,num);
}

void printSpace(std::ostream &stream, int num)
{
  for(int i=0;i<num;i++) stream << ' '; 
}
void printSpace(int num)
{
  printSpace(std::cout,num);
}

void printHash(std::ostream &stream, int num)
{
  for(int i=0;i<num;i++) stream << '#'; 
}
void printHash(int num)
{
  printHash(std::cout,num);
}


ProgressBar::ProgressBar()
{
  /**
   * The default constructor defines a bar with 20 hashes 
   * (given by ProgressBar::length), sets the number visible to be 0 
   *  and the location to be at the beginning.
   */
  length=20; 
  loc=BEG; 
  numVisible = 0;
}

ProgressBar::ProgressBar(int newlength){
  /**
   * This alternative constructor enables the user to define how many
   * hashes should appear. Again, the number visible is set to 0 and
   * the location to be at the beginning.  
   *
   * \param newlength The new number of hashes to appear in the bar.
   */
  length=newlength; 
  loc=BEG; 
  numVisible = 0;
}

ProgressBar::~ProgressBar(){}

void ProgressBar::init(int size){ 
  /**
   * This initialises the bar to deal with a loop of a certain size.
   * This size will imply a certain step size, dependent on the number
   * of hashes that will be written.  A blank bar is written out as
   * well, and we remain at the end.  
   *
   * \param size The maximum number of iterations to be covered by the
   * progress bar.
   */
  stepSize = float(size) / float(length);
  std::cout << "|"; 
  printSpace(length); 
  std::cout << "|" << std::flush;
  loc = END;
}

void ProgressBar::update(int num){
  /**
   * This makes sure the correct number of hashes are drawn.
   *
   * Based on the number provided, as well as the stepsize, we compare
   * the number of hashes we expect to see with the number that are
   * there, and if they differ, the correct number are drawn. Again,
   * we remain at the end.  
   *
   * \param num The loop counter to be translated into the progress
   * bar.
   */
  int numNeeded = 0;
  for(int i=0;i<length;i++)
    if(num>(i*stepSize)) numNeeded++;
    
  if(numNeeded != numVisible){
    numVisible = numNeeded;
    if(loc==END) printBackSpace(length+2); 
    std::cout << "|"; 
    printHash(numNeeded);
    printSpace(length-numNeeded);
    std::cout << "|" << std::flush;
    loc=END;
  }
}

void ProgressBar::rewind(){
  /**
   * If we are at the end, we print out enough backspaces to wipe out
   * the entire bar.  If we are not, the erasing does not need to be
   * done.
   */
  if(loc==END) printBackSpace(length+2); 
  loc=BEG;
  std::cout << std::flush;
}

void ProgressBar::remove(){
  /**
   * We first rewind() to the beginning, overwrite the bar with blank spaces, 
   * and then rewind(). We end up at the beginning.
   */
  rewind();
  printSpace(length+2);
  loc=END; 
  rewind(); 
  std::cout << std::flush;
}

void ProgressBar::fillSpace(std::string someString){
  /**
   * We first remove() the bar and then write out the requested string.
   * \param someString The string to be written over the bar area.
   */
  remove();
  std::cout << someString;
  loc=END;
}

