#include "stdafx.h"
#include "SVGRoot.h"
#include "Rect.h"
#include "Circle.h"
#include "Ellipes.h"
#include "Line.h"
#include "Path.h"
#include "Polygon.h"
#include "Polyline.h"
#include "TextElement.h"
#include "SVGGroup.h"
#include <sstream>

SVGElement* SVGRoot::createNode(xml_node<>* node)
{
	string nodeName = node->name();
	SVGElement* newElement = nullptr;
	if (nodeName == "rect") {
		newElement = new SVGRect();
	}
	else if (nodeName == "circle") {
		newElement = new SVGCircle();
	}
	else if (nodeName == "ellipse") {
		newElement = new SVGEllipse();
	}
	else if (nodeName == "line") {
		newElement = new SVGLine();
	}
	else if (nodeName == "path") {
		newElement = new SVGPath();
	}
	else if (nodeName == "polygon") {
		newElement = new SVGPolygon();
	}
	else if (nodeName == "polyline") {
		newElement = new SVGPolyline();
	}
	else if (nodeName == "text") {
		newElement = new TextElement();
	}
	else if (nodeName == "g") {
		newElement = new SVGGroup();
	}
	else {
		std::cerr << "The khong duoc ho tro!!!" << endl;
		return nullptr;
	}
	return newElement;
}

void SVGRoot::addElement(SVGElement* element)
{
	if (element != nullptr) {
		this->elements.push_back(element);
	}
}

void SVGRoot::parseNodes(xml_node<>* node, SVGGroup* parentGroup)
{
	for (xml_node<>* child = node->first_node(); child != nullptr; child = child->next_sibling()) {
		SVGElement* newElement = this->createNode(child);
		if (newElement != nullptr) {

			newElement->parseAttributes(child);
			SVGGroup* newGroup = dynamic_cast<SVGGroup*>(newElement);
			if (newGroup != nullptr) {
				this->parseNodes(child, newGroup);
			}
			if (parentGroup != nullptr) {
				parentGroup->addElement(newElement);
			}
			else {
				this->addElement(newElement);
			}
		}
	}
}

void SVGRoot::loadFromFile(const string& filename)
{
	ifstream file(filename);
	if (!file) {
		cerr << "Error: Khong the mo file: " << filename << endl;
		return;
	}
	vector<char> buffer((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	xml_document<> doc;

	try {
		doc.parse<0>(buffer.data());
		xml_node<>* rootNode = doc.first_node("svg");
		if (!rootNode) {
			std::cerr << "Error: Khong tim thay <svg> trong file: " << filename << endl;
			return;
		}
		if (xml_attribute<>* widthAttribute = rootNode->first_attribute("width")) {
			this->width = stof(widthAttribute->value());
		}
		if (xml_attribute<>* heightAttribute = rootNode->first_attribute("height")) {
			this->height = stof(heightAttribute->value());
		}
		if (xml_attribute<>* viewBoxAtrribute = rootNode->first_attribute("viewBox")) {
			this->viewBox = viewBoxAtrribute->value();
		}
		this->parseNodes(rootNode, nullptr);
	}
	catch (const std::exception& e) {
		std::cerr << "Error prasing XML: " << e.what() << endl;
	}
}

void SVGRoot::render(Graphics* graphics, int viewPortWidth, int viewPortHeight)
{
	GraphicsState curState = graphics->Save();
	graphics->SetSmoothingMode(SmoothingModeAntiAlias);
	if (!viewBox.empty()) {
		string tempViewBox = viewBox;
		for (auto& ch : tempViewBox) {
			if (ch == ',') {
				ch = ' ';
			}
		}
		stringstream ss(tempViewBox);
		ss.imbue(std::locale("C")); 
		float vbX, vbY, vbWidth, vbHeight;
		vbHeight = vbY = vbX = vbWidth = 0.0f;
		//char trash;
		ss >> vbX >> vbY >>  vbWidth >> vbHeight;
		if (vbWidth > 0 && vbHeight > 0) {
			// tinh ti le scale
			float scaleX = viewPortWidth / vbWidth;
			float scaleY = viewPortHeight / vbHeight;
			// lay min de khong tran 
			float scale = min(scaleX, scaleY);
			// Tinh khoang du ra
			// co 1 phan se fit man hinh
			float tx = (viewPortWidth - vbWidth * scale) / 2.0f;
			float ty = (viewPortHeight - vbHeight * scale) / 2.0f;

			// Bien doi nghich
			// Can giua
			graphics->TranslateTransform(tx, ty);
			// Zoom
			graphics->ScaleTransform(scale, scale);
			// Dich chuyen ve goc toa do
			graphics->TranslateTransform(-vbX, -vbY);
		}
	}
	for (auto element : elements) {
		if (element) {
			element->render(graphics);// thay draw thanh render de trung gian qua element::render xu li transform.
		}
	}
	graphics->Restore(curState);
}

SVGRoot::~SVGRoot()
{
	for (auto element : this->elements) {
		delete element;
	}
	this->elements.clear();
}
