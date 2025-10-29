#pragma once

#include "GeometricElement.h"

class Line : public GeometricElement {
private:
	SVGPoint p1, p2;
public:
	void parseAttributes(xml_node<>*);
	void draw();
	~Line();
};