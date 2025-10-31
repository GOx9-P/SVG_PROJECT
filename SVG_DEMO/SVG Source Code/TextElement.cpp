#include "stdafx.h"
#include "TextElement.h"

SVGPoint TextElement::getPosition() const {
    return position;
}

void TextElement::setPosition(SVGPoint& newPosition) {
    position = newPosition;
}

string TextElement::getTextContent() const {
    return textContent;
}

void TextElement::setTextContent(string& newTextContent) {
    textContent = newTextContent;
}

string TextElement::getFontFamily() const {
    return fontFamily;
}

void TextElement::setFontFamily(string& newFontFamily) {
    fontFamily = newFontFamily;
}

float TextElement::getFontSize() const {
    return fontSize;
}

void TextElement::setFontSize(float& newFontSize) {
    fontSize = newFontSize;
}

string TextElement::getTextAnchor() const {
    return textAnchor;
}

void TextElement::setTextAnchor(string& newTextAnchor) {
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
        cout << "textContent: " << textContent << ' ';
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
        cout << " textAnchor: " << textAnchor << ' ';

    }

}

void TextElement::draw()
{
}

TextElement::~TextElement()
{
}
