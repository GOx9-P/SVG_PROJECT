#include "stdafx.h"
#include "Rect.h"
#include "SVGElement.h"

SVGRect::SVGRect() {
	setWidth(0);
	setHeight(0);
	setRx(0);
	setRy(0);
}

void SVGRect::setWidth(const float& newWidth)
{
	width = newWidth;
}

void SVGRect::setHeight(const float& newHeight)
{
	height = newHeight;
}

void SVGRect::setRx(const float& newRx)
{
	rx = newRx;
}

void SVGRect::setRy(const float& newRy) {
	ry = newRy;
}

float SVGRect::getWidth() const
{
	return width;
}

float SVGRect::getHeight() const
{
	return height;
}	

float SVGRect::getRx() const
{
	return rx;
}

float SVGRect::getRy() const
{
	return ry;
}

void SVGRect::parseAttributes(xml_node<>* Node)
{
	GeometricElement::parseAttributes(Node);
	if (xml_attribute<>* attribute = Node->first_attribute("width"))
	{
		this->setWidth(atof(attribute->value()));
	}
	if (xml_attribute<>* attribute = Node->first_attribute("height"))
	{
		this->setHeight(atof(attribute->value()));
	}
	if (xml_attribute<>* attribute = Node->first_attribute("rx"))
	{
		setRx(atof(attribute->value()));
	}
	if (xml_attribute<>* attribute = Node->first_attribute("ry"))
	{
		float val = atof(attribute->value());
		setRy(val);
	}
}

void SVGRect::draw(Graphics* graphics)
{
	SVGColor fillColor = this->getFill();
	Color GDIFillColor(fillColor.getA(), fillColor.getR(), fillColor.getG(), fillColor.getB());
	  


	Brush* brush = new SolidBrush(GDIFillColor); // tao brush cho fill, DUNG CON TRO DE CO THE INHERITENCE TRONG GDI+, chuan GDI+

	SVGStroke stroke = this->getStroke();
	SVGColor strokeColor = stroke.getColor();

	Color GDIStrokeColor(strokeColor.getA(), strokeColor.getR(), strokeColor.getG(), strokeColor.getB());

	Pen* pen = new Pen(GDIStrokeColor, stroke.getWidth()); // tao pen cho stroke
	pen->SetLineCap(getStroke().getLineCap(), getStroke().getLineCap(), DashCapRound);
	pen->SetLineJoin(getStroke().getLineJoin());

	if (fillColor.getA() > 0)// chi ve khi a lon hon khong de toi uu 
	graphics->FillRectangle(brush, this->getPosition().getX(), this->getPosition().getY(), this->getWidth(), this->getHeight()); // fill

	if (stroke.getWidth() > 0 && strokeColor.getA() > 0) // ve vien neu vien co
	graphics->DrawRectangle(pen, this->getPosition().getX(), this->getPosition().getY(), this->getWidth(), this->getHeight()); // stroke

	delete brush;
	delete pen;
}
	
SVGRect::~SVGRect()
{
}
