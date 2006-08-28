#include <vector>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>

void mergeIntoList(Detection &object, vector <Detection> &objList, Param &par)
{
  /**
   *  mergeIntoList(Detection &, vector<Detection> &, Param &)
   *
   *   A function to add a detection to a list of detections, checking 
   *   first to see if it can be merged with existing members of the list.
   *   The merging is only done if the object is adjacent to one of the existing
   *   members -- this is considered in all three directions.
   *        To this end, the adjacent flag in par is set true, and the velocity
   *        threshold is set to 1. These parameters are changed back before returning.
   */

  bool haveMerged = false;
  bool flagAdjacent = par.getFlagAdjacent();
  par.setFlagAdjacent(true);
  float threshold = par.getThreshV();
  par.setThreshV(1.);

  long ctr = 0;
  if(objList.size()>0){
    do {
      Detection *obj2 = new Detection;
      *obj2 = objList[ctr];
      if(areClose(object, *obj2, par)){
	obj2->addAnObject(object);
	objList.erase( objList.begin() + ctr );
	objList.push_back( *obj2 );
	haveMerged = true;
      }
      else ctr++;
      delete obj2;
      
    } while( !(haveMerged) && (ctr<objList.size()) );
  }

  if(!haveMerged) objList.push_back(object);

  par.setFlagAdjacent(flagAdjacent);
  par.setThreshV(threshold);

}
