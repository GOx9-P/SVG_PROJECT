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

void SVGElement::parseAttributes(xml_node<>* Node)
{
    if (xml_attribute<>* attribute = Node->first_attribute("id"))
    {
        id = attribute->value();
        cout << "id: " << id << ' ';
    }
    if (xml_attribute<>* attribute = Node->first_attribute("class"))
    {
        className = attribute->value();
        cout << "class: " << className << ' ';
    }
    if (xml_attribute<>* attribute = Node->first_attribute("style"))
    {
        style = attribute->value();
        cout << "style: " << style << ' ';
    }
    if (xml_attribute<>* attribute = Node->first_attribute("transform"))
    {
        transform = attribute->value();
        cout << "transform: " << transform << ' ';
    }
    if (xml_attribute<>* attribute = Node->first_attribute("fill"))
    {
        string Tempcolor = attribute->value();
        fill=SVGColor::fromString(Tempcolor);
        //cout << "fill: " << Tempcolor << ' ';
        cout << "fill: ";
        cout << "r: " << (int)fill.getR() << " g: " << (int)fill.getG() << " B: " << (int)fill.getB() << ' ';
    }
    if (xml_attribute<>* attribute = Node->first_attribute("fill-opacity"))
    {
        fill.setA((BYTE)(atof(attribute->value())*255));
        cout << "fill-opacity: " << (int)fill.getA() << ' ';
    }
    if (xml_attribute<>* attribute = Node->first_attribute("stroke"))
    {
        string TempColorStroke = attribute->value();
        SVGColor temp = SVGColor::fromString(TempColorStroke);
        stroke.setColor(temp);
        cout << "stroke: ";
        cout << "r: " << (int)stroke.getColor().getR() << " g: " << (int)stroke.getColor().getG() << " B: " <<(int) stroke.getColor().getB() << ' ';
    }
    if (xml_attribute<>* attribute = Node->first_attribute("stroke-width"))
    {
        stroke.setWidth(atof(attribute->value()));
        cout << "stroke-width: " << stroke.getWidth() << ' ';
    }
    if (xml_attribute<>* attribute = Node->first_attribute("stroke-opacity"))
    {
        stroke.getColor().setA(BYTE(atof(attribute->value())*255));
        cout << "stroke-opacity: " <<(int) stroke.getColor().getA() << ' ';
    }

}