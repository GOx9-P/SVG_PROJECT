#pragma once
#include "GeometricElement.h"

class SVGRect : public GeometricElement {
private:
	float width;
	float height;
	float rx, ry;
public:
	void parseAttributes(xml_node<>*);
	void draw();
	~SVGRect();
};
