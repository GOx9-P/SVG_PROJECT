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

Gdiplus::RectF SVGEllipse::getBoundingBox() {
	float rx = this->getRx();
	float ry = this->getRy();
	 Gdiplus::RectF raw(
		this->getPosition().getX() - rx,
		this->getPosition().getY() - ry,
		2 * rx, 2 * ry
	);
	return this->TransformRect(raw);
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
	Gdiplus::RectF bounds = this->getBoundingBox();
	Brush* brush = this->createBrush(bounds);

	SVGStroke stroke = this->getStroke();
	SVGColor sColor = stroke.getColor();
	Color strokeColor = { sColor.getA(), sColor.getR(), sColor.getG(), sColor.getB() };

	Pen* pen = new Pen(strokeColor, stroke.getWidth());
	pen->SetLineCap(stroke.getLineCap(), stroke.getLineCap(), DashCapRound);
	pen->SetLineJoin(stroke.getLineJoin());

	if (brush) graphics->FillEllipse(brush, bounds);
	if (stroke.getWidth() > 0 && sColor.getA() > 0)
		graphics->DrawEllipse(pen, bounds);

	if (brush) delete brush;
	delete pen;
}

SVGEllipse::~SVGEllipse()
{
}
