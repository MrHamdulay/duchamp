#include <fstream>
#include <iostream>
#include <iomanip>
//#include <Cubes/cubes.hh>
#include <Utils/utils.hh>

using namespace std;

DetectionList *searchSpatially(Cube &cube, Param &par)
{
  DetectionList *objectList = new DetectionList;
  long xdim = cube.getDimX();
  long ydim = cube.getDimY();
  long zdim = cube.getDimZ();
  long *dim = new long[2];
  dim[0] = xdim;
  dim[1] = ydim;
  bool flagMW = par.getFlagMW();
  int minMW=zdim,maxMW=zdim;
  if(flagMW){
    minMW = par.getMinMW();
    maxMW = par.getMaxMW();
  }

  ofstream fout(par.getLogFile().c_str(),ios::app);
  outputDetectionTextHeader(fout);
//   outputDetectionTextHeader(cerr);

  for(long numChannel=0;numChannel<zdim;numChannel++){
    //    at each frequency channel, search the image...

    if((100*numChannel/zdim)%5==0) cout<<setw(3)<<100*numChannel/zdim<<"% done"<<"\b\b\b\b\b\b\b\b\b"<<flush;

    if((!flagMW)||(numChannel<minMW)||(numChannel>maxMW)){
      

      Image *slice = new Image(dim);
      //     for(long j=0;j<slice->getSize();j++){
      float *array = new float[xdim*ydim];
      for(int pos=0; pos<xdim*ydim; pos++){
	long cubepos = pos + numChannel*xdim*ydim;
	array[pos] = cube.getPixValue(cubepos);
      }
      slice->saveArray(array,xdim*ydim);

      /* This was the old way of doing it */
      //findSources(*slice,par);

      /* This is the new way using the atrous reconstruction */
      findSourcesAtrous2D(*slice,par);

      Image *newslice = new Image(dim);
      newslice->saveArray(array,xdim*ydim);
      for(int i=0;i<slice->getNumObj();i++){
	Detection *thisobj = new Detection;
	*thisobj = slice->getObject(i);
	for(int j=0;j<slice->getObject(i).getSize();j++){
// 	  slice->getObject(i).setZ(j,numChannel);
	  thisobj->setZ(j,numChannel);
	}
	thisobj->calcParams();
	newslice->addObject(*thisobj);
	delete thisobj;
      }
      for(int pos=0; pos<xdim*ydim; pos++){
	short int mask = slice->getMaskValue(pos);
	newslice->setMaskValue(pos,mask);
      }
      delete slice;
      slice = newslice;
      delete [] array;

      for(int i=0;i<slice->getNumObj();i++) {
	Detection *thisobj = new Detection;
	*thisobj = slice->getObject(i);
	outputDetectionText(fout,*thisobj,objectList->getSize()+1+i);
// 	outputDetectionText(cerr,*thisobj,objectList->getSize()+1+i);
	delete thisobj;
      }

      for(int i=0;i<slice->getSize();i++){
 	short detect = cube.getDetectMapValue(i);
	detect += slice->getMaskValue(i);
	cube.setDetectMapValue(i, detect);
      }
      
      // add the newly found objects to the list
      for(int i=0;i<slice->getNumObj();i++) 
	objectList->addObject(slice->getObject(i));

      delete slice;
    }
  }

  fout<<endl<<endl;
  fout.close();

  return objectList;
  
}  
