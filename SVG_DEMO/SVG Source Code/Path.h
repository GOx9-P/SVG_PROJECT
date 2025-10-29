#pragma once

#include "GeometricElement.h"

class Path : public GeometricElement {
private:
	string d;
public:
	void parseAttributes(xml_node<>*);
	void draw();
	~Path();
};