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

void SVGEllipse::draw(Graphics* graphics)
{
	Color fillColor = { getFill().getA(),getFill().getR(),getFill().getG(),getFill().getB() };
	if (getFill().getA() == 0 && !getFill().isNone() && getStroke().getWidth() == 0)
	{
		fillColor = Color(255, 0, 0, 0);
	}
	Brush* brush = new SolidBrush(fillColor);
	Color fillColorWidth = { getStroke().getColor().getA(),getStroke().getColor().getR() ,getStroke().getColor().getG() ,getStroke().getColor().getB() };

	Pen* pen = new Pen(fillColorWidth,getStroke().getWidth());
	pen->SetLineCap(getStroke().getLineCap(), getStroke().getLineCap(), DashCapRound);
	pen->SetLineJoin(getStroke().getLineJoin());
	graphics->FillEllipse(brush, getPosition().getX()-rx,getPosition().getY()- ry, 2 * rx, 2 * ry);
	graphics->DrawEllipse(pen, getPosition().getX() - rx, getPosition().getY() - ry, 2 * rx, 2 * ry);

	delete brush;
	delete pen;
}

SVGEllipse::~SVGEllipse()
{
}
