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
	SVGPoint getPosition() const;
	void setPosition(SVGPoint&);

	string getTextContent() const;
	void setTextContent(string&);

	string getFontFamily() const;
	void setFontFamily(string&);

	float getFontSize() const;
	void setFontSize(float&);

	string getTextAnchor() const;
	void setTextAnchor(string&);
	
	void parseAttributes(xml_node<>*);
	void draw();
	~TextElement();
};