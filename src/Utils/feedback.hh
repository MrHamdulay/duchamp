#ifndef FEEDBACK_HH
#define FEEDBACK_HH

#include <iostream>
#include <string>

// A simple functions to print a given number of backspaces or spaces
//  to std::cout
inline void printBackSpace(int num){for(int i=0;i<num;i++) std::cout << '\b';};
inline void printSpace(int num){ for(int i=0;i<num;i++) std::cout << ' '; };
inline void printHash(int num){ for(int i=0;i<num;i++) std::cout << '#'; };

/**
 *  Controls printing out a progress bar.
 *
 *   A class that prints out a progress bar in the form \f$|\#\#\#\ \ \ \ \ \ \ \ \ \ |\f$
 *    that shows how far through a function or loop you are.
 *   The length of it defaults to 20 hashes, but can be set when declaring the object.
 *   There are five functions: 
 *    <ul><li>init(int)   --> prints an empty bar, and defines the increment
 *        <li>update(int) --> prints the correct number of #s, but only when
 *                            num is a multiple of the increment.
 *        <li>rewind()    --> prints backspaces to cover the entire bar.
 *        <li>remove()    --> does a rewind(), then prints spaces to overwrite
 *                            the bar area -- more clean.
 *        <li>fillSpace(std::string) --> does a remove(), then writes the string into
 *                                       the same space.
 * </ul>
 */
class ProgressBar
{
public:
  ProgressBar();                          ///< Default Constructor
  ProgressBar(int newlength);             ///< Alternative constructor
  virtual ~ProgressBar();                 ///< Destructor.
  enum POS {BEG=0,END};                   ///< So that we can record where we are.

  void init(int size);                    ///< Prints empty bar, defines increment.
  void update(int num);                   ///< Prints correct number of hashes
  void rewind();                          ///< Prints backspaces over bar.
  void remove();                          ///< Overwrites bar with blanks
  void fillSpace(std::string someString); ///< Overwrites bar with a string.

private:
  POS loc;                                ///< Are we at the start or end?
  float stepSize;                         ///< What is the interval between hashes?
  int length;                             ///< What's the maximum number of hashes?
  int numVisible;                         ///< How many hashes are there currently visible?
};



#endif // FEEDBACK_HH
