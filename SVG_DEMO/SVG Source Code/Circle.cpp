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
	if (getFill().getA() == 0 && !getFill().isNone() && getStroke().getWidth() == 0)
	{
		fillColor = Color(255, 0, 0, 0);
	}
	Brush* brush = new SolidBrush(fillColor);

	Color fillColorWidth = { getStroke().getColor().getA(),getStroke().getColor().getR() ,getStroke().getColor().getG() ,getStroke().getColor().getB() };
	Pen* pen = new Pen(fillColorWidth, getStroke().getWidth());
	pen->SetLineCap(getStroke().getLineCap(), getStroke().getLineCap(), DashCapRound);
	pen->SetLineJoin(getStroke().getLineJoin());

	graphics->FillEllipse(brush, getPosition().getX()-r, getPosition().getY()-r, 2*r, 2*r);
	graphics->DrawEllipse(pen, getPosition().getX() - r, getPosition().getY() - r, 2 * r, 2 * r);

	delete brush;
	delete pen;
}

SVGCircle::~SVGCircle()
{
}
