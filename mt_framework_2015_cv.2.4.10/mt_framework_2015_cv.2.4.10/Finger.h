#pragma 


#include "../../../MT_lib/opencv/build/include/opencv2/opencv.hpp"
#include <math.h>
using namespace std;
using namespace cv;

class Finger
{
private:

	Point2f coords;
	int id;
	Finger* wanted;
	float x;

public:
	Finger();
	~Finger();
	Finger(int id, Point2f coords);
	void setId(int id);
	void setCoords(Point2f coords);
	Point2f getCoords();
	float getXCoord();
	float getYCoord();
	void setWanted(Finger wanted);
	Finger getWanted();
	int getId();

	void setXCoord(float x);
	void setYCoord(float y);

	int idWanted;
	float wantedDist;


};
