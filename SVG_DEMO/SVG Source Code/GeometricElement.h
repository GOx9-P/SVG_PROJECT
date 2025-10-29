#pragma once
#include "SVGElement.h"
#include "SVGPoint.h"

class GeometricElement : public SVGElement {
private:
	SVGPoint position;
public:
	void draw();
	void parseAttributes(xml_node<>*);
	~GeometricElement();
};
