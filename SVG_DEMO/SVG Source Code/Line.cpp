#include "stdafx.h"
#include "Line.h"

void SVGLine::setPosition1(SVGPoint& newP1) {
	p1 = newP1;
}
SVGPoint SVGLine::getPosition1(SVGPoint&) const {
	return p1;
}
void SVGLine::setPosition2(SVGPoint& newP2) {
	p2 = newP2;
}
SVGPoint SVGLine::getPosition2(SVGPoint&) const {
	return p2;
}

void SVGLine::parseAttributes(xml_node<>* Node)
{
	SVGElement::parseAttributes(Node);
	if (xml_attribute<>* attribute = Node->first_attribute("x1"))
	{
		float newX1 = atof(attribute->value());
		p1.setX(newX1);
	}
	if (xml_attribute<>* attribute = Node->first_attribute("y1"))
	{
		float newY1 = atof(attribute->value());
		p1.setY(newY1);
	}
	if (xml_attribute<>* attribute = Node->first_attribute("x2"))
	{
		float newX2 = atof(attribute->value());
		p2.setX(newX2);
	}
	if (xml_attribute<>* attribute = Node->first_attribute("y2"))
	{
		float newY2 = atof(attribute->value());
		p2.setY(newY2);
	}
}

void SVGLine::draw()
{
}

SVGLine::~SVGLine()
{
}
