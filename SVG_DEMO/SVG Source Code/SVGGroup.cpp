#include "stdafx.h"
#include "SVGGroup.h"

SVGGroup::SVGGroup()
{
}

SVGGroup::SVGGroup(const SVGGroup& origin)
{
	group = origin.group;
}

SVGGroup::SVGGroup(const vector<SVGElement*> origin)
{
	group = origin;
}

void SVGGroup::addElement(SVGElement* element)
{
	if (element) this->group.push_back(element);
}

void SVGGroup::parseAttributes(xml_node<>* node)
{
	SVGElement::parseAttributes(node);
}

void SVGGroup::draw(Graphics* graphics)
{
	if (!graphics) return;

	GraphicsState state = graphics->Save();
	graphics->SetSmoothingMode(SmoothingModeAntiAlias);

	//string transformStr = this->getTransform();
	//if (!transformStr.empty())
	//{
	//	size_t pos = transformStr.find("translate");
	//	if (pos != string::npos)
	//	{
	//		string valStr = transformStr.substr(pos + 10);
	//		size_t endPos = valStr.find(")");
	//		if (endPos != string::npos) {
	//			valStr = valStr.substr(0, endPos);
	//			for (char& c : valStr) if (c == ',') c = ' ';
	//			stringstream ss(valStr);
	//			float tx = 0, ty = 0;
	//			ss >> tx >> ty;
	//			graphics->TranslateTransform(tx, ty);
	//		}
	//	}
	//	pos = transformStr.find("rotate");
	//	if (pos != string::npos) {
	//		string valStr = transformStr.substr(pos + 7);
	//		size_t endPos = valStr.find(")");
	//		if (endPos != string::npos) {
	//			valStr = valStr.substr(0, endPos);
	//			stringstream ss(valStr);
	//			float angle = 0;
	//			ss >> angle;
	//			graphics->RotateTransform(angle);
	//		}
	//	}
	//	pos = transformStr.find("scale");
	//	if (pos != string::npos) {
	//		string valStr = transformStr.substr(pos + 6);
	//		size_t endPos = valStr.find(")");
	//		if (endPos != string::npos) {
	//			valStr = valStr.substr(0, endPos);
	//			stringstream ss(valStr);
	//			float sx = 1.0f, sy = 1.0f;
	//			ss >> sx;
	//			if (!(ss >> sy)) sy = sx;
	//			graphics->ScaleTransform(sx, sy);
	//		}
	//	}
	//}

	float parentFillAlpha = 1.0f;
	if (this->getFill().getA() > 0) {
		parentFillAlpha = (float)this->getFill().getA() / 255.0f;
	}

	float parentStrokeAlpha = 1.0f;
	if (this->getStroke().getColor().getA() < 255 && this->getStroke().getColor().getA() > 0) {
		parentStrokeAlpha = (float)this->getStroke().getColor().getA() / 255.0f;
	}

	SVGStroke parentStrokeStyle = this->getStroke();

	for (SVGElement* element : group)
	{
		if (element)
		{
			SVGColor originalFill = element->getFill();
			SVGStroke originalStroke = element->getStroke();
			if (originalFill.getA() > 0)
			{
				float childFillAlpha = (float)originalFill.getA() / 255.0f;
				float finalFillAlpha = childFillAlpha * parentFillAlpha;
				SVGColor newFill = originalFill;
				newFill.setA((BYTE)(finalFillAlpha * 255.0f));
				element->setFill(newFill);
			}

			SVGStroke newStroke = originalStroke;
			bool strokeUpdated = false;
			if (newStroke.getWidth() == 0 && parentStrokeStyle.getWidth() > 0)
			{
				newStroke = parentStrokeStyle;
				strokeUpdated = true;
			}
			else if (newStroke.getWidth() > 0 && newStroke.getColor().getA() == 0)
			{
				if (parentStrokeStyle.getColor().getA() > 0) {
					SVGColor inheritedColor = parentStrokeStyle.getColor();
					newStroke.setColor(inheritedColor);
					strokeUpdated = true;
				}
			}

			if (newStroke.getWidth() > 0 && newStroke.getColor().getA() > 0)
			{
				float childStrokeAlpha = (float)newStroke.getColor().getA() / 255.0f;
				float finalStrokeAlpha = childStrokeAlpha * parentStrokeAlpha;

				if (finalStrokeAlpha != childStrokeAlpha) {
					SVGColor color = newStroke.getColor();
					color.setA((BYTE)(finalStrokeAlpha * 255.0f));
					newStroke.setColor(color);
					strokeUpdated = true;
				}
			}

			if (strokeUpdated) {
				element->setStroke(newStroke);
			}

			element->draw(graphics);

			element->setFill(originalFill);
			element->setStroke(originalStroke);
		}
	}

	graphics->Restore(state);
}
SVGGroup::~SVGGroup()
{
	for (int i = 0; i < group.size(); i++)
	{
		delete group[i];
		group[i] = nullptr;
	}
}
