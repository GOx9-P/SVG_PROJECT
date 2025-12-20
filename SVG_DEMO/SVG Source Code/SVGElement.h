#ifndef _SVGELEMENT_H_
#define _SVGELEMENT_H_

#include "Library.h"
#include "Color.h"
#include "Stroke.h"
#include "SVGPoint.h"

class SVGElement {
private:
	string id;
	string className;
	string style;
	string transform;
	SVGColor fill;
	SVGStroke stroke;
	float opacity;

protected:
	Gdiplus::Matrix* transformMatrix;
	float fillOpacity;
	float strokeOpacity;
	bool isFillOpSet;
	bool isStrokeOpSet;

	Gdiplus::Brush* createBrush(Gdiplus::RectF bounds);

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
	float getFillOpacity() const;
	void setFillOpacity(const float&);
	float getStrokeOpacity() const;
	void setStrokeOpacity(const float&);
	bool isFillOpacitySet() const;
	void setIsFillOpacitySet(bool);
	bool isStrokeOpacitySet() const;
	void setIsStrokeOpacitySet(bool);

	virtual bool isGroup() const;

	void parseTransform();
	void render(Gdiplus::Graphics* graphics);

	virtual void parseAttributes(xml_node<>*) = 0;
	virtual void draw(Graphics* graphics) = 0;

	// Bắt buộc các hình con phải tự tính khung bao của nó
	virtual Gdiplus::RectF getBoundingBox() = 0;

	virtual ~SVGElement();
};

#endif