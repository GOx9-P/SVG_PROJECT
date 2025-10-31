#pragma once

#include "GeometricElement.h"

class SVGCircle : public GeometricElement {
private:
	float r;
public:
	void parseAttributes(xml_node<>*);
	void draw();
	~SVGCircle();
};