#include "stdafx.h"
#include "Stroke.h"

SVGStroke::SVGStroke() {
	this->width = 0.0f;
	this->lineCap = LineCapFlat;      
	this->lineJoin = LineJoinMiter;
};

SVGStroke::SVGStroke(const SVGColor& c, float w)
{
	this->color = c;
	this->width = w;
}

SVGColor& SVGStroke::getColor() 
{
	return this->color;
}

float SVGStroke::getWidth()
{
	return this->width;
}

LineCap SVGStroke::getLineCap() const 
{ 
	return lineCap; 
}

LineJoin SVGStroke::getLineJoin() const
{ 
	return lineJoin; 
}

void SVGStroke::setColor(SVGColor& c)
{
	this->color = c;
}

void SVGStroke::setWidth(float w)
{
	this->width = w;
}

void SVGStroke::setLineCap(const string& type) {
	if (type == "round") lineCap = LineCapRound;
	else if (type == "square") lineCap = LineCapSquare;
	else lineCap = LineCapFlat; 
}

void SVGStroke::setLineJoin(const string& type) {
	if (type == "round") lineJoin = LineJoinRound;
	else if (type == "bevel") lineJoin = LineJoinBevel;
	else lineJoin = LineJoinMiter;
}


bool SVGStroke::isVisible()
{
	return this->width > 0.0f;
}
