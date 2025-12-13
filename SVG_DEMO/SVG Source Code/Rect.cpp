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

Gdiplus::RectF SVGRect::getBoundingBox() {
    return Gdiplus::RectF(
        this->getPosition().getX(),
        this->getPosition().getY(),
        this->getWidth(),
        this->getHeight()
    );
}

void SVGRect::draw(Graphics* graphics)
{
	// 1. Lấy khung bao
	Gdiplus::RectF bounds = this->getBoundingBox();

	// 2. Tạo Brush (Tự động chọn Solid hoặc Gradient)
	Brush* brush = this->createBrush(bounds);

	// 3. Tạo Pen cho viền
	SVGStroke stroke = this->getStroke();
	SVGColor strokeColor = stroke.getColor();
	Color GDIStrokeColor(strokeColor.getA(), strokeColor.getR(), strokeColor.getG(), strokeColor.getB());

	Pen* pen = new Pen(GDIStrokeColor, stroke.getWidth());
	pen->SetLineCap(stroke.getLineCap(), stroke.getLineCap(), DashCapRound);
	pen->SetLineJoin(stroke.getLineJoin());

	// 4. Vẽ (Hỗ trợ bo góc nếu cần - ở đây vẽ đơn giản trước)
	// Lưu ý: Nếu muốn vẽ bo góc (Rounded Rect) với Gradient chính xác, 
	// bạn cần dùng GraphicsPath, nhưng Rect thường thì FillRectangle là đủ.

	if (brush) { // Chỉ fill nếu có màu/gradient
		graphics->FillRectangle(brush, bounds);
	}

	if (stroke.getWidth() > 0 && strokeColor.getA() > 0) {
		graphics->DrawRectangle(pen, bounds);
	}

	if (brush) delete brush;
	delete pen;
}
	
SVGRect::~SVGRect()
{
}
