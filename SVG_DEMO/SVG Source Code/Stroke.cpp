#include "stdafx.h"
#include "Stroke.h"

SVGStroke::SVGStroke() {
	this->width = 0.0f;
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

void SVGStroke::setColor(SVGColor& c)
{
	this->color = c;
}

void SVGStroke::setWidth(float w)
{
	this->width = w;
}

bool SVGStroke::isVisible()
{
	return this->width > 0.0f;
}
