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

void TextElement::draw(Graphics* graphics)
{
    //Chuyen doi fontFamily
    wstring wFontFamily = L"Arial"; //coi Arial la ngon ngu mac dinh
    if (!this->fontFamily.empty()) {
        int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &this->fontFamily[0], (int)this->fontFamily.size(), NULL, 0); //chuoi fontFamily can bao nhieu cho wstring
        wstring wstr(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, &this->fontFamily[0], (int)this->fontFamily.size(), &wstr[0], sizeNeeded); //dich sang wstring
        wFontFamily = wstr;
    }

    //Chuyen doi textContent
    wstring wTextContent = L"";
    if (!this->textContent.empty()) {
        int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &this->textContent[0], (int)this->textContent.size(), NULL, 0); //chuoi fontFamily can bao nhieu cho wstring
        wstring wstr(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, &this->textContent[0], (int)this->textContent.size(), &wstr[0], sizeNeeded); //dich sang wstring
        wTextContent = wstr;
    }

    //Chuyen mau sac
    SVGColor color = this->getFill();
    Color GDIColor{
        color.getA(),
        color.getR(),
        color.getG(),
        color.getB(),
    };
    
    //Ve
    SolidBrush brush(GDIColor);

    //
    FontFamily fontFamilyObj(wFontFamily.c_str());
    Font font(&fontFamilyObj, this->fontSize, FontStyleRegular, UnitPixel);

    //Lay vi tri
    /*PointF point(this->getPosition().getX(), this->getPosition().getY());*/
    float ascentDesignUnits = (float)fontFamilyObj.GetCellAscent(font.GetStyle());
    float emHeightDesignUnits = (float)fontFamilyObj.GetEmHeight(font.GetStyle());

    // Tính tỷ lệ và nhân với kích thước font (pixel) để ra Ascent bằng pixel
    float ascentPixels = (ascentDesignUnits / emHeightDesignUnits) * this->fontSize;

    // 2. Lấy tọa độ gốc và điều chỉnh 'y'
    PointF originalPoint(this->getPosition().getX(), this->getPosition().getY());

    // Tạo điểm mới đã điều chỉnh: y_mới = y_gốc (baseline) - độ_dâng (ascent)
    PointF adjustedPoint(originalPoint.X, originalPoint.Y - ascentPixels);

    //Canh le
    StringFormat format;
    if (this->textAnchor == "middle") {
        format.SetAlignment(StringAlignmentCenter);
    }
    else if (this->textAnchor == "end") {
        format.SetAlignment(StringAlignmentFar);
    }
    else {
        format.SetAlignment(StringAlignmentNear);
    }

    //Ve len man hinh
    graphics->DrawString(
        wTextContent.c_str(), //Chuoi da dich
        -1, //Tu dong tinh do dai
        &font, //Font da tao
        adjustedPoint, //Vi tri da tao
        &format, //Canh le da tao
        &brush //Co de ve
    );
}

TextElement::~TextElement()
{
}
