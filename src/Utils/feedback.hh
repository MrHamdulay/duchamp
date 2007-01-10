#ifndef FEEDBACK_HH
#define FEEDBACK_HH

#include <iostream>
#include <string>

// A simple functions to print a given number of backspaces or spaces
//  to std::cout
inline void printBackSpace(int num){for(int i=0;i<num;i++) std::cout << '\b';};
inline void printSpace(int num){ for(int i=0;i<num;i++) std::cout << ' '; };
inline void printHash(int num){ for(int i=0;i<num;i++) std::cout << '#'; };
class ProgressBar
{
  /**
   *  ProgressBar Class
   *   A class that prints out a progress bar in the form |####    |
   *    that shows how far through a function or loop you are.
   *   The length of it defaults to 20, but can be set when declaring the
   *    object.
   *   There are five functions: 
   *        init(size)  --> prints an empty bar, and defines the increment
   *        update(num) --> prints the correct number of #s, but only when
   *                         num is a multiple of the increment.
   *        rewind()    --> prints backspaces to cover the entire bar.
   *        remove()    --> does a rewind(), then prints spaces to overwrite
   *                         the bar area -- more clean.
   *        fillSpace(string) --> does a remove(), then writes the string into
   *                               the same space.
   */
public:
  ProgressBar(){length=20; loc=BEG; numVisible = 0;};
  ProgressBar(int newlength){length=newlength; loc=BEG; numVisible = 0;};
  virtual ~ProgressBar(){};
  enum POS {BEG=0,END};

  void init(int size){ 
    stepSize = float(size) / float(length);
    std::cout << "|"; 
    printSpace(length); 
    std::cout << "|" << std::flush;
    loc = END;
  };
  void update(int num){
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
  void rewind(){
    if(loc==END) printBackSpace(length+2); 
    loc=BEG;
    std::cout << std::flush;
  };
  void remove(){
    rewind();
    printSpace(length+2);
    loc=END; 
    rewind(); 
    std::cout << std::flush;
  };
  void fillSpace(std::string someString){
    remove();
    std::cout << someString;
    loc=END;
  }
private:
  POS loc;
  float stepSize;
  int length;
  int numVisible;
};

#endif // FEEDBACK_HH
