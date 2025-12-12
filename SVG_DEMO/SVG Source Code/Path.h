#ifndef _PATH_H_
#define _PATH_H_

#include "GeometricElement.h"

class SVGPath : public GeometricElement {
private:
	string d;
	string fillRule;
public:
	SVGPath();
	SVGPath(const string&);
	SVGPath(const SVGPath&);
	string getD() const;
	void  setD(const string&);
	void parseAttributes(xml_node<>*);
	void handleData();
	void draw(Graphics* graphics);
	~SVGPath();
};

#endif // !_PATH_H_
