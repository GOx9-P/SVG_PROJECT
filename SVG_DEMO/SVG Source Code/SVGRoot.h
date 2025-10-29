#pragma once

#include "SVGElement.h"

class SVGRoot {
private:
	float width;
	float height;
	string viewBox;
	vector<SVGElement*> elements;
public:
	void createNode(xml_node<>*);
	void addElement(SVGElement* element);
	void parseNodes(xml_node<>*);
	void loadFromFile(const string& filename);
	void render();

	~SVGRoot();
};
