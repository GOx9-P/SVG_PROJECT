#pragma once

#include "Library.h"
#include "Color.h"
#include "Stroke.h"

class SVGElement {
private:
	string id;
	string className;
	string style;
	string transform;
	SVGColor fill;
	SVGStroke stroke;
	float opacity;
public:
	string getId() const;
	void setId(const string&);

	string getClassName() const;
	void setClassName(const string&);

	string getStyle() const;
	void setStyle(const string&);

	string getTransform() const;
	void setTransform(const string&);

	string getFill() const;
	void setFill(const string&);

	SVGStroke getStroke() const;
    void setStroke(const SVGStroke&);

	SVGColor getColor() const;
	void setColor(const SVGColor&);

	float getOpacity() const;
	void setOpacity(const float&);
	
	

	virtual void parseAttributes(xml_node<>*) = 0;
	virtual void draw() = 0;

	virtual ~SVGElement();
};