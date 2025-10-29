#pragma once
#include "GeometricElement.h"

class Rect : public GeometricElement {
private:
	float width;
	float height;
	float rx, ry;
public:
	void parseAttributes(xml_node<>*);
	void draw();
	~Rect();
};
