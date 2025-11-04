#pragma once

#include "GeometricElement.h"

class SVGLine : public GeometricElement {
private:
	SVGPoint p1, p2;
public:
	SVGLine();
	SVGPoint getPosition1(SVGPoint&) const;
	void setPosition1(SVGPoint&);
	SVGPoint getPosition2(SVGPoint&) const;
	void setPosition2(SVGPoint&);
	void parseAttributes(xml_node<>*);
	void draw(Graphics* graphics);
	~SVGLine();
};