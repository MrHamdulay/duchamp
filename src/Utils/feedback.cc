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
#include <iomanip>
#include <string>
#include <duchamp/Utils/feedback.hh>

void printBackSpace(std::ostream &stream, int num)
{
    for(int i=0;i<num;i++) stream << '\b';
}

void printSpace(std::ostream &stream, int num)
{
    for(int i=0;i<num;i++) stream << ' '; 
}

void printHash(std::ostream &stream, int num)
{
    for(int i=0;i<num;i++) stream << '#'; 
}


ProgressBar::ProgressBar():
    itsLoc(BEG),itsStepSize(0.),itsLength(DefaultLength),itsNumVisible(0),itsSize(0),itsPercentage(0.)
{
    /// @details
    /// The default constructor defines a bar with 20 hashes 
    /// (given by ProgressBar::length), sets the number visible to be 0 
    ///  and the location to be at the beginning.
}

ProgressBar::ProgressBar(int newlength):
    itsLoc(BEG),itsStepSize(0.),itsLength(newlength),itsNumVisible(0),itsSize(0),itsPercentage(0.)
{
    /// @details
    /// This alternative constructor enables the user to define how many
    /// hashes should appear. Again, the number visible is set to 0 and
    /// the location to be at the beginning.  
    /// 
    /// \param newlength The new number of hashes to appear in the bar.
}

ProgressBar::~ProgressBar(){}

void ProgressBar::printPercentage()
{
    /// @details
    /// Function to print out the percentage, getting the formatting correct

    std::cout << std::setw(PercentWidth) << this->itsPercentage;
    std::cout << "%";

}


void ProgressBar::init(unsigned int size)
{ 
    /// @details
    /// This initialises the bar to deal with a loop of a certain size.
    /// This size will imply a certain step size, dependent on the number
    /// of hashes that will be written.  A blank bar is written out as
    /// well, and we remain at the end.  
    /// 
    /// \param size The maximum number of iterations to be covered by the
    /// progress bar.
    this->itsSize=size;
    if(size < this->itsLength){
	this->itsLength = size;
    }
    if(this->itsLength > 1) {
	this->itsStepSize = float(size) / float(this->itsLength);
	std::cout << "|"; 
	this->space(this->itsLength); 
	std::cout << "|";
	this->printPercentage();
	std::cout << std::flush;
	this->itsLoc = END;
    }
}

void ProgressBar::update(unsigned int num)
{
    /// @details
    /// This makes sure the correct number of hashes are drawn.
    /// 
    /// Based on the number provided, as well as the stepsize, we compare
    /// the number of hashes we expect to see with the number that are
    /// there, and if they differ, the correct number are drawn. Again,
    /// we remain at the end.  
    /// 
    /// \param num The loop counter to be translated into the progress
    /// bar.

    int newPercentage = 100 * num / this->itsSize;

    if( this->itsLength > 1 ){
	int numNeeded = 0;
	for(unsigned int i=0;i<this->itsLength;i++)
	    if(num>(i*this->itsStepSize)) numNeeded++;
    
	if(numNeeded > this->itsNumVisible){
	    this->itsPercentage = newPercentage;
	    this->itsNumVisible = numNeeded;
	    if(this->itsLoc==END) this->backSpace(this->itsLength); 
	    std::cout << "|"; 
	    this->hash(this->itsNumVisible);
	    this->space(this->itsLength-this->itsNumVisible);
	    std::cout << "|";
	    this->printPercentage();
	    std::cout << std::flush;
	    this->itsLoc=END;
	}
	else if(newPercentage > this->itsPercentage){
	    this->itsPercentage = newPercentage;
	    if(this->itsLoc==END) printBackSpace(std::cout,PercentWidth+1);
	    this->printPercentage();
	    std::cout << std::flush;
	    this->itsLoc=END;
	}

    }
}

void ProgressBar::rewind()
{
    /// @details
    /// If we are at the end, we print out enough backspaces to wipe out
    /// the entire bar.  If we are not, the erasing does not need to be
    /// done.
    if(this->itsLoc==END) this->backSpace(this->itsLength); 
    std::cout << std::flush;
}

void ProgressBar::remove()
{
    /// @details
    /// We first rewind() to the beginning, overwrite the bar with blank spaces, 
    /// and then rewind(). We end up at the beginning.
    rewind();
    this->space(this->itsLength+ExtraSpaces);
    this->itsLoc=END; 
    rewind(); 
    std::cout << std::flush;
}

void ProgressBar::fillSpace(std::string someString)
{
    /// @details
    /// We first remove() the bar and then write out the requested string.
    /// \param someString The string to be written over the bar area.
    remove();
    std::cout << someString;
    this->itsLoc=END;
}

