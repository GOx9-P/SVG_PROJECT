#include "stdafx.h"
#include "Color.h"

SVGColor SVGColor::fromString(const string& fillStr, float fillOpacity) {

    string colorStr = fillStr;
    if (colorStr.empty() || colorStr == "none") {
        return SVGColor(0, 0, 0, 0);
    }
    BYTE red = 0, green = 0, blue = 0;

    try {
        if (colorStr.rfind("rgb(", 0) == 0) {
            string values = colorStr.substr(4, colorStr.length() - 5);

            stringstream ss(values);
            string segment;
            int tempVal;
            getline(ss, segment, ',');
            stringstream(segment) >> tempVal;
            red = static_cast<BYTE>(max(0, min(255, tempVal)));
            getline(ss, segment, ',');
            stringstream(segment) >> tempVal;
            green = static_cast<BYTE>(max(0, min(255, tempVal)));
            getline(ss, segment, ',');
            stringstream(segment) >> tempVal;
            blue = static_cast<BYTE>(max(0, min(255, tempVal)));

        }
        else if (colorStr.rfind("#", 0) == 0) {
            string hex = colorStr.substr(1);

            if (hex.length() == 6) {
                red = static_cast<BYTE>(stoul(hex.substr(0, 2), nullptr, 16));
                green = static_cast<BYTE>(stoul(hex.substr(2, 2), nullptr, 16));
                blue = static_cast<BYTE>(stoul(hex.substr(4, 2), nullptr, 16));
            }
            else if (hex.length() == 3) {
                string r_hex = hex.substr(0, 1);
                string g_hex = hex.substr(1, 1);
                string b_hex = hex.substr(2, 1);
                red = static_cast<BYTE>(stoul(r_hex + r_hex, nullptr, 16));
                green = static_cast<BYTE>(stoul(g_hex + g_hex, nullptr, 16));
                blue = static_cast<BYTE>(stoul(b_hex + b_hex, nullptr, 16));
            }
        }
        else {
            red = 0; green = 0; blue = 0;
        }

    }
    catch (const std::exception& e) {
        red = 0; green = 0; blue = 0;
    }
    float clampedOpacity = max(0.0f, min(1.0f, fillOpacity));
    BYTE alpha = static_cast<BYTE>(clampedOpacity * 255.0f);

    return SVGColor(red, green, blue, alpha);
}

void SVGColor::setR(BYTE red) {
    r = red;
}

void SVGColor::setG(BYTE green) {
    g = green;
}

void SVGColor::setB(BYTE blue) {
    b = blue;
}

void SVGColor::setA(BYTE alpha) {
    a = alpha;
}
SVGColor::SVGColor() : r(0), g(0), b(0), a(255) {}
SVGColor::SVGColor(BYTE red, BYTE green, BYTE blue, BYTE alpha)
	: r(red), g(green), b(blue), a(alpha) {
}
BYTE SVGColor::getR() {
    return r;
}
BYTE SVGColor::getG() {
    return g;
}
BYTE SVGColor::getB() {
    return b;
}
BYTE SVGColor::getA() {
    return a;
}
SVGColor::~SVGColor() {}