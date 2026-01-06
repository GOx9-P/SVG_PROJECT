#include "stdafx.h"
#include "Rect.h"
#include "SVGElement.h"

void AddRoundedRectangle(GraphicsPath& path, Gdiplus::RectF bounds, float rx, float ry)
{
	if (rx > bounds.Width / 2.0f) rx = bounds.Width / 2.0f;
	if (ry > bounds.Height / 2.0f) ry = bounds.Height / 2.0f;


	float dX = rx * 2;
	float dY = ry * 2;

	
	path.AddArc(bounds.X, bounds.Y, dX, dY, 180, 90);
	
	path.AddArc(bounds.X + bounds.Width - dX, bounds.Y, dX, dY, 270, 90);
	
	path.AddArc(bounds.X + bounds.Width - dX, bounds.Y + bounds.Height - dY, dX, dY, 0, 90);
	
	path.AddArc(bounds.X, bounds.Y + bounds.Height - dY, dX, dY, 90, 90);

	path.CloseFigure();
}

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

Gdiplus::RectF SVGRect::getBoundingBox() {
	 Gdiplus::RectF raw(
		this->getPosition().getX(),
		this->getPosition().getY(),
		this->getWidth(),
		this->getHeight()
	);
	 return raw;
	
}

void SVGRect::draw(Graphics* graphics)
{
	
	Gdiplus::RectF bounds = this->getBoundingBox();

	float effectiveRx = this->getRx();
	float effectiveRy = this->getRy();

	
	if (effectiveRx > 0 && effectiveRy == 0) effectiveRy = effectiveRx;
	
	if (effectiveRy > 0 && effectiveRx == 0) effectiveRx = effectiveRy;

	Brush* brush = this->createBrush(bounds);

	
	SVGStroke stroke = this->getStroke();
	SVGColor strokeColor = stroke.getColor();
	Color GDIStrokeColor(strokeColor.getA(), strokeColor.getR(), strokeColor.getG(), strokeColor.getB());

	Pen* pen = new Pen(GDIStrokeColor, stroke.getWidth());
	pen->SetLineCap(stroke.getLineCap(), stroke.getLineCap(), DashCapRound);
	pen->SetLineJoin(stroke.getLineJoin());

	
	if (effectiveRx > 0 || effectiveRy > 0)
	{
		
		GraphicsPath path;
		AddRoundedRectangle(path, bounds, effectiveRx, effectiveRy);

		if (brush) {
			graphics->FillPath(brush, &path);
		}
		if (stroke.getWidth() > 0 && strokeColor.getA() > 0) {
			graphics->DrawPath(pen, &path);
		}
	}
	else
	{
		
		if (brush) {
			graphics->FillRectangle(brush, bounds);
		}
		if (stroke.getWidth() > 0 && strokeColor.getA() > 0) {
			graphics->DrawRectangle(pen, bounds);
		}
	}

	if (brush) delete brush;
	delete pen;
}

SVGRect::~SVGRect()
{
}