
void AnnotationKarma::line(float x1, float x2, float y1, float y2)
{
  itsFile << "LINE " << x1 << " " << y1 << " " << x2 << " " << y2 << "\n";
}

void AnnotationKarma::circle(float x, float y, float r)
{
  itsFile << "CIRCLE " << x << " " << y << " " << r << "\n";
}

void AnnotationKarma::ellipse(float x, float y, float maj, float min, float pa)
{
  itsFile << "ELLIPSE " << x << " " << y << " " << maj << " " << min << " " << pa << "\n";
}

void AnnotationKarma::text(float x, float y, std::string content)
{
  itsFile << "TEXT " << x << " " << y << " " << content << "\n";
}

void AnnotationKarma::comment(std::string content)
{
  itsFile << "# " << content << "\n";
}

void AnnotationKarma::fileHeader(Param &par)
{
    stream << "COLOR RED \n";
    stream << "COORD W \n";

}
