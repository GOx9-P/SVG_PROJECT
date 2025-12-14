#ifndef _CIRCLE_H_
#define _CIRCLE_H_

#include "GeometricElement.h"

class SVGCircle : public GeometricElement {
private:
	float r;
public:
	SVGCircle();
	float getR() const;
	void setR(float);
	void parseAttributes(xml_node<>*);
	void draw(Graphics* graphics);
	Gdiplus::RectF getBoundingBox() override;
	~SVGCircle();
};

#endif
