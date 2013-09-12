#include <iostream>
#include <duchamp/PixelMap/Line.hh>
#include <duchamp/PixelMap/Voxel.hh>

namespace PixelInfo
{

    Line::Line(Voxel start, Voxel end):
	itsStart(start),itsEnd(end)
    {
    }

    Line::Line(int x0, int y0, int x1, int y1)
    {
	this->itsStart = Voxel(x0,y0);
	this->itsEnd = Voxel(x1,y1);
    }

    Line::Line(const Line& other)
    {
	this->operator=(other);
    }
	
    Line& Line::operator= (const Line& other)
    {
	if(this == &other) return *this;
	this->itsStart = other.itsStart;
	this->itsEnd = other.itsEnd;
	return *this;
    }

    std::ostream &operator<<(std::ostream &theStream, Line &theLine)
    {
	// theStream << theLine.itsStart << "-->" << theLine.itsEnd; 
	theStream << theLine.itsStart.getX()<<","<<theLine.itsStart.getY() << "-->" << theLine.itsEnd.getX()<<","<<theLine.itsEnd.getY(); 
	return theStream;
    }

}
