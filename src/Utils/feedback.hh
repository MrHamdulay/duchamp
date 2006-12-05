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
   *   There are three functions: 
   *        init(size) --> prints an empty bar, and defines the increment
   *        update(num) --> prints the correct number of #s, but only when
   *                        num is a multiple of the increment.
   *        rewind(num) --> prints backspaces to cover the entire bar.
   */
public:
  ProgressBar(){length=20; loc=BEG;};
  ProgressBar(int newlength){length=newlength; loc=BEG;};
  enum POS {BEG=0,END};

  void init(int size){  
    std::cout << "|"; printSpace(length); 
    std::cout << "|" << std::flush;
    fraction = size/length;
    loc = END;
  };
  void update(int num){
    if(num%fraction==0){
      printBackSpace(length+2); std::cout << "|"; 
      printHash(num/fraction); printSpace(length-num/fraction); 
      std::cout << "|" << std::flush;
    }
    loc=END;
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
    rewind();
    std::cout << someString;
    printSpace(length+2-someString.size());
    loc=END;
  }
private:
  POS loc;
  int fraction;
  int length;
};

#endif // FEEDBACK_HH
