#include <iostream>
#include <PixelMap/Voxel.hh>
#include <PixelMap/Scan.hh>
#include <PixelMap/Object2D.hh>
#include <PixelMap/Object3D.hh>
#include <vector>

namespace PixelInfo
{

  ChanMap::ChanMap(const ChanMap& m){
    this->itsZ=m.itsZ; 
    this->itsObject=m.itsObject;
  }
  //--------------------------------------------

  ChanMap& ChanMap::operator= (const ChanMap& m)
  {
    if(this == &m) return *this;
    this->itsZ=m.itsZ; 
    this->itsObject=m.itsObject;
    return *this;
  }
  //--------------------------------------------
  //============================================
  //--------------------------------------------

  Object3D::Object3D(const Object3D& o)
  {
    this->maplist = o.maplist;
    this->numVox  = o.numVox;
    this->xSum    = o.xSum;
    this->ySum    = o.ySum;
    this->zSum    = o.zSum;
    this->xmin    = o.xmin;
    this->ymin    = o.ymin;
    this->zmin    = o.zmin;
    this->xmax    = o.xmax;
    this->ymax    = o.ymax;
    this->zmax    = o.zmax;
  }
  //--------------------------------------------

  Object3D& Object3D::operator= (const Object3D& o)
  {
    if(this == &o) return *this;
    this->maplist = o.maplist;
    this->numVox  = o.numVox;
    this->xSum    = o.xSum;
    this->ySum    = o.ySum;
    this->zSum    = o.zSum;
    this->xmin    = o.xmin;
    this->ymin    = o.ymin;
    this->zmin    = o.zmin;
    this->xmax    = o.xmax;
    this->ymax    = o.ymax;
    this->zmax    = o.zmax;
    return *this;
  }
  //--------------------------------------------
  
  bool Object3D::isInObject(long x, long y, long z)
  {
    bool returnval = false;
    int mapCount = 0;
    while(!returnval && mapCount < this->maplist.size()){
      if(z == this->maplist[mapCount].itsZ){
	returnval = returnval || 
	  this->maplist[mapCount].itsObject.isInObject(x,y);
      }
      mapCount++;
    }
    return returnval;
  }
  //--------------------------------------------

  void Object3D::addPixel(long x, long y, long z)
  {
    // first test to see if we have a chanmap of same z
    bool haveZ = false;
    int mapCount = 0;
    while(!haveZ && mapCount < this->maplist.size()){
      haveZ = haveZ || (z==this->maplist[mapCount++].itsZ);
    }

    if(!haveZ){ // need to add a new ChanMap
      ChanMap newMap(z);
      newMap.itsObject.addPixel(x,y);
      this->maplist.push_back(newMap);
      this->order();
      // update the centres, min & max, as well as the number of voxels
      if(this->numVox==0){
	this->xSum = this->xmin = this->xmax = x;
	this->ySum = this->ymin = this->ymax = y;
	this->zSum = this->zmin = this->zmax = z;
      }
      else{
	this->xSum += x;
	this->ySum += y;
	this->zSum += z;
	if(x<this->xmin) this->xmin = x;
	if(x>this->xmax) this->xmax = x;
	if(y<this->ymin) this->ymin = y;
	if(y>this->ymax) this->ymax = y;
	// since we've ordered the maplist, the min & max z fall out
	// naturally
	this->zmin = this->maplist[0].itsZ;
	this->zmax = this->maplist[this->maplist.size()-1].itsZ;
      }
      this->numVox++;   
    }
    else{ 
      // there is a ChanMap of matching z. Find it..
      mapCount=0;
      while(this->maplist[mapCount].itsZ!=z) mapCount++;

      // Remove that channel's information from the Object's information
      long oldChanSize = this->maplist[mapCount].itsObject.numPix;
      this->xSum -= this->maplist[mapCount].itsObject.xSum;
      this->ySum -= this->maplist[mapCount].itsObject.ySum;
      this->zSum -= z*oldChanSize;

      // Add the channel
      this->maplist[mapCount].itsObject.addPixel(x,y);
    
      // and update the information...
      // This method deals with the case of a new pixel being added AND
      // with the new pixel already existing in the Object2D
      long newChanSize = this->maplist[mapCount].itsObject.numPix;
    
      this->numVox += (newChanSize - oldChanSize);
      this->xSum += this->maplist[mapCount].itsObject.xSum;
      this->ySum += this->maplist[mapCount].itsObject.ySum;
      this->zSum += z*newChanSize;
      if(x<this->xmin) this->xmin = x;
      if(x>this->xmax) this->xmax = x;
      if(y<this->ymin) this->ymin = y;
      if(y>this->ymax) this->ymax = y;
      // don't need to do anything to zmin/zmax -- the z-value is
      // already in the list
    }

  }
  //--------------------------------------------

  void Object3D::addChannel(ChanMap channel)
  {
    // first test to see if we have a chanmap of same z
    bool haveZ = false;
    int mapCount = 0;
    while( !haveZ && (mapCount < this->maplist.size()) ){
      haveZ = haveZ || (channel.itsZ==this->maplist[mapCount].itsZ);
      mapCount++;
    }

    if(!haveZ){ // need to add a new ChanMap
      this->maplist.push_back(channel);
      this->order();
      // update the centres, min & max, as well as the number of voxels
      if(this->numVox==0){ // ie. if it is the only channel map
	this->xSum = channel.itsObject.xSum;
	this->xmin = channel.itsObject.xmin;
	this->xmax = channel.itsObject.xmax;
	this->ySum = channel.itsObject.ySum;
	this->ymin = channel.itsObject.ymin;
	this->ymax = channel.itsObject.ymax;
	this->zSum = channel.itsObject.numPix*channel.itsZ;
	this->zmin = this->zmax = channel.itsZ;
      }
      else{
	this->xSum += channel.itsObject.xSum;
	this->ySum += channel.itsObject.ySum;
	this->zSum += channel.itsZ*channel.itsObject.numPix;
	if(channel.itsObject.xmin<this->xmin) 
	  this->xmin = channel.itsObject.xmin;
	if(channel.itsObject.xmax>this->xmax) 
	  this->xmax = channel.itsObject.xmax;
	if(channel.itsObject.ymin<this->ymin) 
	  this->ymin = channel.itsObject.ymin;
	if(channel.itsObject.ymax>this->ymax) 
	  this->ymax = channel.itsObject.ymax;
	// since we've ordered the maplist, the min & max z fall out
	// naturally
	this->zmin = this->maplist[0].itsZ;
	this->zmax = this->maplist[this->maplist.size()-1].itsZ;
      }
      this->numVox += channel.itsObject.numPix;
    }
    else{ 
      // there is a ChanMap of matching z. Find it and add the channel
      // map to the correct existing channel map
      mapCount=0;  
      while(this->maplist[mapCount].itsZ!=channel.itsZ) mapCount++;

      // Remove the new channel's information from the Object's information
      long oldChanSize = this->maplist[mapCount].itsObject.numPix;
      this->xSum -= this->maplist[mapCount].itsObject.xSum;
      this->ySum -= this->maplist[mapCount].itsObject.ySum;
      this->zSum -= this->maplist[mapCount].itsZ*oldChanSize;

      // Delete the old map and add the new one on the end. Order the list.
      ChanMap newMap = this->maplist[mapCount] + channel;
      std::vector <ChanMap>::iterator iter;
      iter = this->maplist.begin() + mapCount;
      this->maplist.erase(iter);
      this->maplist.push_back(newMap);
      this->order();    

      // and update the information...
      // This method deals correctly with all cases of adding an object.
      long newChanSize = newMap.itsObject.numPix;
      this->numVox += (newChanSize - oldChanSize);
      this->xSum += newMap.itsObject.xSum;
      this->ySum += newMap.itsObject.ySum;
      this->zSum += newMap.itsZ*newChanSize;
      if(channel.itsObject.xmin<this->xmin) this->xmin = channel.itsObject.xmin;
      if(channel.itsObject.xmax>this->xmax) this->xmax = channel.itsObject.xmax;
      if(channel.itsObject.ymin<this->ymin) this->ymin = channel.itsObject.ymin;
      if(channel.itsObject.ymax>this->ymax) this->ymax = channel.itsObject.ymax;
      // don't need to do anything to zmin/zmax -- the z-value is
      // already in the list

    }

  }
  //--------------------------------------------

  // long Object3D::getSize()
  // {
  //   long size=0;
  //   for(int i=0;i<this->maplist.size();i++) 
  //     size+=this->maplist[i].itsObject.getSize();
  //   return size;
  // }
  //--------------------------------------------

  long Object3D::getNumDistinctZ()
  {
    return this->maplist.size();
  }
  //--------------------------------------------

  long Object3D::getSpatialSize()
  {
    Object2D spatialMap;
    for(int i=0;i<this->maplist.size();i++){
      for(int s=0;s<this->maplist[i].itsObject.getNumScan();s++){
	spatialMap.addScan(this->maplist[i].itsObject.getScan(s));
      }
    }
    return spatialMap.getSize();
  }
  //--------------------------------------------

  Object2D Object3D::getSpatialMap()
  {
    Object2D spatialMap = this->maplist[0].itsObject;
    for(int i=1;i<this->maplist.size();i++){
      spatialMap = spatialMap + this->maplist[i].itsObject;
    }
    return spatialMap;
  }
  //--------------------------------------------
  
  void Object3D::calcParams()
  {
    this->xSum = 0;
    this->ySum = 0;
    this->zSum = 0;
    int count = 0;
    for(int m=0;m<this->maplist.size();m++){

      this->maplist[m].itsObject.calcParams();

      if(m==0){
	this->xmin = this->maplist[m].itsObject.getXmin();
	this->xmax = this->maplist[m].itsObject.getXmax();
	this->ymin = this->maplist[m].itsObject.getYmin();
	this->ymax = this->maplist[m].itsObject.getYmax();
	this->zmin = this->zmax = this->maplist[m].itsZ;
      }
      else{
	if(this->xmin>this->maplist[m].itsObject.getXmin()) 
	  this->xmin = this->maplist[m].itsObject.getXmin();
	if(this->xmax<this->maplist[m].itsObject.getXmax()) 
	  this->xmax = this->maplist[m].itsObject.getXmax();
	if(this->ymin>this->maplist[m].itsObject.getYmin())
	  this->ymin = this->maplist[m].itsObject.getYmin();
	if(this->ymax<this->maplist[m].itsObject.getYmax())
	  this->ymax = this->maplist[m].itsObject.getYmax();
	if(this->zmin>this->maplist[m].itsZ) this->zmin = this->maplist[m].itsZ;
	if(this->zmax<this->maplist[m].itsZ) this->zmax = this->maplist[m].itsZ;
      }

      long size = this->maplist[m].itsObject.getSize();
      this->xSum += this->maplist[m].itsObject.xSum;
      this->ySum += this->maplist[m].itsObject.ySum;
      this->zSum += this->maplist[m].itsZ * size;
      count += size;

    }

  }
  //------------------------------------------------------

  std::ostream& operator<< ( std::ostream& theStream, Object3D& obj)
  {
    for(int m=0;m<obj.maplist.size();m++){
      Object2D tempObject = obj.maplist[m].getObject();
      for(int s=0;s<tempObject.getNumScan();s++){
	Scan tempscan = tempObject.getScan(s);
	//      theStream << obj.maplist[m].getZ() << " " << tempscan << "\n";
	for(int x=tempscan.getX();x<=tempscan.getXmax();x++){
	  theStream << x                     << " " 
		    << tempscan.getY()       << " " 
		    << obj.maplist[m].getZ() << "\n";
	}
      }
    }  
    theStream << "\n";
    return theStream;
  }
  //--------------------------------------------

  Voxel Object3D::getPixel(int pixNum)
  {
    Voxel returnVox(-1,-1,-1,0.);
    if((pixNum<0)||(pixNum>=this->getSize())) return returnVox;
    int count1=0;
    for(int m=0;m<this->maplist.size();m++)
      {
	if(pixNum-count1<this->maplist[m].itsObject.getSize())
	  {
	    returnVox.setZ(this->maplist[m].getZ());
	    int count2=0;
	    for(int s=0;s<this->maplist[m].getNumScan();s++)
	      {
		if(pixNum-count1-count2<this->maplist[m].getScan(s).getXlen())
		  {
		    returnVox.setY(this->maplist[m].getScan(s).getY());
		    returnVox.setX(this->maplist[m].getScan(s).getX()
				   + pixNum - count1 - count2);
		    return returnVox;
		  }
		count2+=this->maplist[m].getScan(s).getXlen();
	      }
	  }
	count1+=this->maplist[m].itsObject.getSize();      
      }
    return returnVox;
  }
  //--------------------------------------------------------------------

  std::vector<Voxel> Object3D::getPixelSet()
  {
    //   long size = this->getSize();
    std::vector<Voxel> voxList(this->numVox);
    long count = 0;
    for(int m=0;m<this->maplist.size();m++){
      Object2D obj = this->maplist[m].itsObject;
      long z = this->maplist[m].itsZ;
      for(int s=0;s<obj.getNumScan();s++){
	Scan scn = obj.getScan(s);
	long y = scn.getY();
	for(long x=scn.getX(); x<=scn.getXmax(); x++){
	  voxList[count].setXYZF(x,y,z,0);
	  count++;
	}
      }
    }
    if(count != this->numVox) {
      std::cerr << "****** " << count << " -- " << this->numVox <<"\n";
      std::cerr << *this << "\n";
    }
    return voxList;

  }

}
