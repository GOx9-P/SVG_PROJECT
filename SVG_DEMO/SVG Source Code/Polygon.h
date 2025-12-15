#ifndef _POLYGON_H_
#define _POLYGON_H_

#include "GeometricElement.h"

class SVGPolygon : public GeometricElement {
private:
	vector<SVGPoint> points;
public:
	SVGPolygon() = default;
	void setPoints(const vector<SVGPoint>& newPoints);
	vector<SVGPoint> getPoints() const;
	void parseAttributes(xml_node<>*);
	void draw(Graphics*);
	Gdiplus::RectF getBoundingBox() override;
	~SVGPolygon();
};

#endif // !_POLYGON_H_