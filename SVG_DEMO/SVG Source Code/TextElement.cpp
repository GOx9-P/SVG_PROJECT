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
            SVGColor blackColor(0, 0, 0, 255);
            this->setFill(blackColor);
        }
    }
    

    string anchorStr = getAttributeCascading(Node, "text-anchor");
    if (!anchorStr.empty()) {
        textAnchor = anchorStr;
    }

}

std::wstring trim(const std::wstring& str) {
    size_t first = str.find_first_not_of(L" ");
    if (std::string::npos == first) return str;
    size_t last = str.find_last_not_of(L" ");
    return str.substr(first, (last - first + 1));
}



void reviseContent(wstring& content)
{
    wstring newContent;
    bool inSpace = true;

    for (wchar_t ch : content)
    {
        if (iswspace(ch))
        {
            if (!inSpace)
            {
                newContent.push_back(L' ');
                inSpace = true;
            }
        }
        else
        {
            newContent.push_back(ch);
            inSpace = false;
        }
    }

    if (!newContent.empty() && newContent.back() == L' ')
        newContent.pop_back();

    content = newContent;
}

Gdiplus::RectF TextElement::getBoundingBox() {
    HDC hdc = GetDC(NULL);
    Graphics g(hdc);

    // 1. TÌM FONT (Logic giống hệt hàm draw)
    FontFamily* pFontFamily = nullptr;
    wstring wFontList = L"Arial";
    if (!this->fontFamily.empty()) {
        int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &this->fontFamily[0], (int)this->fontFamily.size(), NULL, 0);
        wstring wstr(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, &this->fontFamily[0], (int)this->fontFamily.size(), &wstr[0], sizeNeeded);
        wFontList = wstr;
    }

    size_t pos = 0;
    std::wstring token;
    std::wstring delimiter = L",";
    std::wstring s = wFontList;
    bool fontFound = false;

    while ((pos = s.find(delimiter)) != std::string::npos || s.length() > 0) {
        if (pos != std::string::npos) { token = s.substr(0, pos); s.erase(0, pos + delimiter.length()); }
        else { token = s; s = L""; }
        token = trim(token);
        if (token.empty() || token == L"serif" || token == L"sans-serif" || token == L"monospace") continue;

        pFontFamily = new FontFamily(token.c_str());
        if (pFontFamily->GetLastStatus() == Ok && pFontFamily->IsStyleAvailable(FontStyleRegular)) {
            fontFound = true; break;
        }
        delete pFontFamily; pFontFamily = nullptr;
        if (s.length() == 0) break;
    }
    if (!fontFound || pFontFamily == nullptr) pFontFamily = new FontFamily(L"Arial");

    // 2. TÍNH BASELINE
    int style = FontStyleRegular;
    float ascentDesignUnits = (float)pFontFamily->GetCellAscent(style);
    float emHeightDesignUnits = (float)pFontFamily->GetEmHeight(style);
    float ascentPixels = 0;
    if (emHeightDesignUnits > 0) {
        ascentPixels = (ascentDesignUnits / emHeightDesignUnits) * this->fontSize;
    }

    // 3. CHUẨN BỊ TEXT
    wstring wTextContent = L"";
    if (!this->textContent.empty()) {
        int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &this->textContent[0], (int)this->textContent.size(), NULL, 0);
        wstring wstr(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, &this->textContent[0], (int)this->textContent.size(), &wstr[0], sizeNeeded);
        wTextContent = wstr;
        reviseContent(wTextContent);
    }

    // 4. FORMAT
    StringFormat format;
    if (this->textAnchor == "middle") format.SetAlignment(StringAlignmentCenter);
    else if (this->textAnchor == "end") format.SetAlignment(StringAlignmentFar);
    else format.SetAlignment(StringAlignmentNear);

    PointF origin(position.getX(), position.getY() - ascentPixels);

    // 5. ĐO BẰNG GRAPHICSPATH (CHÍNH XÁC TUYỆT ĐỐI)
    GraphicsPath path;
    path.AddString(
        wTextContent.c_str(),
        -1,
        pFontFamily,
        style,
        this->fontSize,
        origin,
        &format
    );

    RectF boundingBox;
    SVGStroke stroke = this->getStroke();

    // Nếu có viền, ta dùng Pen để đo độ dày viền
    if (stroke.getColor().getA() > 0 && stroke.getWidth() > 0) {
        Pen pen(Color::Black, stroke.getWidth());
        // Hàm này tính cả phần lấn ra ngoài của viền
        path.GetBounds(&boundingBox, NULL, &pen);
    }
    else {
        path.GetBounds(&boundingBox);
    }

    // 6. THÊM PADDING AN TOÀN (SAFE BUFFER)
    // Để tránh việc khử răng cưa (antialiasing) ở mép pixel bị cắt, ta mở rộng thêm 5px mỗi chiều
    float safetyPad = 5.0f;
    boundingBox.Inflate(safetyPad, safetyPad);

    delete pFontFamily;
    ReleaseDC(NULL, hdc);

    return this->TransformRect(boundingBox);
}
void TextElement::draw(Graphics* graphics)
{
    if (!graphics) return;

    FontFamily* pFontFamily = nullptr;

    // Chuyển chuỗi font-family từ class sang wstring
    wstring wFontList = L"Arial"; // Mặc định
    if (!this->fontFamily.empty()) {
        int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &this->fontFamily[0], (int)this->fontFamily.size(), NULL, 0);
        wstring wstr(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, &this->fontFamily[0], (int)this->fontFamily.size(), &wstr[0], sizeNeeded);
        wFontList = wstr;
    }

    // Tách chuỗi và tìm font hợp lệ
    size_t pos = 0;
    std::wstring token;
    std::wstring delimiter = L",";
    std::wstring s = wFontList;
    bool fontFound = false;

    while ((pos = s.find(delimiter)) != std::string::npos || s.length() > 0) {
        if (pos != std::string::npos) {
            token = s.substr(0, pos);
            s.erase(0, pos + delimiter.length());
        }
        else {
            token = s;
            s = L"";
        }

        token = trim(token); // Cắt khoảng trắng thừa

        // Bỏ qua các từ khóa generic của CSS mà GDI+ không hiểu
        if (token.empty() || token == L"serif" || token == L"sans-serif" || token == L"monospace" || token == L"cursive" || token == L"fantasy")
            continue;

        // Thử tạo Font
        pFontFamily = new FontFamily(token.c_str());

        // Kiểm tra xem máy có font này không
        if (pFontFamily->GetLastStatus() == Ok && pFontFamily->IsStyleAvailable(FontStyleRegular)) {
            fontFound = true;
            break; // Đã tìm thấy, thoát vòng lặp
        }

        // Không dùng được thì xóa đi để thử cái tiếp theo
        delete pFontFamily;
        pFontFamily = nullptr;

        if (s.length() == 0) break;
    }

    // Nếu không tìm thấy font nào trong list, dùng Arial làm đường cùng
    if (!fontFound || pFontFamily == nullptr) {
        pFontFamily = new FontFamily(L"Arial");
    }

    //2. CHUẨN BỊ NỘI DUNG TEXT
    wstring wTextContent = L"";
    if (!this->textContent.empty()) {
        int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &this->textContent[0], (int)this->textContent.size(), NULL, 0);
        wstring wstr(sizeNeeded, 0);
        MultiByteToWideChar(CP_UTF8, 0, &this->textContent[0], (int)this->textContent.size(), &wstr[0], sizeNeeded);
        wTextContent = wstr;
        reviseContent(wTextContent);
    }

    // 3. TÍNH TOÁN VỊ TRÍ BASELINE
    int style = FontStyleRegular;

    float ascentDesignUnits = (float)pFontFamily->GetCellAscent(style);
    float emHeightDesignUnits = (float)pFontFamily->GetEmHeight(style);

    float ascentPixels = 0;
    if (emHeightDesignUnits > 0) {
        ascentPixels = (ascentDesignUnits / emHeightDesignUnits) * this->fontSize;
    }

    PointF origin(this->getPosition().getX(), this->getPosition().getY() - ascentPixels);

    // 4. XỬ LÝ CĂN LỀ & VẼ 
    StringFormat format;
    if (this->textAnchor == "middle") format.SetAlignment(StringAlignmentCenter);
    else if (this->textAnchor == "end") format.SetAlignment(StringAlignmentFar);
    else format.SetAlignment(StringAlignmentNear);

    GraphicsPath path;
    path.AddString(
        wTextContent.c_str(),
        -1,
        pFontFamily, // Dùng con trỏ font đã tìm được
        style,
        this->fontSize,
        origin,
        &format
    );

    // Tô màu (Fill)
    SVGColor fillColor = this->getFill();
    if (fillColor.getA() > 0) {
        Color gdiFillColor(fillColor.getA(), fillColor.getR(), fillColor.getG(), fillColor.getB());
        SolidBrush brush(gdiFillColor);
        graphics->FillPath(&brush, &path);
    }

    // Viền (Stroke)
    SVGStroke stroke = this->getStroke();
    if (stroke.getColor().getA() > 0 && stroke.getWidth() > 0) {
        SVGColor sColor = stroke.getColor();
        Color gdiStrokeColor(sColor.getA(), sColor.getR(), sColor.getG(), sColor.getB());

        Pen pen(gdiStrokeColor, stroke.getWidth());
        graphics->DrawPath(&pen, &path);
    }

    if (pFontFamily) {
        delete pFontFamily;
        pFontFamily = nullptr;
    }
}

TextElement::~TextElement()
{
}



