#pragma once

#include "GeometricElement.h"

class SVGPolygon : public GeometricElement {
private:
	vector<SVGPoint> points;
public:
	void setPoints(const vector<SVGPoint>& newPoints);
	vector<SVGPoint> getPoints() const;
	void parseAttributes(xml_node<>*);
	void draw();
	~SVGPolygon();
};