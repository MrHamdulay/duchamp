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
#include <duchamp/Outputs/columns.hh>

namespace duchamp
{

  namespace Catalogues
  {


    Column::Column()
    {
      /// @brief Set the default values for the parameters. 
      this->itsWidth=1; 
      this->itsPrecision=0; 
      this->itsName=" "; 
      this->itsUnits=" ";
      this->itsType="";
      this->itsUCD = "";
      this->itsDatatype = "";
      this->itsExtraInfo="";
      this->itsColID="";
    }

    Column::Column(std::string type)
    {
      operator=(defaultColumn(type));
    }

    Column::Column(const Column& c)
    {
      operator=(c);
    }

    Column& Column::operator=(const Column& c)
    {
      if(this==&c) return *this;
      this->itsWidth = c.itsWidth;
      this->itsPrecision = c.itsPrecision;
      this->itsName = c.itsName;
      this->itsUnits = c.itsUnits;
      this->itsUCD = c.itsUCD;
      this->itsDatatype = c.itsDatatype;
      this->itsExtraInfo = c.itsExtraInfo;
      this->itsColID = c.itsColID;
      this->itsType = c.itsType;
      return *this;
    }

    Column::Column(std::string type, std::string name, std::string units, int width, int prec, std::string ucd, std::string datatype, std::string colID, std::string extraInfo):
      itsType(type), itsName(name), itsUnits(units), itsWidth(width), itsPrecision(prec), itsUCD(ucd), itsDatatype(datatype), itsColID(colID), itsExtraInfo(extraInfo)
    {
      /// A generic constructor designed to make a Column other than
      /// the predefined ones listed in the Column namespace
      /// \param name The title of the column
      /// \param units What units the column takes
      /// \param width The starting width
      /// \param prec The starting precision
      /// \param ucd The UCD relating to the quantity (VOTable use only)
      /// \param datatype The datatype (float/char/int/etc) of the quantity (VOTable)
      /// \param colID The column ID string used in the FIELD entry of a VOTable
      /// \param extraInfo Other information used in the VOTable (eg. a reference to a COOSYS)
   }

    //------------------------------------------------------------

    void Column::changePrec(int p)
    {
      /// A direct way to alter the precision without having to use
      /// Column::upPrec(). If the precision increases, the width will
      /// increase by the same amount. If it decreases, no change is
      /// made to the width.
      /// \param p The new precision value.

      if(p > this->itsPrecision) this->itsWidth += (p-this->itsPrecision);
      this->itsPrecision = p;

    }
    
    //------------------------------------------------------------
    void Column::printTitle(std::ostream &stream)
    {
      stream << std::setw(this->itsWidth) 
	     << std::setfill(' ') 
	     << this->itsName;
    }

    void Column::printUnits(std::ostream &stream)
    {
      stream << std::setw(this->itsWidth) 
	     << std::setfill(' ') 
	     << this->itsUnits;
    }
  
    void Column::printDash (std::ostream &stream)
    {
      stream << std::setw(this->itsWidth) 
	     << std::setfill('-')
	     << "" 
	     << std::setfill(' ');
    }

    void Column::printBlank(std::ostream &stream)
    {
      stream << std::setw(this->itsWidth) 
	     << std::setfill(' ') 
	     << "";
    }
  
    template <class T> void Column::printEntry(std::ostream &stream, T value)
    {
      ///  \param stream Where the printing is done.
      ///  \param value  The value to be printed.

      stream << std::setprecision(this->itsPrecision)
	     << std::setw(this->itsWidth) 
	     << std::setfill(' ')
	     << value;
    }
    template void Column::printEntry<int>(std::ostream &stream, int value);
    template void Column::printEntry<long>(std::ostream &stream, long value);
    template void Column::printEntry<unsigned int>(std::ostream &stream, unsigned int value);
    template void Column::printEntry<unsigned long>(std::ostream &stream, unsigned long value);
    template void Column::printEntry<float>(std::ostream &stream, float value);
    template void Column::printEntry<double>(std::ostream &stream, double value);
    template void Column::printEntry<std::string>(std::ostream &stream, std::string value);

 
    bool Column::doCol(DESTINATION tableType, bool flagFint)
    {
      ///  @details Determines whether a given column is used for a
      ///  given table type, based on the Column::type string.
      /// \param tableType The type of table: one of FILE, SCREEN, LOG, VOTABLE.
      /// \param flagFint Whether to use FINT (true) or FTOT
      /// (false). This defaults to true, so need not be given. It only
      /// applies to the screen and votable cases -- both are written for the
      /// results file case.
      /// \return True if column is used for given table type. False
      /// otherwise. False if tableType not one of four listed.
      
 
      const size_t sizeFileList=39;
      std::string FileList[sizeFileList]={"NUM","NAME","X","Y","Z","RA","DEC","VEL","MAJ","MIN","PA","WRA","WDEC",
					  "W50","W20","WVEL","FINT", "FINTERR", "FTOT", "FTOTERR", "FPEAK","SNRPEAK",
					  "X1","X2","Y1","Y2","Z1","Z2","NPIX","FLAG","XAV","YAV",
					  "ZAV","XCENT","YCENT","ZCENT","XPEAK","YPEAK","ZPEAK"};
      const size_t sizeScreenList=22;
      std::string ScreenList[sizeScreenList]={"NUM","NAME","X","Y","Z","RA","DEC","VEL","MAJ","MIN","PA",
					      "W50","FPEAK","SNRPEAK","X1","X2","Y1",
					      "Y2","Z1","Z2","NPIX","FLAG"};
      const size_t sizeLogList=16;
      std::string LogList[sizeLogList]={"NUM","X","Y","Z","FTOT","FPEAK","SNRPEAK",
					"X1","X2","Y1","Y2","Z1","Z2","NPIX","NUMCH","SPATSIZE"};
      const size_t sizeVOList=23;
      std::string VOList[sizeVOList]={"NUM","NAME","RAJD","DECJD","VEL","MAJ","MIN","PA",
				      "W50","W20","WVEL","FPEAK","SNRPEAK","FLAG","XAV","YAV",
				      "ZAV","XCENT","YCENT","ZCENT","XPEAK","YPEAK","ZPEAK"};

      bool doIt=false;
      if(tableType == FILE){
	for(size_t i=0;i<sizeFileList && !doIt;i++) doIt = doIt || (this->itsType==FileList[i]);
      }
      else if(tableType == SCREEN){
      	if(this->itsType=="FTOT") doIt = !flagFint;
	else if(this->itsType == "FINT") doIt = flagFint;
	else for(size_t i=0;i<sizeScreenList && !doIt;i++) doIt = doIt || (this->itsType==ScreenList[i]);
      }
      else if(tableType == LOG){
	for(size_t i=0;i<sizeLogList && !doIt;i++) doIt = doIt || (this->itsType==LogList[i]);
      }
      else if(tableType == VOTABLE){
     	if(this->itsType=="FTOT" || this->itsType=="FTOTERR") doIt = !flagFint;
	else if(this->itsType == "FINT" || this->itsType=="FINTERR") doIt = flagFint;
	else for(size_t i=0;i<sizeVOList && !doIt;i++) doIt = doIt || (this->itsType==VOList[i]);
      }
      return doIt;
	
    }

    Column defaultColumn(std::string type)
    {
      // type|name|units|width|prec|ucd|datatype|extraInfo
      if (type == "NUM") return Column(type,"ObjID","",6,0,"meta.id","int","col_objnum","");
      else if(type=="NAME") return Column(type,"Name","",8,0,"meta.id;meta.main","char","col_name","");
      else if(type=="X") return Column(type,"X","",6,prXYZ,"pos.cartesian.x","float","col_x","");
      else if(type=="Y") return Column(type,"Y","",6,prXYZ,"pos.cartesian.y","float","col_y","");
      else if(type=="Z") return Column(type,"Z","",6,prXYZ,"pos.cartesian.z","float","col_z","");
      else if(type=="RA") return Column(type,"RA","",11,0,"","char","col_ra",coordRef);
      else if(type=="DEC") return Column(type,"DEC","",10,0,"","char","col_dec",coordRef);
      else if(type=="RAJD") return Column(type,"RAJD","[deg]",11,prPOS,"","float","col_rajd",coordRef);
      else if(type=="DECJD") return Column(type,"DECJD","[deg]",11,prPOS,"","float","col_decjd",coordRef);
      else if(type=="VEL") return Column(type,"VEL","",7,prVEL,"","float","col_vel","");
      else if(type=="MAJ") return Column(type,"MAJ","[deg]",6,prWPOS,"phys.angSize.smajAxis","float","col_maj",coordRef);
      else if(type=="MIN") return Column(type,"MIN","[deg]",6,prWPOS,"phys.angSize.sminAxis","float","col_min",coordRef);
      else if(type=="PA") return Column(type,"PA","[deg]",7,prWPOS,"pos.posAng;phys.angSize","float","col_pa",coordRef);
      else if(type=="WRA") return Column(type,"w_RA","[arcmin]",9,prWPOS,"","float","col_wra",coordRef);
      else if(type=="WDEC") return Column(type,"w_DEC","[arcmin]",9,prWPOS,"","float","col_wdec",coordRef);
      else if(type=="W50") return Column(type,"w_50","",7,prVEL,"","float","col_w50","");
      else if(type=="W20") return Column(type,"w_20","",7,prVEL,"","float","col_w20","");
      else if(type=="WVEL") return Column(type,"w_VEL","",7,prVEL,"","float","col_wvel","");
      else if(type=="FINT") return Column(type,"F_int","",10,prFLUX,"phot.flux.density.integrated","float","col_fint","");
      else if(type=="FINTERR") return Column(type,"eF_int","",10,prFLUX,"phot.flux.density.integrated;stat.err","float","col_efint","");
      else if(type=="FTOT") return Column(type,"F_tot","",10,prFLUX,"phot.flux.density","float","col_ftot","");
      else if(type=="FTOTERR") return Column(type,"eF_tot","",10,prFLUX,"phot.flux.density;stat.err","float","col_eftot","");
      else if(type=="FPEAK") return Column(type,"F_peak","",9,prFLUX,"phot.flux;stat.max","float","col_fpeak","");
      else if(type=="SNRPEAK") return Column(type,"S/Nmax","",7,prSNR,"stat.snr;phot.flux","float","col_snrmax","");
      else if(type=="X1") return Column(type,"X1","",4,0,"pos.cartesian.x;stat.min","int","col_x1","");
      else if(type=="Y1") return Column(type,"Y1","",4,0,"pos.cartesian.y;stat.min","int","col_y1","");
      else if(type=="Z1") return Column(type,"Z1","",4,0,"pos.cartesian.z;stat.min","int","col_z1","");
      else if(type=="X2") return Column(type,"X2","",4,0,"pos.cartesian.x;stat.max","int","col_x2","");
      else if(type=="Y2") return Column(type,"Y2","",4,0,"pos.cartesian.y;stat.max","int","col_y2","");
      else if(type=="Z2") return Column(type,"Z2","",4,0,"pos.cartesian.z;stat.max","int","col_z2","");
      else if(type=="NPIX") return Column(type,"Npix","[pix]",6,0,"","int","col_npix","");
      else if(type=="FLAG") return Column(type,"Flag","",5,0,"meta.code.qual","char","col_flag","");
      else if(type=="XAV") return Column(type,"X_av","",6,prXYZ,"pos.cartesian.x;stat.mean","float","col_xav","");
      else if(type=="YAV") return Column(type,"Y_av","",6,prXYZ,"pos.cartesian.y;stat.mean","float","col_yav","");
      else if(type=="ZAV") return Column(type,"Z_av","",6,prXYZ,"pos.cartesian.z;stat.mean","float","col_zav","");
      else if(type=="XCENT") return Column(type,"X_cent","",7,prXYZ,"pos.cartesian.x;stat.centroid","float","col_xcent","");
      else if(type=="YCENT") return Column(type,"Y_cent","",7,prXYZ,"pos.cartesian.y;stat.centroid","float","col_ycent","");
      else if(type=="ZCENT") return Column(type,"Z_cent","",7,prXYZ,"pos.cartesian.z;stat.centroid","float","col_zcent","");
      else if(type=="XPEAK") return Column(type,"X_peak","",7,prXYZ,"pos.cartesian.x;phot.flux;stat.max","int","col_xpeak","");
      else if(type=="YPEAK") return Column(type,"Y_peak","",7,prXYZ,"pos.cartesian.y;phot.flux;stat.max","int","col_ypeak","");
      else if(type=="ZPEAK") return Column(type,"Z_peak","",7,prXYZ,"pos.cartesian.z;phot.flux;stat.max","int","col_zpeak","");
      else if(type=="NUMCH") return Column(type,"Nch","",6,0,"em.bin;stat.sum","int","col_nch","");
      else if(type=="SPATSIZE") return Column(type,"Sp_size","",8,0,"instr.pixel;stat.sum","int","col_spsize","");
      else {
	Column col;
	col.setType(type);
	return col;
      }

    }

    CatalogueSpecification getFullColSet(std::vector<Detection> &objectList, FitsHeader &head)
    {
      ///  @details A function that returns a catalogue specification
      ///  containing information on the columns necessary for output
      ///  to the results file:
      ///  Obj#,NAME,X,Y,Z,RA,DEC,VEL,w_RA,w_DEC,w_VEL,F_tot,F_int,F_peak,
      ///  X1,X2,Y1,Y2,Z1,Z2,Npix,Flag,
      ///  XAV,YAV,ZAV,XCENT,YCENT,ZCENT,XPEAK,YPEAK,ZPEAK
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
      /// \return A CatalogueSpecification

      CatalogueSpecification newset;

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
      newset.addColumn( Column("NUM") );
      newset.addColumn( Column("NAME") );
      newset.addColumn( Column("X") );
      newset.addColumn( Column("Y") );
      newset.addColumn( Column("Z") );
      newset.addColumn( Column("RA") );
      newset.addColumn( Column("DEC") );
      newset.addColumn( Column("RAJD") );
      newset.addColumn( Column("DECJD") );
      newset.addColumn( Column("VEL") );
      newset.addColumn( Column("MAJ") );
      newset.addColumn( Column("MIN") );
      newset.addColumn( Column("PA") );
      newset.addColumn( Column("WRA") );
      newset.addColumn( Column("WDEC") );
      newset.addColumn( Column("W50") );
      newset.addColumn( Column("W20") );
      newset.addColumn( Column("WVEL") );
      newset.addColumn( Column("FINT") );
      newset.addColumn( Column("FINTERR") );
      newset.addColumn( Column("FTOT") );
      newset.addColumn( Column("FTOTERR") );
      newset.addColumn( Column("FPEAK") );
      newset.addColumn( Column("SNRPEAK") );
      newset.addColumn( Column("X1") );
      newset.addColumn( Column("X2") );
      newset.addColumn( Column("Y1") );
      newset.addColumn( Column("Y2") );
      newset.addColumn( Column("Z1") );
      newset.addColumn( Column("Z2") );
      newset.addColumn( Column("NPIX") );
      newset.addColumn( Column("FLAG") );
      newset.addColumn( Column("XAV") );
      newset.addColumn( Column("YAV") );
      newset.addColumn( Column("ZAV") );
      newset.addColumn( Column("XCENT") );
      newset.addColumn( Column("YCENT") );
      newset.addColumn( Column("ZCENT") );
      newset.addColumn( Column("XPEAK") );
      newset.addColumn( Column("YPEAK") );
      newset.addColumn( Column("ZPEAK") );
      newset.addColumn( Column("NUMCH") );
      newset.addColumn( Column("SPATSIZE") );


      // Now test each object against each new column
      std::vector<Detection>::iterator obj;
      for(obj = objectList.begin(); obj < objectList.end(); obj++){
	std::string tempstr;
	int tempwidth;
	float val,minval;
	double valD,minvalD;

	// Obj#
	tempwidth = int( log10(obj->getID()) + 1) + 1;
	for(int i=newset.column("NUM").getWidth();i<tempwidth;i++) newset.column("NUM").widen();

	// Name
	tempwidth = obj->getName().size() + 1;
	for(int i=newset.column("NAME").getWidth();i<tempwidth;i++) newset.column("NAME").widen();

	// X position
	val = obj->getXcentre() + obj->getXOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset.column("X").getPrecision()+1)); 
	  if(val < minval) newset.column("X").upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset.column("X").getPrecision() + 2;
	for(int i=newset.column("X").getWidth();i<tempwidth;i++) newset.column("X").widen();
	// Y position
	val = obj->getYcentre() + obj->getYOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset.column("Y").getPrecision()+1)); 
	  if(val < minval) newset.column("Y").upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset.column("Y").getPrecision() + 2;
	for(int i=newset.column("Y").getWidth();i<tempwidth;i++) newset.column("Y").widen();
	// Z position
	val = obj->getZcentre() + obj->getZOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset.column("Z").getPrecision()+1)); 
	  if((val>0.)&&(val < minval)) newset.column("Z").upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset.column("Z").getPrecision() + 2;
	for(int i=newset.column("Z").getWidth();i<tempwidth;i++) newset.column("Z").widen();

	if(head.isWCS()){  
	  // RA -- assign correct title. Check width but should be ok by definition
	  tempstr = head.lngtype();
	  newset.column("RA").setName(tempstr);
	  tempwidth = obj->getRAs().size() + 1;
	  for(int i=newset.column("RA").getWidth();i<tempwidth;i++) newset.column("RA").widen();
      
	  // Dec -- assign correct title. Check width, should be ok by definition
	  tempstr = head.lattype();
	  newset.column("DEC").setName(tempstr);
	  tempwidth = obj->getDecs().size() + 1;
	  for(int i=newset.column("DEC").getWidth();i<tempwidth;i++) newset.column("DEC").widen();

	  // RA decimal degrees -- assign correct title. Check width but should be OK
	  tempstr = head.lngtype();
	  newset.column("RAJD").setName(tempstr);
	  valD = obj->getRA();
	  tempwidth = int( log10(fabs(valD)) + 1) + newset.column("RAJD").getPrecision() + 2;
	  for(int i=newset.column("RAJD").getWidth();i<tempwidth;i++) newset.column("RAJD").widen();
      
	  // Dec decimal degrees -- assign correct title. Check width but should be OK
	  tempstr = head.lattype();
	  newset.column("DECJD").setName(tempstr);
	  valD = obj->getDec();
	  tempwidth = int( log10(fabs(valD)) + 1) + newset.column("DECJD").getPrecision() + 2;
	  for(int i=newset.column("DECJD").getWidth();i<tempwidth;i++) newset.column("DECJD").widen();

	  // Vel -- check width, title and units.
	  if(head.canUseThirdAxis()){
	    if(head.WCS().spec < 0)  // if it's not a spectral axis
	      newset.column("VEL").setName( head.WCS().ctype[2] );
	    else 
	      newset.column("VEL").setName(head.getSpectralType());
	    tempwidth = newset.column("VEL").getName().size() + 1;
	    for(int i=newset.column("VEL").getWidth();i<tempwidth;i++) newset.column("VEL").widen();
	    if(head.getSpectralUnits().size()>0)
	      newset.column("VEL").setUnits("[" + head.getSpectralUnits() + "]");
	    tempwidth = newset.column("VEL").getUnits().size() + 1;
	    for(int i=newset.column("VEL").getWidth();i<tempwidth;i++) newset.column("VEL").widen();
	
	    valD = obj->getVel();
	    if((fabs(valD) < 1.)&&(valD>0.)){
	      minvalD = pow(10, -1. * (newset.column("VEL").getPrecision()+1)); 
	      if(valD < minvalD) newset.column("VEL").upPrec();
	    }
	    tempwidth = int(log10(fabs(valD)) + 1) + newset.column("VEL").getPrecision() + 2;
	    if(valD<0) tempwidth++;
	    for(int i=newset.column("VEL").getWidth();i<tempwidth;i++) newset.column("VEL").widen();
	  }

	  // MAJ -- check width & title. leave units for the moment.
	  newset.column("MAJ").setUnits("["+head.getShapeUnits()+"]");
	  tempwidth = newset.column("MAJ").getUnits().size() + 1;
	  for(int i=newset.column("MAJ").getWidth();i<tempwidth;i++) newset.column("MAJ").widen();
	  valD = obj->getMajorAxis();
	  if((fabs(valD) < 1.)&&(valD>0.)){
	    minvalD = pow(10, -1. * (newset.column("MAJ").getPrecision())); 
	    if(valD < minvalD) newset.column("MAJ").upPrec();
	  }
	  tempwidth = int( log10(fabs(valD)) + 1) + newset.column("MAJ").getPrecision() + 2;
	  if(valD<0) tempwidth++;
	  for(int i=newset.column("MAJ").getWidth();i<tempwidth;i++) newset.column("MAJ").widen();

	  // MIN -- check width & title. leave units for the moment.
	  newset.column("MIN").setUnits("["+head.getShapeUnits()+"]");
	  tempwidth = newset.column("MIN").getUnits().size() + 1;
	  for(int i=newset.column("MIN").getWidth();i<tempwidth;i++) newset.column("MIN").widen();
	  valD = obj->getMinorAxis();
	  if((fabs(valD) < 1.)&&(valD>0.)){
	    minvalD = pow(10, -1. * (newset.column("MIN").getPrecision())); 
	    if(valD < minvalD) newset.column("MIN").upPrec();
	  }
	  tempwidth = int( log10(fabs(valD)) + 1) + newset.column("MIN").getPrecision() + 2;
	  if(valD<0) tempwidth++;
	  for(int i=newset.column("MIN").getWidth();i<tempwidth;i++) newset.column("MIN").widen();

	  // PA -- check width & title. leave units for the moment.
	  tempwidth = newset.column("PA").getUnits().size() + 1;
	  for(int i=newset.column("PA").getWidth();i<tempwidth;i++) newset.column("PA").widen();
	  valD = obj->getPositionAngle();
	  if((fabs(valD) < 1.)&&(valD>0.)){
	    minvalD = pow(10, -1. * (newset.column("PA").getPrecision()+1)); 
	    if(valD < minvalD) newset.column("PA").upPrec();
	  }
	  tempwidth = int( log10(fabs(valD)) + 1) + newset.column("PA").getPrecision() + 2;
	  if(valD<0) tempwidth++;
	  for(int i=newset.column("PA").getWidth();i<tempwidth;i++) newset.column("PA").widen();

	  // w_RA -- check width & title. leave units for the moment.
	  tempstr = "w_" + head.lngtype();
	  newset.column("WRA").setName(tempstr);
	  tempwidth = newset.column("RA").getUnits().size() + 1;
	  for(int i=newset.column("WRA").getWidth();i<tempwidth;i++) newset.column("WRA").widen();
	  valD = obj->getRAWidth();
	  if((fabs(valD) < 1.)&&(valD>0.)){
	    minvalD = pow(10, -1. * (newset.column("WRA").getPrecision()+1)); 
	    if(valD < minvalD) newset.column("WRA").upPrec();
	  }
	  tempwidth = int( log10(fabs(valD)) + 1) + newset.column("WRA").getPrecision() + 2;
	  if(valD<0) tempwidth++;
	  for(int i=newset.column("WRA").getWidth();i<tempwidth;i++) newset.column("WRA").widen();

	  // w_DEC -- check width & title. leave units for the moment.
	  tempstr = "w_" + head.lattype();
	  newset.column("WDEC").setName(tempstr);
	  tempwidth = newset.column("DEC").getUnits().size() + 1;
	  for(int i=newset.column("WDEC").getWidth();i<tempwidth;i++) newset.column("WDEC").widen();
	  valD = obj->getDecWidth();
	  if((fabs(valD) < 1.)&&(valD>0.)){
	    minvalD = pow(10, -1. * (newset.column("WDEC").getPrecision()+1)); 
	    if(valD < minvalD) newset.column("WDEC").upPrec();
	  }
	  tempwidth = int( log10(fabs(valD)) + 1) + newset.column("WDEC").getPrecision() + 2;
	  if(valD<0) tempwidth++;
	  for(int i=newset.column("WDEC").getWidth();i<tempwidth;i++) newset.column("WDEC").widen();

	  // w_50 -- check width, title and units.
	  if(head.canUseThirdAxis()){
	    if(head.getSpectralUnits().size()>0)
	      newset.column("W50").setUnits("[" + head.getSpectralUnits() + "]");
	    tempwidth = newset.column("W50").getUnits().size() + 1;
	    for(int i=newset.column("W50").getWidth();i<tempwidth;i++) newset.column("W50").widen();
	    valD = obj->getW50();
	    if((fabs(valD) < 1.)&&(valD>0.)){
	      minvalD = pow(10, -1. * (newset.column("W50").getPrecision()+1)); 
	      if(valD < minvalD) newset.column("W50").upPrec();
	    }
	    tempwidth = int( log10(fabs(valD)) + 1) + newset.column("W50").getPrecision() + 2;
	    if(valD<0) tempwidth++;
	    for(int i=newset.column("W50").getWidth();i<tempwidth;i++) newset.column("W50").widen();
	  }

	  // w_20 -- check width, title and units.
	  if(head.canUseThirdAxis()){
	    if(head.getSpectralUnits().size()>0)
	      newset.column("W20").setUnits("[" + head.getSpectralUnits() + "]");
	    tempwidth = newset.column("W20").getUnits().size() + 1;
	    for(int i=newset.column("W20").getWidth();i<tempwidth;i++)newset.column("W20").widen();
	    valD = obj->getW20();
	    if((fabs(valD) < 1.)&&(valD>0.)){
	      minvalD = pow(10, -1. * (newset.column("W20").getPrecision()+1)); 
	      if(valD < minvalD) newset.column("W20").upPrec();
	    }
	    tempwidth = int( log10(fabs(valD)) + 1) + newset.column("W20").getPrecision() + 2;
	    if(valD<0) tempwidth++;
	    for(int i=newset.column("W20").getWidth();i<tempwidth;i++) newset.column("W20").widen();
	  }

	  // w_Vel -- check width, title and units.
	  if(head.canUseThirdAxis()){
	    if(head.WCS().spec < 0) // if it's not a spectral axis
	      newset.column("WVEL").setName( std::string("w_") + head.WCS().ctype[2] );
	    else
	      newset.column("WVEL").setName(std::string("w_")+head.getSpectralType());
	    if(head.getSpectralUnits().size()>0)
	      newset.column("WVEL").setUnits("[" + head.getSpectralUnits() + "]");
	    tempwidth = newset.column("WVEL").getUnits().size() + 1;
	    for(int i=newset.column("WVEL").getWidth();i<tempwidth;i++)newset.column("WVEL").widen();
	    tempwidth = newset.column("WVEL").getName().size() + 1;
	    for(int i=newset.column("WVEL").getWidth();i<tempwidth;i++) newset.column("WVEL").widen();
	    valD = obj->getVelWidth();
	    if((fabs(valD) < 1.)&&(valD>0.)){
	      minvalD = pow(10, -1. * (newset.column("WVEL").getPrecision()+1)); 
	      if(valD < minvalD) newset.column("WVEL").upPrec();
	    }
	    tempwidth = int( log10(fabs(valD)) + 1) + newset.column("WVEL").getPrecision() + 2;
	    if(valD<0) tempwidth++;
	    for(int i=newset.column("WVEL").getWidth();i<tempwidth;i++) newset.column("WVEL").widen();
	  }

	  // F_int -- check width & units
	  if(head.getIntFluxUnits().size()>0)
	    newset.column("FINT").setUnits("[" + head.getIntFluxUnits() + "]");
	  tempwidth = newset.column("FINT").getUnits().size() + 1;
	  for(int i=newset.column("FINT").getWidth();i<tempwidth;i++) newset.column("FINT").widen();
	  valD = obj->getIntegFlux();
	  if((fabs(valD) < 1.)// &&(valD>0.)
	     ){
	    int minprec = int(fabs(log10(fabs(valD))))+2;
	    for(int i=newset.column("FINT").getPrecision();i<minprec;i++) newset.column("FINT").upPrec();
	  }
	  tempwidth = int( log10(fabs(valD)) + 1) + newset.column("FINT").getPrecision() + 2;
	  if(valD<0) tempwidth++;
	  for(int i=newset.column("FINT").getWidth();i<tempwidth;i++) newset.column("FINT").widen();
      
	  // eF_int -- check width & units
	  if(obj->getIntegFluxError()>0.){
	      if(head.getIntFluxUnits().size()>0)
		  newset.column("FINTERR").setUnits("[" + head.getIntFluxUnits() + "]");
	      tempwidth = newset.column("FINTERR").getUnits().size() + 1;
	      for(int i=newset.column("FINTERR").getWidth();i<tempwidth;i++) newset.column("FINTERR").widen();
	      valD = obj->getIntegFluxError();
	      if((fabs(valD) < 1.)// &&(valD>0.)
		  ){
		  int minprec = int(fabs(log10(fabs(valD))))+2;
		  for(int i=newset.column("FINTERR").getPrecision();i<minprec;i++) newset.column("FINTERR").upPrec();
	      }
	      tempwidth = int( log10(fabs(valD)) + 1) + newset.column("FINTERR").getPrecision() + 2;
	      if(valD<0) tempwidth++;
	      for(int i=newset.column("FINTERR").getWidth();i<tempwidth;i++) newset.column("FINTERR").widen();
	  }

	}

	// F_tot
	newset.column("FTOT").setUnits("[" + head.getFluxUnits() + "]");
	tempwidth = newset.column("FTOT").getUnits().size() + 1;
	for(int i=newset.column("FTOT").getWidth();i<tempwidth;i++) newset.column("FTOT").widen();
	val = obj->getTotalFlux();
	//     std::cerr << val << "\n";
	if((fabs(val) < 1.)// &&(val>0.)
	   ){
	  int minprec = int(fabs(log10(fabs(val))))+2;
	  for(int i=newset.column("FTOT").getPrecision();i<minprec;i++) newset.column("FTOT").upPrec();
	}
	tempwidth = int( log10(fabs(val)) + 1) + newset.column("FTOT").getPrecision() + 2;
	if(val<0) tempwidth++;
	for(int i=newset.column("FTOT").getWidth();i<tempwidth;i++) newset.column("FTOT").widen();

	// eF_tot -- check width & units
	if(obj->getIntegFluxError()>0.){
	    newset.column("FTOTERR").setUnits("[" + head.getFluxUnits() + "]");
	    tempwidth = newset.column("FTOTERR").getUnits().size() + 1;
	    for(int i=newset.column("FTOTERR").getWidth();i<tempwidth;i++) newset.column("FTOTERR").widen();
	    valD = obj->getTotalFluxError();
	    if((fabs(valD) < 1.)// &&(valD>0.)
		){
		int minprec = int(fabs(log10(fabs(valD))))+2;
		for(int i=newset.column("FTOTERR").getPrecision();i<minprec;i++) newset.column("FTOTERR").upPrec();
	      }
	    tempwidth = int( log10(fabs(valD)) + 1) + newset.column("FTOTERR").getPrecision() + 2;
	    if(valD<0) tempwidth++;
	    for(int i=newset.column("FTOTERR").getWidth();i<tempwidth;i++) newset.column("FTOTERR").widen();
	}

	// F_peak
	newset.column("FPEAK").setUnits("[" + head.getFluxUnits() + "]");
	tempwidth = newset.column("FPEAK").getUnits().size() + 1;
	for(int i=newset.column("FPEAK").getWidth();i<tempwidth;i++) newset.column("FPEAK").widen();
	val = obj->getPeakFlux();
	if((fabs(val) < 1.)// &&(val>0.)
	   ){
	  int minprec = int(fabs(log10(fabs(val))))+2;
	  for(int i=newset.column("FPEAK").getPrecision();i<minprec;i++) newset.column("FPEAK").upPrec();
	}
	tempwidth = int( log10(fabs(val)) + 1) + newset.column("FPEAK").getPrecision() + 2;
	if(val<0) tempwidth++;
	for(int i=newset.column("FPEAK").getWidth();i<tempwidth;i++) newset.column("FPEAK").widen();

	// S/N_peak
	if(obj->getPeakSNR()>0.){
	    val = obj->getPeakSNR();
	    if((fabs(val) < 1.)&&(val>0.)){
		minval = pow(10, -1. * (newset.column("SNRPEAK").getPrecision()+1)); 
		if(val < minval) newset.column("SNRPEAK").upPrec();
	    }
	    tempwidth = int( log10(fabs(val)) + 1) + newset.column("SNRPEAK").getPrecision() +2;
	    if(val<0) tempwidth++;
	    for(int i=newset.column("SNRPEAK").getWidth();i<tempwidth;i++) newset.column("SNRPEAK").widen();
	}

	// X1 position
	val = obj->getXmin() + obj->getXOffset();
	tempwidth = int( log10(val) + 1) + newset.column("X1").getPrecision() + 1;
	for(int i=newset.column("X1").getWidth();i<tempwidth;i++) newset.column("X1").widen();
	// X2 position
	val = obj->getXmax() + obj->getXOffset();
	tempwidth = int( log10(val) + 1) + newset.column("X2").getPrecision() + 1;
	for(int i=newset.column("X2").getWidth();i<tempwidth;i++) newset.column("X2").widen();
	// Y1 position
	val = obj->getYmin() + obj->getYOffset();
	tempwidth = int( log10(val) + 1) + newset.column("Y1").getPrecision() + 1;
	for(int i=newset.column("Y1").getWidth();i<tempwidth;i++) newset.column("Y1").widen();
	// Y2 position
	val = obj->getYmax() + obj->getYOffset();
	tempwidth = int( log10(val) + 1) + newset.column("Y2").getPrecision() + 1;
	for(int i=newset.column("Y2").getWidth();i<tempwidth;i++) newset.column("Y2").widen();
	// Z1 position
	val = obj->getZmin() + obj->getZOffset();
	tempwidth = int( log10(val) + 1) + newset.column("Z1").getPrecision() + 1;
	for(int i=newset.column("Z1").getWidth();i<tempwidth;i++) newset.column("Z1").widen();
	// Z2 position
	val = obj->getZmax() + obj->getZOffset();
	tempwidth = int( log10(val) + 1) + newset.column("Z2").getPrecision() + 1;
	for(int i=newset.column("Z2").getWidth();i<tempwidth;i++) newset.column("Z2").widen();

	// Npix
	tempwidth = int( log10(obj->getSize()) + 1) + 
	  newset.column("NPIX").getPrecision() + 1;
	for(int i=newset.column("NPIX").getWidth();i<tempwidth;i++) newset.column("NPIX").widen();
    
	// average X position
	val = obj->getXaverage() + obj->getXOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset.column("XAV").getPrecision()+1)); 
	  if(val < minval) newset.column("XAV").upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset.column("XAV").getPrecision() + 2;
	for(int i=newset.column("XAV").getWidth();i<tempwidth;i++) newset.column("XAV").widen();
	// average Y position
	val = obj->getYaverage() + obj->getYOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset.column("YAV").getPrecision()+1)); 
	  if(val < minval) newset.column("YAV").upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset.column("YAV").getPrecision() + 2;
	for(int i=newset.column("YAV").getWidth();i<tempwidth;i++) newset.column("YAV").widen();
	// average Z position
	val = obj->getZaverage() + obj->getZOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset.column("ZAV").getPrecision()+1)); 
	  if((val>0.)&&(val < minval)) newset.column("ZAV").upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset.column("ZAV").getPrecision() + 2;
	for(int i=newset.column("ZAV").getWidth();i<tempwidth;i++) newset.column("ZAV").widen();
    
	// X position of centroid
	val = obj->getXCentroid() + obj->getXOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset.column("XCENT").getPrecision()+1)); 
	  if(val < minval) newset.column("XCENT").upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset.column("XCENT").getPrecision() + 2;
	for(int i=newset.column("XCENT").getWidth();i<tempwidth;i++) newset.column("XCENT").widen();
	// Y position of centroid
	val = obj->getYCentroid() + obj->getYOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset.column("YCENT").getPrecision()+1)); 
	  if(val < minval) newset.column("YCENT").upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset.column("YCENT").getPrecision() + 2;
	for(int i=newset.column("YCENT").getWidth();i<tempwidth;i++) newset.column("YCENT").widen();
	// Z position of centroid
	val = obj->getZCentroid() + obj->getZOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset.column("ZCENT").getPrecision()+1)); 
	  if((val>0.)&&(val < minval)) newset.column("ZCENT").upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset.column("ZCENT").getPrecision() + 2;
	for(int i=newset.column("ZCENT").getWidth();i<tempwidth;i++) newset.column("ZCENT").widen();
    
	// X position of peak flux
	val = obj->getXPeak() + obj->getXOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset.column("XPEAK").getPrecision()+1)); 
	  if(val < minval) newset.column("XPEAK").upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset.column("XPEAK").getPrecision() + 2;
	for(int i=newset.column("XPEAK").getWidth();i<tempwidth;i++) newset.column("XPEAK").widen();
	// Y position of peak flux
	val = obj->getYPeak() + obj->getYOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset.column("YPEAK").getPrecision()+1)); 
	  if(val < minval) newset.column("YPEAK").upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset.column("YPEAK").getPrecision() + 2;
	for(int i=newset.column("YPEAK").getWidth();i<tempwidth;i++) newset.column("YPEAK").widen();
	// Z position of peak flux
	val = obj->getZPeak() + obj->getZOffset();
	if((val<1.)&&(val>0.)){
	  minval = pow(10, -1. * (newset.column("ZPEAK").getPrecision()+1)); 
	  if((val>0.)&&(val < minval)) newset.column("ZPEAK").upPrec();
	}
	tempwidth = int( log10(val) + 1) + newset.column("ZPEAK").getPrecision() + 2;
	for(int i=newset.column("ZPEAK").getWidth();i<tempwidth;i++) newset.column("ZPEAK").widen();

	//Number of contiguous channels
	tempwidth = int( log10(obj->getMaxAdjacentChannels()) + 1) + 
	  newset.column("NUMCH").getPrecision() + 1;
	for(int i=newset.column("NUMCH").getWidth();i<tempwidth;i++) newset.column("NUMCH").widen();
	//Spatial size
	tempwidth = int( log10(obj->getSpatialSize()) + 1) + 
	  newset.column("SPATSIZE").getPrecision() + 1;
	for(int i=newset.column("SPATSIZE").getWidth();i<tempwidth;i++) newset.column("SPATSIZE").widen();
	
      }
      
      return newset;

    }

  }

}
