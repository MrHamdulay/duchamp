#ifndef SECTION_H
#define SECTION_H

#include <string>
#include <vector>

/**
 * A class to store information on subsections of a cube. This keeps
 * the subsection itself as a string, plus the parsed information in
 * the format of vectors of starting positions and lengths of each
 * dimension.
 */

class Section
{
public:
  Section(){numSections=0;};
  Section(std::string &s){subsection=s;};
  Section(const Section& s);
  Section& operator= (const Section& s);
  virtual ~Section(){};

  /** Convert the subsection string into the lists of numerical values. */
  int parse(std::vector<long> dimAxes);

  /** Test whether a given voxel (x,y,z) lies within the subsection */
  bool isInside(int x, int y, int z){
    return (  ( (x>=starts[0])&&(x<starts[0]+dims[0])  ) &&
	      ( (y>=starts[1])&&(y<starts[1]+dims[1])  ) &&
	      ( (z>=starts[2])&&(z<starts[2]+dims[2])  )   );
  }

  /** Save the subsection string */
  void setSection(std::string s){subsection=s;};
  /** Return the subsection string */
  std::string getSection(){return subsection;};

  /** Return a particular starting value */
  int getStart(int i){return starts[i];};
  /** Return a particular dimension length */
  int getDim(int i){return dims[i];};
  /** Return a particular ending value */
  int getEnd(int i){return starts[i]+dims[i]-1;};

  /** Return the full list of start values */
  std::vector<int> getStartList(){return starts;};
  /** Return the full list of dimension lengths */
  std::vector<int> getDimList(){return dims;};

private:
  std::string subsection;   ///< The string indicating the subsection,
			    ///   of the format [a:b,c:d,e:f] etc.
  int numSections;          ///< The number of sections in the string.
  std::vector<int> starts;  ///< The list of starting values (a,c,e)
  std::vector<int> dims;    ///< The list of lengths of each dimension
			    ///   (b-a+1,d-c+1,f-e+1)
};


#endif //SECTION_H
