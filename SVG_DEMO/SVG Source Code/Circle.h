#pragma once

#include "Ellipes.h"

class SVGCircle : public SVGEllipse {
private:
	float r;
public:
	SVGCircle();
	float getR() const;
	void setR(float);
	void parseAttributes(xml_node<>*);
	void draw(Graphics* graphics);
	~SVGCircle();
};