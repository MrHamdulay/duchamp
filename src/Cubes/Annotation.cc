
void Annotation::drawCircle(Detection &obj, FitsHeader &header)
{

  

}

void Annotation::fileComments(Param &par)
{
  std::string fname = par.getImageFile();
  if(par.getFlagSubsection()) fname+=par.getSubsection();
  this->itsFile << this->comment() << " Duchamp Source Finder v."<< VERSION << "\n";
  this->itsFile << this->comment() << " Results for FITS file: " << fname << "\n";
  if(par.getFlagFDR())
    this->itsFile<<this->comment() << " FDR Significance = " << par.getAlpha() << "\n";
  else
    this->itsFile<<this->comment() << " Threshold = " << par.getCut() << "\n";
  if(par.getFlagATrous()){
    this->itsFile<<this->comment() << " The a trous reconstruction method was used, with the following parameters:\n";
    this->itsFile<<this->comment() << "  Dimension = " << par.getReconDim() << "\n";
    this->itsFile<<this->comment() << "  Threshold = " << par.getAtrousCut() << "\n";
    this->itsFile<<this->comment() << "  Minimum Scale =" << par.getMinScale() << "\n";
    this->itsFile<<this->comment() << "  Filter = " << par.getFilterName() << "\n";
  }
  else if(par.getFlagSmooth()){
    this->itsFile<<this->comment() << " The data was smoothed prior to searching, with the following parameters:\n";
    this->itsFile<<this->comment() << "  Smoothing type = " << par.getSmoothType() << "\n";
    if(par.getSmoothType()=="spectral"){
      this->itsFile << this->comment() << "  Hanning width = " << par.getHanningWidth() << "\n";
    }
    else{
      this->itsFile << this->comment() << "  Kernel Major axis = " << par.getKernMaj() << "\n";
      if(par.getKernMin()>0) 
	this->itsFile << this->comment() << "  Kernel Minor axis = " << par.getKernMin() << "\n";
      else
	this->itsFile << this->comment() << "  Kernel Minor axis = " << par.getKernMaj() << "\n";
      this->itsFile << this->comment() << "  Kernel Major axis = " << par.getKernPA() << "\n";
    }
  }
}
