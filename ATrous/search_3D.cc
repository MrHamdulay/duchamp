#include <iostream>
#include <iomanip>
//#include <Cubes/cubes.hh>
#include <Utils/utils.hh>

using namespace std;

void printDetails(int num,Detection &obj);
DetectionList *search3D(Cube &cube, Param &par)
{
  DetectionList *objectList = new DetectionList;
  int count=0;
  long size = cube.getSize();
  float *arr = new float[size];
  float *newarr = new float[size];
  int goodSize=0;
  float blankPixValue = par.getBlankPixVal();
  bool flagFDR = par.getFlagFDR();

  for(int i=0;i<size;i++){
    if(!par.isBlank(cube.getPixValue(i)))
      arr[goodSize++] = cube.getPixValue(i);
  }
  float mean,sigma;
  if(goodSize>0){
    findMedianStats(arr,goodSize,mean,sigma);
    cube.getArray(arr);
    long xdim = cube.getDimX();
    long ydim = cube.getDimY();
    long zdim = cube.getDimZ();
    cerr<<xdim<<"x"<<ydim<<"x"<<zdim<<"-->";
    cerr<<par.getCut()<<" "<<par.getAtrousCut()<<" "<<par.getFlagFDR()<<endl;
    atrous3DReconstruct(xdim,ydim,zdim,arr,newarr,par);
    //atrous3DReconstructBetter(xdim,ydim,zdim,arr,newarr,par);
    cerr<<"<--"<<endl;

    //    cube.saveArray(newarr,size);
    //    cube.saveOrig(arr,size);
    cube.saveRecon(newarr,size);

//     for(int i=0;i<size;i++)
//       if(cube.getPixValue(i)<(mean+2.*sigma)) cube.setPixValue(i,0.);

    /* Search spatially */
    long dim[2]={xdim,ydim};
    for(int channel=0;channel<zdim;channel++){
      bool skipThisOne = par.getFlagMW() && 
	(channel>=par.getMinMW()) && (channel<=par.getMaxMW()); // ignore the Milky Way.
      if(!skipThisOne){
	Image *slice = new Image(dim);
	for(long j=0;j<xdim*ydim;j++) 
	  slice->setPixValue( j, newarr[j+channel*xdim*ydim]);

	float *temparr = new float[xdim*ydim];
	int ct=0;
	for(int i=0;i<xdim*ydim;i++)
	  if(!par.isBlank(arr[channel*xdim*ydim+i]))
	    temparr[ct++] = arr[channel*xdim*ydim+i];
	findMedianStats(temparr,ct,mean,sigma);
	delete [] temparr;

	slice->setStats(mean,sigma,par.getCut());
	if(flagFDR) setupFDR(*slice,par);
	lutz_detect(*slice,par);
	for(int i=0;i<slice->getNumObj();i++){
	  count++;
	  Detection *obj = new Detection;
	  *obj = slice->getObject(i);
	  for(int j=0;j<obj->getSize();j++) obj->setZ(j,channel);
	  obj->calcParams();
	  objectList->addObject(*obj);
	  printDetails(count,*obj);
	  delete obj;
	}
	for(int i=0;i<slice->getSize();i++) 
	  cube.setDetectMapValue(i,cube.getDetectMapValue(i) + slice->getMaskValue(i));
	delete slice;
      }
    }

    /* Search in velocity */
    cerr<<endl;
    long dim2[2]={zdim,1};
    for(int npixel=0;npixel<xdim*ydim;npixel++){
      Image *spectrum = new Image(dim2);
      for(long zpos=0;zpos<zdim;zpos++)
	spectrum->setPixValue( zpos, newarr[npixel+zpos*xdim*ydim] ); 

      float *temparr = new float[zdim];
      int ct=0;
      for(int i=0;i<zdim;i++)
	if(!par.isBlank(arr[npixel+i*xdim*ydim]))
	  temparr[ct++] = arr[npixel+i*xdim*ydim];
      findMedianStats(temparr,zdim,mean,sigma);
      delete [] temparr;

      spectrum->setStats(mean,sigma,par.getCut());
      if(flagFDR) setupFDR(*spectrum,par);
      lutz_detect(*spectrum,par);
      for(int i=0;i<spectrum->getNumObj();i++){
	count++;
	Detection *obj = new Detection;
	*obj = spectrum->getObject(i);
	for(int j=0;j<obj->getSize();j++){
      	  obj->setZ(j,obj->getX(j));
	  obj->setX(j,npixel%xdim);
	  obj->setY(j,npixel/xdim);
	}
	obj->calcParams();
	objectList->addObject(*obj);
	printDetails(count,*obj);
	delete obj;
      }
      for(int i=0;i<spectrum->getSize();i++) 
	cube.setDetectMapValue(npixel,cube.getDetectMapValue(npixel)+spectrum->getMaskValue(i));
      delete spectrum;
    }

  }
  delete [] arr;
  delete [] newarr;

  cerr<<"Found "<<count<<" objects.\n";
  return objectList;
}  

void printDetails(int num,Detection &obj)
{
  cerr<<"Obj#";
  cerr<<setw(4)<<num<<":  ";
  cerr.setf(ios::fixed);
  cerr<<setw(5)<<setprecision(1)<<obj.getXcentre()<<" ";
  cerr<<setw(5)<<setprecision(1)<<obj.getYcentre()<<" ";
  cerr<<setw(3)<<setprecision(1)<<obj.getZcentre()<<" ";
  cerr<<" npix = "<<obj.getSize();
  cerr<<" F = "<<obj.getTotalFlux()<<endl;
}
