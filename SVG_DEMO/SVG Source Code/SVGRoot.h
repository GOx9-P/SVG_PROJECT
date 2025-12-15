#ifndef _SVGROOT_H_
#define _SVGROOT_H_

#include "SVGElement.h"
#include "SVGGroup.h"
#include <map>           
#include "SVGGradient.h" 

class SVGRoot {
private:
	float width;
	float height;
	string viewBox;
	vector<SVGElement*> elements;

public:
	static std::map<string, SVGGradient*> defsMap;

	// Hàm dọn dẹp
	static void CleanupStaticResources();

	SVGElement* createNode(xml_node<>*);
	void addElement(SVGElement* element);
	void parseNodes(xml_node<>*, SVGGroup* parentGroup);
	void loadFromFile(const string& filename);
	void render(Graphics* graphics, int viewPortWidth, int viewPortHeight);

	~SVGRoot();
};

#endif // !_SVGROOT_H_