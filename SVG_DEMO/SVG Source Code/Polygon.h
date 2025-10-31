#pragma once

#include "GeometricElement.h"

class SVGPolygon : public GeometricElement {
private:
	vector<SVGPoint> points;
public:
	void parseAttributes(xml_node<>*);
	void draw();
	~SVGPolygon();
};