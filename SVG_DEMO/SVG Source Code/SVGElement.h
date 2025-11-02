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

	SVGElement();

	string getId() const;
	void setId(const string&);

	string getClassName() const;
	void setClassName(const string&);

	string getStyle() const;
	void setStyle(const string&);

	string getTransform() const;
	void setTransform(const string&);

	SVGColor getFill() const;
	void setFill(const SVGColor&);

	SVGStroke getStroke() const;
    void setStroke(const SVGStroke&);


	float getOpacity() const;
	void setOpacity(const float&);
	
	

	virtual void parseAttributes(xml_node<>*) = 0;
	virtual void draw() = 0;

	virtual ~SVGElement();
};