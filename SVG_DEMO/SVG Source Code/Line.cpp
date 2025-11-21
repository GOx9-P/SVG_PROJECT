#include "stdafx.h"
#include "Line.h"

SVGLine::SVGLine() {
	SVGPoint p1, p2;
	this->p1 = p1;
	this->p2 = p2;
}

void SVGLine::setPosition1(SVGPoint& newP1) {
	p1 = newP1;
}
SVGPoint SVGLine::getPosition1() const {
	return p1;
}
void SVGLine::setPosition2(SVGPoint& newP2) {
	p2 = newP2;
}
SVGPoint SVGLine::getPosition2() const {
	return p2;
}

void SVGLine::parseAttributes(xml_node<>* Node)
{
	GeometricElement::parseAttributes(Node);
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

void SVGLine::draw(Graphics* graphics)
{
	SVGStroke stroke = this->getStroke();
	SVGColor StrokeColor = stroke.getColor();
	Color lineColor = { StrokeColor.getA(), StrokeColor.getR(), StrokeColor.getG(), StrokeColor.getB() };
	Pen* pen = new Pen(lineColor, stroke.getWidth());

	SVGPoint p1 = this->getPosition1();
	SVGPoint p2 = this->getPosition2();

	graphics->DrawLine(pen, p1.getX(), p1.getY(), p2.getX(), p2.getY());

	delete pen;
}

SVGLine::~SVGLine()
{
}
