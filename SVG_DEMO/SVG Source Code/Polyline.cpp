#include "stdafx.h"
#include "Polyline.h"

void SVGPolyline::setPoints(const vector<SVGPoint>& newPoints) {
	points = newPoints;
}

vector<SVGPoint> SVGPolyline::getPoints() const {
	return points;
}

void SVGPolyline::parseAttributes(xml_node<>* Node)
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

void SVGPolyline::draw()
{
}

SVGPolyline::~SVGPolyline()
{
}
