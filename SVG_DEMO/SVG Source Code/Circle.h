#pragma once

#include "GeometricElement.h"

class SVGCircle : public GeometricElement {
private:
	float r;
public:
	SVGCircle();
	float getR() const;
	void setR(float);
	void parseAttributes(xml_node<>*);
	void draw();
	~SVGCircle();
};