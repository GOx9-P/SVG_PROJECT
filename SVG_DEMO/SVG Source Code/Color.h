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
public:
	SVGColor();
	SVGColor(BYTE red, BYTE green, BYTE blue, BYTE alpha);
	BYTE getR();
	BYTE getG();
	BYTE getB();
	BYTE getA();
	void setR(BYTE red);
	void setG(BYTE green);
	void setB(BYTE blue);
	void setA(BYTE alpha);
	static SVGColor fromString(const string& fillStr, float fillOpacity = 1.0f);
	~SVGColor();
};

#endif
