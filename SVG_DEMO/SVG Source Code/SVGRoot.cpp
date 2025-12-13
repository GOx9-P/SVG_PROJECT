//#include "stdafx.h"
//#include "SVGRoot.h"
//#include "Rect.h"
//#include "Circle.h"
//#include "Ellipes.h"
//#include "Line.h"
//#include "Path.h"
//#include "Polygon.h"
//#include "Polyline.h"
//#include "TextElement.h"
//#include "SVGGroup.h"
//#include <sstream>
//
//SVGElement* SVGRoot::createNode(xml_node<>* node)
//{
//	string nodeName = node->name();
//	SVGElement* newElement = nullptr;
//	if (nodeName == "rect") {
//		newElement = new SVGRect();
//	}
//	else if (nodeName == "circle") {
//		newElement = new SVGCircle();
//	}
//	else if (nodeName == "ellipse") {
//		newElement = new SVGEllipse();
//	}
//	else if (nodeName == "line") {
//		newElement = new SVGLine();
//	}
//	else if (nodeName == "path") {
//		newElement = new SVGPath();
//	}
//	else if (nodeName == "polygon") {
//		newElement = new SVGPolygon();
//	}
//	else if (nodeName == "polyline") {
//		newElement = new SVGPolyline();
//	}
//	else if (nodeName == "text") {
//		newElement = new TextElement();
//	}
//	else if (nodeName == "g") {
//		newElement = new SVGGroup();
//	}
//	else {
//		std::cerr << "The khong duoc ho tro!!!" << endl;
//		return nullptr;
//	}
//	return newElement;
//}
//
//void SVGRoot::addElement(SVGElement* element)
//{
//	if (element != nullptr) {
//		this->elements.push_back(element);
//	}
//}
//
//void SVGRoot::parseNodes(xml_node<>* node, SVGGroup* parentGroup)
//{
//	for (xml_node<>* child = node->first_node(); child != nullptr; child = child->next_sibling()) {
//		SVGElement* newElement = this->createNode(child);
//		if (newElement != nullptr) {
//
//			newElement->parseAttributes(child);
//			SVGGroup* newGroup = dynamic_cast<SVGGroup*>(newElement);
//			if (newGroup != nullptr) {
//				this->parseNodes(child, newGroup);
//			}
//			if (parentGroup != nullptr) {
//				parentGroup->addElement(newElement);
//			}
//			else {
//				this->addElement(newElement);
//			}
//		}
//	}
//}
//
//void SVGRoot::loadFromFile(const string& filename)
//{
//	ifstream file(filename);
//	if (!file) {
//		cerr << "Error: Khong the mo file: " << filename << endl;
//		return;
//	}
//	vector<char> buffer((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
//	buffer.push_back('\0');
//	xml_document<> doc;
//
//	try {
//		doc.parse<0>(buffer.data());
//		xml_node<>* rootNode = doc.first_node("svg");
//		if (!rootNode) {
//			std::cerr << "Error: Khong tim thay <svg> trong file: " << filename << endl;
//			return;
//		}
//		if (xml_attribute<>* widthAttribute = rootNode->first_attribute("width")) {
//			this->width = stof(widthAttribute->value());
//		}
//		if (xml_attribute<>* heightAttribute = rootNode->first_attribute("height")) {
//			this->height = stof(heightAttribute->value());
//		}
//		if (xml_attribute<>* viewBoxAtrribute = rootNode->first_attribute("viewBox")) {
//			this->viewBox = viewBoxAtrribute->value();
//		}
//		this->parseNodes(rootNode, nullptr);
//	}
//	catch (const std::exception& e) {
//		std::cerr << "Error prasing XML: " << e.what() << endl;
//	}
//}
//
//void SVGRoot::render(Graphics* graphics, int viewPortWidth, int viewPortHeight)
//{
//	GraphicsState curState = graphics->Save();
//	graphics->SetSmoothingMode(SmoothingModeAntiAlias);
//	if (!viewBox.empty()) {
//		string tempViewBox = viewBox;
//		for (auto& ch : tempViewBox) {
//			if (ch == ',') {
//				ch = ' ';
//			}
//		}
//		stringstream ss(tempViewBox);
//		ss.imbue(std::locale("C")); 
//		float vbX, vbY, vbWidth, vbHeight;
//		vbHeight = vbY = vbX = vbWidth = 0.0f;
//		//char trash;
//		ss >> vbX >> vbY >>  vbWidth >> vbHeight;
//		if (vbWidth > 0 && vbHeight > 0) {
//			// tinh ti le scale
//			float scaleX = viewPortWidth / vbWidth;
//			float scaleY = viewPortHeight / vbHeight;
//			// lay min de khong tran 
//			float scale = min(scaleX, scaleY);
//			// Tinh khoang du ra
//			// co 1 phan se fit man hinh
//			float tx = (viewPortWidth - vbWidth * scale) / 2.0f;
//			float ty = (viewPortHeight - vbHeight * scale) / 2.0f;
//
//			// Bien doi nghich
//			// Can giua
//			graphics->TranslateTransform(tx, ty);
//			// Zoom
//			graphics->ScaleTransform(scale, scale);
//			// Dich chuyen ve goc toa do
//			graphics->TranslateTransform(-vbX, -vbY);
//		}
//	}
//	for (auto element : elements) {
//		if (element) {
//			SVGColor originalFill = element->getFill();
//			SVGStroke originalStroke = element->getStroke();
//			bool changed = false;
//
//			// 1. Fallback Black cho Root Elements (Yin Yang fix)
//			if (!element->getFill().isSet() && !element->getFill().isNone()) {
//				element->setFill(SVGColor(0, 0, 0, 255));
//				changed = true;
//			}
//
//			// 2. Apply Opacity cho Root Elements
//			if (element->getFill().isSet()) {
//				float effectiveOp = element->getFillOpacity();
//				float globalOp = element->getOpacity();
//				float baseAlpha = (float)element->getFill().getA();
//				float finalAlpha = 255.0f;
//
//				if (element->isGroup()) {
//					finalAlpha = baseAlpha * globalOp;
//				}
//				else {
//					finalAlpha = baseAlpha * effectiveOp * globalOp;
//				}
//
//				SVGColor c = element->getFill();
//				c.setA((BYTE)finalAlpha);
//				element->setFill(c);
//				changed = true;
//			}
//
//			if (element->getStroke().getColor().isSet() && !element->getStroke().getColor().isNone()) {
//				SVGStroke s = element->getStroke();
//				SVGColor c = s.getColor();
//
//				float effectiveOp = element->getStrokeOpacity();
//				float globalOp = element->getOpacity();
//				float baseAlpha = (float)c.getA();
//				float finalAlpha = 255.0f;
//
//				if (element->isGroup()) {
//					finalAlpha = baseAlpha * globalOp;
//				}
//				else {
//					finalAlpha = baseAlpha * effectiveOp * globalOp;
//				}
//
//				c.setA((BYTE)finalAlpha);
//				s.setColor(c);
//				element->setStroke(s);
//				changed = true;
//			}
//
//			element->render(graphics);
//
//			if (changed) {
//				element->setFill(originalFill);
//				element->setStroke(originalStroke);
//			}
//		}
//	}
//	graphics->Restore(curState);
//}
//
//SVGRoot::~SVGRoot()
//{
//	for (auto element : this->elements) {
//		delete element;
//	}
//	this->elements.clear();
//}

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
#include "SVGLinearGradient.h" // Bắt buộc include để nhận diện class
#include <sstream>

// --- 1. KHỞI TẠO BIẾN TĨNH (STATIC) ---
std::map<string, SVGGradient*> SVGRoot::defsMap;
// --------------------------------------

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
	else if (nodeName == "linearGradient") {
		newElement = new SVGLinearGradient();
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
		string nodeName = child->name();

		// --- FIX: XỬ LÝ THẺ DEFS ---
		if (nodeName == "defs") {
			// Nếu gặp defs, ta chỉ duyệt con của nó để tìm Gradient
			for (xml_node<>* defChild = child->first_node(); defChild; defChild = defChild->next_sibling()) {
				SVGElement* defElement = this->createNode(defChild);
				if (defElement) {
					defElement->parseAttributes(defChild);

					// Chỉ quan tâm nếu là Gradient để đăng ký vào Map
					SVGGradient* grad = dynamic_cast<SVGGradient*>(defElement);
					if (grad) {
						if (!grad->getId().empty()) {
							SVGRoot::defsMap[grad->getId()] = grad;
						}
						// Nếu Gradient có cấu trúc lồng nhau (như stops), parse tiếp
						SVGGroup* gradGroup = dynamic_cast<SVGGroup*>(defElement);
						if (gradGroup) {
							this->parseNodes(defChild, gradGroup);
						}
					}
					else {
						// Các thẻ khác trong defs (như rect, circle...) không cần hiển thị
						// nên ta xóa đi để tránh leak memory
						delete defElement;
					}
				}
			}
			continue; // Xử lý xong defs thì qua thẻ tiếp theo của vòng lặp chính
		}
		// ---------------------------

		SVGElement* newElement = this->createNode(child);
		if (newElement != nullptr) {
			// ... (Giữ nguyên code cũ của bạn ở dưới) ...
			newElement->parseAttributes(child);

			// ... logic xử lý Gradient cũ (nếu nó nằm ngoài defs) ...
			SVGGradient* grad = dynamic_cast<SVGGradient*>(newElement);
			if (grad != nullptr) {
				if (!grad->getId().empty()) {
					SVGRoot::defsMap[grad->getId()] = grad;
				}
				SVGGroup* gradGroup = dynamic_cast<SVGGroup*>(newElement);
				if (gradGroup != nullptr) {
					this->parseNodes(child, gradGroup);
				}
			}
			else {
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
		// --- 3. FIX LỖI RAPIDXML CHO VISUAL STUDIO CŨ ---
		// Dùng &buffer[0] thay vì buffer.data()
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
		std::cerr << "Error parsing XML: " << e.what() << endl;
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

		ss >> vbX >> vbY >> vbWidth >> vbHeight;
		if (vbWidth > 0 && vbHeight > 0) {
			float scaleX = viewPortWidth / vbWidth;
			float scaleY = viewPortHeight / vbHeight;
			float scale = min(scaleX, scaleY);
			float tx = (viewPortWidth - vbWidth * scale) / 2.0f;
			float ty = (viewPortHeight - vbHeight * scale) / 2.0f;

			graphics->TranslateTransform(tx, ty);
			graphics->ScaleTransform(scale, scale);
			graphics->TranslateTransform(-vbX, -vbY);
		}
	}
	for (auto element : elements) {
		if (element) {
			SVGColor originalFill = element->getFill();
			SVGStroke originalStroke = element->getStroke();
			bool changed = false;

			if (!element->getFill().isSet() && !element->getFill().isNone()) {
				element->setFill(SVGColor(0, 0, 0, 255));
				changed = true;
			}

			if (element->getFill().isSet()) {
				float effectiveOp = element->getFillOpacity();
				float globalOp = element->getOpacity();
				float baseAlpha = (float)element->getFill().getA();
				float finalAlpha = 255.0f;

				if (element->isGroup()) {
					finalAlpha = baseAlpha * globalOp;
				}
				else {
					finalAlpha = baseAlpha * effectiveOp * globalOp;
				}

				SVGColor c = element->getFill();
				c.setA((BYTE)finalAlpha);
				element->setFill(c);
				changed = true;
			}

			if (element->getStroke().getColor().isSet() && !element->getStroke().getColor().isNone()) {
				SVGStroke s = element->getStroke();
				SVGColor c = s.getColor();

				float effectiveOp = element->getStrokeOpacity();
				float globalOp = element->getOpacity();
				float baseAlpha = (float)c.getA();
				float finalAlpha = 255.0f;

				if (element->isGroup()) {
					finalAlpha = baseAlpha * globalOp;
				}
				else {
					finalAlpha = baseAlpha * effectiveOp * globalOp;
				}

				c.setA((BYTE)finalAlpha);
				s.setColor(c);
				element->setStroke(s);
				changed = true;
			}

			element->render(graphics);

			if (changed) {
				element->setFill(originalFill);
				element->setStroke(originalStroke);
			}
		}
	}
	graphics->Restore(curState);
}

// --- 4. HÀM HỦY (DESTRUCTOR) ---
SVGRoot::~SVGRoot()
{
	// Chỉ xóa elements thuộc về đối tượng này
	for (auto element : this->elements) {
		if (element) delete element;
	}
	this->elements.clear();
	// KHÔNG được xóa defsMap ở đây
}

// --- 5. HÀM DỌN DẸP STATIC (QUAN TRỌNG) ---
void SVGRoot::CleanupStaticResources() {
	// Dùng iterator kiểu cũ để tương thích mọi phiên bản VS và tránh lỗi auto
	std::map<string, SVGGradient*>::iterator it;
	for (it = SVGRoot::defsMap.begin(); it != SVGRoot::defsMap.end(); ++it) {
		if (it->second != nullptr) {
			delete it->second;
		}
	}
	SVGRoot::defsMap.clear();
}