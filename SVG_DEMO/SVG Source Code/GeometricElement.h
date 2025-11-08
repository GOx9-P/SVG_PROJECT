#ifndef _GEOMETRIC_ELEMENT_H_
#define _GEOMETRIC_ELEMENT_H_

#include "SVGElement.h"
#include "SVGPoint.h"

class GeometricElement : public SVGElement {
private:
	SVGPoint position;
public:
	GeometricElement();
	void setPosition(SVGPoint&);
	SVGPoint getPosition() const;
	void draw();
	void parseAttributes(xml_node<>*);
	~GeometricElement();
};

#endif // 

