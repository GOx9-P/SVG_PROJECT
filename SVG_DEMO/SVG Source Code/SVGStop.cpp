//#include "stdafx.h"
//#include "SVGStop.h"
//


#include "stdafx.h"
#include "SVGStop.h"
#include <sstream>
#include <string>

using namespace std;

//SVGStop::SVGStop() : offset(0.0f), stopOpacity(1.0f) {
//    stopColor = SVGColor(0, 0, 0, 255);
//}
//
//SVGStop::SVGStop(float offset, const SVGColor& color, float opacity) {
//    this->offset = offset;
//    this->stopColor = color;
//    this->stopOpacity = opacity;
//}

float SVGStop::getOffset() const { return offset; }
void SVGStop::setOffset(float val) { offset = val; }

SVGColor SVGStop::getStopColor() const { return stopColor; }
void SVGStop::setStopColor(const SVGColor& val) { stopColor = val; }

float SVGStop::getStopOpacity() const { return stopOpacity; }
void SVGStop::setStopOpacity(float val) { stopOpacity = val; }

// Helper: Hàm parse số an toàn (Bắt buộc dùng dấu chấm)
static float parseFloatSafe(const string& str) {
    if (str.empty()) return 0.0f;
    string s = str;
    // Thay thế dấu phẩy thành dấu chấm (nếu có)
    for (char& c : s) if (c == ',') c = '.';

    // Xóa ký tự % nếu có
    if (!s.empty() && s.back() == '%') s.pop_back();

    stringstream ss(s);
    ss.imbue(std::locale::classic()); // Ép kiểu số chuẩn quốc tế
    float val = 0;
    ss >> val;
    return val;
}

SVGStop::SVGStop() {
    offset = 0;
    stopOpacity = 1.0;
}

SVGStop::SVGStop(float offset, const SVGColor& color, float opacity) {
    this->offset = offset;
    this->stopColor = color;
    this->stopOpacity = opacity;
}

void SVGStop::parseAttributes(xml_node<>* node) {
    // 1. Parse Offset
    if (xml_attribute<>* attr = node->first_attribute("offset")) {
        string val = attr->value();
        float rawVal = parseFloatSafe(val);
        // Nếu chuỗi gốc có %, chia 100
        if (val.find('%') != string::npos) {
            this->offset = rawVal / 100.0f;
        }
        else {
            this->offset = rawVal;
        }
    }

    // 2. Parse Color & Opacity
    string colorStr;
    if (xml_attribute<>* attr = node->first_attribute("stop-color")) {
        colorStr = attr->value();
    }
    if (xml_attribute<>* attr = node->first_attribute("stop-opacity")) {
        this->stopOpacity = parseFloatSafe(attr->value());
    }

    // 3. Parse Style (Xử lý trường hợp CSS inline)
    if (xml_attribute<>* attr = node->first_attribute("style")) {
        string style = attr->value();

        // Tìm stop-color
        size_t cPos = style.find("stop-color:");
        if (cPos != string::npos) {
            size_t end = style.find(';', cPos);
            if (end == string::npos) end = style.length();
            colorStr = style.substr(cPos + 11, end - (cPos + 11));
            // Xóa khoảng trắng thừa
            while (!colorStr.empty() && colorStr.front() == ' ') colorStr.erase(0, 1);
        }

        // Tìm stop-opacity
        size_t oPos = style.find("stop-opacity:");
        if (oPos != string::npos) {
            size_t end = style.find(';', oPos);
            if (end == string::npos) end = style.length();
            string opStr = style.substr(oPos + 13, end - (oPos + 13));
            this->stopOpacity = parseFloatSafe(opStr);
        }
    }

    if (!colorStr.empty()) {
        this->stopColor = SVGColor::fromString(colorStr, 1.0f); // Giả sử SVGColor có hàm này
    }
}