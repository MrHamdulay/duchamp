// -----------------------------------------------------------------------
// Object2D.cc: Member functions for the Object2D class.
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
#include <duchamp/PixelMap/Object2D.hh>
#include <duchamp/PixelMap/Scan.hh>
#include <duchamp/PixelMap/Voxel.hh>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>

namespace PixelInfo
{

  Object2D::Object2D()
  {
    this->numPix = 0;
  }

  Object2D::Object2D(const Object2D& o)
  {
    operator=(o);
  }  

  Object2D& Object2D::operator= (const Object2D& o)
  {
    if(this == &o) return *this;
    this->scanlist = o.scanlist;
    this->numPix   = o.numPix;
    this->xSum     = o.xSum;
    this->ySum     = o.ySum;
    this->xmin     = o.xmin;
    this->ymin     = o.ymin;
    this->xmax     = o.xmax;
    this->ymax     = o.ymax;
    return *this;
  }  
  //------------------------------------------------------

  Object2D operator+ (Object2D lhs, Object2D rhs)
  {
    Object2D output = lhs;
    for(size_t s=0;s<rhs.scanlist.size();s++) output.addScan(rhs.scanlist[s]);
    return output;
  }

  //------------------------------------------------------
  void Object2D::addPixel(long x, long y)
  {
    ///  This function has three parts to it:
    ///  <ul><li> First, it searches through the existing scans to see if 
    ///            a) there is a scan of the same y-value present, and 
    ///            b) whether the (x,y) pixel lies in or next to an existing 
    ///            Scan. If so, it is added and the Scan is grown if need be.
    ///            If this isn't the case, a new Scan of length 1 is added to 
    ///            the list. 
    ///      <li> If the Scan list was altered, all are checked to see whether 
    ///            there is now a case of Scans touching. If so, they are 
    ///            combined and added to the end of the list.
    ///      <li> If the pixel was added, the parameters are updated and the 
    ///           pixel counter incremented.
    ///  </ul>


    bool flagDone=false,flagChanged=false, isNew=false;

    size_t scanCount = 0;
    while(!flagDone && (scanCount<this->scanlist.size()) ){
      Scan scan = this->scanlist[scanCount];    
      if(y == scan.itsY){ // if the y value is already in the list
	if(scan.isInScan(x,y)) flagDone = true; // pixel already here.
	else{ // it's a new pixel!
	  if((x==(scan.itsX-1)) || (x == (scan.getXmax()+1)) ){
	    // if it is adjacent to the existing range, add to that range
	    if(x==(scan.itsX-1)) this->scanlist[scanCount].growLeft();
	    else                 this->scanlist[scanCount].growRight();
	    flagDone = true;
	    flagChanged = true;
	    isNew = true;
	  }
	}     
      }
      scanCount++;
    }

    if(!flagDone){
      Scan newOne(y,x,1);
      this->scanlist.push_back(newOne);
      if(this->scanlist.size()>1) this->order();
      isNew = true;
    }
  
    if(flagChanged){ 
      // this is true only if one of the pre-existing scans has changed
      //
      // need to clean up, to see if there is a case of two scans when
      // there should be one.
      //
      // because there has been only one pixel added, only 2 AT MOST
      // scans will need to be combined, so once we meet a pair that
      // overlap, we can finish.

      std::vector <Scan>::iterator iter;
      bool combined = false;
      int size = this->scanlist.size();
      int count1=0;
      while(!combined && count1<size){
	int count2 = count1 + 1;
	while(!combined && count2<size){
	  Scan first = this->scanlist[count1];
	  Scan second = this->scanlist[count2];
	  if(y==first.itsY && y==second.itsY){
	    combined = touching(first, second);
	    if(combined){
	      Scan newOne = unite(first,second);;
	      iter = this->scanlist.begin() + count2;
	      this->scanlist.erase(iter);
	      iter = this->scanlist.begin() + count1;
	      this->scanlist.erase(iter);
	      this->scanlist.push_back(newOne);
	    }
	  }	
	  count2++;
	}
	count1++;
      }

      this->order(); // if we've added something, re-order everything

    }

    if(isNew){  
      // update the centres, mins, maxs and increment the pixel counter
      if(this->numPix==0){
	this->xSum = this->xmin = this->xmax = x;
	this->ySum = this->ymin = this->ymax = y;
      }
      else{
	this->xSum += x;
	this->ySum += y;
	if(x<this->xmin) this->xmin = x;
	if(x>this->xmax) this->xmax = x;
	if(y<this->ymin) this->ymin = y;
	if(y>this->ymax) this->ymax = y;
      }
      this->numPix++;
    }

  }
  //------------------------------------------------------

  void Object2D::addScan(Scan scan)
  {

    for(int x=scan.getX();x<=scan.getXmax();x++) this->addPixel(x,scan.getY());
    this->order();

  }
//   void Object2D::addScan(Scan scan)
//   {

//     bool flagDone=false,flagChanged=false;;
//     long y = scan.itsY;
//     unsigned int scanCount = 0;
//     std::vector <Scan>::iterator iter;

//     while(!flagDone && (scanCount<this->scanlist.size()) ){
//       Scan existing = this->scanlist[scanCount];    
//       if(y==existing.itsY){ //ie. if the y value has a scan present
// 	if(scan == existing) flagDone = true; // the scan is already there
// 	else if(touching(scan,existing)){ // if it overlaps or is next to an existing scan
// 	  // 	Scan joined = unite(scan,existing);
// 	  // 	iter = this->scanlist.begin() + scanCount;
// 	  // 	this->scanlist.erase(iter);
// 	  // 	this->scanlist.push_back(joined);
// 	  // 	flagDone = true;
// 	  flagChanged = true;
// 	}
//       }
//       scanCount++;
//     }

//     // if it is unconnected with any existing scan, add it to the end of
//     // the list
//     if(!flagDone) this->scanlist.push_back(scan);

//     // if the scan has been added, we need to change the centres, mins,
//     // maxs etc. First add all the pixels from the scan. We then remove
//     // any doubled up ones later.
//     if( (!flagDone) || flagChanged ){
//       if(this->numPix==0){
// 	this->ySum = y*scan.itsXLen;
// 	this->ymin = this->ymax = y;
// 	this->xmin = scan.itsX;
// 	this->xmax = scan.getXmax();
// 	this->xSum = scan.itsX;
// 	for(int x=scan.itsX+1;x<=scan.getXmax();x++) this->xSum += x;
// 	this->numPix = scan.itsXLen;
//       }
//       else{
// 	this->ySum += y*scan.itsXLen;
// 	for(int x=scan.itsX; x<=scan.getXmax(); x++) this->xSum += x;
// 	if(y<this->ymin) this->ymin = y;
// 	if(y>this->ymax) this->ymax = y;
// 	if(scan.itsX<this->xmin)      this->xmin = scan.itsX;
// 	if(scan.getXmax()>this->xmax) this->xmax = scan.getXmax();
// 	this->numPix += scan.itsXLen;
//       }
//     }

//     if(flagChanged){ 
//       // this is true only if one of the pre-existing scans has changed
//       // 
//       // In this case, we are adding a scan, and the possibility exists
//       // that more than one other scan could be affected. We therefore
//       // need to look over all scans, not just stopping at the first
//       // match we come across (as for addPixel() above).
//       unsigned int count1=0;
//       while(count1<this->scanlist.size()-1){
// 	unsigned int count2 = count1 + 1;
// 	do {
// 	  Scan first = this->scanlist[count1];
// 	  Scan second = this->scanlist[count2];
// 	  if(y==first.itsY && y==second.itsY){ 
// 	    // only look at the y-value where there would have been a change.
// 	    if(touching(first,second)){
// 	      Scan newOne = unite(first,second);
// 	      iter = this->scanlist.begin() + count2;
// 	      this->scanlist.erase(iter);
// 	      iter = this->scanlist.begin() + count1;
// 	      this->scanlist.erase(iter);
// 	      this->scanlist.push_back(newOne);
	    
// 	      count2 = count1 + 1;

// 	      // Need to remove the doubled-up pixels from the info
// 	      Scan overlap = intersect(first,second);
// 	      this->ySum -= overlap.itsY*overlap.itsXLen;
// 	      for(int x=overlap.itsX; x<=overlap.getXmax(); x++)
// 		this->xSum -= x;
// 	      this->numPix -= overlap.itsXLen;

// 	    }
// 	    else count2++;
// 	  }	
// 	  else count2++;
// 	} while(count2 < this->scanlist.size());

// 	count1++;
//       }
//     }

//     this->order();

//   }
  //------------------------------------------------------

  bool Object2D::isInObject(long x, long y)
  {

    size_t scanCount = 0;
    do{

      if(y == this->scanlist[scanCount].itsY){
	// if the y value is already in the list

 	if((x>=this->scanlist[scanCount].itsX)&&
	   (x<=this->scanlist[scanCount].getXmax())){
	  // if the x value is already in the range, the pixel is
	  //  already stored, so return true
	  return true;
	}

      }

      scanCount++;

    }while( scanCount<this->scanlist.size() );

    // if we get to here, we've looked at each pixel with no match.
    return false; 

  }
  //------------------------------------------------------

  // long Object2D::getSize()
  // {
  //   long size=0;
  //   for(int i=0;i<this->scanlist.size();i++){
  //     size += this->scanlist[i].getXlen();
  //   }
  //   return size;
  // }
  //------------------------------------------------------

  std::ostream& operator<< ( std::ostream& theStream, Object2D& obj)
  {
    for(size_t i=0;i<obj.scanlist.size();i++)
      theStream << obj.scanlist[i] << "\n";
    theStream<<"---\n";
    return theStream;

  }
  //------------------------------------------------------

  void Object2D::calcParams()
  {
    this->xSum = 0;
    this->ySum = 0;
    std::vector<Scan>::iterator s;
    for(s=this->scanlist.begin();s<this->scanlist.end();s++){

      if(s==this->scanlist.begin()){
	this->ymin = this->ymax = s->itsY;
	this->xmin = s->itsX;
	this->xmax = s->getXmax();
      }
      else{
	if(this->ymin>s->itsY)    this->ymin = s->itsY;
	if(this->xmin>s->itsX)    this->xmin = s->itsX;
	if(this->ymax<s->itsY)    this->ymax = s->itsY;
	if(this->xmax<s->getXmax()) this->xmax = s->getXmax();
      }

      this->ySum += s->itsY*s->getXlen();
      for(int x=s->itsX;x<=s->getXmax();x++)
	this->xSum += x;
//       this->xSum += (s->getXmax()*(s->getXmax()+1) -
// 		     s->itsX*(s->itsX-1) ) / 2;

    }

  }
  //------------------------------------------------------

  void Object2D::cleanup()
  {
    size_t counter=0, compCounter=1;
    std::vector<Scan>::iterator iter;
    while(counter < this->scanlist.size())
      {
	compCounter = counter + 1;
      
	while( compCounter < this->scanlist.size() ) {

	  Scan scan1 = this->scanlist[counter];
	  Scan scan2 = this->scanlist[compCounter];
	  if(overlap(scan1,scan2)){
	    Scan newscan = unite(scan1,scan2);
	    iter = this->scanlist.begin()+compCounter;
	    this->scanlist.erase(iter);
	    iter = this->scanlist.begin()+counter;
	    this->scanlist.erase(iter);
	    this->scanlist.push_back(newscan);
	    compCounter = counter + 1;
	  }
	  else compCounter++;

	} 

	counter++;
      }

    //   this->order();

  }
  //------------------------------------------------------

  long Object2D::getNumDistinctY()
  {
    std::vector<long> ylist;
    if(this->scanlist.size()==0) return 0;
    if(this->scanlist.size()==1) return 1;
    ylist.push_back(this->scanlist[0].itsY);
    std::vector<Scan>::iterator scn;
    for(scn=this->scanlist.begin();scn<this->scanlist.end();scn++){
      bool inList = false;
      unsigned int j=0;
      long y = scn->itsY;
      while(!inList && j<ylist.size()){
	if(y==ylist[j]) inList=true;
	j++;
      };
      if(!inList) ylist.push_back(y);
    }
    return ylist.size();
  }
  //------------------------------------------------------

  long Object2D::getNumDistinctX()
  {
    std::vector<long> xlist;
    if(this->scanlist.size()==0) return 0;
    if(this->scanlist.size()==1) return 1;
    for(int x=this->scanlist[0].itsX;x<this->scanlist[0].getXmax();x++)
      xlist.push_back(x);
    std::vector<Scan>::iterator scn;
    for(scn=this->scanlist.begin();scn<this->scanlist.end();scn++){
      //    for(unsigned int i=1;i<this->scanlist.size();i++){
      //      for(int x=this->scanlist[0].itsX;x<this->scanlist[0].getXmax();x++){
      for(int x=scn->itsX;x<scn->getXmax();x++){
	bool inList = false;
	unsigned int j=0;
	while(!inList && j<xlist.size()){
	  if(x==xlist[j]) inList=true;
	  j++;
	};
	if(!inList) xlist.push_back(x);
      }
    }
    return xlist.size();
  }
  //------------------------------------------------------

  bool Object2D::scanOverlaps(Scan scan)
  {
    bool returnval = false;
    unsigned int scanCount=0;
    while(!returnval && scanCount<this->scanlist.size()){
      returnval = returnval || overlap(scan,this->scanlist[scanCount++]);
    }
    return returnval;
  }

  //------------------------------------------------------

  void Object2D::addOffsets(long xoff, long yoff)
  {
    std::vector<Scan>::iterator scn;
    for(scn=this->scanlist.begin();scn<this->scanlist.end();scn++)
      scn->addOffsets(xoff,yoff);
    this->xSum += xoff*numPix;
    this->xmin += xoff; xmax += xoff;
    this->ySum += yoff*numPix;
    this->ymin += yoff; ymax += yoff;
  }

  //------------------------------------------------------

  double Object2D::getPositionAngle()
  {

    int sumxx=0;
    int sumyy=0;
    int sumxy=0;
    std::vector<Scan>::iterator scn=this->scanlist.begin();
    for(;scn<this->scanlist.end();scn++){
      sumyy += (scn->itsY*scn->itsY)*scn->itsXLen;
      for(int x=scn->itsX;x<=scn->getXmax();x++){
	sumxx += x*x;
	sumxy += x*scn->itsY;
      }
    }

    // Calculate net moments
    double mxx = sumxx - this->xSum*this->xSum / double(this->numPix);
    double myy = sumyy - this->ySum*this->ySum / double(this->numPix);
    double mxy = sumxy - this->xSum*this->ySum / double(this->numPix);

    if(mxy==0){
      if(mxx>myy) return M_PI/2.;
      else return 0.;
    }

    // Angle of the minimum moment
    double tantheta = (mxx - myy + sqrt( (mxx-myy)*(mxx-myy) + 4.*mxy*mxy))/(2.*mxy);

    return atan(tantheta);
  }

  //------------------------------------------------------

  std::pair<double,double> Object2D::getPrincipleAxes()
  {

    double theta = this->getPositionAngle();
    double x0 = this->getXaverage();
    double y0 = this->getYaverage();
    std::vector<double> majorAxes, minorAxes;
    std::vector<Scan>::iterator scn=this->scanlist.begin();
    for(;scn<this->scanlist.end();scn++){
      for(int x=scn->itsX;x<=scn->getXmax();x++){
	majorAxes.push_back((x-x0+0.5)*cos(theta) + (scn->itsY-y0+0.5)*sin(theta));
	minorAxes.push_back((x-x0+0.5)*sin(theta) + (scn->itsY-y0+0.5)*cos(theta));
      }
    }

    std::sort(majorAxes.begin(),majorAxes.end());
    std::sort(minorAxes.begin(),minorAxes.end());
    int size = majorAxes.size();
    std::pair<double,double> axes;
    axes.first = fabs(majorAxes[0]-majorAxes[size-1]);
    axes.second = fabs(minorAxes[0]-minorAxes[size-1]);
    if(axes.first<0.5) axes.first=0.5;
    if(axes.second<0.5) axes.second=0.5;
    return axes;

  }

}
