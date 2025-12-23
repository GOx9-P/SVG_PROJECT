#include "stdafx.h"
#include "SVGGradient.h"
#include "SVGRoot.h" // BẮT BUỘC CÓ để truy cập defsMap
#include <sstream>

SVGGradient::SVGGradient() {
    gradientUnits = "objectBoundingBox";
    spreadMethod = "pad";
    gradientTransform = new Gdiplus::Matrix();
}

SVGGradient::~SVGGradient() {
    if (gradientTransform) {
        delete gradientTransform;
        gradientTransform = nullptr;
    }
}

void SVGGradient::addStop(const SVGStop& stop) {
    stops.push_back(stop);
}

std::vector<SVGStop> SVGGradient::getStops() const {
    return stops;
}

float SVGGradient::parseOffset(const string& val) {
    if (val.empty()) return 0.0f;
    string temp = val;
    if (temp.back() == '%') {
        temp.pop_back();
        return atof(temp.c_str()) / 100.0f;
    }
    return atof(temp.c_str());
}

static string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (string::npos == first) return str;
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

void SVGGradient::parseAttributes(xml_node<>* node) {
    SVGElement::parseAttributes(node); // Parse ID

    if (xml_attribute<>* attr = node->first_attribute("gradientUnits"))
        gradientUnits = attr->value();
    if (xml_attribute<>* attr = node->first_attribute("spreadMethod"))
        spreadMethod = attr->value();

    if (xml_attribute<>* attr = node->first_attribute("gradientTransform")) {
        this->setTransform(attr->value());
        this->parseTransform();
        if (this->transformMatrix) {
            float elem[6];
            this->transformMatrix->GetElements(elem);
            gradientTransform->SetElements(elem[0], elem[1], elem[2], elem[3], elem[4], elem[5]);
        }
    }

    // 1. Duyệt thẻ con <stop>
    for (xml_node<>* child = node->first_node("stop"); child; child = child->next_sibling("stop")) {
        float offset = 0.0f;
        float opacity = 1.0f;
        SVGColor color;

        if (xml_attribute<>* attr = child->first_attribute("offset"))
            offset = parseOffset(attr->value());

        if (xml_attribute<>* attr = child->first_attribute("stop-color"))
            color = SVGColor::fromString(attr->value());

        if (xml_attribute<>* attr = child->first_attribute("stop-opacity"))
            opacity = atof(attr->value());

        if (xml_attribute<>* attr = child->first_attribute("style")) {
            string style = attr->value();
            stringstream ss(style);
            string segment;
            while (getline(ss, segment, ';')) {
                size_t colonPos = segment.find(':');
                if (colonPos != string::npos) {
                    string key = trim(segment.substr(0, colonPos));
                    string val = trim(segment.substr(colonPos + 1));
                    if (key == "stop-color") color = SVGColor::fromString(val);
                    else if (key == "stop-opacity") opacity = atof(val.c_str());
                }
            }
        }
        this->addStop(SVGStop(offset, color, opacity));
    }

    // 2. Xử lý xlink:href (QUAN TRỌNG CHO INSTAGRAM LOGO)
    if (this->stops.empty()) {
        if (xml_attribute<>* attr = node->first_attribute("xlink:href")) {
            string href = attr->value();
            if (href.length() > 1 && href[0] == '#') {
                string refID = href.substr(1);

                // Kiểm tra xem ID có tồn tại trong map không trước khi truy cập
                if (SVGRoot::defsMap.find(refID) != SVGRoot::defsMap.end()) {
                    SVGGradient* parentGrad = SVGRoot::defsMap[refID];
                    if (parentGrad) {
                        this->stops = parentGrad->getStops();
                    }
                }
            }
        }
    }
}

Gdiplus::RectF SVGGradient::getBoundingBox() {
    return Gdiplus::RectF(0, 0, 0, 0);
}