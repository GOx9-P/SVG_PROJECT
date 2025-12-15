#ifndef _SVGGROUP_H_
#define _SVGGROUP_H_

#include "SVGElement.h"

class SVGGroup : public SVGElement {
private:
	vector<SVGElement*> group;
public:
	SVGGroup();
	SVGGroup(const SVGGroup&);
	SVGGroup(const vector<SVGElement*>);
	void parseAttributes(xml_node<>* node);
	void draw(Graphics* graphics);
	void addElement(SVGElement* element); //Them 1 con vao nhom
	bool isGroup() const override;
	Gdiplus::RectF getBoundingBox() override;
	~SVGGroup();
};

#endif

