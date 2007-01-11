#include <iostream>
#include <string>
#include <Utils/feedback.hh>


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
};

ProgressBar::ProgressBar(int newlength){
  /**
   * This alternative constructor enables the user to define how many
   * hashes should appear. Again, the number visible is set to 0 and
   * the location to be at the beginning.  \param newlength The new
   * number of hashes to appear in the bar.
   */
  length=newlength; 
  loc=BEG; 
  numVisible = 0;
};

ProgressBar::~ProgressBar(){};

void ProgressBar::init(int size){ 
  /**
   * This initialises the bar to deal with a loop of a certain size.
   * This size will imply a certain step size, dependent on the number
   * of hashes that will be written.  A blank bar is written out as
   * well, and we remain at the end.  \param size The maximum number
   * of iterations to be covered by the progress bar.
   */
  stepSize = float(size) / float(length);
  std::cout << "|"; 
  printSpace(length); 
  std::cout << "|" << std::flush;
  loc = END;
};

void ProgressBar::update(int num){
  /**
   * This makes sure the correct number of hashes are drawn.
   *
   * Based on the number provided, as well as the stepsize, we compare
   * the number of hashes we expect to see with the number that are
   * there, and if they differ, the correct number are drawn. Again,
   * we remain at the end.  \param num The loop counter to be
   * translated into the progress bar.
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
};

void ProgressBar::rewind(){
  /**
   * If we are at the end, we print out enough backspaces to wipe out
   * the entire bar.  If we are not, the erasing does not need to be
   * done.
   */
  if(loc==END) printBackSpace(length+2); 
  loc=BEG;
  std::cout << std::flush;
};

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
};

void ProgressBar::fillSpace(std::string someString){
  /**
   * We first remove() the bar and then write out the requested string.
   * \param someString The string to be written over the bar area.
   */
  remove();
  std::cout << someString;
  loc=END;
}

