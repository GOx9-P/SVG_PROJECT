#include "stdafx.h"
#include "Polygon.h"

void SVGPolygon::setPoints(const vector<SVGPoint>& newPoints) {
	points = newPoints;
}

vector<SVGPoint> SVGPolygon::getPoints() const {
	return points;
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
		vector<SVGPoint> parsePoints;
		float x, y;
		SVGPoint point;
		while (ss >> x >> y) {
			point.setX(x);
			point.setY(y);
			parsePoints.push_back(point);
		}
		this->setPoints(parsePoints);
	}
}

void SVGPolygon::draw(Graphics* graphics)
{
	size_t numPoints = this->points.size();
	SVGColor fill_color = this->getFill();
	Color fillColor = { fill_color.getA(), fill_color.getR(), fill_color.getG(), fill_color.getB()};
	// tao brush cho fill
	Brush* fillBrush = new SolidBrush(fillColor);
	SVGStroke stroke = this->getStroke();
	SVGColor stroke_color = stroke.getColor();
	Color strokeColor = { stroke_color.getA(), stroke_color.getR(), stroke_color.getG(), stroke_color.getB()};
	// tao pen cho stroke
	Pen* strokePen =  new Pen(strokeColor, stroke.getWidth());
	// ep ve point theo gdi
	vector<PointF> GDIPoints;
	for (auto p : points) {
		GDIPoints.push_back({ p.getX(), p.getY() });
	}
	graphics->FillPolygon(fillBrush, GDIPoints.data(), static_cast<INT>(numPoints));
	graphics->DrawPolygon(strokePen, GDIPoints.data(), static_cast<INT>(numPoints));

	delete fillBrush;
	delete strokePen;
}

SVGPolygon::~SVGPolygon()
{
}
