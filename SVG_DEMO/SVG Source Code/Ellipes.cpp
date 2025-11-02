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
	if (xml_attribute<>* attribute = Node->first_attribute("rx"))
	{
		setRx(atof(attribute->value()));
	}
	if (xml_attribute<>* attribute = Node->first_attribute("ry"))
	{
		setRy(atof(attribute->value()));
	}
}

void SVGEllipse::draw() 
{
}

SVGEllipse::~SVGEllipse()
{
}
