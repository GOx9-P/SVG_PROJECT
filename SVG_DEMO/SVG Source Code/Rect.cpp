#include "stdafx.h"
#include "Rect.h"
#include "SVGElement.h"

SVGRect::SVGRect() {
	setWidth(0);
	setHeight(0);
	setRx(0);
	setRy(0);
}

void SVGRect::setWidth(const float& newWidth)
{
	width = newWidth;
}

void SVGRect::setHeight(const float& newHeight)
{
	height = newHeight;
}

void SVGRect::setRx(const float& newRx)
{
	rx = newRx;
}

void SVGRect::setRy(const float& newRy) {
	ry = newRy;
}

float SVGRect::getWidth() const
{
	return width;
}

float SVGRect::getHeight() const
{
	return height;
}	

float SVGRect::getRx() const
{
	return rx;
}

float SVGRect::getRy() const
{
	return ry;
}

void SVGRect::parseAttributes(xml_node<>* Node)
{
	GeometricElement::parseAttributes(Node);
	if (xml_attribute<>* attribute = Node->first_attribute("width"))
	{
		this->setWidth(atof(attribute->value()));
	}
	if (xml_attribute<>* attribute = Node->first_attribute("height"))
	{
		this->setHeight(atof(attribute->value()));
	}
	if (xml_attribute<>* attribute = Node->first_attribute("rx"))
	{
		setRx(atof(attribute->value()));
	}
	if (xml_attribute<>* attribute = Node->first_attribute("ry"))
	{
		float val = atof(attribute->value());
		setRy(val);
	}
}

void SVGRect::draw(Graphics* graphics)
{
}

SVGRect::~SVGRect()
{
}
