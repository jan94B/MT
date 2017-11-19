#include "Finger.h"
#include <vector>
#include "../../../MT_lib/opencv/build/include/opencv2/opencv.hpp"
#include <math.h>


Finger::Finger()
{
}


Finger::~Finger()
{
}

Finger::Finger(int id, Point2f coords) {
	this->id = id;
	this->coords = coords;
	this->x = coords.x;
	this->wantedDist = NULL;
	this->wanted = NULL;
	this->idWanted = NULL;
}

void Finger::setId(int id) {
	this->id = id;
}


void Finger::setCoords(Point2f coords) {
	this->coords = coords;
	this->x = coords.x;
}


Point2f Finger::getCoords() {
	return this->coords;
}

float Finger::getXCoord()
{

	return (float)this->coords.x;
}

float Finger::getYCoord()
{

	return this->coords.y;
}

void Finger::setWanted(Finger wanted)
{
	this->wanted = &wanted; //wrong?
}

Finger Finger::getWanted()
{
	return *wanted;
}

int Finger::getId()
{
	return id;
}

void Finger::setYCoord(float y)
{
	this->coords.y = y;
}

void Finger::setXCoord(float x)
{
	this->coords.x = x;
}

