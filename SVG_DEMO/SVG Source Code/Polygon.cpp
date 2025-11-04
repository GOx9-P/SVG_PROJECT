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
		vector<SVGPoint> parsedPoints;
		size_t start = 0;
		size_t end = pointsStr.find(' ');
		while (end != string::npos)
		{
			string pointStr = pointsStr.substr(start, end - start);
			size_t commaPos = pointStr.find(',');
			if (commaPos != string::npos)
			{
				float x = atof(pointStr.substr(0, commaPos).c_str());
				float y = atof(pointStr.substr(commaPos + 1).c_str());
				SVGPoint point;
				point.setX(x);
				point.setY(y);
				parsedPoints.push_back(point);
			}
			start = end + 1;
			end = pointsStr.find(' ', start);
		}
		string pointStr = pointsStr.substr(start);
		size_t commaPos = pointStr.find(',');
		if (commaPos != string::npos)
		{
			float x = atof(pointStr.substr(0, commaPos).c_str());
			float y = atof(pointStr.substr(commaPos + 1).c_str());
			SVGPoint point;
			point.setX(x);
			point.setY(y);
			parsedPoints.push_back(point);
		}
		this->setPoints(parsedPoints);
	}
}

void SVGPolygon::draw(Graphics* graphics)
{
	size_t numPoints = this->points.size();
	SVGColor fill_color = this->getFill();
	Color fillColor = { fill_color.getA(), fill_color.getR(), fill_color.getG(), fill_color.getB() };
	// tao brush cho fill
	SolidBrush fillBrush = fillColor;
	SVGStroke stroke = this->getStroke();
	SVGColor stroke_color = stroke.getColor();
	Color strokeColor = { stroke_color.getA(), stroke_color.getR(), stroke_color.getG(), stroke_color.getB() };
	// tao pen cho stroke
	Pen strokePen = { strokeColor, stroke.getWidth() };
	// ep ve point theo gdi
	vector<PointF> GDIPoints;
	for (auto p : points) {
		GDIPoints.push_back({ p.getX(), p.getY() });
	}
	graphics->FillPolygon(&fillBrush, GDIPoints.data(), static_cast<INT>(numPoints));
	graphics->DrawPolygon(&strokePen, GDIPoints.data(), static_cast<INT>(numPoints));
}

SVGPolygon::~SVGPolygon()
{
}
