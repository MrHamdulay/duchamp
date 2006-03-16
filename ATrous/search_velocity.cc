#include <fstream>
#include <iostream>
#include <iomanip>
//#include <Cubes/cubes.hh>
using namespace std;

//void searchVelocity(Cube &cube, Detection *objectList, int &numObjects)
DetectionList *searchVelocity(Cube &cube, Param &par)
{
  DetectionList *objectList = new DetectionList;
  int xdim=cube.getDimX(),ydim=cube.getDimY(),zdim = cube.getDimZ();
  long *dim = new long[2];
  dim[0] = zdim;
  dim[1] = 1;

  ofstream fout(par.getLogFile().c_str(),ios::app);
  outputDetectionTextHeader(fout);
//   outputDetectionTextHeader(cerr);
  for(int npixel=0;npixel<xdim*ydim;npixel++){
    //    at each spatial pixel in the cube, search in the spectrum

    if((100*npixel/(xdim*ydim))%5==0) 
      cout<<setw(3)<<100*npixel/(xdim*ydim)<<"% done"<<"\b\b\b\b\b\b\b\b\b"<<flush;

    Image *spectrum = new Image(dim);
    float *array = new float[zdim];
    for(long zpos=0;zpos<zdim;zpos++){
      int cubepos = npixel + zpos*xdim*ydim;
      array[zpos] = cube.getPixValue(cubepos);
    }
    spectrum->saveArray(array,zdim);

    /* This was the old way of doing it */
    //findSources(*spectrum,par);

    /* This is the new way using the atrous reconstruction */
    for(int i=0;i<zdim;i++) spectrum->setMaskValue(i,0);
    findSourcesAtrous1D(*spectrum,par);
    for(long zpos=0;zpos<zdim;zpos++) {
      int mask = spectrum->getMaskValue(zpos);
      if(mask>1) cerr << "**** "<<npixel<<" "<<zpos<<" "<<mask<<endl;
    }
    
    Image *newSpectrum = new Image(dim);
    newSpectrum->saveArray(array,zdim);
    // set the proper positional parameters for each object
    for(int objCtr=0;objCtr<spectrum->getNumObj();objCtr++){
      Detection *obj = new Detection;
      *obj = spectrum->getObject(objCtr);

      for(int pixCtr=0;pixCtr<obj->getSize();pixCtr++){
	int zpos = obj->getX(pixCtr);
	int xpos = npixel%xdim;
	int ypos = npixel/xdim;
	obj->setZ(pixCtr,zpos);
	obj->setY(pixCtr,ypos);
	obj->setX(pixCtr,xpos);
      }
      obj->calcParams();
      newSpectrum->addObject(*obj);
      delete obj;
    }
    for(long zpos=0;zpos<zdim;zpos++) {
      int mask = spectrum->getMaskValue(zpos);
      if(mask>1) cerr << "**** "<<zpos<<" "<<mask<<endl;
      newSpectrum->setMaskValue(zpos, mask );
    }
    delete spectrum;
    spectrum = newSpectrum;
    delete [] array;
 
    for(int objCtr=0;objCtr<spectrum->getNumObj();objCtr++){
      Detection *obj = new Detection;
      *obj = spectrum->getObject(objCtr);
      outputDetectionText(fout,*obj,objectList->getSize()+1+objCtr);
//       outputDetectionText(cerr,*obj,objectList->getSize()+1+objCtr);
      delete obj;
    }

    short detect = cube.getDetectMapValue(npixel);
    for(int i=0;i<zdim;i++){
      short mask = spectrum->getMaskValue(i);
      detect += mask;
    }
    cube.setDetectMapValue(npixel, detect);

    // add the newly found objects to the list
    for(int i=0;i<spectrum->getNumObj();i++) 
      objectList->addObject(spectrum->getObject(i));

    delete spectrum;
  
  }
  delete [] dim;
  fout<<endl<<endl;
  fout.close();

  return objectList;

}
