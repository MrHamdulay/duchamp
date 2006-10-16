#include <vector>
#include <Detection/detection.hh>
#include <Utils/utils.hh>

using std::vector;

void Detection::SortByZ()
{
  /**
   * Detection::SortByZ():
   *   A Function that takes a Detection and
   *   sorts the pixels by z-pixel
   *   Upon return, the inputList is sorted.
   */

  long size = this->pix.size();
  float *positions = new float[size];
  float *z = new float[size];
  
  for(int i=0;i<size;i++){
    positions[i] = float(i);
    z[i] = this->pix[i].getZ();
  }

  sort(z, positions, 0, size);
  
  vector <Voxel> sorted(size);
  for(int i=0;i<size;i++) sorted[i] = this->pix[ int(positions[i]) ] ;

  delete [] positions;
  delete [] z;

  for(int i=0;i<size;i++){
    this->pix.erase(this->pix.begin()+i);
    this->pix.insert(this->pix.begin()+i, sorted[i]);
  }

  sorted.clear();
  
}

/**
 * SortByZ(vector <Detection> &):
 *   A Function that takes a list of Detections and
 *   sorts them in order of increasing z-pixel value.
 *   Upon return, the inputList is sorted.
 */

void SortByZ(vector <Detection> &inputList)
{

  long size = inputList.size();
  float *positions = new float[size];
  float *z = new float[size];
  
  for(int i=0;i<size;i++){
    positions[i] = float(i);
    Detection *obj = new Detection;
    *obj = inputList[i];
    z[i] = obj->getZcentre();
    delete obj;
  }

  sort(z, positions, 0, size);
  
  vector <Detection> sorted;
  for(int i=0;i<size;i++) sorted.push_back( inputList[ int(positions[i]) ] );

  delete [] positions;
  delete [] z;

  inputList.clear();
  for(int i=0;i<size;i++) inputList.push_back( sorted[i] );
  sorted.clear();
  
}

/**
 * SortByVel(vector <Detection> &):
 *   A Function that takes a list of Detections and
 *   sorts them in order of increasing velocity.
 *   Every member of the vector needs to have WCS defined, (and if so,
 *     then vel is assumed to be defined for all), otherwise no sorting
 *     is done.
 *   Upon return (if all WCS are good), the inputList is sorted.
 */

void SortByVel(vector <Detection> &inputList)
{

  bool isGood = true;
  for(int i=0;i<inputList.size();i++) isGood = isGood && inputList[i].isWCS();

  if(isGood){

    long size = inputList.size();
    float *positions = new float[size];
    float *vel = new float[size];
  
    for(int i=0;i<size;i++){
      positions[i] = float(i);
      Detection *obj = new Detection;
      *obj = inputList[i];
      vel[i] = obj->getVel();
      delete obj;
    }

    sort(vel, positions, 0, size);
  
    vector <Detection> sorted;
    for(int i=0;i<size;i++) sorted.push_back( inputList[ int(positions[i]) ] );

    delete [] positions;
    delete [] vel;

    inputList.clear();
    for(int i=0;i<size;i++) inputList.push_back( sorted[i] );
    sorted.clear();
  
  }

}  
