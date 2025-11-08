#ifndef _SVGROOT_H_
#define _SVGROOT_H_

#include "SVGElement.h"

class SVGRoot {
private:
	float width;
	float height;
	string viewBox;
	vector<SVGElement*> elements;
public:
	SVGElement* createNode(xml_node<>*);
	void addElement(SVGElement* element);
	void parseNodes(xml_node<>*);
	void loadFromFile(const string& filename);
	void render(Graphics* graphics);

	~SVGRoot();
};

#endif // !_SVGROOT_H_



