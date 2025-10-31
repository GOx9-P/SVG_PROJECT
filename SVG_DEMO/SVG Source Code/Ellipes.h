#pragma once

#include "GeometricElement.h"

class SVGEllipse : public GeometricElement {
private:
	float rx;
	float ry;
public:
	void parseAttributes(xml_node<>*);
	void draw();
	~SVGEllipse();
};