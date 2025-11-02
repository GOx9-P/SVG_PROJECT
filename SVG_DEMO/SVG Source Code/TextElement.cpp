#include "stdafx.h"
#include "TextElement.h"

TextElement::TextElement()
{
    textAnchor = textContent = fontFamily = "";
    fontSize = 0;
}

TextElement::TextElement(const SVGPoint& point, const string& content, const  string& family, const float& size, const string& anchor)
{
    position = point;
    textContent = content;
    fontFamily = family;
    fontSize = size;
    textAnchor = anchor;
}

TextElement::TextElement(const TextElement& other)
{
    position = other.position;
    textContent = other.textContent;
    fontFamily = other.fontFamily;
    fontSize = other.fontSize;
    textAnchor = other.textAnchor;
}

SVGPoint TextElement::getPosition() const {
    return position;
}

void TextElement::setPosition(const SVGPoint& newPosition) {
    position = newPosition;
}

string TextElement::getTextContent() const {
    return textContent;
}

void TextElement::setTextContent(const string& newTextContent) {
    textContent = newTextContent;
}

string TextElement::getFontFamily() const {
    return fontFamily;
}

void TextElement::setFontFamily(const string& newFontFamily) {
    fontFamily = newFontFamily;
}

float TextElement::getFontSize() const {
    return fontSize;
}

void TextElement::setFontSize(const float& newFontSize) {
    fontSize = newFontSize;
}

string TextElement::getTextAnchor() const {
    return textAnchor;
}

void TextElement::setTextAnchor(const string& newTextAnchor) {
    textAnchor = newTextAnchor;
}


void TextElement::parseAttributes(xml_node<>* Node)
{
    SVGElement::parseAttributes(Node);
    if (xml_attribute<>* attribute = Node->first_attribute("x"))
    {
        float newX = atof(attribute->value());
        position.setX(newX);
        cout << "x: " << newX << ' ';
    }
    if (xml_attribute<>* attribute = Node->first_attribute("y"))
    {
        float newY = atof(attribute->value());
        position.setY(newY);
        cout << "y: " << newY << ' ';
    }
    if (Node->value())
    {
        textContent = Node->value();
        cout << "content: " << Node->value() << ' ';
    }
    if (xml_attribute<>* attribute = Node->first_attribute("font-size"))
    {
        fontSize = atof(attribute->value());
        cout << "fontSize: " << fontSize << ' ';
    }
    if (xml_attribute<>* attribute = Node->first_attribute("font-family"))
    {
        fontFamily = attribute->value();
        cout << "fontFamily: " << fontFamily << ' ';
    }
    if (xml_attribute<>* attribute = Node->first_attribute("text-anchor"))
    {
        textAnchor = attribute->value();
        cout << "textAnchor: " << textAnchor << ' ';
    }

}

void TextElement::draw()
{
}

TextElement::~TextElement()
{
}
