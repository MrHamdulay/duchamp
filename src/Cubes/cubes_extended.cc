#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <string>
#include <math.h>

#include <wcs.h>

#include <duchamp.hh>
#include <param.hh>
#include <fitsHeader.hh>
#include <Cubes/cubes.hh>
#include <Detection/detection.hh>
#include <Detection/columns.hh>
#include <Utils/utils.hh>
#include <Utils/mycpgplot.hh>
#include <Utils/Statistics.hh>
using namespace Column;
using namespace mycpgplot;
using namespace Statistics;

void Cube::sortDetections()
{
  /** 
   *  A front end to the sort-by functions.
   *  If there is a good WCS, the detection list is sorted by velocity.
   *  Otherwise, it is sorted by increasing z-pixel value.
   *  The ID numbers are then re-calculated.
   */
  
  if(this->head.isWCS()) SortByVel(this->objectList);
  else SortByZ(this->objectList);
  for(int i=0; i<this->objectList.size();i++) this->objectList[i].setID(i+1);

}
//--------------------------------------------------------------------

void Cube::readSavedArrays()
{
  /**
   *  This function reads in reconstructed and/or smoothed arrays that have 
   *   been saved on disk in FITS files. 
   *  To do this it calls the functions Cube::readReconCube() and 
   *   Cube::readSmoothCube().
   *  The Param set is consulted to determine which of these arrays are needed.
   */

  // If the reconstructed array is to be read in from disk
  if( this->par.getFlagReconExists() && this->par.getFlagATrous() ){
    std::cout << "Reading reconstructed array: "<<std::endl;
    if( this->readReconCube() == FAILURE){
      std::stringstream errmsg;
      errmsg <<"Could not read in existing reconstructed array.\n"
	     <<"Will perform reconstruction using assigned parameters.\n";
      duchampWarning("Duchamp", errmsg.str());
      this->par.setFlagReconExists(false);
    }
    else std::cout << "Reconstructed array available.\n";
  }

  if( this->par.getFlagSmoothExists() && this->par.getFlagSmooth() ){
    std::cout << "Reading smoothed array: "<<std::endl;
    if( this->readSmoothCube() == FAILURE){
      std::stringstream errmsg;
      errmsg <<"Could not read in existing smoothed array.\n"
	     <<"Will smooth the cube using assigned parameters.\n";
      duchampWarning("Duchamp", errmsg.str());
      this->par.setFlagSmoothExists(false);
    }
    else std::cout << "Smoothed array available.\n";
  }
    
}

//--------------------------------------------------------------------

void Cube::plotBlankEdges()
{
  /** 
   *  A front end to the drawBlankEdges() function. This draws the lines
   *   indicating the extent of the non-BLANK region of the cube in the 
   *   PGPLOT colour MAGENTA (from the namespace mycpgplot), using the Cube's
   *   arrays and dimensions.
   *
   *  Note that a PGPLOT device needs to be open. This is only done if the 
   *   appropriate Param parameter is set.
   */
  if(this->par.drawBlankEdge()){
    int colour;
    cpgqci(&colour);
    cpgsci(MAGENTA);
    drawBlankEdges(this->array,this->axisDim[0],this->axisDim[1],this->par);
    cpgsci(colour);
  }
}
//--------------------------------------------------------------------

