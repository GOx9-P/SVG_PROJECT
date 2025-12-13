#include "stdafx.h"
#include "Polygon.h"

void SVGPolygon::setPoints(const vector<SVGPoint>& newPoints) {
	points = newPoints;
}

vector<SVGPoint> SVGPolygon::getPoints() const {
	return points;
}

Gdiplus::RectF SVGPolygon::getBoundingBox() {
	if (points.empty()) return Gdiplus::RectF(0, 0, 0, 0);

	float minX = points[0].getX(), maxX = points[0].getX();
	float minY = points[0].getY(), maxY = points[0].getY();

	for (const auto& p : points) {
		if (p.getX() < minX) minX = p.getX();
		if (p.getX() > maxX) maxX = p.getX();
		if (p.getY() < minY) minY = p.getY();
		if (p.getY() > maxY) maxY = p.getY();
	}
	return Gdiplus::RectF(minX, minY, maxX - minX, maxY - minY);
}

void SVGPolygon::parseAttributes(xml_node<>* Node)
{
	GeometricElement::parseAttributes(Node);
	if (xml_attribute<>* attribute = Node->first_attribute("points"))
	{
		string pointsStr = attribute->value();
		for (size_t i = 0; i < pointsStr.size(); ++i) {
			if (pointsStr[i] == ',') {
				pointsStr[i] = ' ';
			}
		}
		stringstream ss(pointsStr);
		vector<SVGPoint> parsedPoints;
		float x, y;
		SVGPoint point;
		while (ss >> x >> y) {
			point.setX(x);
			point.setY(y);
			parsedPoints.push_back(point);
		}
		this->setPoints(parsedPoints);
	}
}

void SVGPolygon::draw(Graphics* graphics)
{
	if (points.empty()) return;

	Gdiplus::RectF bounds = this->getBoundingBox();
	Brush* brush = this->createBrush(bounds); // Hỗ trợ Gradient cho Polygon

	SVGStroke stroke = this->getStroke();
	SVGColor sColor = stroke.getColor();
	Color strokeColor = { sColor.getA(), sColor.getR(), sColor.getG(), sColor.getB() };

	Pen* pen = new Pen(strokeColor, stroke.getWidth());
	pen->SetLineCap(stroke.getLineCap(), stroke.getLineCap(), DashCapRound);
	pen->SetLineJoin(stroke.getLineJoin());

	vector<PointF> GDIPoints;
	for (auto p : points) GDIPoints.push_back({ p.getX(), p.getY() });

	if (brush) graphics->FillPolygon(brush, GDIPoints.data(), (INT)GDIPoints.size(), FillModeWinding);
	if (stroke.getWidth() > 0 && sColor.getA() > 0)
		graphics->DrawPolygon(pen, GDIPoints.data(), (INT)GDIPoints.size());

	if (brush) delete brush;
	delete pen;
}

SVGPolygon::~SVGPolygon()
{
}
