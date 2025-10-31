#pragma once

#include "GeometricElement.h"

class SVGPath : public GeometricElement {
private:
	string d;
public:
	void parseAttributes(xml_node<>*);
	void draw();
	~SVGPath();
};