// -----------------------------------------------------------------------
// Object3D.cc: Member functions for Object3D class.
// -----------------------------------------------------------------------
// Copyright (C) 2006, Matthew Whiting, ATNF
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// Duchamp is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with Duchamp; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
//
// Correspondence concerning Duchamp may be directed to:
//    Internet email: Matthew.Whiting [at] atnf.csiro.au
//    Postal address: Dr. Matthew Whiting
//                    Australia Telescope National Facility, CSIRO
//                    PO Box 76
//                    Epping NSW 1710
//                    AUSTRALIA
// -----------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <sstream>
#include <duchamp/PixelMap/Voxel.hh>
#include <duchamp/PixelMap/Line.hh>
#include <duchamp/PixelMap/Scan.hh>
#include <duchamp/PixelMap/Object2D.hh>
#include <duchamp/PixelMap/Object3D.hh>
#include <duchamp/Utils/Section.hh>
#include <vector>
#include <map>

namespace PixelInfo
{
  Object3D::Object3D()
  {
    this->numVox=0;
    this->xSum = 0;
    this->ySum = 0;
    this->zSum = 0;
    this->xmin = this->xmax = this->ymin = this->ymax = this->zmin = this->zmax = -1;
  }
  //--------------------------------------------

  Object3D::Object3D(const Object3D& o)
  {
    operator=(o);
  }
  //--------------------------------------------

  Object3D& Object3D::operator= (const Object3D& o)
  {
    if(this == &o) return *this;
    this->chanlist = o.chanlist;
    this->numVox  = o.numVox;
    this->spatialMap = o.spatialMap;
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

  Object3D operator+ (Object3D lhs, Object3D rhs)
  {
    Object3D output = lhs;
    for(std::map<long, Object2D>::iterator it = rhs.chanlist.begin(); it!=rhs.chanlist.end();it++)
      output.addChannel(it->first, it->second);
      //      output.addChannel(*it);
    return output;
  }

  //--------------------------------------------
  float Object3D::getXaverage()
  {
    if(numVox>0) return xSum/float(numVox); 
    else return 0.;
  }
  //--------------------------------------------

  float Object3D::getYaverage()
  {
    if(numVox>0) return ySum/float(numVox);
    else return 0.;
  }
  //--------------------------------------------

  float Object3D::getZaverage()
  {
    if(numVox>0) return zSum/float(numVox); 
    else return 0.;
  }
  //--------------------------------------------
  
  bool Object3D::isInObject(long x, long y, long z)
  {
    std::map<long,Object2D>::iterator it=this->chanlist.begin();
    while(it!=this->chanlist.end() && it->first!=z) it++;

    if(it==this->chanlist.end()) return false;
    else                         return it->second.isInObject(x,y);
  }
  //--------------------------------------------

  void Object3D::addPixel(long x, long y, long z)
  {
 
    std::map<long,Object2D>::iterator it=this->chanlist.begin();
    while(it!=this->chanlist.end() && it->first!=z) it++;

    if(it==this->chanlist.end()){ //new channel
      Object2D obj;
      obj.addPixel(x,y);
      chanlist.insert( std::pair<int,Object2D>(z,obj) );
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
	// since we've ordered the maplist, the min & max z fall out naturally
	this->zmin = this->chanlist.begin()->first;
	this->zmax = this->chanlist.rbegin()->first;
      }
      this->numVox++;   
    }
    else{ // existing channel
      // This method deals with the case of a new pixel being added AND
      // with the new pixel already existing in the Object2D
 
      // Remove that channel's information from the Object's information
      this->xSum -= it->second.xSum;
      this->ySum -= it->second.ySum;
      this->zSum -= z*it->second.numPix;
      this->numVox -= it->second.numPix;

      // Add the pixel
      it->second.addPixel(x,y);
    
      this->numVox += it->second.numPix;
      this->xSum += it->second.xSum;
      this->ySum += it->second.ySum;
      this->zSum += z*it->second.numPix;
      if(x<this->xmin) this->xmin = x;
      if(x>this->xmax) this->xmax = x;
      if(y<this->ymin) this->ymin = y;
      if(y>this->ymax) this->ymax = y;
      // don't need to do anything to zmin/zmax -- the z-value is
      // already in the list
    }

    this->spatialMap.addPixel(x,y);

  }
  //--------------------------------------------

  void Object3D::addScan(Scan s, long z)
  {
    long y=s.getY();
    for(int x=s.getX(); x<=s.getXmax(); x++) 
      this->addPixel(x,y,z);
  }

  //--------------------------------------------

  // void Object3D::addChannel(const std::pair<long, Object2D> *chan)
  // {
  //   long z = chan->first;
  //   Object2D
  // }


  void Object3D::addChannel(const long &z, Object2D &obj)
  {

    std::map<long,Object2D>::iterator it=this->chanlist.begin();
    while(it!=this->chanlist.end() && it->first!=z) it++;

    if(it == this->chanlist.end()){ // channel z is not already in object, so add it.
      this->chanlist.insert(std::pair<long,Object2D>(z,obj));
      if(this->numVox == 0){ // if there are no other pixels, so initialise mins,maxs,sums
	this->xmin = obj.xmin;
	this->xmax = obj.xmax;
	this->ymin = obj.ymin;
	this->ymax = obj.ymax;
	this->zmin = this->zmax = z;
	this->xSum = obj.xSum;
	this->ySum = obj.ySum;
	this->zSum = z * obj.getSize();
      }
      else{ // there are other pixels in other channels, so update mins, maxs, sums
	if(obj.xmin<this->xmin) this->xmin = obj.xmin;
	if(obj.xmax>this->xmax) this->xmax = obj.xmax;
	if(obj.ymin<this->ymin) this->ymin = obj.ymin;
	if(obj.ymax>this->ymax) this->ymax = obj.ymax;
	if(z<this->zmin) this->zmin = z;
	if(z>this->zmax) this->zmax = z;
	this->xSum += obj.xSum;
	this->ySum += obj.ySum;
	this->zSum += z * obj.getSize();
      }
      this->numVox += obj.getSize();
    }
    else{ // channel is already present, so need to combine objects.
      this->xSum -= it->second.xSum;
      this->ySum -= it->second.ySum;
      this->zSum -= z*it->second.getSize();
      this->numVox -= it->second.getSize();
      it->second = it->second + obj;
      this->xSum += it->second.xSum;
      this->ySum += it->second.ySum;
      this->zSum += z*it->second.getSize();
      this->numVox += it->second.getSize();
      if(obj.xmin<this->xmin) this->xmin = obj.xmin;
      if(obj.xmax>this->xmax) this->xmax = obj.xmax;
      if(obj.ymin<this->ymin) this->ymin = obj.ymin;
      if(obj.ymax>this->ymax) this->ymax = obj.ymax;
    }

    this->spatialMap = this->spatialMap + obj;

  }

  //--------------------------------------------
  
  void Object3D::calcParams()
  {
    this->xSum = 0;
    this->ySum = 0;
    this->zSum = 0;
    this->numVox = 0;

    this->zmin = this->chanlist.begin()->first;
    this->zmax = this->chanlist.rbegin()->first;
    for(std::map<long, Object2D>::iterator it = this->chanlist.begin(); 
	it!=this->chanlist.end();it++){

      it->second.calcParams();

      if(it==this->chanlist.begin()){
	this->xmin = it->second.getXmin();
	this->xmax = it->second.getXmax();
	this->ymin = it->second.getYmin();
	this->ymax = it->second.getYmax();
      }
      else{
	if(it->second.xmin<this->xmin) this->xmin = it->second.xmin;
	if(it->second.xmax>this->xmax) this->xmax = it->second.xmax;
	if(it->second.ymin<this->ymin) this->ymin = it->second.ymin;
	if(it->second.ymax>this->ymax) this->ymax = it->second.ymax;
      }

      this->xSum += it->second.xSum;
      this->ySum += it->second.ySum;
      this->zSum += it->first * it->second.getSize();
      this->numVox += it->second.getSize();

    }

  }
  //------------------------------------------------------

  void Object3D::write(std::string filename)
  {
    std::ofstream outfile(filename.c_str(), std::ios::out | std::ios::binary | std::ios::app);
    size_t size=this->chanlist.size();
    outfile.write(reinterpret_cast<const char*>(&size), sizeof size);
    for(std::map<long, Object2D>::iterator it = this->chanlist.begin(); 
	it!=this->chanlist.end();it++){
      
      outfile.write(reinterpret_cast<const char*>(&(it->first)), sizeof it->first);
      size=it->second.scanlist.size();
      outfile.write(reinterpret_cast<const char*>(&size), sizeof size);
      for(std::vector<Scan>::iterator s=it->second.scanlist.begin();s!=it->second.scanlist.end();s++){
	outfile.write(reinterpret_cast<const char*>(&(s->itsX)),sizeof s->itsX);
	outfile.write(reinterpret_cast<const char*>(&(s->itsXLen)),sizeof s->itsXLen);
	outfile.write(reinterpret_cast<const char*>(&(s->itsY)),sizeof s->itsY);
      }

    }
    outfile.close();
      
  }

  std::streampos Object3D::read(std::string filename, std::streampos loc)
  {
    std::ifstream infile(filename.c_str(), std::ios::in | std::ios::binary);
    infile.seekg(loc);
    size_t mapsize,scanlistsize;
    long z,x,xl,y;
    infile.read(reinterpret_cast<char*>(&mapsize), sizeof mapsize);
    for(size_t i=0;i<mapsize;i++){
      infile.read(reinterpret_cast<char*>(&z), sizeof z);
      Object2D obj;
      infile.read(reinterpret_cast<char*>(&scanlistsize), sizeof scanlistsize);
      for(size_t j=0;j<scanlistsize;j++){
	infile.read(reinterpret_cast<char*>(&x),sizeof x);
	infile.read(reinterpret_cast<char*>(&xl),sizeof xl);
	infile.read(reinterpret_cast<char*>(&y),sizeof y);
	Scan scn(y,x,xl);
	obj.addScan(scn);
      }
      this->addChannel(z,obj);
    }
    std::streampos newloc = infile.tellg();
    infile.close();
    return newloc;
  }


  void Object3D::print(std::ostream& theStream)
  {
    for(std::map<long, Object2D>::iterator it = this->chanlist.begin(); 
	it!=this->chanlist.end();it++){
      for(std::vector<Scan>::iterator s=it->second.scanlist.begin();s!=it->second.scanlist.end();s++){
	theStream << *s << "," << it->first << "\n";
      }
    }  
    theStream << "\n";
  }


  std::ostream& operator<< ( std::ostream& theStream, Object3D& obj)
  {
    obj.print(theStream);
    return theStream;
  }
  //--------------------------------------------

  std::vector<Voxel> Object3D::getPixelSet()
  {
    /// @details Returns a vector of the Voxels in the object. All
    /// flux values are set to 0.

    std::vector<Voxel> voxList(this->numVox);
    long count = 0;
    for(std::map<long, Object2D>::iterator it = this->chanlist.begin(); 
	it!=this->chanlist.end();it++){
      long z = it->first;
      for(std::vector<Scan>::iterator s=it->second.scanlist.begin();s!=it->second.scanlist.end();s++){
	long y = s->getY();
	for(long x=s->getX(); x<=s->getXmax(); x++){
	  voxList[count].setXYZF(x,y,z,0);
	  count++;
	}
      }
    }
    return voxList;

  }

  //--------------------------------------------------------------------

  std::vector<Voxel> Object3D::getPixelSet(float *array, size_t *dim)
  {
    /// @details Returns a vector of Voxels with the flux values for each voxel
    /// taken from the array provided. No check is made as to whether
    /// the pixels fall in the array boundaries
    /// @param array Array of pixel values
    /// @param dim Array of axis dimensions

    std::vector<Voxel> voxList(this->numVox);
    long count = 0;
    for(std::map<long, Object2D>::iterator it = this->chanlist.begin(); 
	it!=this->chanlist.end();it++){
      long z = it->first;
      for(std::vector<Scan>::iterator s=it->second.scanlist.begin();s!=it->second.scanlist.end();s++){
	long y = s->getY();
	for(long x=s->getX(); x<=s->getXmax(); x++){
	  voxList[count].setXYZF(x,y,z,array[x+dim[0]*y+dim[0]*dim[1]*z]);
	  count++;
	}
      }
    }
    return voxList;

  }

  //--------------------------------------------------------------------

  std::vector<long> Object3D::getChannelList()
  {

    std::vector<long> chanlist;
    for(std::map<long, Object2D>::iterator it = this->chanlist.begin();
	it != this->chanlist.end(); it++){
      chanlist.push_back(it->first);
    }
    return chanlist;
  }

  //--------------------------------------------------------------------

  Object2D Object3D::getChanMap(long z)
  {
    Object2D obj;
    std::map<long,Object2D>::iterator it=this->chanlist.begin();
    while(it!=this->chanlist.end() && it->first!=z) it++;

    if(it==this->chanlist.end()) obj = Object2D();
    else obj = it->second;

    return obj;
  }

  //--------------------------------------------------------------------

  unsigned int Object3D::getMaxAdjacentChannels()
  {
    /// @details Find the maximum number of contiguous channels in the
    /// object. Since there can be gaps in the channels included in an
    /// object, we run through the list of channels and keep track of
    /// sizes of contiguous segments. Then return the largest size.

    unsigned int maxnumchan=0;
    unsigned int zcurrent=0, zprevious,zcount=0;
    std::map<long, Object2D>::iterator it;
    for(it = this->chanlist.begin(); it!=this->chanlist.end();it++)
      {
	if(it==this->chanlist.begin()){
	  zcount++;
	  zcurrent = it->first;
	}
	else{
	  zprevious = zcurrent;
	  zcurrent = it->first;
	  if(zcurrent-zprevious>1){
	    maxnumchan = std::max(maxnumchan, zcount);
	    zcount=1;
	  }
	  else zcount++;
	}
      }
    maxnumchan = std::max(maxnumchan,zcount);
    return maxnumchan;
  }

  //--------------------------------------------------------------------

  void Object3D::addOffsets(long xoff, long yoff, long zoff)
  {
    std::map<long,Object2D> newmap;
    for(std::map<long, Object2D>::iterator it = this->chanlist.begin(); it!=this->chanlist.end();it++){
      std::pair<long,Object2D> newOne(it->first+zoff, it->second);
      newOne.second.addOffsets(xoff,yoff);
      newmap.insert(newOne);
    }
    this->chanlist.clear();
    this->chanlist = newmap;
    if(this->numVox>0){
      this->xSum += xoff*numVox;
      this->xmin += xoff; this->xmax += xoff;
      this->ySum += yoff*numVox;
      this->ymin += yoff; this->ymax += yoff;
      this->zSum += zoff*numVox;
      this->zmin += zoff; this->zmax += zoff;
    }
  }


  duchamp::Section Object3D::getBoundingSection(int boundary)
  {
    std::stringstream ss;
    ss << "[" << this->xmin-boundary <<":"<<this->xmax+boundary<<","<< this->ymin-boundary <<":"<<this->ymax+boundary<<","<< this->zmin-boundary <<":"<<this->zmax+boundary<<"]";
    std::string sec=ss.str();
    duchamp::Section section(sec);
    return section;
  }

  std::vector<std::vector<Voxel> > Object3D::getVertexSet()
  {
    ///  @details
    /// Returns a vector of vectors of Voxels. Each vector of Voxels is a join-the-dots outline of an island of pixels, and there is one vector for each separate island (since an Object3D may have more than one island of connected pixels).
    /// \return The vector of vectors of Voxels.

    int xmin = this->getXmin() - 1;
    int xmax = this->getXmax() + 1;
    int ymin = this->getYmin() - 1;
    int ymax = this->getYmax() + 1;
    int xsize = xmax - xmin + 1;
    int ysize = ymax - ymin + 1;

    std::vector<Voxel> voxlist = this->getPixelSet();
    std::vector<bool> isObj(xsize*ysize,false);
    std::vector<Voxel>::iterator vox;
    for(vox=voxlist.begin();vox<voxlist.end();vox++){
      size_t pos = (vox->getX()-xmin) + (vox->getY()-ymin)*xsize;
      isObj[pos] = true;
    }
    voxlist.clear();

    std::vector<Line> linelist;    
    for(int x=xmin; x<=xmax; x++){
      // for each column...
      for(int y=ymin+1;y<=ymax;y++){
	int current  = (y-ymin)*xsize + x-xmin;
	int previous = (y-ymin-1)*xsize + x-xmin;
	if((isObj[current]&&!isObj[previous])   ||
	   (!isObj[current]&&isObj[previous])){
	  linelist.push_back(Line(x,y,x+1,y));
	}
      }
    }
    for(int y=ymin; y<=ymax; y++){
      // now for each row...
      for(int x=xmin+1;x<=xmax;x++){
	int current  = (y-ymin)*xsize + x-xmin;
	int previous = (y-ymin)*xsize + x-xmin - 1;
	if((isObj[current]&&!isObj[previous])   ||
	   (!isObj[current]&&isObj[previous])){
	  linelist.push_back(Line(x,y,x,y+1));
	}
      }
    }

    std::vector<std::vector<Voxel> > vertSet;
    while(linelist.size()>0){
	std::vector<Voxel> orderedVertices;
	std::vector<Line>::iterator line=linelist.begin();
	orderedVertices.push_back(line->start());
	Voxel comp=line->end(),prev=line->start();
	orderedVertices.push_back(comp);
	linelist.erase(line);
	bool atEnd=false;
	do{
	    bool foundIt=false;
	    line=linelist.begin();
	    while(line!=linelist.end() && !foundIt){
		foundIt = line->has(comp);
		atEnd = line->has(orderedVertices[0]);
		if(comp == line->start()){
		    orderedVertices.push_back(line->end());
		    comp=line->end();
		}
		else if (comp==line->end()){
		    orderedVertices.push_back(line->start());
		    comp=line->start();
		}
		if(foundIt) linelist.erase(line);
		else line++;
	    }
	}  while(linelist.size()>0 && !atEnd);
	vertSet.push_back(orderedVertices);
    }

    return vertSet;
  
  }




}
