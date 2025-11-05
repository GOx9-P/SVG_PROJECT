#include "stdafx.h"
#include "Circle.h"

SVGCircle::SVGCircle() {
	setR(0);
}

void SVGCircle::setR(float newR) {
	r = newR;
}

float SVGCircle::getR() const {
	return r;
}

void SVGCircle::parseAttributes(xml_node<>* Node)
{
	GeometricElement::parseAttributes(Node);
	if (xml_attribute<>* attribute = Node->first_attribute("r"))
	{
		this->setR((atof(attribute->value())));
	}
}


void SVGCircle::draw(Graphics* graphics)
{
	Color fillColor = { getFill().getA(),getFill().getR(),getFill().getG(),getFill().getB()};
	SolidBrush brush(fillColor);
	Color fillColorWidth = { getStroke().getColor().getA(),getStroke().getColor().getR() ,getStroke().getColor().getG() ,getStroke().getColor().getB() };
	Pen pen = { fillColorWidth,getStroke().getWidth()};
	graphics->FillEllipse(&brush, getPosition().getX()-r, getPosition().getY()-r, 2*r, 2*r);
	graphics->DrawEllipse(&pen, getPosition().getX() - r, getPosition().getY() - r, 2 * r, 2 * r);
}

SVGCircle::~SVGCircle()
{
}
