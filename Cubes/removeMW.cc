#include <iostream>
#include <Cubes/cubes.hh>

Cube removeMW(Cube &cube, Param &par)
{
  int maxMW = par.getMaxMW();
  int minMW = par.getMinMW();
  bool flagBlank = par.getFlagBlankPix();
  bool flagMW = par.getFlagMW();
  float nPV = par.getBlankPixVal();
  long xdim=cube.getDimX(),ydim=cube.getDimY(),zdim = cube.getDimZ();
  long *dim = new long[3];
  dim[0] = xdim; dim[1]=ydim; dim[2]=zdim;
  Cube *newcube = new Cube(cube);
//   Cube *newcube = new Cube(dim);
//   *newcube = cube;
//   newcube->setWCS(cube.getWCS());
  int pos;
  bool isMW;
  float val;
  for(int pix=0;pix<xdim*ydim;pix++){
    for(int z=0;z<zdim;z++){
      pos = z*xdim*ydim + pix;
      val = cube.getPixValue(pos);
      isMW = flagMW && (z>=minMW) && (z<=maxMW);
      if(!par.isBlank(val) && isMW) newcube->setPixValue(pos,0.);
      //if(!isBlank && isMW) newcube->setPixValue(pos,nPV);
    }
  }
  return *newcube;
}

void Cube::removeMW()
{
  int maxMW = this->par.getMaxMW();
  int minMW = this->par.getMinMW();
  bool flagBlank = this->par.getFlagBlankPix();
  bool flagMW = this->par.getFlagMW();
  float nPV = this->par.getBlankPixVal();
  long xdim=axisDim[0], ydim=axisDim[1], zdim=axisDim[2];
  for(int pix=0;pix<xdim*ydim;pix++){
    for(int z=0;z<zdim;z++){
      int pos = z*xdim*ydim + pix;
      bool isMW = flagMW && (z>=minMW) && (z<=maxMW);
      if(!(this->par.isBlank(this->array[pos])) && isMW) this->array[pos]=0.;
    }
  }
}

