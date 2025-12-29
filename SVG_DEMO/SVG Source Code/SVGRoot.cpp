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
#include "SVGLinearGradient.h" 
#include "SVGRadialGradient.h"
#include <sstream>


std::map<string, SVGGradient*> SVGRoot::defsMap;

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
	else if (nodeName == "radialGradient")
	{
		newElement = new SVGRadialGradient();
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
						delete defElement;
					}
				}
			}
			continue;
		}

		SVGElement* newElement = this->createNode(child);
		if (newElement != nullptr) {
			newElement->parseAttributes(child);

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
		doc.parse<0>(buffer.data());

		xml_node<>* rootNode = doc.first_node("svg");
		if (!rootNode) {
			std::cerr << "Error: Khong tim thay <svg> trong file: " << filename << endl;
			return;
		}
		if (xml_attribute<>* widthAttribute = rootNode->first_attribute("width")) {
			string val = widthAttribute->value();
			this->width = stof(val);
			if (val.find("pt") != string::npos) this->width *= 1.33333f;
		}
		if (xml_attribute<>* heightAttribute = rootNode->first_attribute("height")) {
			string val = heightAttribute->value();
			this->height = stof(val);
			if (val.find("pt") != string::npos) this->height *= 1.33333f;
		}
		if (xml_attribute<>* viewBoxAtrribute = rootNode->first_attribute("viewBox")) {
			this->viewBox = viewBoxAtrribute->value();

			
			string temp = this->viewBox;
			for (char& c : temp) if (c == ',') c = ' '; // Thay dấu phẩy bằng dấu cách
			stringstream ss(temp);
			ss >> this->vbX >> this->vbY >> this->vbWidth >> this->vbHeight;
		}
		this->parseNodes(rootNode, nullptr);
	}
	catch (const std::exception& e) {
		std::cerr << "Error parsing XML: " << e.what() << endl;
	}
}

void SVGRoot::render(Graphics* graphics, int viewPortWidth, int viewPortHeight, bool ignoreViewBox)
{
	GraphicsState curState = graphics->Save();
	graphics->SetSmoothingMode(SmoothingModeAntiAlias);
	// 2. Thiết lập nội suy chất lượng cao nhất (Giúp vùng tím sáng mịn và rực rỡ)
	graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	// 3. Tăng chất lượng hòa trộn màu (Alpha Blending cho lông cáo và quả cầu)
	graphics->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);

	// 4. Chế độ bù đắp pixel để hình ảnh sắc nét, không bị nhòe ở biên
	graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	if (!ignoreViewBox&&!viewBox.empty()) {
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

SVGRoot::~SVGRoot()
{
	for (auto element : this->elements) {
		if (element) delete element;
	}
	this->elements.clear();
}

void SVGRoot::CleanupStaticResources() {
	std::map<string, SVGGradient*>::iterator it;
	for (it = SVGRoot::defsMap.begin(); it != SVGRoot::defsMap.end(); ++it) {
		if (it->second != nullptr) {
			delete it->second;
		}
	}
	SVGRoot::defsMap.clear();
}

RectF SVGRoot::getBoundingBox() {
	RectF totalBounds(0, 0, 0, 0);
	bool isFirst = true;

	// Duyệt qua danh sách m_elements
	for (auto* element : elements) {
		// Gọi hàm GetBoundingBox (lúc này đã đúng cho từng loại hình)
		RectF childBounds = element->getBoundingBox();

		if (childBounds.Width <= 0 || childBounds.Height <= 0) continue;

		if (isFirst) {
			totalBounds = childBounds;
			isFirst = false;
		}
		else {
			RectF temp = totalBounds;
			RectF::Union(totalBounds, temp, childBounds);
		}
	}
	return totalBounds;
}


float SVGRoot::getVbX()
{
	return vbX;
}
float SVGRoot::getVbY()
{
	return vbY;
}
float SVGRoot::getVbWidth()
{
	return vbWidth;
}
float SVGRoot::getVbHeight()
{
	return vbHeight;
}
