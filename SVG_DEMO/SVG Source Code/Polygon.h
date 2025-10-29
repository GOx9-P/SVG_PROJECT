#pragma once

#include "GeometricElement.h"

class Polygon : public GeometricElement {
private:
	vector<SVGPoint> points;
public:
	void parseAttributes(xml_node<>*);
	void draw();
	~Polygon();
};