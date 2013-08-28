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

      void Column::checkPrec(double val)
      {
	  /// Checks a decimal value to see if it is in -1<val<1. If
	  /// so, we need sufficient precision to represent at least
	  /// one significant figure.
	  if((fabs(val)<1.)){
	      int impliedPrec = int(fabs(log10(val))+1);
	      for(int i=this->itsPrecision;i<=impliedPrec;i++) this->upPrec();
	  }
      }

      void Column::checkWidth(int width)
      {
	  /// Three checks on the width, looking at the name, the
	  /// units string, and then some minimum width. This can be
	  /// obtained from the other check() functions that work
	  /// from various value types.

	  for(int i=this->itsWidth;i<=width;i++) this->widen();// +1 for the space
	  for(int i=this->itsWidth;i<=int(this->itsName.size());i++) this->widen();  // +1 for the space
	  for(int i=this->itsWidth;i<=int(this->itsUnits.size());i++) this->widen(); // +1 for the space 

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
      
 
      const size_t sizeFileList=43;
      std::string FileList[sizeFileList]={"NUM","NAME","X","Y","Z","RA","DEC","RAJD","DECJD","VEL","MAJ","MIN","PA","WRA","WDEC",
					  "W50","W20","WVEL","FINT","FINTERR","FTOT","FTOTERR","FPEAK","SNRPEAK",
					  "X1","X2","Y1","Y2","Z1","Z2","NVOX","NUMCH","SPATSIZE","FLAG","XAV","YAV",
					  "ZAV","XCENT","YCENT","ZCENT","XPEAK","YPEAK","ZPEAK"};
      const size_t sizeScreenList=24;
      std::string ScreenList[sizeScreenList]={"NUM","NAME","X","Y","Z","RA","DEC","VEL","MAJ","MIN","PA",
					      "W50","FPEAK","SNRPEAK","X1","X2","Y1",
					      "Y2","Z1","Z2","NVOX","NUMCH","SPATSIZE","FLAG"};
      const size_t sizeLogList=16;
      std::string LogList[sizeLogList]={"NUM","X","Y","Z","FTOT","FPEAK","SNRPEAK",
					"X1","X2","Y1","Y2","Z1","Z2","NVOX","NUMCH","SPATSIZE"};
      // const size_t sizeVOList=23;
      // std::string VOList[sizeVOList]={"NUM","NAME","RAJD","DECJD","VEL","MAJ","MIN","PA",
      // 				      "W50","W20","WVEL","FPEAK","SNRPEAK","FLAG","XAV","YAV",
      // 				      "ZAV","XCENT","YCENT","ZCENT","XPEAK","YPEAK","ZPEAK"};

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
	// else for(size_t i=0;i<sizeVOList && !doIt;i++) doIt = doIt || (this->itsType==VOList[i]);
	else{
	    for(size_t i=0;i<sizeFileList && !doIt;i++){
		if(FileList[i]!="RA" && FileList[i]!="DEC")
		    doIt = doIt || (this->itsType==FileList[i]);
	    }
	}
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
      else if(type=="NVOX") return Column(type,"Nvoxel","",6,0,"phys.size;instr.pixel;em.bin","int","col_nvox","");
      else if(type=="NUMCH") return Column(type,"Nchan","",6,0,"spect.line.width.full;em.bin","int","col_nch","");
      else if(type=="SPATSIZE") return Column(type,"Nspatpix","",8,0,"phys.angArea;instr.pixel","int","col_spsize","");
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
      ///  X1,X2,Y1,Y2,Z1,Z2,Nvox,Flag,
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
      newset.addColumn( Column("NVOX") );
      newset.addColumn( Column("NUMCH") );
      newset.addColumn( Column("SPATSIZE") );
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
      
      // Define the column names and units where not predifined by the
      // default settings (ie. for those columns that depend on the
      // WCS or image units)
      if(head.isWCS()){
	  newset.column("RA").setName(head.lngtype());
	  newset.column("DEC").setName(head.lattype());
	  newset.column("RAJD").setName(head.lngtype());
	  newset.column("DECJD").setName(head.lattype());
	  if(head.canUseThirdAxis()){
	      if(head.WCS().spec < 0)  // if it's not a spectral axis
		  newset.column("VEL").setName( head.WCS().ctype[2] );
	      else 
		  newset.column("VEL").setName(head.getSpectralType());
	      if(head.getSpectralUnits().size()>0)
		  newset.column("VEL").setUnits("[" + head.getSpectralUnits() + "]");
	  }
	  newset.column("MAJ").setUnits("["+head.getShapeUnits()+"]");
	  newset.column("MIN").setUnits("["+head.getShapeUnits()+"]");
	  newset.column("WRA").setName("w_"+head.lngtype());
	  newset.column("WDEC").setName("w_"+head.lattype());
	  if(head.canUseThirdAxis()){
	      if(head.getSpectralUnits().size()>0){
		  newset.column("W50").setUnits("[" + head.getSpectralUnits() + "]");
		  newset.column("W20").setUnits("[" + head.getSpectralUnits() + "]");
		  newset.column("WVEL").setUnits("[" + head.getSpectralUnits() + "]");
	      }
	      if(head.WCS().spec < 0) // if it's not a spectral axis
		  newset.column("WVEL").setName( std::string("w_") + head.WCS().ctype[2] );
	      else
		  newset.column("WVEL").setName(std::string("w_")+head.getSpectralType());
	  }
      }
      if(head.getIntFluxUnits().size()>0){
	  newset.column("FINT").setUnits("[" + head.getIntFluxUnits() + "]");
	  newset.column("FINTERR").setUnits("[" + head.getIntFluxUnits() + "]");
      }
      newset.column("FTOT").setUnits("[" + head.getFluxUnits() + "]");
      newset.column("FTOTERR").setUnits("[" + head.getFluxUnits() + "]");
      newset.column("FPEAK").setUnits("[" + head.getFluxUnits() + "]");
      

      // Now test each object against each new column, ensuring each
      // column has sufficient width and (in most cases) precision to
      // accomodate the data.
      std::vector<Detection>::iterator obj;
      for(obj = objectList.begin(); obj < objectList.end(); obj++){

	newset.column("NUM").check(obj->getID());
	newset.column("NAME").check(obj->getName());
	newset.column("X").check(obj->getXcentre()+obj->getXOffset());
	newset.column("Y").check(obj->getYcentre()+obj->getYOffset());
	newset.column("Z").check(obj->getZcentre()+obj->getZOffset());
	if(head.isWCS()){
	    newset.column("RA").check(obj->getRAs());
	    newset.column("DEC").check(obj->getDecs());
	    newset.column("RAJD").check(obj->getRA());
	    newset.column("DECJD").check(obj->getDec());
	    if(head.canUseThirdAxis()){
		newset.column("VEL").check(obj->getVel());
	    }
	    newset.column("MAJ").check(obj->getMajorAxis());
	    newset.column("MIN").check(obj->getMinorAxis());
	    // For the PA column, we don't increase the precision. If
	    // something is very close to zero position angle, then
	    // we're happy to call it zero.
	    newset.column("PA").check(obj->getPositionAngle(),false);
	    newset.column("WRA").check(obj->getRAWidth());
	    newset.column("WDEC").check(obj->getDecWidth());
	    if(head.canUseThirdAxis()){
		newset.column("W50").check(obj->getW50());
		newset.column("W20").check(obj->getW20());
		newset.column("WVEL").check(obj->getVelWidth());
	    }
	    
	    newset.column("FINT").check(obj->getIntegFlux());
	    if(obj->getIntegFluxError()>0.)
		newset.column("FINTERR").check(obj->getIntegFluxError());
	}
	newset.column("FTOT").check(obj->getTotalFlux());
	if(obj->getTotalFluxError()>0.)
	    newset.column("FTOTERR").check(obj->getTotalFluxError());
	newset.column("FPEAK").check(obj->getPeakFlux());
	if(obj->getPeakSNR()>0.)
	    newset.column("SNRPEAK").check(obj->getPeakSNR());
	newset.column("X1").check(obj->getXmin()+obj->getXOffset());
	newset.column("X2").check(obj->getXmax()+obj->getXOffset());
	newset.column("Y1").check(obj->getYmin()+obj->getYOffset());
	newset.column("Y2").check(obj->getYmax()+obj->getYOffset());
	newset.column("Z1").check(obj->getZmin()+obj->getZOffset());
	newset.column("Z2").check(obj->getZmax()+obj->getZOffset());
	newset.column("NVOX").check(obj->getSize());
	newset.column("XAV").check(obj->getXaverage()+obj->getXOffset());
	newset.column("YAV").check(obj->getYaverage()+obj->getYOffset());
	newset.column("ZAV").check(obj->getZaverage()+obj->getZOffset());
	newset.column("XCENTROID").check(obj->getXCentroid()+obj->getXOffset());
	newset.column("YCENTROID").check(obj->getYCentroid()+obj->getYOffset());
	newset.column("ZCENTROID").check(obj->getZCentroid()+obj->getZOffset());
	newset.column("XPEAK").check(obj->getXPeak()+obj->getXOffset());
	newset.column("YPEAK").check(obj->getYPeak()+obj->getYOffset());
	newset.column("ZPEAK").check(obj->getZPeak()+obj->getZOffset());
	newset.column("NUMCH").check(obj->getMaxAdjacentChannels());
	newset.column("SPATSIZE").check(obj->getSpatialSize());

      }
	  
      return newset;
	  
    }

  }

}
