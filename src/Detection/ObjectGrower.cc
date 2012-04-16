#include <iostream>
#include <duchamp/duchamp.hh>
#include <Detection/ObjectGrower.hh>
#include <Detection/detection.hh>
#include <Cubes/cubes.hh>
#include <duchamp/Utils/Statistics.hh>
#include <duchamp/PixelMap/Voxel.hh>


namespace duchamp {

  ObjectGrower::ObjectGrower() {
  }

  ObjectGrower::ObjectGrower(ObjectGrower &o)
  {
    this->operator=(o);
  }

  ObjectGrower& ObjectGrower::operator=(const ObjectGrower &o)
  {
    if(this == &o) return *this;
    this->itsFlagArray = o.itsFlagArray;
    this->itsArrayDim = o.itsArrayDim; 
    this->itsGrowthStats = o.itsGrowthStats;
    this->itsSpatialThresh = o.itsSpatialThresh;
    this->itsVelocityThresh = o.itsVelocityThresh;
    this->itsFluxArray = o.itsFluxArray;
    return *this;
  }

  void ObjectGrower::define( Cube *theCube )
  {
    /// @details This copies all necessary information from the Cube
    /// and its parameters & statistics. It also defines the array of
    /// pixel flags, which involves looking at each object to assign
    /// them as detected, all blank & "milky-way" pixels to assign
    /// them appropriately, and all others to "available". It is only
    /// the latter that will be considered in the growing function.
    /// @param theCube A pointer to a duchamp::Cube 

    this->itsGrowthStats = Statistics::StatsContainer<float>(theCube->stats());	
    if(theCube->pars().getFlagUserGrowthThreshold())
      this->itsGrowthStats.setThreshold(theCube->pars().getGrowthThreshold());
    else
      this->itsGrowthStats.setThresholdSNR(theCube->pars().getGrowthCut());    
    this->itsGrowthStats.setUseFDR(false);

    if(theCube->isRecon()) this->itsFluxArray = theCube->getRecon();
    else this->itsFluxArray = theCube->getArray();

    this->itsArrayDim = std::vector<long>(3);
    this->itsArrayDim[0]=theCube->getDimX();
    this->itsArrayDim[1]=theCube->getDimY();
    this->itsArrayDim[2]=theCube->getDimZ();
    size_t spatsize=this->itsArrayDim[0]*this->itsArrayDim[1];
    size_t fullsize=spatsize*this->itsArrayDim[2];

    if(theCube->pars().getFlagAdjacent())
      this->itsSpatialThresh = 1;
    else
      this->itsSpatialThresh = int(theCube->pars().getThreshS());
    this->itsVelocityThresh = int(theCube->pars().getThreshV());

    this->itsFlagArray = std::vector<STATE>(fullsize,AVAILABLE);

    for(size_t o=0;o<theCube->getNumObj();o++){
      std::vector<Voxel> voxlist = theCube->getObject(o).getPixelSet();
      for(size_t i=0; i<voxlist.size(); i++){
	size_t pos = voxlist[i].getX() + voxlist[i].getY()*this->itsArrayDim[0] + voxlist[i].getZ()*spatsize;
	this->itsFlagArray[pos] = DETECTED;
      }
    }

    if(theCube->pars().getFlagMW()){
      int minz=std::max(0,theCube->pars().getMinMW());
      int maxz=std::min(int(theCube->getDimZ())-1,theCube->pars().getMaxMW());
      if(minz<maxz){
	for(size_t i=minz*spatsize;i<(maxz+1)*spatsize;i++) this->itsFlagArray[i]=MW;
      }
    }

    for(size_t i=0;i<fullsize;i++)
      if(theCube->isBlank(i)) this->itsFlagArray[i]=BLANK;

  }

  void ObjectGrower::grow(Detection *theObject)
  {
    /// @details This function grows the provided object out to the
    /// secondary threshold provided in itsGrowthStats. For each pixel
    /// in an object, all surrounding pixels are considered and, if
    /// their flag is AVAILABLE, their flux is examined. If it's above
    /// the threshold, that pixel is added to the list to be looked at
    /// and their flag is changed to DETECTED. 
    /// @param theObject The duchamp::Detection object to be grown. It
    /// is returned with new pixels in place. Only the basic
    /// parameters that belong to PixelInfo::Object3D are
    /// recalculated.

    // long spatsize=this->itsArrayDim[0]*this->itsArrayDim[1];
    // long zero = 0;
    std::vector<Voxel> voxlist = theObject->getPixelSet();
    int origSize = voxlist.size();
    for(size_t i=0; i<voxlist.size(); i++){
      
      std::vector<Voxel> newvox = this->growFromPixel(voxlist[i]);
      for(size_t v=0;v<newvox.size();v++) voxlist.push_back(newvox[v]);

      // long xpt=voxlist[i].getX();
      // long ypt=voxlist[i].getY();
      // long zpt=voxlist[i].getZ();
      
      // int xmin = std::max(xpt - this->itsSpatialThresh, zero);
      // int xmax = std::min(xpt + this->itsSpatialThresh, this->itsArrayDim[0]-1);
      // int ymin = std::max(ypt - this->itsSpatialThresh, zero);
      // int ymax = std::min(ypt + this->itsSpatialThresh, this->itsArrayDim[1]-1);
      // int zmin = std::max(zpt - this->itsVelocityThresh, zero);
      // int zmax = std::min(zpt + this->itsVelocityThresh, this->itsArrayDim[2]-1);
      
      // //loop over surrounding pixels.
      // for(int x=xmin; x<=xmax; x++){
      // 	for(int y=ymin; y<=ymax; y++){
      // 	  for(int z=zmin; z<=zmax; z++){

      // 	    int pos=x+y*this->itsArrayDim[0]+z*spatsize;
      // 	    if( ((x!=xpt) || (y!=ypt) || (z!=zpt))
      // 		&& this->itsFlagArray[pos]==AVAILABLE ) {

      // 	      if(this->itsGrowthStats.isDetection(this->itsFluxArray[pos])){
      // 		this->itsFlagArray[pos]=DETECTED;
      // 		voxlist.push_back(Voxel(x,y,z));
      // 	      }
      // 	    }

      // 	  } //end of z loop
      // 	} // end of y loop
      // } // end of x loop

    } // end of i loop (voxels)

    // Add in new pixels to the Detection
    for(size_t i=origSize; i<voxlist.size(); i++){
      theObject->addPixel(voxlist[i]);
    }
   

  }


  std::vector<Voxel> ObjectGrower::growFromPixel(Voxel vox)
  {
    std::vector<Voxel> newVoxels;

    long xpt=vox.getX();
    long ypt=vox.getY();
    long zpt=vox.getZ();
    long spatsize=this->itsArrayDim[0]*this->itsArrayDim[1];
    long zero = 0;
      
    int xmin = std::max(xpt - this->itsSpatialThresh, zero);
    int xmax = std::min(xpt + this->itsSpatialThresh, this->itsArrayDim[0]-1);
    int ymin = std::max(ypt - this->itsSpatialThresh, zero);
    int ymax = std::min(ypt + this->itsSpatialThresh, this->itsArrayDim[1]-1);
    int zmin = std::max(zpt - this->itsVelocityThresh, zero);
    int zmax = std::min(zpt + this->itsVelocityThresh, this->itsArrayDim[2]-1);
      
    //loop over surrounding pixels.
    for(int x=xmin; x<=xmax; x++){
      for(int y=ymin; y<=ymax; y++){
	for(int z=zmin; z<=zmax; z++){

	  int pos=x+y*this->itsArrayDim[0]+z*spatsize;
	  if( ((x!=xpt) || (y!=ypt) || (z!=zpt))
	      && this->itsFlagArray[pos]==AVAILABLE ) {

	    if(this->itsGrowthStats.isDetection(this->itsFluxArray[pos])){
	      this->itsFlagArray[pos]=DETECTED;
	      newVoxels.push_back(Voxel(x,y,z));
	      std::vector<Voxel> morevox = this->growFromPixel(Voxel(x,y,z));
	      for(size_t v=0;v<morevox.size();v++) newVoxels.push_back(morevox[v]);
	      
	    }
	  }

	} //end of z loop
      } // end of y loop
    } // end of x loop

    return newVoxels;

  }

  // void ObjectGrower::resetDetectionFlags()
  // {
  //   for(size_t i=0;i<itsFlagArray.size();i++)
  //     if(itsFlagArray[i]==DETECTED) itsFlagArray[i] = AVAILABLE;
  // }

  // void ObjectGrower::growInwardsFromEdge()
  // {
    


  // }

}
