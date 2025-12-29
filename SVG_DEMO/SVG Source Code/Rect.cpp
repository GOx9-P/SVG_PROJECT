#include "stdafx.h"
#include "Rect.h"
#include "SVGElement.h"

void AddRoundedRectangle(GraphicsPath& path, Gdiplus::RectF bounds, float rx, float ry)
{
	if (rx > bounds.Width / 2.0f) rx = bounds.Width / 2.0f;
	if (ry > bounds.Height / 2.0f) ry = bounds.Height / 2.0f;

	// Đường kính của góc bo
	float dX = rx * 2;
	float dY = ry * 2;

	// Bắt đầu vẽ 4 cung tròn ở 4 góc
	// 1. Góc trên-trái (Top-Left): 180 độ đến 270 độ
	path.AddArc(bounds.X, bounds.Y, dX, dY, 180, 90);

	// 2. Góc trên-phải (Top-Right): 270 độ đến 360 độ
	path.AddArc(bounds.X + bounds.Width - dX, bounds.Y, dX, dY, 270, 90);

	// 3. Góc dưới-phải (Bottom-Right): 0 độ đến 90 độ
	path.AddArc(bounds.X + bounds.Width - dX, bounds.Y + bounds.Height - dY, dX, dY, 0, 90);

	// 4. Góc dưới-trái (Bottom-Left): 90 độ đến 180 độ
	path.AddArc(bounds.X, bounds.Y + bounds.Height - dY, dX, dY, 90, 90);

	// Đóng hình (nối điểm cuối về đầu)
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
	 return this->TransformRect(raw);
}

void SVGRect::draw(Graphics* graphics)
{
	// 1. Lấy khung bao
	Gdiplus::RectF bounds = this->getBoundingBox();

	// 2. Xử lý Logic Rx, Ry theo chuẩn SVG
	float effectiveRx = this->getRx();
	float effectiveRy = this->getRy();

	// Nếu rx có nhưng ry không -> ry = rx
	if (effectiveRx > 0 && effectiveRy == 0) effectiveRy = effectiveRx;
	// Nếu ry có nhưng rx không -> rx = ry
	if (effectiveRy > 0 && effectiveRx == 0) effectiveRx = effectiveRy;

	// 3. Tạo Brush (Tự động chọn Solid hoặc Gradient từ lớp cha)
	Brush* brush = this->createBrush(bounds);

	// 4. Tạo Pen cho viền
	SVGStroke stroke = this->getStroke();
	SVGColor strokeColor = stroke.getColor();
	Color GDIStrokeColor(strokeColor.getA(), strokeColor.getR(), strokeColor.getG(), strokeColor.getB());

	Pen* pen = new Pen(GDIStrokeColor, stroke.getWidth());
	pen->SetLineCap(stroke.getLineCap(), stroke.getLineCap(), DashCapRound);
	pen->SetLineJoin(stroke.getLineJoin());

	// 5. Vẽ
	if (effectiveRx > 0 || effectiveRy > 0)
	{
		// TRƯỜNG HỢP CÓ BO GÓC: Dùng GraphicsPath
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
		// TRƯỜNG HỢP HÌNH CHỮ NHẬT THƯỜNG
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