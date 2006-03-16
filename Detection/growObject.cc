#include <iostream>
#include <iomanip>
#include <vector>
#include <Cubes/cubes.hh>
#include <Utils/utils.hh>

using std::vector;
using std::setw;

// void Detection::growObject(Image &image)
// {
//   vector <bool> isInObj(image.getSize(),false);
//   for(int i=0;i<this->pix.size();i++) 
//     isInObj[this->pix[i].getX() + this->pix[i].getY()*image.getDimX()] = true;

//   for(int pix=0; pix<this->pix.size(); pix++){ // for each pixel in the object

//     for(int xnbr=-1; xnbr<=1; xnbr++){
//       for(int ynbr=-1; ynbr<=1; ynbr++){
// 	if((xnbr!=0)||(ynbr!=0)){ // ignore when both=0 ie. the current object pixel

// 	  Voxel *pixnew = new Voxel;
// 	  *pixnew = object.getPixel(pix);
// 	  long newx = this->pix[pix].getX() + xnbr;
// 	  long newy = this->pix[pix].getY() + ynbr;
	  
// 	  if((newx<image.getDimX())&&(newy<image.getDimY())&&(newx>=0)&&(newy>=0)){
	    
// 	    pixnew->setX(newx);
// 	    pixnew->setY(newy);
// 	    pixnew->setF(image.getPixValue(newx,newy));
	    
// 	    if( (!isInObj[newx+newy*image.getDimX()]) && 
// 		(isDetection(pixnew->getF(), image.getMean(), 
// 			     image.getSigma(), image.pars().getGrowthCut())) ){
// 	      isInObj[newx+newy*image.getDimX()] = true;
// 	      this->addPixel(*pixnew);
// 	    }
// 	  }
// 	  delete pixnew;
// 	}
//       }
//     }

//   }

//   isInObj.clear();

// }


void growObject(Detection &object, Image &image)
{
  vector <bool> isInObj(image.getSize(),false);
  for(int i=0;i<object.getSize();i++) 
    isInObj[object.getX(i) + object.getY(i)*image.getDimX()] = true;

  for(int pix=0; pix<object.getSize(); pix++){ // for each pixel in the object

    for(int xnbr=-1; xnbr<=1; xnbr++){
      for(int ynbr=-1; ynbr<=1; ynbr++){
	if((xnbr!=0)||(ynbr!=0)){ // ignore when both=0 ie. the current object pixel

	  Voxel *pixnew = new Voxel;
	  *pixnew = object.getPixel(pix);
	  long newx = object.getX(pix) + xnbr;
	  long newy = object.getY(pix) + ynbr;
	  
	  if((newx<image.getDimX())&&(newy<image.getDimY())&&(newx>=0)&&(newy>=0)){
	    
	    pixnew->setX(newx);
	    pixnew->setY(newy);
	    pixnew->setF(image.getPixValue(newx,newy));
	    
	    if( (!isInObj[newx+newy*image.getDimX()]) && 
		(isDetection(pixnew->getF(), image.getMean(), 
			     image.getSigma(), image.pars().getGrowthCut())) ){
	      isInObj[newx+newy*image.getDimX()] = true;
	      object.addPixel(*pixnew);
	    }
	  }
	  delete pixnew;
	}
      }
    }

  }

  isInObj.clear();

}

void growObject(Detection &object, Cube &cube)
{
  vector <bool> isInObj(cube.getSize(),false);
  float *cut     = new float;	    
  float *flux    = new float;
  float *thresh1 = new float;
  float *thresh2 = new float;
  long  *pos     = new long;
  long  *chanpos = new long;
  long  *newx    = new long;
  long  *newy    = new long;
  long  *newz    = new long;

  for(int i=0;i<object.getSize();i++) {
    long pos = object.getX(i) + object.getY(i)*cube.getDimX() + 
      object.getZ(i)*cube.getDimX()*cube.getDimY();
    isInObj[pos] = true;
  }
  *cut = cube.pars().getGrowthCut();
  
  for(int pix=0; pix<object.getSize(); pix++){ // for each pixel in the object

//     std::cout.setf(std::ios::right);
//     std::cout <<":"<< setw(6) << pix+1 << "/";
//     std::cout.unsetf(std::ios::right);
//     std::cout.setf(std::ios::left);
//     std::cout << setw(6) << object.getSize() << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b" << std::flush;
//     std::cout.unsetf(std::ios::left);


    for(int xnbr=-1; xnbr<=1; xnbr++){
      for(int ynbr=-1; ynbr<=1; ynbr++){
	for(int znbr=-1; znbr<=1; znbr++){

	  if((xnbr!=0)||(ynbr!=0)||(znbr!=0)){ // ignore when all=0 ie. the current object pixel

	    Voxel *pixnew = new Voxel;
	    *pixnew = object.getPixel(pix);
	    *newx = object.getX(pix) + xnbr;
	    *newy = object.getY(pix) + ynbr;
	    *newz = object.getZ(pix) + znbr;
	  
	    if((*newx<cube.getDimX())&&(*newx>=0)&&
	       (*newy<cube.getDimY())&&(*newy>=0)&&
	       (*newz<cube.getDimZ())&&(*newz>=0)){
	    
// 	      std::cerr << *newx<<" "<<*newy<<" "<<*newz<<"  "
// 			<<cube.getDimX()<<" "<<cube.getDimY()<<" "<<cube.getDimZ()
// 			<<std::endl;
	      pixnew->setX(*newx);
	      pixnew->setY(*newy);
	      pixnew->setZ(*newz);
	      // In following R=recon, B=baseline
	      // Four cases: i) B+R -- use recon
	      //            ii) R   -- use recon
	      //           iii) B   -- use array
	      //            iv) none-- use array
	      if(cube.isRecon()) *flux = cube.getReconValue(*newx,*newy,*newz);
	      else  *flux = cube.getPixValue(*newx,*newy,*newz);
	      pixnew->setF(*flux);

	      *chanpos = *newx + *newy * cube.getDimX();
	      *pos = *newx + *newy * cube.getDimX() + *newz * cube.getDimX() * cube.getDimY();
	      // thresh1 = spectral growth threshold
	      *thresh1 = cube.getSpecMean(*chanpos) + *cut * cube.getSpecSigma(*chanpos);
	      // thresh2 = channel map growth threshold
	      *thresh2 = cube.getChanMean(*newz) + *cut * cube.getChanSigma(*newz);
	      if( (!isInObj[*pos]) && 
		  ( (*flux > *thresh1) || (*flux > *thresh2) ) ){
		isInObj[*pos] = true;
		object.addPixel(*pixnew);
	      }
	    }
	    delete pixnew;
	  }

	}
      }
    }
      
  } // end of pix loop

  object.calcParams();

  delete cut;	    
  delete flux;
  delete thresh1;
  delete thresh2;
  delete pos;
  delete chanpos;
  delete newx;
  delete newy;
  delete newz;

  isInObj.clear();

}
