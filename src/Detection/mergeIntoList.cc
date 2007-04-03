#include <vector>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>

void mergeIntoList(Detection &object, std::vector <Detection> &objList, Param &par)
{
  /**
   * A function to add a detection to a list of detections, checking
   * first to see if it can be combined with existing members of the
   * list.
   *
   * The areClose testing and combining is now done with the
   * parameters as given by the Param set, not just assuming adjacency
   * (as previously done).
   *
   * \param object The Detection to be merged into the list.
   * \param objList The vector list of Detections.
   * \param par The Param set, used for testing if merging needs to be done.
   */

  bool haveMerged = false;
//   bool flagAdjacent = par.getFlagAdjacent();
//   par.setFlagAdjacent(true);
//   float threshold = par.getThreshV();
//   par.setThreshV(1.5);

  std::vector<Detection>::iterator iter;
  for(int ctr=0; (!(haveMerged) && (ctr<objList.size())); ctr++){
    
    if(areClose(object, objList[ctr], par)){
      Detection newobj = objList[ctr] + object;
      iter = objList.begin() + ctr;
      objList.erase( iter );
      objList.push_back( newobj );
      haveMerged = true;
    }
    
  }
  
  if(!haveMerged) objList.push_back(object);

//   par.setFlagAdjacent(flagAdjacent);
//   par.setThreshV(threshold);

}
