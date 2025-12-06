#ifndef _SVGROOT_H_
#define _SVGROOT_H_

#include "SVGElement.h"
#include "SVGGroup.h"

class SVGRoot {
private:
	float width;
	float height;
	string viewBox;
	vector<SVGElement*> elements;
public:
	SVGElement* createNode(xml_node<>*);
	void addElement(SVGElement* element);
	void parseNodes(xml_node<>*, SVGGroup* parentGroup); //argument thu 2 la de de quy cho group
	void loadFromFile(const string& filename);
	void render(Graphics* graphics, int viewPortWidth, int viewPortHeight); // lay tham so console

	~SVGRoot();
};

#endif // !_SVGROOT_H_



