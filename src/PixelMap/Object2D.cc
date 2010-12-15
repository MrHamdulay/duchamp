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
  void Object2D::addPixel(long &x, long &y)
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

    size_t scanCount = 0, size=this->scanlist.size();
    while(!flagDone && (scanCount<size) ){
      if(y == this->scanlist[scanCount].itsY){ // if the y value is already in the list
	if(this->scanlist[scanCount].isInScan(x,y)) flagDone = true; // pixel already here.
	else{ // it's a new pixel!
	  if((x==(this->scanlist[scanCount].itsX-1)) || (x == (this->scanlist[scanCount].getXmax()+1)) ){
	    // if it is adjacent to the existing range, add to that range
	    if(x==(this->scanlist[scanCount].itsX-1)) this->scanlist[scanCount].growLeft();
	    else                                      this->scanlist[scanCount].growRight();
	    flagDone = true;
	    flagChanged = true;
	    isNew = true;
	  }
	}     
      }
      scanCount++;
    }

    if(!flagDone){ 
      // we got to the end of the scanlist, so there is no pre-existing scan with this y value
      // add a new scan consisting of just this pixel
      Scan newOne(y,x,1);
      this->scanlist.push_back(newOne);
      if(this->scanlist.size()>1) this->order();
      isNew = true;
    }
    else if(flagChanged){ 
      // this is true only if one of the pre-existing scans has changed
      //
      // need to clean up, to see if there is a case of two scans when
      // there should be one. Only need to look at scans with matching
      // y-value
      //
      // because there has been only one pixel added, only 2 AT MOST
      // scans will need to be combined, so once we meet a pair that
      // overlap, we can finish.

      std::vector <Scan>::iterator iter;
      bool combined = false;
      int size = this->scanlist.size();
      int count1=0;
      while(!combined && count1<size){
	if(this->scanlist[count1].itsY==y){
	  int count2 = count1 + 1;
	  while(!combined && count2<size){
	    if(this->scanlist[count2].itsY==y){
	      combined = touching(this->scanlist[count1], this->scanlist[count2]);
	      if(combined){
		for(int i=0;i<this->scanlist[count2].getXlen();i++) this->scanlist[count1].growRight();
		iter = this->scanlist.begin() + count2;
		this->scanlist.erase(iter);
	      }
	    }	
	    count2++;
	  }
	}
	count1++;
      }

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
    long y=scan.getY();
    for(long x=scan.getX();x<=scan.getXmax();x++) this->addPixel(x,y);

  }
  //------------------------------------------------------

  bool Object2D::isInObject(long x, long y)
  {

    std::vector<Scan>::iterator scn;
    bool returnval=false;
    for(scn=this->scanlist.begin();scn<this->scanlist.end()&&!returnval;scn++){

      returnval = ((y == scn->itsY) && (x>= scn->itsX) && (x<=scn->getXmax()));

    }
       
    return returnval;

  }
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


  //------------------------------------------------------


  bool Object2D::canMerge(Object2D &other, float threshS, bool flagAdj)
  {
    long gap;
    if(flagAdj) gap = 1;
    else gap = long( ceil(threshS) );
    bool near = this->isNear(other,gap);
    if(near) return this->isClose(other,threshS,flagAdj);
    else return near;
  }

  //------------------------------------------------------

  bool Object2D::isNear(Object2D &other, long gap)
  {
    bool areNear;
    // Test X ranges
    if((this->xmin-gap)<other.xmin) areNear=((this->xmax+gap)>=other.xmin);
    else areNear=(other.xmax>=(this->xmin-gap));
    // Test Y ranges
    if(areNear){
      if((this->ymin-gap)<other.ymin) areNear=areNear&&((this->ymax+gap)>=other.ymin);
      else areNear=areNear&&(other.ymax>=(this->ymin-gap));
    }
    return areNear;
  }

  //------------------------------------------------------

  bool Object2D::isClose(Object2D &other, float threshS, bool flagAdj)
  {

    long gap = long(ceil(threshS));
    if(flagAdj) gap=1;

    bool close = false;

    long ycommonMin=std::max(this->ymin-gap,other.ymin)-gap;
    long ycommonMax=std::min(this->ymax+gap,other.ymax)+gap;

    std::vector<Scan>::iterator iter1,iter2;
    for(iter1=this->scanlist.begin();!close && iter1<this->scanlist.end();iter1++){
      if(iter1->itsY >= ycommonMin && iter1->itsY<=ycommonMax){
	for(iter2=other.scanlist.begin();!close && iter2<other.scanlist.end();iter2++){
	  if(iter2->itsY >= ycommonMin && iter2->itsY<=ycommonMax){
	      
	    if(abs(iter1->itsY-iter2->itsY)<=gap){
	      if(flagAdj) {
		if((iter1->itsX-gap)>iter2->itsX) close=((iter2->itsX+iter2->itsXLen-1)>=(iter1->itsX-gap));
		else close = ( (iter1->itsX+iter1->itsXLen+gap-1)>=iter2->itsX);
	      }
	      else close = (minSep(*iter1,*iter2) < threshS);
	    }

	  }
	}
      }
    }
	    
    return close;
    
  }



}
