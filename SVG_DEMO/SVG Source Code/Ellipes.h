#ifndef _ELLIPES_H_
#define _ELLPIES_H_

#include "GeometricElement.h"

class SVGEllipse : public GeometricElement {
private:
	float rx;
	float ry;
public:
	SVGEllipse();
	void setRx(const float& other);
	float getRx() const;
	void setRy(const float& other);
	float getRy() const;
	void parseAttributes(xml_node<>*);
	void draw(Graphics* graphics);
	Gdiplus::RectF getBoundingBox() override;
	~SVGEllipse();
};

#endif

