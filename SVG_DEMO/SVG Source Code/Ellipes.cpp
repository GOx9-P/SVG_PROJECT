#include "stdafx.h"
#include "Ellipes.h"

SVGEllipse::SVGEllipse() {
	setRx(0);
	setRy(0);
}

void SVGEllipse::setRx(const float& newRx) {
	rx = newRx;
}
float SVGEllipse::getRx() const {
	return rx;
}
void SVGEllipse::setRy(const float& newRy) {
	ry = newRy;
}
float SVGEllipse::getRy() const {
	return ry;
}

void SVGEllipse::parseAttributes(xml_node<>* Node)
{
	GeometricElement::parseAttributes(Node);
	if (xml_attribute<>* attribute = Node->first_attribute("cx"))
	{
		setRx(atof(attribute->value()));
	}
	if (xml_attribute<>* attribute = Node->first_attribute("cy"))
	{
		setRy(atof(attribute->value()));
	}
}

void SVGEllipse::draw(Graphics* graphics)
{
	Color fillColor = { getFill().getA(),getFill().getR(),getFill().getG(),getFill().getB() };
	SolidBrush brush(fillColor);
	Color fillColorWidth = { getStroke().getColor().getA(),getStroke().getColor().getR() ,getStroke().getColor().getG() ,getStroke().getColor().getB() };
	Pen pen = { fillColorWidth,getStroke().getWidth() };
	graphics->FillEllipse(&brush, rx, ry, 2*rx, 2*ry);
	graphics->DrawEllipse(&pen, rx, ry, 2 * rx, 2 * ry);
}

SVGEllipse::~SVGEllipse()
{
}
