#ifndef DUCHAMP_LINE_H
#define DUCHAMP_LINE_H

#include <iostream>
#include <duchamp/PixelMap/Voxel.hh>

namespace PixelInfo
{

    class Line
    {
    public:
	Line(){};
	Line(Voxel start, Voxel end);
	Line(int x0, int y0, int x1, int y1);
	Line(const Line& other);
	Line& operator= (const Line& other);
	friend std::ostream &operator<<(std::ostream &theStream, Line &theLine);
	virtual ~Line(){};

	Voxel start(){return itsStart;};
	Voxel end(){return itsEnd;};
	bool has(Voxel &vox){return ( (itsStart==vox) || (itsEnd==vox) );};

	bool operator==(const Line &other){return (itsStart==other.itsStart) && (itsEnd==other.itsEnd);};

	int x0(){return itsStart.getX();};
	int y0(){return itsStart.getY();};
	int x1(){return itsEnd.getX();};
	int y1(){return itsEnd.getY();};

    protected:
	Voxel itsStart;
	Voxel itsEnd;
    };


}

#endif //LINE
