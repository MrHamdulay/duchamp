#include <Detection/columns.hh>
#include <duchamp.hh>
#include <param.hh>
#include <Cubes/cubes.hh>
#include <vector> 
#include <string>

using std::string;
using std::vector;
using namespace Column;

Col getDefaultCol(int i){
  ColSet columnSet;
  switch(i){
  case 0:
    return Col(5, "Obj#",   ""); //[0] -- in briefCol
    break;
  case 1:
    return Col(8,"Name",   ""); //[1] -- in briefCol
    break;
  case 2:
    return Col(6, "X",      ""); //[2] -- in briefCol
    break;
  case 3:
    return Col(6, "Y",      ""); //[3] -- in briefCol
    break;
  case 4:
    return Col(6, "Z",      ""); //[4] -- in briefCol
    break;
  case 5:
    return Col(13,"",       ""); //[5] (RA)
    break;
  case 6:
    return Col(13,"",       ""); //[6] (Dec)
    break;
  case 7:
    return Col(7, "VEL",    ""); //[7]
    break;
  case 8:
    return Col(9, "",       "[arcmin]"); //[8] (wRA)
    break;
  case 9:
    return Col(9, "",       "[arcmin]"); //[9] (wDEC)
    break;
  case 10:
    return Col(7, "w_VEL",  ""); //[10]
    break;
  case 12:
    return Col(9, "F_peak", ""); //[12] -- in briefCol
    break;
  case 13:
    return Col(4, "X1",     ""); //[13] -- in briefCol
    break;
  case 14:
    return Col(4, "X2",     ""); //[14] -- in briefCol
    break;
  case 15:
    return Col(4, "Y1",     ""); //[15] -- in briefCol
    break;
  case 16:
    return Col(4, "Y2",     ""); //[16] -- in briefCol
    break;
  case 17:
    return Col(4, "Z1",     ""); //[17] -- in briefCol
    break;
  case 18:
    return Col(4, "Z2",     ""); //[18] -- in briefCol
    break;
  case 19:
    return Col(6, "Npix",   "[pix]"); //[19] -- in briefCol
    break;
  case 20:
    return Col(5, "Flag",   ""); //[20]
    break;
  case 11:
  default:
    std::stringstream errmsg;
    errmsg << "bad value of argument : " << i<<std::endl;
    duchampError("getDefaultCol", errmsg.str());
    break;
  }
} 

void Cube::setupColumns(){

  this->fullColSet.vec.clear();
  this->logColSet.vec.clear();

  // initialise

  Col fint(10,"F_int",  "");
  Col ftot(10,"F_tot",  "");

  Col tempC;
  string tempstr;
  int size;

  // Obj# and Name
  this->fullColSet.vec.push_back(getDefaultCol(0));
  tempC = getDefaultCol(1);
  for(int o=0;o<this->objectList.size();o++){
    int tempsize = this->objectList[o].getName().size() + 1;
    if((o==0)||(size < tempsize)) size = tempsize;
  }
  for(int i=tempC.getWidth();i<size;i++) tempC.widen();
  this->fullColSet.vec.push_back(tempC);

  // X, Y, Z -- check for widened columns
  tempC = getDefaultCol(2);
  for(int o=0;o<this->objectList.size();o++){
    int tempsize = int( log10(this->objectList[o].getXcentre()) + 1) + xyzPrec + 2;
    if((o==0)||(size < tempsize)) size = tempsize;
  }
  for(int i=tempC.getWidth();i<size;i++) tempC.widen();
  this->fullColSet.vec.push_back(tempC);
  tempC = getDefaultCol(3);
  for(int o=0;o<this->objectList.size();o++){
    int tempsize = int( log10(this->objectList[o].getYcentre()) + 1) + xyzPrec + 2;
    if((o==0)||(size < tempsize)) size = tempsize;
  }
  for(int i=tempC.getWidth();i<size;i++) tempC.widen();
  this->fullColSet.vec.push_back(tempC);
  tempC = getDefaultCol(4);
  for(int o=0;o<this->objectList.size();o++){
    int tempsize = int( log10(this->objectList[o].getZcentre()) + 1) + xyzPrec + 2;
    if((o==0)||(size < tempsize)) size = tempsize;
  }
  for(int i=tempC.getWidth();i<size;i++) tempC.widen();
  this->fullColSet.vec.push_back(tempC);

  // RA & Dec -- should be fine, need to assign correct column title
  tempstr = this->head.getWCS()->lngtyp;
  tempC = getDefaultCol(5);
  tempC.setName(tempstr);
  this->fullColSet.vec.push_back(tempC);
  tempC = getDefaultCol(6);
  tempstr = this->head.getWCS()->lattyp;
  tempC.setName(tempstr);
  this->fullColSet.vec.push_back(tempC);

  // VEL -- title & width to be checked
  tempC = getDefaultCol(7);
  tempC.setUnit("[" + this->head.getSpectralUnits() + "]");
  size = this->head.getSpectralUnits().size() + 3;
  for(int i=tempC.width;i<size;i++) tempC.widen();
  for(int o=0;o<this->objectList.size();o++){
    int tempsize = int( log10(fabs(this->objectList[o].getVel())) + 1) + velPrec + 2;
    if(this->objectList[o].getVel()<0) tempsize++; // for - sign    
    if((o==0)||(tempsize > size)) size = tempsize;
  }
  for(int i=tempC.width;i<size;i++) tempC.widen();
  this->fullColSet.vec.push_back(tempC);

  // w_RA & w_DEC -- titles and widths to be checked.
  tempstr = "w_" + this->fullColSet.vec[5].getName();
  tempC = getDefaultCol(8);
  tempC.setName(tempstr);
  for(int o=0;o<this->objectList.size();o++){
    int tempsize =  int( log10(this->objectList[o].getRAWidth()) + 1) + wposPrec + 2;
    if((o==0)||(size < tempsize)) size = tempsize;
  }
  for(int i=tempC.width;i<size;i++) tempC.widen();
  this->fullColSet.vec.push_back(tempC);

  tempstr = "w_" + this->fullColSet.vec[6].name;
  tempC = getDefaultCol(9);
  tempC.setName(tempstr);
  for(int o=0;o<this->objectList.size();o++){
    int tempsize = int( log10(this->objectList[o].getDecWidth()) + 1) + wposPrec + 2;
    if((o==0)||(size < tempsize)) size = tempsize;
  }
  for(int i=tempC.width;i<size;i++) tempC.widen();
  this->fullColSet.vec.push_back(tempC);

  // w_VEL -- title & width to be checked
  tempC = getDefaultCol(10);
  tempC.setUnit("[" + this->head.getSpectralUnits() + "]");
  size = this->head.getSpectralUnits().size() + 3;
  for(int i=tempC.width;i<size;i++) tempC.widen();
  for(int o=0;o<this->objectList.size();o++){
    int tempsize = int( log10(this->objectList[o].getVelWidth()) + 1) + velPrec + 2;
    if((o==0)||(size < tempsize)) size = tempsize;
  }
  for(int i=tempC.width;i<size;i++) tempC.widen();
  this->fullColSet.vec.push_back(tempC);

  // F_int
  fint.setUnit("[" + this->head.getIntFluxUnits() + "]");
  size = this->head.getIntFluxUnits().size();
  for(int i=fint.width-3;i<size;i++) fint.widen();
  for(int o=0;o<this->objectList.size();o++){
    int tempsize = int( log10(fabs(this->objectList[o].getIntegFlux())) + 1) + fluxPrec + 2;
    if(this->objectList[o].getIntegFlux()<0) tempsize++;
    if((o==0)||(size < tempsize ))  size = tempsize;
  }
  for(int i=fint.width;i<size;i++) fint.widen();

  // F_tot
  ftot.setUnit("[" + this->head.getFluxUnits() + "]");
  size = this->head.getFluxUnits().size();
  for(int i=ftot.width-3;i<size;i++) ftot.widen();
  for(int o=0;o<this->objectList.size();o++){
    int tempsize = int( log10(fabs(this->objectList[o].getTotalFlux())) + 1) + fluxPrec + 2;
    if(this->objectList[o].getTotalFlux()<0) tempsize++;
    if((o==0)||(size < tempsize)) size = tempsize;
  }
  for(int i=ftot.width;i<size;i++) ftot.widen();

  // Add F_int or F_tot depending on WCS status
  if(this->head.isWCS()) this->fullColSet.vec.push_back(fint);
  else this->fullColSet.vec.push_back(ftot);

  // F_peak
  tempC = getDefaultCol(12);
  tempC.setUnit("[" + this->head.getFluxUnits() + "]");
  size = this->head.getFluxUnits().size();
  for(int i=tempC.width-3;i<size;i++) tempC.widen();
  for(int o=0;o<this->objectList.size();o++){
    int tempsize = int( log10(fabs(this->objectList[o].getPeakFlux())) + 1);
    if(this->objectList[o].getPeakFlux()<0) tempsize++;
    if((o==0)||(size < tempsize)) size = tempsize;
  }
  for(int i=tempC.width;i<size+fluxPrec+2;i++) tempC.widen();
  this->fullColSet.vec.push_back(tempC);

  // X1,X2,Y1,Y2,Z1,Z2 -- just check widths
  tempC = getDefaultCol(13);
  for(int o=0;o<this->objectList.size();o++){
    int tempsize = int( log10(this->objectList[o].getXmin()) + 1 + 1);
    if((o==0)||(size < tempsize)) size = tempsize;
  }
  for(int i=tempC.width;i<size;i++) tempC.widen();
  this->fullColSet.vec.push_back(tempC);
  tempC = getDefaultCol(14);
  for(int o=0;o<this->objectList.size();o++){
    int tempsize = int( log10(this->objectList[o].getXmax()) + 1 + 1);
    if((o==0)||(size < tempsize)) size = tempsize;
  }
  for(int i=tempC.width;i<size;i++) tempC.widen();
  this->fullColSet.vec.push_back(tempC);

  tempC = getDefaultCol(15);
  for(int o=0;o<this->objectList.size();o++){
    int tempsize = int( log10(this->objectList[o].getYmin()) + 1) + 1;
    if((o==0)||(size < tempsize)) size = tempsize;
  }
  for(int i=tempC.width;i<size;i++) tempC.widen();
  this->fullColSet.vec.push_back(tempC);
  tempC = getDefaultCol(16);
  for(int o=0;o<this->objectList.size();o++){
    int tempsize = int( log10(this->objectList[o].getYmax()) + 1) + 1;
    if((o==0)||(size < tempsize)) size = tempsize;
  }
  for(int i=tempC.width;i<size;i++) tempC.widen();
  this->fullColSet.vec.push_back(tempC);

  tempC = getDefaultCol(17);
  for(int o=0;o<this->objectList.size();o++){
    int tempsize = int( log10(this->objectList[o].getZmin()) + 1) + 1;
    if((o==0)||(size < tempsize)) size = tempsize;
  }
  for(int i=tempC.width;i<size;i++) tempC.widen();
  this->fullColSet.vec.push_back(tempC);
  tempC = getDefaultCol(18);
  for(int o=0;o<this->objectList.size();o++){
    int tempsize = int( log10(this->objectList[o].getZmax()) + 1) + 1;
    if((o==0)||(size < tempsize)) size = tempsize;
  }
  for(int i=tempC.width;i<size;i++) tempC.widen();
  this->fullColSet.vec.push_back(tempC);

  // Npix -- check for width.
  tempC = getDefaultCol(19);
  for(int o=0;o<this->objectList.size();o++){
    size = int( log10(this->objectList[o].getSize()) ) + 2;
    for(int i=tempC.width;i<size;i++) tempC.widen();
  }    
  this->fullColSet.vec.push_back(tempC);

  // Flags.
  this->fullColSet.vec.push_back(getDefaultCol(20));

  // Now copy the correct columns to the log columnset
  this->logColSet.vec.push_back(this->fullColSet.vec[0]);
  for(int i=1;i<4;i++) this->logColSet.vec.push_back(this->fullColSet.vec[i+1]);
  this->logColSet.vec.push_back(ftot);
  for(int i=5;i<13;i++) this->logColSet.vec.push_back(this->fullColSet.vec[i+7]);
  this->logColSet.defined= true;

}
