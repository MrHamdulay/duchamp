
class Annotation {
public:
  Annotation(){};
  
  virtual void line(float x1, float x2, float y1, float y2)=0;
  virtual void circle(float x, float y, float r)=0;
  virtual void ellipse(float x, float y, float maj, float min, float pa)=0;
  virtual void text(std::string content)=0;
  virtual void comment(std::string content)=0;
  void fileComments(Param &par);
  virtual void fileHeader()=0;
  void drawCircle(Detection &obj, FitsHeader &header);

  void std::string comment()=0;

protected:
  std::ofstream itsFile;
  


};
