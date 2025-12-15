#ifndef _POLYLINE_H_
#define _POLYLINE_H_

#include "GeometricElement.h"

class SVGPolyline : public GeometricElement {
private:
	vector<SVGPoint> points;
public:
	SVGPolyline() = default;
	void setPoints(const vector<SVGPoint>& other);
	vector<SVGPoint> getPoints() const;
	void parseAttributes(xml_node<>*);
	void draw(Graphics*);
	Gdiplus::RectF getBoundingBox() override;
	~SVGPolyline();
};

#endif // !_POLYLINE_H_