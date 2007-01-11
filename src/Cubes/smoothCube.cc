#include <Cubes/cubes.hh>
#include <ATrous/atrous.hh>
#include <Utils/feedback.hh>
#include <Utils/Hanning.hh>

void Cube::SmoothCube()
{
  /**
   *   A function that smooths each spectrum in the cube using the 
   *    Hanning smoothing function. The degree of smoothing is given
   *    by the parameter hannWidth.
   */

  Hanning::Hanning hann(this->par.getHanningWidth());
  
  long xySize = this->axisDim[0]*this->axisDim[1];
  long zdim = this->axisDim[2];

  float *spectrum = new float[this->axisDim[2]];

  ProgressBar bar;
  if(this->par.isVerbose()) {
    std::cout<<"  Smoothing... ";
    bar.init(xySize);
  }

  for(int pix=0;pix<xySize;pix++){

    if( this->par.isVerbose() ) bar.update(pix+1);
    
    for(int z=0;z<zdim;z++){
      if(this->isBlank(z*xySize+pix)) spectrum[z]=0.;
      else spectrum[z] = this->array[z*xySize+pix];
    }

    float *smoothed = hann.smooth(spectrum,zdim);

    for(int z=0;z<zdim;z++){
      if(this->isBlank(z*xySize+pix)) 
	this->recon[z*xySize+pix]=this->array[z*xySize+pix];
      else 
	this->recon[z*xySize+pix] = smoothed[z];
    }
    delete [] smoothed;
  }
  this->reconExists = true;
  if(this->par.isVerbose()) bar.fillSpace("All Done.\n");

  delete [] spectrum;

}


void Cube::SmoothSearch()
{
  /**
   * The Cube is first smoothed, using Cube::SmoothCube().
   * It is then searched, using searchReconArray()
   * The resulting object list is stored in the Cube, and outputted
   *  to the log file if the user so requests.
   */
  
  this->SmoothCube();
  if(this->par.isVerbose()) std::cout << "  ";
  this->setCubeStats();
  if(this->par.isVerbose()) std::cout << "  Searching... " << std::flush;
  
  this->objectList = search3DArray(this->axisDim,this->recon,
				   this->par,this->Stats);

  this->updateDetectMap();
  if(this->par.getFlagLog()) this->logDetectionList();
  

}
