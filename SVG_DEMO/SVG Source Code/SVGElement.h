#pragma once

#include "Library.h"

class SVGElement {
private:
	string id;
	string className;
	string style;
	string transform;
	string fill;
	string stroke;
	float strokeWidth;
	float opacity;
public:
	virtual void parseAttributes(xml_node<>*) = 0;
	virtual void draw() = 0;

	virtual ~SVGElement();
};