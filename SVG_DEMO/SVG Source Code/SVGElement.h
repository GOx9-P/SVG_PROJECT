#pragma once

#include "Library.h"

class SVGElement {
private:
	string id;
	string className;
	string style;
	string transform;
	string fill;
	string stroke;
	float strokeWidth;
	float opacity;
public:
    string getId() const;
	void setId(string&);

	string getClassName() const;
	void setClassName(string&);

	string getStyle() const;
	void setStyle(string&);

	string getTransform() const;
	void setTransform(string&);

	string getFill() const ;
	void setFill(string&);

	string getStroke() const;
	void setStroke(string&);

	float getStrokeWidth() const;
	void setStrokeWidth(float&);

	float getOpacity() const;
	void setOpacity(float&);

	virtual void parseAttributes(xml_node<>*) = 0;
	virtual void draw() = 0;

	virtual ~SVGElement();
};