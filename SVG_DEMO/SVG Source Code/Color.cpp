#include "stdafx.h"
#include "Color.h"
#include "stdafx.h"
#include "Color.h"
#include <map> 


static const std::map<string, SVGColor> namedColors = {
    // Special
    { "none",                 SVGColor(0, 0, 0, 0) },
    { "transparent",          SVGColor(0, 0, 0, 0) },

    // A
    { "aliceblue",            SVGColor(240, 248, 255, 255) },
    { "antiquewhite",         SVGColor(250, 235, 215, 255) },
    { "aqua",                 SVGColor(0, 255, 255, 255) },
    { "aquamarine",           SVGColor(127, 255, 212, 255) },
    { "azure",                SVGColor(240, 255, 255, 255) },

    // B
    { "beige",                SVGColor(245, 245, 220, 255) },
    { "bisque",               SVGColor(255, 228, 196, 255) },
    { "black",                SVGColor(0, 0, 0, 255) },
    { "blanchedalmond",       SVGColor(255, 235, 205, 255) },
    { "blue",                 SVGColor(0, 0, 255, 255) },
    { "blueviolet",           SVGColor(138, 43, 226, 255) },
    { "brown",                SVGColor(165, 42, 42, 255) },
    { "burlywood",            SVGColor(222, 184, 135, 255) },

    // C
    { "cadetblue",            SVGColor(95, 158, 160, 255) },
    { "chartreuse",           SVGColor(127, 255, 0, 255) },
    { "chocolate",            SVGColor(210, 105, 30, 255) },
    { "coral",                SVGColor(255, 127, 80, 255) },
    { "cornflowerblue",       SVGColor(100, 149, 237, 255) },
    { "cornsilk",             SVGColor(255, 248, 220, 255) },
    { "crimson",              SVGColor(220, 20, 60, 255) },
    { "cyan",                 SVGColor(0, 255, 255, 255) },

    // D
    { "darkblue",             SVGColor(0, 0, 139, 255) },
    { "darkcyan",             SVGColor(0, 139, 139, 255) },
    { "darkgoldenrod",        SVGColor(184, 134, 11, 255) },
    { "darkgray",             SVGColor(169, 169, 169, 255) },
    { "darkgreen",            SVGColor(0, 100, 0, 255) },
    { "darkgrey",             SVGColor(169, 169, 169, 255) },
    { "darkkhaki",            SVGColor(189, 183, 107, 255) },
    { "darkmagenta",          SVGColor(139, 0, 139, 255) },
    { "darkolivegreen",       SVGColor(85, 107, 47, 255) },
    { "darkorange",           SVGColor(255, 140, 0, 255) },
    { "darkorchid",           SVGColor(153, 50, 204, 255) },
    { "darkred",              SVGColor(139, 0, 0, 255) },
    { "darksalmon",           SVGColor(233, 150, 122, 255) },
    { "darkseagreen",         SVGColor(143, 188, 143, 255) },
    { "darkslateblue",        SVGColor(72, 61, 139, 255) },
    { "darkslategray",        SVGColor(47, 79, 79, 255) },
    { "darkslategrey",        SVGColor(47, 79, 79, 255) },
    { "darkturquoise",        SVGColor(0, 206, 209, 255) },
    { "darkviolet",           SVGColor(148, 0, 211, 255) },
    { "deeppink",             SVGColor(255, 20, 147, 255) },
    { "deepskyblue",          SVGColor(0, 191, 255, 255) },
    { "dimgray",              SVGColor(105, 105, 105, 255) },
    { "dimgrey",              SVGColor(105, 105, 105, 255) },
    { "dodgerblue",           SVGColor(30, 144, 255, 255) },

    // F
    { "firebrick",            SVGColor(178, 34, 34, 255) },
    { "floralwhite",          SVGColor(255, 250, 240, 255) },
    { "forestgreen",          SVGColor(34, 139, 34, 255) },
    { "fuchsia",              SVGColor(255, 0, 255, 255) },

    // G
    { "gainsboro",            SVGColor(220, 220, 220, 255) },
    { "ghostwhite",           SVGColor(248, 248, 255, 255) },
    { "gold",                 SVGColor(255, 215, 0, 255) },
    { "goldenrod",            SVGColor(218, 165, 32, 255) },
    { "gray",                 SVGColor(128, 128, 128, 255) },
    { "green",                SVGColor(0, 128, 0, 255) },
    { "greenyellow",          SVGColor(173, 255, 47, 255) },
    { "grey",                 SVGColor(128, 128, 128, 255) },

    // H
    { "honeydew",             SVGColor(240, 255, 240, 255) },
    { "hotpink",              SVGColor(255, 105, 180, 255) },

    // I
    { "indianred",            SVGColor(205, 92, 92, 255) },
    { "indigo",               SVGColor(75, 0, 130, 255) },
    { "ivory",                SVGColor(255, 255, 240, 255) },

    // K
    { "khaki",                SVGColor(240, 230, 140, 255) },

    // L
    { "lavender",             SVGColor(230, 230, 250, 255) },
    { "lavenderblush",        SVGColor(255, 240, 245, 255) },
    { "lawngreen",            SVGColor(124, 252, 0, 255) },
    { "lemonchiffon",         SVGColor(255, 250, 205, 255) },
    { "lightblue",            SVGColor(173, 216, 230, 255) },
    { "lightcoral",           SVGColor(240, 128, 128, 255) },
    { "lightcyan",            SVGColor(224, 255, 255, 255) },
    { "lightgoldenrodyellow", SVGColor(250, 250, 210, 255) },
    { "lightgray",            SVGColor(211, 211, 211, 255) },
    { "lightgreen",           SVGColor(144, 238, 144, 255) },
    { "lightgrey",            SVGColor(211, 211, 211, 255) },
    { "lightpink",            SVGColor(255, 182, 193, 255) },
    { "lightsalmon",          SVGColor(255, 160, 122, 255) },
    { "lightseagreen",        SVGColor(32, 178, 170, 255) },
    { "lightskyblue",         SVGColor(135, 206, 250, 255) },
    { "lightslategray",       SVGColor(119, 136, 153, 255) },
    { "lightslategrey",       SVGColor(119, 136, 153, 255) },
    { "lightsteelblue",       SVGColor(176, 196, 222, 255) },
    { "lightyellow",          SVGColor(255, 255, 224, 255) },
    { "lime",                 SVGColor(0, 255, 0, 255) },
    { "limegreen",            SVGColor(50, 205, 50, 255) },
    { "linen",                SVGColor(250, 240, 230, 255) },

    // M
    { "magenta",              SVGColor(255, 0, 255, 255) },
    { "maroon",               SVGColor(128, 0, 0, 255) },
    { "mediumaquamarine",     SVGColor(102, 205, 170, 255) },
    { "mediumblue",           SVGColor(0, 0, 205, 255) },
    { "mediumorchid",         SVGColor(186, 85, 211, 255) },
    { "mediumpurple",         SVGColor(147, 112, 219, 255) },
    { "mediumseagreen",       SVGColor(60, 179, 113, 255) },
    { "mediumslateblue",      SVGColor(123, 104, 238, 255) },
    { "mediumspringgreen",    SVGColor(0, 250, 154, 255) },
    { "mediumturquoise",      SVGColor(72, 209, 204, 255) },
    { "mediumvioletred",      SVGColor(199, 21, 133, 255) },
    { "midnightblue",         SVGColor(25, 25, 112, 255) },
    { "mintcream",            SVGColor(245, 255, 250, 255) },
    { "mistyrose",            SVGColor(255, 228, 225, 255) },
    { "moccasin",             SVGColor(255, 228, 181, 255) },

    // N
    { "navajowhite",          SVGColor(255, 222, 173, 255) },
    { "navy",                 SVGColor(0, 0, 128, 255) },

    // O
    { "oldlace",              SVGColor(253, 245, 230, 255) },
    { "olive",                SVGColor(128, 128, 0, 255) },
    { "olivedrab",            SVGColor(107, 142, 35, 255) },
    { "orange",               SVGColor(255, 165, 0, 255) },
    { "orangered",            SVGColor(255, 69, 0, 255) },
    { "orchid",               SVGColor(218, 112, 214, 255) },

    // P
    { "palegoldenrod",        SVGColor(238, 232, 170, 255) },
    { "palegreen",            SVGColor(152, 251, 152, 255) },
    { "paleturquoise",        SVGColor(175, 238, 238, 255) },
    { "palevioletred",        SVGColor(219, 112, 147, 255) },
    { "papayawhip",           SVGColor(255, 239, 213, 255) },
    { "peachpuff",            SVGColor(255, 218, 185, 255) },
    { "peru",                 SVGColor(205, 133, 63, 255) },
    { "pink",                 SVGColor(255, 192, 203, 255) },
    { "plum",                 SVGColor(221, 160, 221, 255) },
    { "powderblue",           SVGColor(176, 224, 230, 255) },
    { "purple",               SVGColor(128, 0, 128, 255) },

    // R
    { "rebeccapurple",        SVGColor(102, 51, 153, 255) },
    { "red",                  SVGColor(255, 0, 0, 255) },
    { "rosybrown",            SVGColor(188, 143, 143, 255) },
    { "royalblue",            SVGColor(65, 105, 225, 255) },

    // S
    { "saddlebrown",          SVGColor(139, 69, 19, 255) },
    { "salmon",               SVGColor(250, 128, 114, 255) },
    { "sandybrown",           SVGColor(244, 164, 96, 255) },
    { "seagreen",             SVGColor(46, 139, 87, 255) },
    { "seashell",             SVGColor(255, 245, 238, 255) },
    { "sienna",               SVGColor(160, 82, 45, 255) },
    { "silver",               SVGColor(192, 192, 192, 255) },
    { "skyblue",              SVGColor(135, 206, 235, 255) },
    { "slateblue",            SVGColor(106, 90, 205, 255) },
    { "slategray",            SVGColor(112, 128, 144, 255) },
    { "slategrey",            SVGColor(112, 128, 144, 255) },
    { "snow",                 SVGColor(255, 250, 250, 255) },
    { "springgreen",          SVGColor(0, 255, 127, 255) },
    { "steelblue",            SVGColor(70, 130, 180, 255) },

    // T
    { "tan",                  SVGColor(210, 180, 140, 255) },
    { "teal",                 SVGColor(0, 128, 128, 255) },
    { "thistle",              SVGColor(216, 191, 216, 255) },
    { "tomato",               SVGColor(255, 99, 71, 255) },
    { "turquoise",            SVGColor(64, 224, 208, 255) },

    // V
    { "violet",               SVGColor(238, 130, 238, 255) },

    // W
    { "wheat",                SVGColor(245, 222, 179, 255) },
    { "white",                SVGColor(255, 255, 255, 255) },
    { "whitesmoke",           SVGColor(245, 245, 245, 255) },

    // Y
    { "yellow",               SVGColor(255, 255, 0, 255) },
    { "yellowgreen",          SVGColor(154, 205, 50, 255) }
};

SVGColor::SVGColor() : r(0), g(0), b(0), a(0), isNoneFlag(false), isNotSet(true), isUrlFlag(false) {}

SVGColor::SVGColor(BYTE red, BYTE green, BYTE blue, BYTE alpha)
    : r(red), g(green), b(blue), a(alpha), isNoneFlag(false), isNotSet(false), isUrlFlag(false) {}

bool SVGColor::isUrl() const { return isUrlFlag; }

string SVGColor::getUrlRef() const { return urlRef; }

void SVGColor::setUrlRef(const string& id) {
    this->urlRef = id;
    this->isUrlFlag = true;
    this->isNotSet = false; // Đã set (là URL)
}

// Helper xóa khoảng trắng
static string trimString(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (string::npos == first) return str;
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

SVGColor SVGColor::fromString(const string& fillStr, float fillOpacity) {
    SVGColor color;
    string colorStr = trimString(fillStr); // Xóa khoảng trắng thừa

    if (colorStr.empty() || colorStr == "none") {
        SVGColor noneColor(0, 0, 0, 0);
        noneColor.setIsNone(true);
        return noneColor;
    }

    //Kiểm tra URL
    // Ví dụ: url(#fill0)
    if (colorStr.find("url(") == 0) {
        size_t start = colorStr.find('#');
        size_t end = colorStr.find(')');
        if (start != string::npos && end != string::npos) {
            string id = colorStr.substr(start + 1, end - start - 1);
            color.setUrlRef(id); // Lưu ID lại
            return color;
        }
    }

    transform(colorStr.begin(), colorStr.end(), colorStr.begin(), [](unsigned char c) { return std::tolower(c); });

    BYTE red = 0, green = 0, blue = 0;

    try {
        if (colorStr.rfind("rgb(", 0) == 0) {
            string values = colorStr.substr(4, colorStr.length() - 5);
            stringstream ss(values);
            string segment;
            int tempVal;
            getline(ss, segment, ','); stringstream(segment) >> tempVal;
            red = static_cast<BYTE>(max(0, min(255, tempVal)));
            getline(ss, segment, ','); stringstream(segment) >> tempVal;
            green = static_cast<BYTE>(max(0, min(255, tempVal)));
            getline(ss, segment, ','); stringstream(segment) >> tempVal;
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
                string r_hex = hex.substr(0, 1); string g_hex = hex.substr(1, 1); string b_hex = hex.substr(2, 1);
                red = static_cast<BYTE>(stoul(r_hex + r_hex, nullptr, 16));
                green = static_cast<BYTE>(stoul(g_hex + g_hex, nullptr, 16));
                blue = static_cast<BYTE>(stoul(b_hex + b_hex, nullptr, 16));
            }
        }
        else {
            auto it = namedColors.find(colorStr);
            if (it != namedColors.end()) {
                SVGColor foundColor = it->second;
                red = foundColor.getR(); green = foundColor.getG(); blue = foundColor.getB();
            }
        }
    }
    catch (...) { red = 0; green = 0; blue = 0; }

    float clampedOpacity = max(0.0f, min(1.0f, fillOpacity));
    BYTE alpha = static_cast<BYTE>(clampedOpacity * 255.0f);

    // Set màu bình thường
    color.setR(red); color.setG(green); color.setB(blue); color.setA(alpha);
    color.isNotSet = false;

    return color;
}

BYTE SVGColor::getR() { return r; }
BYTE SVGColor::getG() { return g; }
BYTE SVGColor::getB() { return b; }
BYTE SVGColor::getA() { return a; }
void SVGColor::setR(BYTE red) { r = red; isNotSet = false; }
void SVGColor::setG(BYTE green) { g = green; isNotSet = false; }
void SVGColor::setB(BYTE blue) { b = blue; isNotSet = false; }
void SVGColor::setA(BYTE alpha) { a = alpha; isNotSet = false; }
void SVGColor::setIsNone(bool val) { isNoneFlag = val; isNotSet = false; }
bool SVGColor::isNone() const { return isNoneFlag; }
bool SVGColor::isSet() const { return !isNotSet; }
SVGColor::~SVGColor() {}