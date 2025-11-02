#include "stdafx.h"
#include "SVGPoint.h"


float SVGPoint::getX() const
{
	return x;
}

void SVGPoint::setX(const float& newX)
{
	x = newX;
}

float SVGPoint::getY()const
{
	return y;
}

void SVGPoint::setY(const float& newY)
{
	y = newY;
}

SVGPoint::SVGPoint()
{
	x = 0;
	y = 0;
}

void SVGPoint::translate(const float dx,const float dy)
{
	x += dx;
	y += dy;
}


SVGPoint::~SVGPoint()
{
}
