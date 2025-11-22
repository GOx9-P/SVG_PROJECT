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

// tim thuoc tinh o cha.
string getAttributeCascading(xml_node<>* node, const char* attrName)
{
    xml_node<>* current = node;
    while (current)
    {
        if (xml_attribute<>* attr = current->first_attribute(attrName))
        {
            return attr->value(); // Tìm thấy thì trả về ngay
        }
        current = current->parent(); // Không thấy thì nhảy lên cha tìm tiếp
    }
    return ""; // Lên đến đỉnh mà không thấy thì trả về rỗng
}

void TextElement::parseAttributes(xml_node<>* Node)
{
    SVGElement::parseAttributes(Node);
    float currentX = 0;
    float currentY = 0;

    if (xml_attribute<>* attr = Node->first_attribute("x"))
        currentX = atof(attr->value());

    if (xml_attribute<>* attr = Node->first_attribute("dx"))
        currentX += atof(attr->value());

    if (xml_attribute<>* attr = Node->first_attribute("y"))
        currentY = atof(attr->value());
    if (xml_attribute<>* attr = Node->first_attribute("dy"))
        currentY += atof(attr->value());

    position.setX(currentX);
    position.setY(currentY);


    xml_node<>* contentNode = Node->first_node();
    if (contentNode && contentNode->value())
    {
        textContent = contentNode->value();
    }


    string sizeStr = getAttributeCascading(Node, "font-size");
    if (!sizeStr.empty()) {
        fontSize = atof(sizeStr.c_str());
    }
    else {
        fontSize = 12; // Mặc định nếu tìm khắp nơi không thấy
    }

    string familyStr = getAttributeCascading(Node, "font-family");
    if (!familyStr.empty()) {
        fontFamily = familyStr;
    }
    else {
        fontFamily = "Arial";
    }

    // Nếu node hiện tại không có fill, ta phải tìm ở cha.
    if (Node->first_attribute("fill") == NULL)
    {
        string fillStr = getAttributeCascading(Node, "fill");
        if (!fillStr.empty()) {
            // Parse lại màu từ chuỗi tìm được ở cha
            SVGColor parentColor = SVGColor::fromString(fillStr);
            this->setFill(parentColor);
        }
        else {
            // Mặc định text màu đen nếu không nói gì
            SVGColor blackColor(255, 0, 0, 0);
            this->setFill(blackColor);
        }
    }
    

    string anchorStr = getAttributeCascading(Node, "text-anchor");
    if (!anchorStr.empty()) {
        textAnchor = anchorStr;
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
    Brush* brush = new SolidBrush(GDIColor);

    //
    FontFamily fontFamilyObj(wFontFamily.c_str());
    Font font(&fontFamilyObj,fontSize , FontStyleRegular, UnitPixel);

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
        brush //Co de ve
    );
}

TextElement::~TextElement()
{
}
