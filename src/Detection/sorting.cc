#include <vector>
#include <algorithm>
#include <Detection/detection.hh>

using std::vector;

class Pair
{
public:
  Pair(){};
  virtual ~Pair(){};
  friend bool operator< (const Pair& lhs, const Pair& rhs){
    return (lhs.primary < rhs.primary);
  };
  void define(float p, float m){primary=p; matching=m;};
  float get1(){return primary;};
  float get2(){return matching;};
private:
  float primary;
  float matching;
};

void Detection::SortByZ()
{
  /**
   * Detection::SortByZ():
   *   A Function that takes a Detection and
   *   sorts the pixels by z-pixel
   *   Upon return, the inputList is sorted.
   *   We use stable_sort, so that the order of objects with the same
   *    z-value is preserved.
   */

  long size = this->pix.size();
  Pair *info = new Pair[size];

  for(int i=0;i<size;i++) info[i].define(this->pix[i].getZ(),float(i));

  std::stable_sort(info,info+size);
  
  vector <Voxel> sorted(size);
  for(int i=0;i<size;i++) sorted[i] = this->pix[int(info[i].get2())] ;

  delete [] info;

  for(int i=0;i<size;i++){
    this->pix.erase(this->pix.begin()+i);
    this->pix.insert(this->pix.begin()+i, sorted[i]);
  }

  sorted.clear();
  
}


void SortByZ(vector <Detection> &inputList)
{
  /**
   * SortByZ(vector <Detection> &):
   *   A Function that takes a list of Detections and
   *   sorts them in order of increasing z-pixel value.
   *   Upon return, the inputList is sorted.
   */

  long size = inputList.size();
  Pair *info = new Pair[size];
  
  for(int i=0;i<size;i++) info[i].define(inputList[i].getZcentre(), float(i));

  std::stable_sort(info,info+size);
  
  vector <Detection> sorted;
  for(int i=0;i<size;i++) sorted.push_back( inputList[int(info[i].get2())] );

  delete [] info;

  inputList.clear();
  for(int i=0;i<size;i++) inputList.push_back( sorted[i] );
  sorted.clear();
  
}

void SortByVel(vector <Detection> &inputList)
{
  /**
   * SortByVel(vector <Detection> &):
   *   A Function that takes a list of Detections and
   *   sorts them in order of increasing velocity.
   *   Every member of the vector needs to have WCS defined, (and if so,
   *     then vel is assumed to be defined for all), otherwise no sorting
   *     is done.
   *   Upon return (if all WCS are good), the inputList is sorted.
   */

  bool isGood = true;
  for(int i=0;i<inputList.size();i++) isGood = isGood && inputList[i].isWCS();

  if(isGood){

    long size = inputList.size();
    Pair *info = new Pair[size];
    
    for(int i=0;i<size;i++) info[i].define(inputList[i].getVel(), float(i));

    std::stable_sort(info, info+size);
  
    vector <Detection> sorted;
    for(int i=0;i<size;i++) sorted.push_back( inputList[int(info[i].get2())] );

    delete [] info;

    inputList.clear();
    for(int i=0;i<size;i++) inputList.push_back( sorted[i] );
    sorted.clear();
  
  }

}  
