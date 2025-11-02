#pragma once

#include "GeometricElement.h"

class SVGPath : public GeometricElement {
private:
	string d;
public:
	SVGPath();
	SVGPath(const string&);
	SVGPath(const SVGPath&);
	string getD() const;
	void  setD(const string&);
	void parseAttributes(xml_node<>*);
	void draw();
	~SVGPath();
};