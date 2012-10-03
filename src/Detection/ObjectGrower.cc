
#include <iostream>
#include <vector>
#include <duchamp/duchamp.hh>
#include <duchamp/Detection/ObjectGrower.hh>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Cubes/cubes.hh>
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

    this->itsArrayDim = std::vector<size_t>(3);
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


  void ObjectGrower::updateDetectMap(short *map)
  {

    int numNondegDim=0;
    for(int i=0;i<3;i++) if(this->itsArrayDim[i]>1) numNondegDim++;

    if(numNondegDim>1){
      size_t spatsize=this->itsArrayDim[0]*this->itsArrayDim[1];
      for(size_t xy=0;xy<spatsize;xy++){
	short ct=0;
	for(size_t z=0;z<this->itsArrayDim[2];z++){
	  if(this->itsFlagArray[xy+z*spatsize] == DETECTED) ct++;
	}
	map[xy]=ct;
      }
    }
    else{
      for(size_t z=0;z<this->itsArrayDim[2];z++){
	map[z] = (this->itsFlagArray[z] == DETECTED) ? 1 : 0;
      }
    }

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

    size_t spatsize=this->itsArrayDim[0]*this->itsArrayDim[1];
    long zero = 0;
    std::vector<Voxel> voxlist = theObject->getPixelSet();
    size_t origSize = voxlist.size();
    long xpt,ypt,zpt;
    long xmin,xmax,ymin,ymax,zmin,zmax,x,y,z;
    size_t pos;
    for(size_t i=0; i<voxlist.size(); i++){
      
      // std::vector<Voxel> newvox = this->growFromPixel(voxlist[i]);
      // for(size_t v=0;v<newvox.size();v++) voxlist.push_back(newvox[v]);

      xpt=voxlist[i].getX();
      ypt=voxlist[i].getY();
      zpt=voxlist[i].getZ();
      
      xmin = size_t(std::max(xpt - this->itsSpatialThresh, zero));
      xmax = size_t(std::min(xpt + this->itsSpatialThresh, long(this->itsArrayDim[0])-1));
      ymin = size_t(std::max(ypt - this->itsSpatialThresh, zero));
      ymax = size_t(std::min(ypt + this->itsSpatialThresh, long(this->itsArrayDim[1])-1));
      zmin = size_t(std::max(zpt - this->itsVelocityThresh, zero));
      zmax = size_t(std::min(zpt + this->itsVelocityThresh, long(this->itsArrayDim[2])-1));
      
      //loop over surrounding pixels.
      for(x=xmin; x<=xmax; x++){
      	for(y=ymin; y<=ymax; y++){
      	  for(z=zmin; z<=zmax; z++){

      	    pos=x+y*this->itsArrayDim[0]+z*spatsize;
      	    if( ((x!=xpt) || (y!=ypt) || (z!=zpt))
      		&& this->itsFlagArray[pos]==AVAILABLE ) {

      	      if(this->itsGrowthStats.isDetection(this->itsFluxArray[pos])){
      		this->itsFlagArray[pos]=DETECTED;
      		voxlist.push_back(Voxel(x,y,z));
      	      }
      	    }

      	  } //end of z loop
      	} // end of y loop
      } // end of x loop

    } // end of i loop (voxels)

    // Add in new pixels to the Detection
    for(size_t i=origSize; i<voxlist.size(); i++){
      theObject->addPixel(voxlist[i]);
    }
   

  }


  std::vector<Voxel> ObjectGrower::growFromPixel(Voxel &vox)
  {

    std::vector<Voxel> newVoxels;
    // std::cerr << vox << "\n";
    long xpt=vox.getX();
    long ypt=vox.getY();
    long zpt=vox.getZ();
    size_t spatsize=this->itsArrayDim[0]*this->itsArrayDim[1];
    long zero = 0;
    // std::cerr << "--> " << xpt << " " << ypt << " " << zpt << "\n";

    int xmin = std::max(xpt - this->itsSpatialThresh, zero);
    int xmax = std::min(xpt + this->itsSpatialThresh, long(this->itsArrayDim[0])-1);
    int ymin = std::max(ypt - this->itsSpatialThresh, zero);
    int ymax = std::min(ypt + this->itsSpatialThresh, long(this->itsArrayDim[1])-1);
    int zmin = std::max(zpt - this->itsVelocityThresh, zero);
    int zmax = std::min(zpt + this->itsVelocityThresh, long(this->itsArrayDim[2])-1);
      
    // std::cerr << xmin << " " << xmax << "  " << ymin << " " << ymax << "  " << zmin << " " << zmax << "\n";
    //loop over surrounding pixels.
    size_t pos;
    Voxel nvox;
    std::vector<Voxel> morevox;
    for(int x=xmin; x<=xmax; x++){
      for(int y=ymin; y<=ymax; y++){
	for(int z=zmin; z<=zmax; z++){

	  pos=x+y*this->itsArrayDim[0]+z*spatsize;
	  if( ((x!=xpt) || (y!=ypt) || (z!=zpt))
	      && this->itsFlagArray[pos]==AVAILABLE ) {

	    if(this->itsGrowthStats.isDetection(this->itsFluxArray[pos])){
	      this->itsFlagArray[pos]=DETECTED;
	      nvox.setXYZF(x,y,z,this->itsFluxArray[pos]);
	      newVoxels.push_back(nvox);
	      // std::cerr << x << " " << y << " " << z << " " << this->itsFluxArray[pos] << "  =  " << nvox << "\n";
	      // morevox = this->growFromPixel(nvox);
	      // if(morevox.size()>0)
	      // 	for(size_t v=0;v<morevox.size();v++) newVoxels.push_back(morevox[v]);
	      
	    }
	  }

	} //end of z loop
      } // end of y loop
    } // end of x loop

    std::vector<Voxel>::iterator v,v2;
    for(v=newVoxels.begin();v<newVoxels.end();v++) {
      std::vector<Voxel> morevox = this->growFromPixel(*v);
      for(v2=morevox.begin();v2<morevox.end();v2++) 
	newVoxels.push_back(*v2);
    }

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
