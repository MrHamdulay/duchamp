#include <PixelMap/Scan.hh>
#include <iostream>

namespace PixelInfo
{

  Scan nullScan()
  {
    /** 
     * A simple way of returning a scan with zero length.
     */ 
    Scan null(-1,-1,0); 
    return null;
  }
  //------------------------------------------------------

  Scan unite(Scan &scan1, Scan &scan2)
  {
    /**
     * Return a scan that includes all pixels from both scans, but
     * only if they overlap. If they do not, return the null scan.
     *
     */

    // TEST FOR FAILURES:
    bool fail = false;
    fail = fail || (scan1.getY()!=scan2.getY());
    fail = fail || ( (scan1.getX() < scan2.getX()) && 
		     (scan2.getX() > scan1.getXmax()+1) );
    fail = fail || ( (scan2.getX() < scan1.getX()) && 
		     (scan1.getX() > scan2.getXmax()+1) );
    Scan joined;
    if(fail){
      //     std::cerr << "Joining scans failed! (" 
      // 	      << scan1 <<"),("<<scan2<<") don't overlap\n";
      joined = nullScan();
    }
    else{
      long y = scan1.getY();
      long x = scan1.getX();
      if(scan2.getX()<x) x=scan2.getX();
      long xmax=scan1.getXmax();
      if(scan2.getXmax()>xmax) xmax=scan2.getXmax();
      joined.define(y,x,xmax-x+1);
    }
    return joined;
  }

  //------------------------------------------------------

  Scan intersect(Scan &scan1, Scan &scan2)
  {
    /**
     * Return a scan that includes all pixels that lie in both scans.
     * 
     * If they do not overlap, return the null scan.
     *
     */

    bool fail = false;
    fail = fail || (scan1.getY()!=scan2.getY());
    fail = fail || ( (scan1.getX() < scan2.getX()) && 
		     (scan2.getX() > scan1.getXmax()+1) );
    fail = fail || ( (scan2.getX() < scan1.getX()) && 
		     (scan1.getX() > scan2.getXmax()+1) );

    Scan intersection;
    if(fail){
      //     std::cerr << "Intersecting scans failed! (" 
      // 	      << scan1 <<"),("<<scan2<<") don't overlap.\n";
      intersection = nullScan();
    }
    else{
      long y = scan1.getY();
      long x = scan1.getX();
      if(scan2.getX()>x) x=scan2.getX();
      long xmax=scan1.getXmax();
      if(scan2.getXmax()<xmax) xmax=scan2.getXmax();
      intersection.define(y,x,xmax-x+1);
    }
    return intersection;
  }
  //------------------------------------------------------

  bool touching(Scan &scan1, Scan &scan2)
  {
    /**
     *  Test whether two scans either overlap, or lie adjacent
     *  (ie. there are no pixels lying between the two scans).
     * \return A bool value.
     */
    if(scan1.getY()!=scan2.getY()) return false;
    else if(scan1.getX() <= scan2.getX())
      return (scan2.getX() <= scan1.getXmax()+1);
    else
      return (scan1.getX() <= scan2.getXmax()+1);
  
  }
  //------------------------------------------------------

  bool overlap(Scan &scan1, Scan &scan2)
  {
    /**
     *  Test whether two scans overlap, ie. they have pixels in
     *  common.
     * \return A bool value.
     */
    if(scan1.getY()!=scan2.getY()) return false;
    else if(scan1.getX() <= scan2.getX())
      return (scan2.getX() <= scan1.getXmax());
    else
      return (scan1.getX() <= scan2.getXmax());
  
  }
  //------------------------------------------------------

  bool adjacent(Scan &scan1, Scan &scan2)
  {
     /**
     *  Test whether two scans lie adjacent (ie. there are no pixels
     *  lying between the two scans).  If they overlap, return false.
     *  \return A bool value.
     */
    if(scan1.getY()!=scan2.getY()) return false;
    else if(scan1.getX() <= scan2.getX())
      return (scan2.getX() == scan1.getXmax()+1);
    else
      return (scan1.getX() == scan2.getXmax()+1);
  
  }
  //------------------------------------------------------

  std::ostream& operator<< ( std::ostream& theStream, Scan& scan)
  {
    /**
     *  Output the three key parameters of the scan.
     */
    theStream << scan.itsY;
    theStream << " " << scan.itsX;
    theStream << " " << scan.itsXLen;
  }
  //------------------------------------------------------

  bool operator< (Scan lhs, Scan rhs)
  {
    /**
     * Test for less-than first on the y-values, and if they are
     * equal, test on the starting x-value, and then finally on the
     * length.
     *
     * This is necessary for sorting functions on lists of Scans (used
     * by the Object2D class).
     */
    if(lhs.itsY != rhs.itsY)      return (lhs.itsY    < rhs.itsY);
    else if(lhs.itsX != rhs.itsX) return (lhs.itsX    < rhs.itsX);
    else                          return (lhs.itsXLen < rhs.itsXLen);
  }
  //------------------------------------------------------

  bool operator== (Scan lhs, Scan rhs)
  {
    /**
     * For two scans to be equal, all three parameters must be equal.
     */ 
    return (lhs.itsY == rhs.itsY) &&
      (lhs.itsX == rhs.itsX) &&
      (lhs.itsXLen == rhs.itsXLen);
  }
  //------------------------------------------------------

  bool Scan::isInScan(long x, long y)
  {
    return (y == this->itsY) &&
      ( (x>= this->itsX) && (x < (this->itsXLen+this->itsX)) );
  }
  //------------------------------------------------------

  void mergeList(std::vector<Scan> scanlist)
  {
    std::vector<Scan>::iterator iter;
    int counter=0,compCounter;
    while(counter<(scanlist.size()-1)){

      
      compCounter = counter+1;
      
      do{
	
	if(touching(scanlist[counter],scanlist[compCounter])){
	  Scan temp = unite(scanlist[counter],scanlist[compCounter]);
	  iter = scanlist.begin()+compCounter;
	  scanlist.erase(iter);
	  iter = scanlist.begin()+counter;
	  scanlist.erase(iter);
	  scanlist.push_back(temp);
	}
	else compCounter ++;
      }while(compCounter < scanlist.size());

      counter++;
    }

  }


}
