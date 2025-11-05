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
	//GeometricElement::parseAttributes(Node);
	SVGEllipse::parseAttributes(Node);
	if (xml_attribute<>* attribute = Node->first_attribute("r"))
	{
		this->setR((atof(attribute->value())));
	}
}


void SVGCircle::draw()
{
	Color fillColor = { getFill().getA(),getFill().getR(),getFill().getG(),getFill().getB()};
	SolidBrush brush(fillColor);
	Color fillColorWidth = { getStroke().getColor().getA(),getStroke().getColor().getR() ,getStroke().getColor().getG() ,getStroke().getColor().getB() };
	Pen pen = { fillColorWidth,getStroke().getWidth()};
	float diameter = 2 * r;
	float x = getRx() - r;
	float y = getRy() - r;
	graphics->FillEllipse(&brush, x, y, diameter, diameter);
	graphics->DrawEllipse(&pen, x, y, diameter, diameter);
}

SVGCircle::~SVGCircle()
{
}
