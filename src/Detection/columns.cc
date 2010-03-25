// -----------------------------------------------------------------------
// columns.cc: Define a set of columns for Duchamp output.
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
#include <sstream>

#include <math.h>
#include <duchamp/duchamp.hh>
#include <duchamp/fitsHeader.hh>
#include <vector> 
#include <string>
#include <duchamp/Detection/detection.hh>
#include <duchamp/Detection/columns.hh>

namespace duchamp
{

  namespace Column
  {


    Col::Col()
    {
      /// @brief Set the default values for the parameters. 
      this->width=1; 
      this->precision=0; 
      this->name=" "; 
      this->units=" ";
      this->type=UNKNOWN;
    }

    Col::~Col(){}

    Col::Col(const Col& c)
    {
      operator=(c);
    }

    Col& Col::operator=(const Col& c)
    {
      if(this==&c) return *this;
      this->width = c.width;
      this->precision = c.precision;
      this->name = c.name;
      this->units = c.units;
      this->type = c.type;
      return *this;
    }

    Col::Col(int num, int prec)
    {
      /// A specialised constructor that defines one of the default 
      ///  columns, as defined in the Column namespace
      /// \param num The number of the column to be constructed. 
      ///            Corresponds to the order of the columns in the const 
      ///            arrays in the Column namespace.
      /// \param prec The precision to use, if not the default. A value
      /// <0 or no value given results in the default being used.

      if((num>=0)&&(num<numColumns)){
	this->width     = defaultWidth[num];
	if(prec<0) this->precision = defaultPrec[num];
	else this->precision = prec;
	this->name      = defaultName[num];
	this->units     = defaultUnits[num];
	this->type = COLNAME(num);
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
	this->type=UNKNOWN;
      }
    }

    Col::Col(std::string name, std::string units, int width, int prec)
    {
      /// A generic constructor designed to make a Column other than
      /// the predefined ones listed in the Column namespace
      /// \param name The title of the column
      /// \param units What units the column takes
      /// \param width The starting width
      /// \param prec The starting precision

      this->width = width;
      this->precision = prec;
      this->name = name;
      this->units = units;
      this->type = UNKNOWN;
    }

    //------------------------------------------------------------

    void Col::changePrec(int p)
    {
      /// A direct way to alter the precision without having to use
      /// Col::upPrec(). If the precision increases, the width will
      /// increase by the same amount. If it decreases, no change is
      /// made to the width.
      /// \param p The new precision value.

      if(p > this->precision) this->width += (p-this->precision);
      this->precision = p;

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
  
    template <class T> void Col::printEntry(std::ostream &stream, T value)
    {
      ///  \param stream Where the printing is done.
      ///  \param value  The value to be printed.

      stream << std::setprecision(this->precision)
	     << std::setw(this->width) 
	     << std::setfill(' ')
	     << value;
    }
    template void Col::printEntry<int>(std::ostream &stream, int value);
    template void Col::printEntry<long>(std::ostream &stream, long value);
    template void Col::printEntry<unsigned int>(std::ostream &stream, unsigned int value);
    template void Col::printEntry<unsigned long>(std::ostream &stream, unsigned long value);
    template void Col::printEntry<float>(std::ostream &stream, float value);
    template void Col::printEntry<double>(std::ostream &stream, double value);
    template void Col::printEntry<std::string>(std::ostream &stream, std::string value);

 
    bool Col::doCol(std::string tableType, bool flagFint)
    {
      ///  @details Uses the info in the isFile etc arrays to
      ///  determine whether a given column, referenced by the
      ///  enumeration COLNAME, is used for a given table type.
      /// \param tableType The type of table: one of file, screen, log, votable.
      /// \param flagFint Whether to use FINT (true) or FTOT
      /// (false). This defaults to true, so need not be given. It only
      /// applies to the screen and votable cases -- both are written for the
      /// results file case.
      /// \return True if column is used for given table type. False
      /// otherwise. False if tableType not one of four listed.
      
      if(tableType == "file") return isFile[this->type];
      else if(tableType == "screen"){
	if(flagFint && this->type==FTOT) return false;
	if(!flagFint && this->type==FINT) return false;
	return isScreen[this->type];
      }
      else if(tableType == "log") return isLog[this->type];
      else if(tableType == "votable"){
	if(flagFint && this->type==FTOT) return false;
	if(!flagFint && this->type==FINT) return false;
	return isVOTable[this->type];
      }
      else return false;
    }

  

    std::vector<Col> getFullColSet(std::vector<Detection> &objectList, 
				   FitsHeader &head)
    {
      ///  @details A function that returns a std::vector of Col
      ///  objects containing information on the columns necessary for
      ///  output to the results file:
      ///    Obj#,NAME,X,Y,Z,RA,DEC,VEL,w_RA,w_DEC,w_VEL,F_tot,F_int,F_peak,
      ///                X1,X2,Y1,Y2,Z1,Z2,Npix,Flag,
      ///                XAV,YAV,ZAV,XCENT,YCENT,ZCENT,XPEAK,YPEAK,ZPEAK
      /// 
      ///   Each object in the provided objectList is checked to see if it
      ///   requires any column to be widened, or for that column to have
      ///   its precision increased.
      /// 
      ///   Both Ftot and Fint are provided -- it is up to the calling
      ///   function to determine which to use.
      /// 
      /// \param objectList A std::vector list of Detection objects that the
      /// columns need to fit.
      /// \param head The FitsHeader object defining the World Coordinate
      /// System.
      /// \return A std::vector list of Col definitions.

      std::vector<Col> newset;

      // desired precisions for fluxes, velocities and SNR value
      int precVel,precFlux,precSNR;
      if(objectList.size()>0){
	precVel=objectList[0].getVelPrec();
	precFlux=objectList[0].getFpeakPrec(); // same as FintPrec at this point.
	precSNR=objectList[0].getSNRPrec();
      }
      else {
	precVel = prVEL;
	precFlux = prFLUX;
	precSNR = prSNR;
      }

      // set up the default columns 
      newset.push_back( Col(NUM) );
      newset.push_back( Col(NAME) );
      newset.push_back( Col(X) );
      newset.push_back( Col(Y) );
      newset.push_back( Col(Z) );
      newset.push_back( Col(RA) );
      newset.push_back( Col(DEC) );
      newset.push_back( Col(RAJD) );
      newset.push_back( Col(DECJD) );
      newset.push_back( Col(VEL, precVel) );
      newset.push_back( Col(WRA) );
      newset.push_back( Col(WDEC) );
      newset.push_back( Col(W50, precVel) );
      newset.push_back( Col(W20, precVel) );
      newset.push_back( Col(WVEL, precVel) );
      newset.push_back( Col(FINT, precFlux) );
      newset.push_back( Col(FTOT, precFlux) );
      newset.push_back( Col(FPEAK, precFlux) );
      newset.push_back( Col(SNRPEAK, precSNR) );
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
      newset.push_back( Col(NUMCH) );
      newset.push_back( Col(SPATSIZE) );


      // Now test each object against each new column
      std::vector<Detection>::iterator obj;
      for(obj = objectList.begin(); obj < objectList.end(); obj++){
	std::string tempstr;
	int tempwidth;
	float val,minval;

	// Obj#
	tempwidth = int( log10(obj->getID()) + 1) + 1;
	for(int i=newset[NUM].getWidth();i<tempwidth;i++) newset[NUM].widen();

	// Name
	tempwidth = obj->getName().size() + 1;
	for(int i=newset[NAME].getWidth();i<tempwidth;i++) newset[NAME].widen();

	// X position
	val = obj->getXcentre() + obj->getXOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset[X].getPrecision()+1)); 
	  if(val < minval) newset[X].upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset[X].getPrecision() + 2;
	for(int i=newset[X].getWidth();i<tempwidth;i++) newset[X].widen();
	// Y position
	val = obj->getYcentre() + obj->getYOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset[Y].getPrecision()+1)); 
	  if(val < minval) newset[Y].upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset[Y].getPrecision() + 2;
	for(int i=newset[Y].getWidth();i<tempwidth;i++) newset[Y].widen();
	// Z position
	val = obj->getZcentre() + obj->getZOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset[Z].getPrecision()+1)); 
	  if((val>0.)&&(val < minval)) newset[Z].upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset[Z].getPrecision() + 2;
	for(int i=newset[Z].getWidth();i<tempwidth;i++) newset[Z].widen();

	if(head.isWCS()){  
	  // RA -- assign correct title. Check width but should be ok by definition
	  tempstr = head.WCS().lngtyp;
	  newset[RA].setName(tempstr);
	  tempwidth = obj->getRAs().size() + 1;
	  for(int i=newset[RA].getWidth();i<tempwidth;i++) newset[RA].widen();
      
	  // Dec -- assign correct title. Check width, should be ok by definition
	  tempstr = head.WCS().lattyp;
	  newset[DEC].setName(tempstr);
	  tempwidth = obj->getDecs().size() + 1;
	  for(int i=newset[DEC].getWidth();i<tempwidth;i++) newset[DEC].widen();

	  // RA decimal degrees -- assign correct title. Check width but should be OK
	  tempstr = head.WCS().lngtyp;
	  newset[RAJD].setName(tempstr);
	  val = obj->getRA();
	  tempwidth = int( log10(fabs(val)) + 1) + newset[RAJD].getPrecision() + 2;
	  for(int i=newset[RAJD].getWidth();i<tempwidth;i++) newset[RAJD].widen();
      
	  // Dec decimal degrees -- assign correct title. Check width but should be OK
	  tempstr = head.WCS().lattyp;
	  newset[DECJD].setName(tempstr);
	  val = obj->getDec();
	  tempwidth = int( log10(fabs(val)) + 1) + newset[DECJD].getPrecision() + 2;
	  for(int i=newset[DECJD].getWidth();i<tempwidth;i++) newset[DECJD].widen();

	  // Vel -- check width, title and units.
	  //if(head.isSpecOK()){
	  if(head.canUseThirdAxis()){
	    if(head.WCS().spec < 0)  // if it's not a spectral axis
	      newset[VEL].setName( head.WCS().ctype[2] );
// 	    else if(head.WCS().restfrq == 0) // using frequency, not velocity
	    else if(head.getSpectralDescription()==duchamp::duchampSpectralDescription[FREQUENCY]) // using frequency, not velocity
	      newset[VEL].setName("FREQ");
	    if(head.getSpectralUnits().size()>0)
	      newset[VEL].setUnits("[" + head.getSpectralUnits() + "]");
	    tempwidth = newset[VEL].getUnits().size() + 1;
	    for(int i=newset[VEL].getWidth();i<tempwidth;i++) newset[VEL].widen();
	
	    val = obj->getVel();
	    if((fabs(val) < 1.)&&(val>0.)){
	      minval = pow(10, -1. * (newset[VEL].getPrecision()+1)); 
	      if(val < minval) newset[VEL].upPrec();
	    }
	    tempwidth = int(log10(fabs(val)) + 1) + newset[VEL].getPrecision() + 2;
	    if(val<0) tempwidth++;
	    for(int i=newset[VEL].getWidth();i<tempwidth;i++) newset[VEL].widen();
	  }

	  // w_RA -- check width & title. leave units for the moment.
	  tempwidth = newset[RA].getUnits().size() + 1;
	  for(int i=newset[RA].getWidth();i<tempwidth;i++) newset[RA].widen();
	  val = obj->getRAWidth();
	  if((fabs(val) < 1.)&&(val>0.)){
	    minval = pow(10, -1. * (newset[WRA].getPrecision()+1)); 
	    if(val < minval) newset[WRA].upPrec();
	  }
	  tempwidth = int( log10(fabs(val)) + 1) + newset[WRA].getPrecision() + 2;
	  if(val<0) tempwidth++;
	  for(int i=newset[WRA].getWidth();i<tempwidth;i++) newset[WRA].widen();

	  // w_DEC -- check width & title. leave units for the moment.
	  tempwidth = newset[DEC].getUnits().size() + 1;
	  for(int i=newset[DEC].getWidth();i<tempwidth;i++) newset[DEC].widen();
	  val = obj->getDecWidth();
	  if((fabs(val) < 1.)&&(val>0.)){
	    minval = pow(10, -1. * (newset[WDEC].getPrecision()+1)); 
	    if(val < minval) newset[WDEC].upPrec();
	  }
	  tempwidth = int( log10(fabs(val)) + 1) + newset[WDEC].getPrecision() + 2;
	  if(val<0) tempwidth++;
	  for(int i=newset[WDEC].getWidth();i<tempwidth;i++) newset[WDEC].widen();

	  // w_50 -- check width, title and units.
	  if(head.canUseThirdAxis()){
	    if(head.getSpectralUnits().size()>0)
	      newset[W50].setUnits("[" + head.getSpectralUnits() + "]");
	    tempwidth = newset[W50].getUnits().size() + 1;
	    for(int i=newset[W50].getWidth();i<tempwidth;i++) newset[W50].widen();
	    val = obj->getW50();
	    if((fabs(val) < 1.)&&(val>0.)){
	      minval = pow(10, -1. * (newset[W50].getPrecision()+1)); 
	      if(val < minval) newset[W50].upPrec();
	    }
	    tempwidth = int( log10(fabs(val)) + 1) + newset[W50].getPrecision() + 2;
	    if(val<0) tempwidth++;
	    for(int i=newset[W50].getWidth();i<tempwidth;i++) newset[W50].widen();
	  }

	  // w_20 -- check width, title and units.
	  if(head.canUseThirdAxis()){
	    if(head.getSpectralUnits().size()>0)
	      newset[W20].setUnits("[" + head.getSpectralUnits() + "]");
	    tempwidth = newset[W20].getUnits().size() + 1;
	    for(int i=newset[W20].getWidth();i<tempwidth;i++)newset[W20].widen();
	    val = obj->getW20();
	    if((fabs(val) < 1.)&&(val>0.)){
	      minval = pow(10, -1. * (newset[W20].getPrecision()+1)); 
	      if(val < minval) newset[W20].upPrec();
	    }
	    tempwidth = int( log10(fabs(val)) + 1) + newset[W20].getPrecision() + 2;
	    if(val<0) tempwidth++;
	    for(int i=newset[W20].getWidth();i<tempwidth;i++) newset[W20].widen();
	  }

	  // w_Vel -- check width, title and units.
	  if(head.canUseThirdAxis()){
	    if(head.WCS().spec < 0) // if it's not a spectral axis
	      newset[WVEL].setName( std::string("w_") + head.WCS().ctype[2] );
// 	    else if(head.WCS().restfrq == 0) // using frequency, not velocity
	    else if(head.getSpectralDescription()==duchamp::duchampSpectralDescription[FREQUENCY]) // using frequency, not velocity
	      newset[WVEL].setName("w_FREQ");
	    if(head.getSpectralUnits().size()>0)
	      newset[WVEL].setUnits("[" + head.getSpectralUnits() + "]");
	    tempwidth = newset[WVEL].getUnits().size() + 1;
	    for(int i=newset[WVEL].getWidth();i<tempwidth;i++)newset[WVEL].widen();
	    tempwidth = newset[WVEL].getName().size() + 1;
	    for(int i=newset[WVEL].getWidth();i<tempwidth;i++) newset[WVEL].widen();
	    val = obj->getVelWidth();
	    if((fabs(val) < 1.)&&(val>0.)){
	      minval = pow(10, -1. * (newset[WVEL].getPrecision()+1)); 
	      if(val < minval) newset[WVEL].upPrec();
	    }
	    tempwidth = int( log10(fabs(val)) + 1) + newset[WVEL].getPrecision() + 2;
	    if(val<0) tempwidth++;
	    for(int i=newset[WVEL].getWidth();i<tempwidth;i++) newset[WVEL].widen();
	  }

	  // F_int -- check width & units
	  if(head.getIntFluxUnits().size()>0)
	    newset[FINT].setUnits("[" + head.getIntFluxUnits() + "]");
	  tempwidth = newset[FINT].getUnits().size() + 1;
	  for(int i=newset[FINT].getWidth();i<tempwidth;i++) newset[FINT].widen();
	  val = obj->getIntegFlux();
	  if((fabs(val) < 1.)// &&(val>0.)
	     ){
	    int minprec = int(fabs(log10(fabs(val))))+2;
	    for(int i=newset[FINT].getPrecision();i<minprec;i++) newset[FINT].upPrec();
	  }
	  tempwidth = int( log10(fabs(val)) + 1) + newset[FINT].getPrecision() + 2;
	  if(val<0) tempwidth++;
	  for(int i=newset[FINT].getWidth();i<tempwidth;i++) newset[FINT].widen();
      
	}

	// F_tot
	newset[FTOT].setUnits("[" + head.getFluxUnits() + "]");
	tempwidth = newset[FTOT].getUnits().size() + 1;
	for(int i=newset[FTOT].getWidth();i<tempwidth;i++) newset[FTOT].widen();
	val = obj->getTotalFlux();
	//     std::cerr << val << "\n";
	if((fabs(val) < 1.)// &&(val>0.)
	   ){
	  int minprec = int(fabs(log10(fabs(val))))+2;
	  for(int i=newset[FTOT].getPrecision();i<minprec;i++) newset[FTOT].upPrec();
	}
	tempwidth = int( log10(fabs(val)) + 1) + newset[FTOT].getPrecision() + 2;
	if(val<0) tempwidth++;
	for(int i=newset[FTOT].getWidth();i<tempwidth;i++) newset[FTOT].widen();

	// F_peak
	newset[FPEAK].setUnits("[" + head.getFluxUnits() + "]");
	tempwidth = newset[FPEAK].getUnits().size() + 1;
	for(int i=newset[FPEAK].getWidth();i<tempwidth;i++) newset[FPEAK].widen();
	val = obj->getPeakFlux();
	if((fabs(val) < 1.)// &&(val>0.)
	   ){
	  int minprec = int(fabs(log10(fabs(val))))+2;
	  for(int i=newset[FPEAK].getPrecision();i<minprec;i++) newset[FPEAK].upPrec();
	}
	tempwidth = int( log10(fabs(val)) + 1) + newset[FPEAK].getPrecision() + 2;
	if(val<0) tempwidth++;
	for(int i=newset[FPEAK].getWidth();i<tempwidth;i++) newset[FPEAK].widen();

	// S/N_peak
	val = obj->getPeakSNR();
	if((fabs(val) < 1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset[SNRPEAK].getPrecision()+1)); 
	  if(val < minval) newset[SNRPEAK].upPrec();
	}
	tempwidth = int( log10(fabs(val)) + 1) + newset[SNRPEAK].getPrecision() +2;
	if(val<0) tempwidth++;
	for(int i=newset[SNRPEAK].getWidth();i<tempwidth;i++) newset[SNRPEAK].widen();

	// X1 position
	val = obj->getXmin() + obj->getXOffset();
	tempwidth = int( log10(val) + 1) + newset[X1].getPrecision() + 1;
	for(int i=newset[X1].getWidth();i<tempwidth;i++) newset[X1].widen();
	// X2 position
	val = obj->getXmax() + obj->getXOffset();
	tempwidth = int( log10(val) + 1) + newset[X2].getPrecision() + 1;
	for(int i=newset[X2].getWidth();i<tempwidth;i++) newset[X2].widen();
	// Y1 position
	val = obj->getYmin() + obj->getYOffset();
	tempwidth = int( log10(val) + 1) + newset[Y1].getPrecision() + 1;
	for(int i=newset[Y1].getWidth();i<tempwidth;i++) newset[Y1].widen();
	// Y2 position
	val = obj->getYmax() + obj->getYOffset();
	tempwidth = int( log10(val) + 1) + newset[Y2].getPrecision() + 1;
	for(int i=newset[Y2].getWidth();i<tempwidth;i++) newset[Y2].widen();
	// Z1 position
	val = obj->getZmin() + obj->getZOffset();
	tempwidth = int( log10(val) + 1) + newset[Z1].getPrecision() + 1;
	for(int i=newset[Z1].getWidth();i<tempwidth;i++) newset[Z1].widen();
	// Z2 position
	val = obj->getZmax() + obj->getZOffset();
	tempwidth = int( log10(val) + 1) + newset[Z2].getPrecision() + 1;
	for(int i=newset[Z2].getWidth();i<tempwidth;i++) newset[Z2].widen();

	// Npix
	tempwidth = int( log10(obj->getSize()) + 1) + 
	  newset[NPIX].getPrecision() + 1;
	for(int i=newset[NPIX].getWidth();i<tempwidth;i++) newset[NPIX].widen();
    
	// average X position
// 	val = obj->getXAverage() + obj->getXOffset();
	val = obj->getXaverage() + obj->getXOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset[XAV].getPrecision()+1)); 
	  if(val < minval) newset[XAV].upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset[XAV].getPrecision() + 2;
	for(int i=newset[XAV].getWidth();i<tempwidth;i++) newset[XAV].widen();
	// average Y position
// 	val = obj->getYAverage() + obj->getYOffset();
	val = obj->getYaverage() + obj->getYOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset[YAV].getPrecision()+1)); 
	  if(val < minval) newset[YAV].upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset[YAV].getPrecision() + 2;
	for(int i=newset[YAV].getWidth();i<tempwidth;i++) newset[YAV].widen();
	// average Z position
// 	val = obj->getZAverage() + obj->getZOffset();
	val = obj->getZaverage() + obj->getZOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset[ZAV].getPrecision()+1)); 
	  if((val>0.)&&(val < minval)) newset[ZAV].upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset[ZAV].getPrecision() + 2;
	for(int i=newset[ZAV].getWidth();i<tempwidth;i++) newset[ZAV].widen();
    
	// X position of centroid
	val = obj->getXCentroid() + obj->getXOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset[XCENT].getPrecision()+1)); 
	  if(val < minval) newset[XCENT].upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset[XCENT].getPrecision() + 2;
	for(int i=newset[XCENT].getWidth();i<tempwidth;i++) newset[XCENT].widen();
	// Y position of centroid
	val = obj->getYCentroid() + obj->getYOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset[YCENT].getPrecision()+1)); 
	  if(val < minval) newset[YCENT].upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset[YCENT].getPrecision() + 2;
	for(int i=newset[YCENT].getWidth();i<tempwidth;i++) newset[YCENT].widen();
	// Z position of centroid
	val = obj->getZCentroid() + obj->getZOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset[ZCENT].getPrecision()+1)); 
	  if((val>0.)&&(val < minval)) newset[ZCENT].upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset[ZCENT].getPrecision() + 2;
	for(int i=newset[ZCENT].getWidth();i<tempwidth;i++) newset[ZCENT].widen();
    
	// X position of peak flux
	val = obj->getXPeak() + obj->getXOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset[XPEAK].getPrecision()+1)); 
	  if(val < minval) newset[XPEAK].upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset[XPEAK].getPrecision() + 2;
	for(int i=newset[XPEAK].getWidth();i<tempwidth;i++) newset[XPEAK].widen();
	// Y position of peak flux
	val = obj->getYPeak() + obj->getYOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset[YPEAK].getPrecision()+1)); 
	  if(val < minval) newset[YPEAK].upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset[YPEAK].getPrecision() + 2;
	for(int i=newset[YPEAK].getWidth();i<tempwidth;i++) newset[YPEAK].widen();
	// Z position of peak flux
	val = obj->getZPeak() + obj->getZOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset[ZPEAK].getPrecision()+1)); 
	  if((val>0.)&&(val < minval)) newset[ZPEAK].upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset[ZPEAK].getPrecision() + 2;
	for(int i=newset[ZPEAK].getWidth();i<tempwidth;i++) newset[ZPEAK].widen();

	//Number of contiguous channels
	tempwidth = int( log10(obj->getMaxAdjacentChannels()) + 1) + 
	  newset[NUMCH].getPrecision() + 1;
	for(int i=newset[NUMCH].getWidth();i<tempwidth;i++) newset[NUMCH].widen();
	//Spatial size
	tempwidth = int( log10(obj->getSpatialSize()) + 1) + 
	  newset[SPATSIZE].getPrecision() + 1;
	for(int i=newset[SPATSIZE].getWidth();i<tempwidth;i++) newset[SPATSIZE].widen();
	
      }
      
      return newset;

    }

    std::vector<Col> getLogColSet(std::vector<Detection> &objectList, 
				  FitsHeader &head)
    {
      ///  @details A function that returns a std::vector of Col
      ///    objects containing information on the columns necessary
      ///    for logfile output:
      ///    Obj#,X,Y,Z,F_tot,F_peak,X1,X2,Y1,Y2,Z1,Z2,Npix
      /// 
      ///   Each object in the provided objectList is checked to see if 
      ///    it requires any column to be widened, or for that column to
      ///    have its precision increased.
      /// 
      /// \param objectList A std::vector list of Detection objects that the columns need to fit.
      /// \param head The FitsHeader object defining the World Coordinate System.
      /// \return A std::vector list of Col definitions.

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
      newset.push_back( tempset[NUMCH] );
      newset.push_back( tempset[SPATSIZE] );

      return newset;

    }

  }

}
