#pragma once
#include "Library.h"

class SVGPoint {
private:
	float x;
	float y;
public:
	float getX() const;
	void setX(const float&);
	float getY() const;
	void setY(const float&);
	SVGPoint();
<<<<<<< HEAD
	void translate(const float&,const float&);
=======
	void translate(const float,const float);
>>>>>>> NhatTruongDz
	~SVGPoint();
};
