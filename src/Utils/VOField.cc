// -----------------------------------------------------------------------
// VOField.cc: Specification of a field for output to a VOTable
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
#include <iomanip>
#include <string>
#include <duchamp/Utils/VOField.hh>
#include <duchamp/Detection/columns.hh>

namespace duchamp
{


  std::string fixUnitsVOT(std::string oldstring)
  {
    ///  @details
    /// Fix a string containing units to make it acceptable for a VOTable.
    /// 
    /// This function makes the provided units string acceptable
    /// according to the standard found at
    /// http://vizier.u-strasbg.fr/doc/catstd-3.2.htx 
    /// This should then be able to convert the units used in the text
    /// table to units suitable for putting in a VOTable.
    /// 
    /// Specifically, it removes any square brackets [] from the
    /// start/end of the string, and replaces blank spaces (representing
    /// multiplication) with a '.' (full stop).
    /// 
    /// \param oldstring Input unit string to be fixed
    /// \return String with fixed units

    std::string newstring;
    for(size_t i=0;i<oldstring.size();i++){
      if((oldstring[i]!='[')&&(oldstring[i]!=']')){
	if(oldstring[i]==' ') newstring += '.';
	else newstring += oldstring[i];
      }
    }
    return newstring;  
  }

  VOField::VOField()
  {
  }

  VOField::VOField(const VOField& other)
  {
    operator=(other);
  }

  VOField& VOField::operator= (const VOField& other)
  {
    if(this==&other) return *this;
    this->ID = other.ID;
    this->name = other.name;
    this->UCD = other.UCD;
    this->units = other.units;
    this->datatype=other.datatype;
    this->ref = other.ref;
    this->width = other.width;
    this->precision = other.precision;
    return *this;
  }


  void VOField::define(std::string i, std::string n, std::string U, std::string u, std::string d, std::string r, int w, int p)
  {
    /// @details
    /// A basic definition function, defining each parameter individually. 
    /// \param i The ID parameter
    /// \param n The name parameter
    /// \param U The UCD
    /// \param u The units (fixed by fixUnits())
    /// \param d The datatype
    /// \param r The ref
    /// \param w The width
    /// \param p The precision

    this->ID = i;
    this->name = n;
    this->UCD = U;
    this->units = fixUnitsVOT(u);
    this->datatype = d;
    this->ref = r;
    this->width = w;
    this->precision = p;
  }

  void VOField::define(Column::Col column, std::string i, std::string U, std::string d, std::string r)
  {
    /// @details
    /// Another definition function, using the information in a Column::Col object for some parameters.
    /// \param column A Column::Col object, used for name, units, width & precision
    /// \param i The ID parameter
    /// \param U The UCD
    /// \param d The datatype
    /// \param r The ref

    this->ID = i;
    this->name = column.getName();
    this->UCD = U;
    this->units = fixUnitsVOT(column.getUnits());
    this->datatype = d;
    this->ref = r;
    this->width = column.getWidth();
    this->precision = column.getPrecision();
  }

  VOField::VOField(Column::Col column)
  {
    /// @details
    /// A more useful definition function, using the Column::COLNAME
    /// key to specify particular values for each of the parameters for
    /// different columns.
    /// \param column A Column::Col object of a particular type. The
    /// column.getType() function is used to decide which call to
    /// VOField::define(Column::Col column, std::string i, std::string
    /// U, std::string d, std::string r) to use

    switch(column.getType())
      {
      case Column::NUM:
	this->define(column,"col01","meta.id","int","");
	break;
      case Column::NAME:
	this->define(column,"col02","meta.id;meta.main","char","");
	break;
      case Column::RAJD:
	this->define(column,"col03","","float","J2000");
	break;
      case Column::DECJD:
	this->define(column,"col04","","float","J2000");
	break;
      case Column::VEL:
	this->define(column,"col05","phys.veloc;src.dopplerVeloc","float","");
	break;
      case Column::WRA:
	this->define(column,"col06","","float","J2000");
	break;
      case Column::WDEC:
	this->define(column,"col07","","float","J2000");
	break;
      case Column::W50:
	this->define(column,"col08","phys.veloc;src.dopplerVeloc;spect.line.width","float","");
	break;
      case Column::W20:
	this->define(column,"col09","phys.veloc;src.dopplerVeloc;spect.line.width","float","");
	break;
      case Column::WVEL:
	this->define(column,"col10","phys.veloc;src.dopplerVeloc;spect.line.width","float","");
	break;
      case Column::FINT:
	this->define(column,"col11","phot.flux;spect.line.intensity","float","");
	this->name = "Integrated_Flux";
	break;
      case Column::FTOT:
	this->define(column,"col11","phot.flux;spect.line.intensity","float","");
	this->name = "Total_Flux";
	break;
      case Column::FPEAK:
	this->define(column,"col12","phot.flux;spect.line.intensity","float","");
	this->name = "Peak_Flux";
	break;
      case Column::SNRPEAK:
	this->define(column,"col13","phot.flux;stat.snr","float","");
	break;
      case Column::FLAG:
	this->define(column,"col14","meta.code.qual","char","");
	break;
      case Column::XAV:
	this->define(column,"col15","pos.cartesian.x","float","");
	break;
      case Column::YAV:
	this->define(column,"col16","pos.cartesian.y","float","");
	break;
      case Column::ZAV:
	this->define(column,"col17","pos.cartesian.z","float","");
	break;
      case Column::XCENT:
	this->define(column,"col18","pos.cartesian.x","float","");
	this->name = "X_Centroid";
	break;
      case Column::YCENT:
	this->define(column,"col19","pos.cartesian.y","float","");
	this->name = "Y_Centroid";
	break;
      case Column::ZCENT:
	this->define(column,"col20","pos.cartesian.z","float","");
	this->name = "Z_Centroid";
	break;
      case Column::XPEAK:
	this->define(column,"col21","pos.cartesian.x","int","");
	break;
      case Column::YPEAK:
	this->define(column,"col22","pos.cartesian.y","int","");
	break;
      case Column::ZPEAK:
	this->define(column,"col23","pos.cartesian.z","int","");
	break;
      default:
	break;
      };
  }

  void VOField::printField(std::ostream &stream)
  {
    /// @details
    /// Print the Field entry with appropriate formatting.
    /// \param stream The output stream to send the text to.

    stream << "<FIELD name=\"" <<this->name
	   << "\" ID=\"" << this->ID
	   << "\" ucd=\"" << this->UCD;
    if(this->ref!="") stream << "\" ref=\"" << this->ref;
    stream << "\" datatype=\"" << this->datatype;
    stream << "\" units=\"" << this->units;
    if(datatype=="char")
      stream << "\" arraysize=\"" << this->width;
    else{
      stream << "\" width=\"" << this->width;
      if(datatype=="float" || datatype=="double")
	stream << "\" precision=\"" << this->precision;
    }
    stream << "\"/>\n";

  }




}
