#include <iostream>
#include <sstream>

#include <duchamp.hh>
#include <param.hh>
#include <vector> 
#include <string>
#include <Detection/detection.hh>
#include <Detection/columns.hh>

using std::string;
using std::vector;
using namespace Column;

Col::Col(int num)
{
  /**
   *  Col::Col(int num)
   *   A specialised constructor that defines one of the default 
   *    columns, as defined in the Column namespace
   */ 
  if((num>=0)&&(num<22)){
    this->width =     defaultWidth[num];
    this->precision = defaultPrec[num];
    this->name =      defaultName[num];
    this->units =     defaultUnits[num];
  }
  else{
    std::stringstream errmsg;
    errmsg << "Incorrect value for Col(num) --> num="<<num
	   << ", should be between 0 and 21.\n";
    duchampError("Col constructor", errmsg.str());
    this->width = 1;
    this->precision = 0;
    this->name = " ";
    this->units = " ";
  }
}

vector<Col> getFullColSet(vector<Detection> &objectList, FitsHeader &head)
{
  /**
   *  getFullColSet(objectlist, head)
   *   A function that returns a vector of Col objects containing
   *    information on the columns necessary for output to the results file:
   *    Obj#,NAME,X,Y,Z,RA,DEC,VEL,w_RA,w_DEC,w_VEL,F_tot,F_int,F_peak,
   *                X1,X2,Y1,Y2,Z1,Z2,Npix,Flag
   *   Each object in the provided objectList is checked to see if 
   *    it requires any column to be widened, or for that column to
   *    have its precision increased.
   *   Both Ftot and Fint are provided -- it is up to the calling function to
   *    determine which to use.
   */

  vector<Col> newset;

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
  newset.push_back( Col(X1) );
  newset.push_back( Col(X2) );
  newset.push_back( Col(Y1) );
  newset.push_back( Col(Y2) );
  newset.push_back( Col(Z1) );
  newset.push_back( Col(Z2) );
  newset.push_back( Col(NPIX) );
  newset.push_back( Col(FLAG) );

   // Now test each object against each new column
  for( int obj = 0; obj < objectList.size(); obj++){
    string tempstr;
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
    if(val < 1.){
      minval = pow(10, -1. * newset[X].getPrecision()+1); 
      if(val < minval) newset[X].upPrec();
    }
    // Y position
    val = objectList[obj].getYcentre();
    tempwidth = int( log10(val) + 1) + newset[Y].getPrecision() + 2;
    for(int i=newset[Y].getWidth();i<tempwidth;i++) newset[Y].widen();
    if(val < 1.){
      minval = pow(10, -1. * newset[Y].getPrecision()+1); 
      if(val < minval) newset[Y].upPrec();
    }
    // Z position
    val = objectList[obj].getZcentre();
    tempwidth = int( log10(val) + 1) + newset[Z].getPrecision() + 2;
    for(int i=newset[Z].getWidth();i<tempwidth;i++) newset[Z].widen();
    if(val < 1.){
      minval = pow(10, -1. * newset[Z].getPrecision()+1); 
      if(val < minval) newset[Z].upPrec();
    }

    if(head.isWCS()){  
      // RA -- assign correct title. Check width but should be ok by definition
      tempstr = head.getWCS()->lngtyp;
      newset[RA].setName(tempstr);
      tempwidth = objectList[obj].getRAs().size() + 1;
      for(int i=newset[RA].getWidth();i<tempwidth;i++) newset[RA].widen();
      
      // Dec -- assign correct title. Check width but should be ok by definition
      tempstr = head.getWCS()->lattyp;
      newset[DEC].setName(tempstr);
      tempwidth = objectList[obj].getDecs().size() + 1;
      for(int i=newset[DEC].getWidth();i<tempwidth;i++) newset[DEC].widen();

      // Vel -- check width, title and units.
      if(head.getWCS()->restfrq == 0) // using frequency, not velocity
	newset[VEL].setName("FREQ");
      newset[VEL].setUnits("[" + head.getSpectralUnits() + "]");
      tempwidth = newset[VEL].getUnits().size() + 1;
      for(int i=newset[VEL].getWidth();i<tempwidth;i++) newset[VEL].widen();      
      val = objectList[obj].getVel();
      tempwidth = int( log10(fabs(val)) + 1) + newset[VEL].getPrecision() + 2;
      if(val<0) tempwidth++;
      for(int i=newset[VEL].getWidth();i<tempwidth;i++) newset[VEL].widen();
      if(val < 1.){
	minval = pow(10, -1. * newset[VEL].getPrecision()+1); 
	if(val < minval) newset[VEL].upPrec();
      }

      // w_RA -- check width & title. leave units for the moment.
      tempwidth = newset[RA].getUnits().size() + 1;
      for(int i=newset[RA].getWidth();i<tempwidth;i++) newset[RA].widen();      
      val = objectList[obj].getRAWidth();
      tempwidth = int( log10(fabs(val)) + 1) + newset[WRA].getPrecision() + 2;
      if(val<0) tempwidth++;
      for(int i=newset[WRA].getWidth();i<tempwidth;i++) newset[WRA].widen();
      if(val < 1.){
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
      if(val < 1.){
	minval = pow(10, -1. * newset[WDEC].getPrecision()+1); 
	if(val < minval) newset[WDEC].upPrec();
      }

      // w_Vel -- check width, title and units.
      if(head.getWCS()->restfrq == 0) // using frequency, not velocity
	newset[WVEL].setName("w_FREQ");
      newset[WVEL].setUnits("[" + head.getSpectralUnits() + "]");
      tempwidth = newset[WVEL].getUnits().size() + 1;
      for(int i=newset[WVEL].getWidth();i<tempwidth;i++) newset[WVEL].widen();      
      val = objectList[obj].getVel();
      tempwidth = int( log10(fabs(val)) + 1) + newset[WVEL].getPrecision() + 2;
      if(val<0) tempwidth++;
      for(int i=newset[WVEL].getWidth();i<tempwidth;i++) newset[WVEL].widen();
      if(val < 1.){
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
      if(val < 1.){
	minval = pow(10, -1. * newset[FINT].getPrecision()+1); 
	if(val < minval) newset[FINT].upPrec();
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
    if(val < 1.){
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
    if(val < 1.){
      minval = pow(10, -1. * newset[FPEAK].getPrecision()+1); 
      if(val < minval) newset[FPEAK].upPrec();
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
    
  }

  return newset;

}

vector<Col> getLogColSet(vector<Detection> &objectList, FitsHeader &head)
{
  /**
   *  getLogColSet(objectlist)
   *   A function that returns a vector of Col objects containing
   *    information on the columns necessary for logfile output:
   *    Obj#,X,Y,Z,F_tot,F_peak,X1,X2,Y1,Y2,Z1,Z2,Npix
   *   Each object in the provided objectList is checked to see if 
   *    it requires any column to be widened, or for that column to
   *    have its precision increased.
   */

  vector<Col> newset,tempset;
  
  // set up the default columns:
  //  get from FullColSet, and select only the ones we want.
  tempset = getFullColSet(objectList,head);

  newset.push_back( tempset[0] );
  newset.push_back( tempset[2] );
  newset.push_back( tempset[3] );
  newset.push_back( tempset[4] );
  newset.push_back( tempset[12] );
  newset.push_back( tempset[13] );
  newset.push_back( tempset[14] );
  newset.push_back( tempset[15] );
  newset.push_back( tempset[16] );
  newset.push_back( tempset[17] );
  newset.push_back( tempset[18] );
  newset.push_back( tempset[19] );
  newset.push_back( tempset[20] );

  return newset;

}
