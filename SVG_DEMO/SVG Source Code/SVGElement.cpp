#include "stdafx.h"
#include "SVGElement.h"

SVGElement::SVGElement()
{
    id = className = style = transform =  "";
    opacity = 0;
}

SVGElement::~SVGElement()
{
}

string SVGElement::getId() const {
    return id;
}

void SVGElement::setId(const string& newId) {
    id = newId;
}

string SVGElement::getClassName() const {
    return className;
}

void SVGElement::setClassName(const string& newClassName) {
    className = newClassName;
}

string SVGElement::getStyle() const {
    return style;
}

void SVGElement::setStyle(const string& newStyle) {
    style = newStyle;
}

string SVGElement::getTransform() const {
    return transform;
}

void SVGElement::setTransform(const string& newTransform) {
    transform = newTransform;
}

SVGColor SVGElement::getFill() const {
    return fill;
}

void SVGElement::setFill(const SVGColor& newFill) {
    fill = newFill;
}

SVGStroke SVGElement::getStroke() const {
    return stroke;
}

void SVGElement::setStroke(const SVGStroke& newStroke) {
    stroke = newStroke;
}

float SVGElement::getOpacity() const {
    return opacity;
}

void SVGElement::setOpacity(const float& newOpacity) {
    opacity = newOpacity;
}

//void SVGElement::parseAttributes(xml_node<>* Node)
//{
//    if (xml_attribute<>* attribute = Node->first_attribute("id"))
//    {
//        id = attribute->value();
//        cout << "id: " << id << ' ';
//    }
//    if (xml_attribute<>* attribute = Node->first_attribute("class"))
//    {
//        className = attribute->value();
//        cout << "class: " << className << ' ';
//    }
//    if (xml_attribute<>* attribute = Node->first_attribute("style"))
//    {
//        style = attribute->value();
//        cout << "style: " << style << ' ';
//    }
//    if (xml_attribute<>* attribute = Node->first_attribute("transform"))
//    {
//        transform = attribute->value();
//        cout << "transform: " << transform << ' ';
//    }
//    if (xml_attribute<>* attribute = Node->first_attribute("fill"))
//    {
//        string Tempcolor = attribute->value();
//        fill=SVGColor::fromString(Tempcolor);
//        //cout << "fill: " << Tempcolor << ' ';
//        cout << "fill: ";
//        cout << "r: " << (int)fill.getR() << " g: " << (int)fill.getG() << " B: " << (int)fill.getB() << ' ';
//    }
//    if (xml_attribute<>* attribute = Node->first_attribute("fill-opacity"))
//    {
//        fill.setA((BYTE)(atof(attribute->value())*255));
//        cout << "fill-opacity: " << (int)fill.getA() << ' ';
//    }
//    if (xml_attribute<>* attribute = Node->first_attribute("stroke"))
//    {
//        string TempColorStroke = attribute->value();
//        SVGColor temp = SVGColor::fromString(TempColorStroke);
//        stroke.setColor(temp);
//        cout << "stroke: ";
//        cout << "r: " << (int)stroke.getColor().getR() << " g: " << (int)stroke.getColor().getG() << " B: " <<(int) stroke.getColor().getB() << ' ';
//    }
//    if (xml_attribute<>* attribute = Node->first_attribute("stroke-width"))
//    {
//        stroke.setWidth(atof(attribute->value()));
//        cout << "stroke-width: " << stroke.getWidth() << ' ';
//    }
//    if (xml_attribute<>* attribute = Node->first_attribute("stroke-opacity"))
//    {
//        stroke.getColor().setA(BYTE(atof(attribute->value())*255));
//        cout << "stroke-opacity: " <<(int) stroke.getColor().getA() << ' ';
//    }
//}

void SVGElement::parseAttributes(xml_node<>* Node)
{
	if (xml_attribute<>* attribute = Node->first_attribute("id"))
		id = attribute->value();
	if (xml_attribute<>* attribute = Node->first_attribute("class"))
		className = attribute->value();
	if (xml_attribute<>* attribute = Node->first_attribute("style"))
		style = attribute->value();
	if (xml_attribute<>* attribute = Node->first_attribute("transform"))
		transform = attribute->value();

	// --- FIX 1: LOGIC FILL ---
	bool isFillNone = false;
	if (xml_attribute<>* attribute = Node->first_attribute("fill"))
	{
		string Tempcolor = attribute->value();
		if (Tempcolor == "none") isFillNone = true;
		fill = SVGColor::fromString(Tempcolor);
	}

	if (xml_attribute<>* attribute = Node->first_attribute("fill-opacity"))
	{
		float opacityVal = atof(attribute->value());
		// FIX: Bo dieu kien fill.getA() > 0. 
		// Cho phep set alpha ke ca khi mau la mac dinh (den/trong suot), 
		// MIEN LA khong phai "none" tuong minh.
		if (!isFillNone) {
			// Neu dang la 0 (mac dinh), ta cu set no thanh 255 * opacity 
			// de Group co the luu gia tri nay truyen cho con.
			float currentAlpha = (fill.getA() == 0) ? 255.0f : (float)fill.getA();
			fill.setA((BYTE)(currentAlpha * opacityVal));
		}
	}

	// --- FIX 2: LOGIC STROKE ---
	bool isStrokeNone = false;
	if (xml_attribute<>* attribute = Node->first_attribute("stroke"))
	{
		string TempColorStroke = attribute->value();
		if (TempColorStroke == "none") isStrokeNone = true;
		SVGColor temp = SVGColor::fromString(TempColorStroke);
		stroke.setColor(temp);
	}

	if (xml_attribute<>* attribute = Node->first_attribute("stroke-width"))
	{
		stroke.setWidth(atof(attribute->value()));
	}

	if (xml_attribute<>* attribute = Node->first_attribute("stroke-opacity"))
	{
		float opacityVal = atof(attribute->value());
		if (!isStrokeNone) {
			SVGColor c = stroke.getColor();
			float currentAlpha = (c.getA() == 0) ? 255.0f : (float)c.getA();
			c.setA((BYTE)(currentAlpha * opacityVal));
			stroke.setColor(c);
		}
	}

	// Chot ha do day vien (nhu cu)
	if (stroke.getColor().getA() > 0 && stroke.getWidth() <= 0.0f)
	{
		stroke.setWidth(1.0f);
	}
}