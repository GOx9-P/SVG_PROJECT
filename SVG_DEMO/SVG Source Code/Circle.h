#pragma once

#include "GeometricElement.h"

class Circle : public GeometricElement {
private:
	float r;
public:
	void parseAttributes(xml_node<>*);
	void draw();
	~Circle();
};