#ifndef _TEXTELEMENT_H_
#define _TEXTELEMENT_H_

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

	TextElement();
	TextElement(const SVGPoint&, const string&, const  string&, const float&, const string&);
	TextElement(const TextElement&);

	SVGPoint getPosition() const;
	void setPosition(const SVGPoint&);

	string getTextContent() const;
	void setTextContent(const string&);

	string getFontFamily() const;
	void setFontFamily(const string&);

	float getFontSize() const;
	void setFontSize(const float&);

	string getTextAnchor() const;
	void setTextAnchor(const string&);

	void parseAttributes(xml_node<>*);
	void draw(Graphics* graphics);

	Gdiplus::RectF getBoundingBox() override;
	~TextElement();
};

#endif // !_TEXTELEMENT_H_

