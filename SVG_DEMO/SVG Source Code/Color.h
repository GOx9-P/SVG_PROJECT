#ifndef _COLOR_H_
#define _COLOR_H_

#include "Library.h"
#include <cstdint> 

typedef uint8_t BYTE; // 0-255

class SVGColor {
private:
    BYTE r;
    BYTE g;
    BYTE b;
    BYTE a; // opacity.
    bool isNoneFlag;
    bool isNotSet;

    bool isUrlFlag;
    string urlRef; // Lưu ID (ví dụ: "fill0")

public:
    SVGColor();
    SVGColor(BYTE red, BYTE green, BYTE blue, BYTE alpha);

    BYTE getR();
    BYTE getG();
    BYTE getB();
    BYTE getA();
    bool isNone() const;
    bool isSet() const;

    bool isUrl() const;
    string getUrlRef() const;
    void setUrlRef(const string& id);

    void setR(BYTE red);
    void setG(BYTE green);
    void setB(BYTE blue);
    void setA(BYTE alpha);
    void setIsNone(bool val);

    static SVGColor fromString(const string& fillStr, float fillOpacity = 1.0f);
    ~SVGColor();
};

#endif