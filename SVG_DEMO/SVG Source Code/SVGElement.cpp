#include "stdafx.h"
#include "SVGElement.h"

SVGElement::~SVGElement()
{
}

string SVGElement::getId() const {
    return id;
}

void SVGElement::setId(string& newId) {
    id = newId;
}

string SVGElement::getClassName() const {
    return className;
}

void SVGElement::setClassName(string& newClassName) {
    className = newClassName;
}

string SVGElement::getStyle() const {
    return style;
}

void SVGElement::setStyle(string& newStyle) {
    style = newStyle;
}

string SVGElement::getTransform() const {
    return transform;
}

void SVGElement::setTransform(string& newTransform) {
    transform = newTransform;
}

string SVGElement::getFill() const {
    return fill;
}

void SVGElement::setFill(string& newFill) {
    fill = newFill;
}

string SVGElement::getStroke() const {
    return stroke;
}

void SVGElement::setStroke(string& newStroke) {
    stroke = newStroke;
}

float SVGElement::getStrokeWidth() const {
    return strokeWidth;
}

void SVGElement::setStrokeWidth(float& newStrokeWidth) {
    strokeWidth = newStrokeWidth;
}

float SVGElement::getOpacity() const {
    return opacity;
}

void SVGElement::setOpacity(float& newOpacity) {
    opacity = newOpacity;
}

void SVGElement::parseAttributes(xml_node<>* Node)
{
    if (xml_attribute<>* attribute = Node->first_attribute("id"))
    {
        id = attribute->value();
    }
    if (xml_attribute<>* attribute = Node->first_attribute("class"))
    {
        className = attribute->value();
    }
    if (xml_attribute<>* attribute = Node->first_attribute("style"))
    {
        style = attribute->value();
    }
    if (xml_attribute<>* attribute = Node->first_attribute("transform"))
    {
        transform = attribute->value();;
    }
    if (xml_attribute<>* attr = Node->first_attribute("fill"))
    {
        fill = attr->value();
    }
    if (xml_attribute<>* attr = Node->first_attribute("stroke"))
    {
        stroke = attr->value();
    }
    if (xml_attribute<>* attr = Node->first_attribute("stroke-width"))
    {
        strokeWidth = stof(attr->value());
    }
    if (xml_attribute<>* attr = Node->first_attribute("opacity"))
    {
        opacity = stof(attr->value());
    }
}