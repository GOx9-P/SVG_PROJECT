#include "stdafx.h"
#include "GeometricElement.h"

GeometricElement::GeometricElement() {}

void GeometricElement::setPosition(SVGPoint& newPosition) {
	position = newPosition;
}

SVGPoint GeometricElement::getPosition() const {
	return position;
}

void GeometricElement::draw() 
{

}

void GeometricElement::parseAttributes(xml_node<>* Node) 
{
	SVGElement::parseAttributes(Node);
	if (xml_attribute<>* attribute = Node->first_attribute("x"))
	{
		float newX = atof(attribute->value());
		position.setX(newX);
	}
	// chi dung de doc toa do tam cua circle va elip vi no se khong doc duoc x va y
	if (xml_attribute<>* attribute = Node->first_attribute("cx"))
	{
		float newX = atof(attribute->value());
		position.setX(newX);
	}
	if (xml_attribute<>* attribute = Node->first_attribute("y"))
	{
		float newY = atof(attribute->value());
		position.setY(newY);
	}
	// chi dung de doc toa do tam cua circle va elip vi no se khong doc duoc x va y
	if (xml_attribute<>* attribute = Node->first_attribute("cy"))
	{
		float newY = atof(attribute->value());
		position.setY(newY);
	}

}

GeometricElement::~GeometricElement()
{
}

	



