#ifndef _RECT_H_
#define _RECT_H_

#include "GeometricElement.h"

class SVGRect : public GeometricElement {
private:
	float width;
	float height;
	float rx, ry;
public:
	SVGRect();
	float getWidth() const;
	float getHeight() const;
	float getRx() const;
	float getRy() const;
	void setWidth(const float&);
	void setHeight(const float&);
	void setRx(const float&);
	void setRy(const float&);
	void parseAttributes(xml_node<>*);
	void draw(Graphics* graphics);
	Gdiplus::RectF getBoundingBox() override;
	~SVGRect();
};

#endif // !_RECT_H_

