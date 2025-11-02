#include "stdafx.h"
#include "Circle.h"

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


void SVGCircle::draw()
{

}

SVGCircle::~SVGCircle()
{
}
