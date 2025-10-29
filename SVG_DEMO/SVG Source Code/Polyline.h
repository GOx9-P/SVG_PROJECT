#pragma once

#include "GeometricElement.h"

class Polyline : public GeometricElement {
private:
	vector<SVGPoint> points;
public:
	void parseAttributes(xml_node<>*);
	void draw();
	~Polyline();
};