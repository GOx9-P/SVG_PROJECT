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

Gdiplus::RectF SVGCircle::getBoundingBox() {
	float r = this->getR();
	// Khung bao của hình tròn là hình vuông ngoại tiếp nó
	// Toạ độ góc trái trên = Tâm - Bán kính
	 Gdiplus::RectF raw(
		this->getPosition().getX() - r,
		this->getPosition().getY() - r,
		2 * r,
		2 * r
	);
	 return this->TransformRect(raw);
}

void SVGCircle::draw(Graphics* graphics)
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

SVGCircle::~SVGCircle()
{
}
