#include <iostream>
#include <iomanip>
#include <string>
#include <duchamp.hh>

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
