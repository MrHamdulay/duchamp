#include <iostream>
#include <sstream>

#include <math.h>
#include <duchamp.hh>
#include <fitsHeader.hh>
#include <vector> 
#include <string>
#include <Detection/detection.hh>
#include <Detection/columns.hh>

using namespace Column;

namespace Column
{

  Col::Col()
  {
    /** Set the default values for the parameters. */
    width=1; 
    precision=0; 
    name=" "; 
    units=" ";
  };

  Col::~Col(){}

  Col::Col(int num)
  {
    /**
     * A specialised constructor that defines one of the default 
     *  columns, as defined in the Column namespace
     * \param num The number of the column to be constructed. 
     *            Corresponds to the order of the columns in the const 
     *            arrays in the Column namespace.
     */ 
    if((num>=0)&&(num<numColumns)){
      this->width     = defaultWidth[num];
      this->precision = defaultPrec[num];
      this->name      = defaultName[num];
      this->units     = defaultUnits[num];
    }
    else{
      std::stringstream errmsg;
      errmsg << "Incorrect value for Col(num) --> num="<<num
	     << ", should be between 0 and " << numColumns-1 << ".\n";
      duchampError("Column constructor", errmsg.str());
      this->width = 1;
      this->precision = 0;
      this->name = " ";
      this->units = " ";
    }
  }
  //------------------------------------------------------------
  void Col::printTitle(std::ostream &stream)
  {
    stream << std::setw(this->width) 
	   << std::setfill(' ') 
	   << this->name;
  }

  void Col::printUnits(std::ostream &stream)
  {
    stream << std::setw(this->width) 
	   << std::setfill(' ') 
	   << this->units;
  }
  
  void Col::printDash (std::ostream &stream)
  {
    stream << std::setw(this->width) 
	   << std::setfill('-')
	   << "" 
	   << std::setfill(' ');
  }

  void Col::printBlank(std::ostream &stream)
  {
    stream << std::setw(this->width) 
	   << std::setfill(' ') 
	   << "";
  }
  
  template <class T> 
  void Col::printEntry(std::ostream &stream, T value)
  {
    /**
     *  \param stream Where the printing is done.
     *  \param value  The value to be printed.
     */
    stream << std::setprecision(this->precision)
	   << std::setw(this->width) 
	   << std::setfill(' ')
	   << value;
  }
  template void Col::printEntry<int>(std::ostream &stream, int value);
  template void Col::printEntry<long>(std::ostream &stream, long value);
  template void Col::printEntry<unsigned>(std::ostream &stream, unsigned value);
  template void Col::printEntry<float>(std::ostream &stream, float value);
  template void Col::printEntry<double>(std::ostream &stream, double value);
  template void Col::printEntry<std::string>(std::ostream &stream, std::string value);

}

std::vector<Col> getFullColSet(std::vector<Detection> &objectList, 
			       FitsHeader &head)
{
  /**
   *  A function that returns a std::vector of Col objects containing
   *  information on the columns necessary for output to the results
   *  file:
   *    Obj#,NAME,X,Y,Z,RA,DEC,VEL,w_RA,w_DEC,w_VEL,F_tot,F_int,F_peak,
   *                X1,X2,Y1,Y2,Z1,Z2,Npix,Flag
   *
   *   Each object in the provided objectList is checked to see if it
   *   requires any column to be widened, or for that column to have
   *   its precision increased.
   *
   *   Both Ftot and Fint are provided -- it is up to the calling
   *   function to determine which to use.
   *
   * \param objectList A std::vector list of Detection objects that the
   * columns need to fit.
   * \param head The FitsHeader object defining the World Coordinate
   * System.
   * \return A std::vector list of Col definitions.
   */

  std::vector<Col> newset;

  // set up the default columns 

  newset.push_back( Col(NUM) );
  newset.push_back( Col(NAME) );
  newset.push_back( Col(X) );
  newset.push_back( Col(Y) );
  newset.push_back( Col(Z) );
  newset.push_back( Col(RA) );
  newset.push_back( Col(DEC) );
  newset.push_back( Col(VEL) );
  newset.push_back( Col(WRA) );
  newset.push_back( Col(WDEC) );
  newset.push_back( Col(WVEL) );
  newset.push_back( Col(FINT) );
  newset.push_back( Col(FTOT) );
  newset.push_back( Col(FPEAK) );
  newset.push_back( Col(SNRPEAK) );
  newset.push_back( Col(X1) );
  newset.push_back( Col(X2) );
  newset.push_back( Col(Y1) );
  newset.push_back( Col(Y2) );
  newset.push_back( Col(Z1) );
  newset.push_back( Col(Z2) );
  newset.push_back( Col(NPIX) );
  newset.push_back( Col(FLAG) );
  newset.push_back( Col(XAV) );
  newset.push_back( Col(YAV) );
  newset.push_back( Col(ZAV) );
  newset.push_back( Col(XCENT) );
  newset.push_back( Col(YCENT) );
  newset.push_back( Col(ZCENT) );
  newset.push_back( Col(XPEAK) );
  newset.push_back( Col(YPEAK) );
  newset.push_back( Col(ZPEAK) );

  // Now test each object against each new column
  for( int obj = 0; obj < objectList.size(); obj++){
    std::string tempstr;
    int tempwidth;
    float val,minval;

    // Obj#
    tempwidth = int( log10(objectList[obj].getID()) + 1) + 
      newset[NUM].getPrecision() + 1;
    for(int i=newset[NUM].getWidth();i<tempwidth;i++) newset[NUM].widen();

    // Name
    tempwidth = objectList[obj].getName().size() + 1;
    for(int i=newset[NAME].getWidth();i<tempwidth;i++) newset[NAME].widen();

    // X position
    val = objectList[obj].getXcentre();
    tempwidth = int( log10(val) + 1) + newset[X].getPrecision() + 2;
    for(int i=newset[X].getWidth();i<tempwidth;i++) newset[X].widen();
    if((val<1.)&&(val>0.)){
      minval = pow(10, -1. * newset[X].getPrecision()+1); 
      if(val < minval) newset[X].upPrec();
    }
    // Y position
    val = objectList[obj].getYcentre();
    tempwidth = int( log10(val) + 1) + newset[Y].getPrecision() + 2;
    for(int i=newset[Y].getWidth();i<tempwidth;i++) newset[Y].widen();
    if((val<1.)&&(val>0.)){
      minval = pow(10, -1. * newset[Y].getPrecision()+1); 
      if(val < minval) newset[Y].upPrec();
    }
    // Z position
    val = objectList[obj].getZcentre();
    tempwidth = int( log10(val) + 1) + newset[Z].getPrecision() + 2;
    for(int i=newset[Z].getWidth();i<tempwidth;i++) newset[Z].widen();
    if((val<1.)&&(val>0.)){
      minval = pow(10, -1. * newset[Z].getPrecision()+1); 
      if((val>0.)&&(val < minval)) newset[Z].upPrec();
    }

    if(head.isWCS()){  
      // RA -- assign correct title. Check width but should be ok by definition
      tempstr = head.WCS().lngtyp;
      newset[RA].setName(tempstr);
      tempwidth = objectList[obj].getRAs().size() + 1;
      for(int i=newset[RA].getWidth();i<tempwidth;i++) newset[RA].widen();
      
      // Dec -- assign correct title. Check width, should be ok by definition
      tempstr = head.WCS().lattyp;
      newset[DEC].setName(tempstr);
      tempwidth = objectList[obj].getDecs().size() + 1;
      for(int i=newset[DEC].getWidth();i<tempwidth;i++) newset[DEC].widen();

      // Vel -- check width, title and units.
      if(head.getNumAxes() > 2){
	if(head.WCS().restfrq == 0) // using frequency, not velocity
	  newset[VEL].setName("FREQ");
	newset[VEL].setUnits("[" + head.getSpectralUnits() + "]");
	tempwidth = newset[VEL].getUnits().size() + 1;
	for(int i=newset[VEL].getWidth();i<tempwidth;i++) newset[VEL].widen();
	
	val = objectList[obj].getVel();
	tempwidth = int(log10(fabs(val)) + 1) + newset[VEL].getPrecision() + 2;
	if(val<0) tempwidth++;
	for(int i=newset[VEL].getWidth();i<tempwidth;i++) newset[VEL].widen();
	if((fabs(val) < 1.)&&(val>0.)){
	  minval = pow(10, -1. * newset[VEL].getPrecision()+1); 
	  if(val < minval) newset[VEL].upPrec();
	}
      }

      // w_RA -- check width & title. leave units for the moment.
      tempwidth = newset[RA].getUnits().size() + 1;
      for(int i=newset[RA].getWidth();i<tempwidth;i++) newset[RA].widen();
      val = objectList[obj].getRAWidth();
      tempwidth = int( log10(fabs(val)) + 1) + newset[WRA].getPrecision() + 2;
      if(val<0) tempwidth++;
      for(int i=newset[WRA].getWidth();i<tempwidth;i++) newset[WRA].widen();
      if((fabs(val) < 1.)&&(val>0.)){
	minval = pow(10, -1. * newset[WRA].getPrecision()+1); 
	if(val < minval) newset[WRA].upPrec();
      }

      // w_DEC -- check width & title. leave units for the moment.
      tempwidth = newset[DEC].getUnits().size() + 1;
      for(int i=newset[DEC].getWidth();i<tempwidth;i++) newset[DEC].widen();
      val = objectList[obj].getDecWidth();
      tempwidth = int( log10(fabs(val)) + 1) + newset[WDEC].getPrecision() + 2;
      if(val<0) tempwidth++;
      for(int i=newset[WDEC].getWidth();i<tempwidth;i++) newset[WDEC].widen();
      if((fabs(val) < 1.)&&(val>0.)){
	minval = pow(10, -1. * newset[WDEC].getPrecision()+1); 
	if(val < minval) newset[WDEC].upPrec();
      }

      // w_Vel -- check width, title and units.
      if(head.getNumAxes() > 2){
	if(head.WCS().restfrq == 0) // using frequency, not velocity
	  newset[WVEL].setName("w_FREQ");
	newset[WVEL].setUnits("[" + head.getSpectralUnits() + "]");
	tempwidth = newset[WVEL].getUnits().size() + 1;
	for(int i=newset[WVEL].getWidth();i<tempwidth;i++)newset[WVEL].widen();
	val = objectList[obj].getVel();
	tempwidth = int( log10(fabs(val)) + 1) + newset[WVEL].getPrecision() + 2;
	if(val<0) tempwidth++;
	for(int i=newset[WVEL].getWidth();i<tempwidth;i++) newset[WVEL].widen();
	if((fabs(val) < 1.)&&(val>0.)){
	  minval = pow(10, -1. * newset[WVEL].getPrecision()+1); 
	  if(val < minval) newset[WVEL].upPrec();
	}

	// F_int -- check width & units
	newset[FINT].setUnits("[" + head.getIntFluxUnits() + "]");
	tempwidth = newset[FINT].getUnits().size() + 1;
	for(int i=newset[FINT].getWidth();i<tempwidth;i++) newset[FINT].widen();
	val = objectList[obj].getIntegFlux();
	tempwidth = int( log10(fabs(val)) + 1) + newset[FINT].getPrecision() + 2;
	if(val<0) tempwidth++;
	for(int i=newset[FINT].getWidth();i<tempwidth;i++) newset[FINT].widen();
	if((fabs(val) < 1.)&&(val>0.)){
	  minval = pow(10, -1. * newset[FINT].getPrecision()+1); 
	  if(val < minval) newset[FINT].upPrec();
	}
      }
    }

    // F_tot
    newset[FTOT].setUnits("[" + head.getFluxUnits() + "]");
    tempwidth = newset[FTOT].getUnits().size() + 1;
    for(int i=newset[FTOT].getWidth();i<tempwidth;i++) newset[FTOT].widen();
    val = objectList[obj].getTotalFlux();
    tempwidth = int( log10(fabs(val)) + 1) + newset[FTOT].getPrecision() + 2;
    if(val<0) tempwidth++;
    for(int i=newset[FTOT].getWidth();i<tempwidth;i++) newset[FTOT].widen();
    if((fabs(val) < 1.)&&(val>0.)){
      minval = pow(10, -1. * newset[FTOT].getPrecision()+1); 
      if(val < minval) newset[FTOT].upPrec();
    }

    // F_peak
    newset[FPEAK].setUnits("[" + head.getFluxUnits() + "]");
    tempwidth = newset[FPEAK].getUnits().size() + 1;
    for(int i=newset[FPEAK].getWidth();i<tempwidth;i++) newset[FPEAK].widen();
    val = objectList[obj].getPeakFlux();
    tempwidth = int( log10(fabs(val)) + 1) + newset[FPEAK].getPrecision() + 2;
    if(val<0) tempwidth++;
    for(int i=newset[FPEAK].getWidth();i<tempwidth;i++) newset[FPEAK].widen();
    if((fabs(val) < 1.)&&(val>0.)){
      minval = pow(10, -1. * newset[FPEAK].getPrecision()+1); 
      if(val < minval) newset[FPEAK].upPrec();
    }

    // S/N_peak
    val = objectList[obj].getPeakSNR();
    tempwidth = int( log10(fabs(val)) + 1) + newset[SNRPEAK].getPrecision() +2;
    if(val<0) tempwidth++;
    for(int i=newset[SNRPEAK].getWidth();i<tempwidth;i++) 
      newset[SNRPEAK].widen();
    if((fabs(val) < 1.)&&(val>0.)){
      minval = pow(10, -1. * newset[SNRPEAK].getPrecision()+1); 
      if(val < minval) newset[SNRPEAK].upPrec();
    }

    // X1 position
    tempwidth = int( log10(objectList[obj].getXmin()) + 1) + 
      newset[X1].getPrecision() + 1;
    for(int i=newset[X1].getWidth();i<tempwidth;i++) newset[X1].widen();
    // X2 position
    tempwidth = int( log10(objectList[obj].getXmax()) + 1) + 
      newset[X2].getPrecision() + 1;
    for(int i=newset[X2].getWidth();i<tempwidth;i++) newset[X2].widen();
    // Y1 position
    tempwidth = int( log10(objectList[obj].getYmin()) + 1) + 
      newset[Y1].getPrecision() + 1;
    for(int i=newset[Y1].getWidth();i<tempwidth;i++) newset[Y1].widen();
    // Y2 position
    tempwidth = int( log10(objectList[obj].getYmax()) + 1) + 
      newset[Y2].getPrecision() + 1;
    for(int i=newset[Y2].getWidth();i<tempwidth;i++) newset[Y2].widen();
    // Z1 position
    tempwidth = int( log10(objectList[obj].getZmin()) + 1) + 
      newset[Z1].getPrecision() + 1;
    for(int i=newset[Z1].getWidth();i<tempwidth;i++) newset[Z1].widen();
    // Z2 position
    tempwidth = int( log10(objectList[obj].getZmax()) + 1) + 
      newset[Z2].getPrecision() + 1;
    for(int i=newset[Z2].getWidth();i<tempwidth;i++) newset[Z2].widen();

    // Npix
    tempwidth = int( log10(objectList[obj].getSize()) + 1) + 
      newset[NPIX].getPrecision() + 1;
    for(int i=newset[NPIX].getWidth();i<tempwidth;i++) newset[NPIX].widen();
    
    // average X position
    val = objectList[obj].getXAverage();
    tempwidth = int( log10(val) + 1) + newset[XAV].getPrecision() + 2;
    for(int i=newset[XAV].getWidth();i<tempwidth;i++) newset[XAV].widen();
    if((val<1.)&&(val>0.)){
      minval = pow(10, -1. * newset[XAV].getPrecision()+1); 
      if(val < minval) newset[XAV].upPrec();
    }
    // average Y position
    val = objectList[obj].getYAverage();
    tempwidth = int( log10(val) + 1) + newset[YAV].getPrecision() + 2;
    for(int i=newset[YAV].getWidth();i<tempwidth;i++) newset[YAV].widen();
    if((val<1.)&&(val>0.)){
      minval = pow(10, -1. * newset[YAV].getPrecision()+1); 
      if(val < minval) newset[YAV].upPrec();
    }
    // average Z position
    val = objectList[obj].getZAverage();
    tempwidth = int( log10(val) + 1) + newset[ZAV].getPrecision() + 2;
    for(int i=newset[ZAV].getWidth();i<tempwidth;i++) newset[ZAV].widen();
    if((val<1.)&&(val>0.)){
      minval = pow(10, -1. * newset[ZAV].getPrecision()+1); 
      if((val>0.)&&(val < minval)) newset[ZAV].upPrec();
    }
    
    // X position of centroid
    val = objectList[obj].getXCentroid();
    tempwidth = int( log10(val) + 1) + newset[XCENT].getPrecision() + 2;
    for(int i=newset[XCENT].getWidth();i<tempwidth;i++) newset[XCENT].widen();
    if((val<1.)&&(val>0.)){
      minval = pow(10, -1. * newset[XCENT].getPrecision()+1); 
      if(val < minval) newset[XCENT].upPrec();
    }
    // Y position of centroid
    val = objectList[obj].getYCentroid();
    tempwidth = int( log10(val) + 1) + newset[YCENT].getPrecision() + 2;
    for(int i=newset[YCENT].getWidth();i<tempwidth;i++) newset[YCENT].widen();
    if((val<1.)&&(val>0.)){
      minval = pow(10, -1. * newset[YCENT].getPrecision()+1); 
      if(val < minval) newset[YCENT].upPrec();
    }
    // Z position of centroid
    val = objectList[obj].getZCentroid();
    tempwidth = int( log10(val) + 1) + newset[ZCENT].getPrecision() + 2;
    for(int i=newset[ZCENT].getWidth();i<tempwidth;i++) newset[ZCENT].widen();
    if((val<1.)&&(val>0.)){
      minval = pow(10, -1. * newset[ZCENT].getPrecision()+1); 
      if((val>0.)&&(val < minval)) newset[ZCENT].upPrec();
    }
    
    // X position of peak flux
    val = objectList[obj].getXPeak();
    tempwidth = int( log10(val) + 1) + newset[XPEAK].getPrecision() + 2;
    for(int i=newset[XPEAK].getWidth();i<tempwidth;i++) newset[XPEAK].widen();
    if((val<1.)&&(val>0.)){
      minval = pow(10, -1. * newset[XPEAK].getPrecision()+1); 
      if(val < minval) newset[XPEAK].upPrec();
    }
    // Y position of peak flux
    val = objectList[obj].getYPeak();
    tempwidth = int( log10(val) + 1) + newset[YPEAK].getPrecision() + 2;
    for(int i=newset[YPEAK].getWidth();i<tempwidth;i++) newset[YPEAK].widen();
    if((val<1.)&&(val>0.)){
      minval = pow(10, -1. * newset[YPEAK].getPrecision()+1); 
      if(val < minval) newset[YPEAK].upPrec();
    }
    // Z position of peak flux
    val = objectList[obj].getZPeak();
    tempwidth = int( log10(val) + 1) + newset[ZPEAK].getPrecision() + 2;
    for(int i=newset[ZPEAK].getWidth();i<tempwidth;i++) newset[ZPEAK].widen();
    if((val<1.)&&(val>0.)){
      minval = pow(10, -1. * newset[ZPEAK].getPrecision()+1); 
      if((val>0.)&&(val < minval)) newset[ZPEAK].upPrec();
    }
  }

  return newset;

}

std::vector<Col> getLogColSet(std::vector<Detection> &objectList, 
			      FitsHeader &head)
{
  /**
   *  A function that returns a std::vector of Col objects containing
   *    information on the columns necessary for logfile output:
   *    Obj#,X,Y,Z,F_tot,F_peak,X1,X2,Y1,Y2,Z1,Z2,Npix
   *
   *   Each object in the provided objectList is checked to see if 
   *    it requires any column to be widened, or for that column to
   *    have its precision increased.
   *
   * \param objectList A std::vector list of Detection objects that the columns need to fit.
   * \param head The FitsHeader object defining the World Coordinate System.
   * \return A std::vector list of Col definitions.
   */

  std::vector<Col> newset,tempset;
  
  // set up the default columns:
  //  get from FullColSet, and select only the ones we want.
  tempset = getFullColSet(objectList,head);

  newset.push_back( tempset[NUM] );
  newset.push_back( tempset[X] );
  newset.push_back( tempset[Y] );
  newset.push_back( tempset[Z] );
  newset.push_back( tempset[FTOT] );
  newset.push_back( tempset[FPEAK] );
  newset.push_back( tempset[SNRPEAK] );
  newset.push_back( tempset[X1] );
  newset.push_back( tempset[X2] );
  newset.push_back( tempset[Y1] );
  newset.push_back( tempset[Y2] );
  newset.push_back( tempset[Z1] );
  newset.push_back( tempset[Z2] );
  newset.push_back( tempset[NPIX] );

  return newset;

}
