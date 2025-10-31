#pragma once

#include "GeometricElement.h"

class SVGPolyline : public GeometricElement {
private:
	vector<SVGPoint> points;
public:
	void parseAttributes(xml_node<>*);
	void draw();
	~SVGPolyline();
};