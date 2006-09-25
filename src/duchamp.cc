#include <iostream>
#include <iomanip>
#include <string>
#include <duchamp.hh>

void duchampWarning(std::string subroutine, std::string warning)
{
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
  std::string preamble = "\aERROR <" + subroutine + "> : ";
  std::cerr << preamble;
  for(int i=0;i<error.size();i++){
    std::cerr << error[i];
    if((i!=error.size()-1)&&(error[i]=='\n'))
      std::cerr << std::setw(preamble.size()-1) <<": ";
  }
}
