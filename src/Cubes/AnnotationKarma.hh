
class AnnotationKarma : public Annotation 
{
public:
  void line(float x1, float x2, float y1, float y2);
  void circle(float x, float y, float r);
  void ellipse(float x, float y, float maj, float min, float pa);
  void text(float x, float y, std::string content);
  void comment(std::string content);

protected:
  

};


