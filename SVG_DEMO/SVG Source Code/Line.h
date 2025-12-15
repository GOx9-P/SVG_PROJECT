#ifndef _LINE_H_
#define _LINE_H_

#include "GeometricElement.h"

class SVGLine : public GeometricElement {
private:
	SVGPoint p1, p2;
public:
	SVGLine();
	SVGPoint getPosition1() const;
	void setPosition1(SVGPoint&);
	SVGPoint getPosition2() const;
	void setPosition2(SVGPoint&);
	void parseAttributes(xml_node<>*);
	void draw(Graphics* graphics);
	Gdiplus::RectF getBoundingBox() override;
	~SVGLine();
};

#endif // !_LINE_H_


