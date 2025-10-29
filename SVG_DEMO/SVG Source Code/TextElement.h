#pragma once

#include "SVGElement.h"
#include "SVGPoint.h"

class TextElement : public SVGElement {
private:
	SVGPoint position;
	string textContent;
	string fontFamily;
	float fontSize;	
	string textAnchor;
public:
	void parseAttributes(xml_node<>*);
	void draw();
	~TextElement();
};