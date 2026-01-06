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

bool SVGGroup::isGroup() const
{
    return true;
}


Gdiplus::RectF SVGGroup::getBoundingBox() {
    Gdiplus::RectF bounds;
    bool first = true;
    for (auto child : group) {
        if (!child) continue;
        Gdiplus::RectF childBounds = child->getBoundingBox();

        
        if (childBounds.Width == 0 && childBounds.Height == 0) continue;

        if (first) {
            bounds = childBounds;
            first = false;
        }
        else {
            bounds.Union(bounds, bounds, childBounds);
        }
    }
  
    return bounds;
}

void SVGGroup::draw(Graphics* graphics)
{
    if (!graphics) return;

    GraphicsState state = graphics->Save();
    graphics->SetSmoothingMode(SmoothingModeAntiAlias);

    SVGColor parentFill = this->getFill();
    SVGStroke parentStroke = this->getStroke();

   
    float groupAlpha = this->getOpacity();

 
    for (SVGElement* element : group)
    {
        if (!element) continue;

      
        SVGColor originalFill = element->getFill();
        float originalFillOp = element->getFillOpacity();
        bool originalFillOpSet = element->isFillOpacitySet();

        SVGStroke originalStroke = element->getStroke();
        float originalStrokeOp = element->getStrokeOpacity();
        bool originalStrokeOpSet = element->isStrokeOpacitySet();

        bool isFillChanged = false;
        bool isFillOpChanged = false;
        bool isStrokeChanged = false;
        bool isStrokeOpChanged = false;

      
        if (!originalFill.isSet() && !originalFill.isNone() && parentFill.isSet())
        {
            element->setFill(parentFill);
            isFillChanged = true;
        }

        if (!element->getFill().isSet() && !element->getFill().isNone())
        {
            SVGColor defaultBlack(0, 0, 0, 255);
            element->setFill(defaultBlack);
            isFillChanged = true;
        }

       
        if (!element->isFillOpacitySet() && this->isFillOpacitySet())
        {
            element->setFillOpacity(this->getFillOpacity());
            element->setIsFillOpacitySet(true);
            isFillOpChanged = true; 
        }

        
        if (element->getFill().isSet())
        {
            float effectiveOp = element->getFillOpacity(); 
            float baseAlpha = (float)element->getFill().getA();
            float finalAlpha = 255.0f;

            if (element->isGroup()) {
                finalAlpha = baseAlpha * groupAlpha * element->getOpacity();
            }
            else {
                finalAlpha = baseAlpha * effectiveOp * groupAlpha * element->getOpacity();
            }

            SVGColor c = element->getFill();
            c.setA((BYTE)finalAlpha);
            element->setFill(c);
            isFillChanged = true;
        }

        SVGStroke newStroke = originalStroke;
        SVGColor strokeColor = newStroke.getColor();

    
        if (!strokeColor.isSet() && !strokeColor.isNone() && parentStroke.getColor().isSet()) {
            newStroke.setColor(parentStroke.getColor());
            strokeColor = newStroke.getColor();
            isStrokeChanged = true;
        }
       
        if (newStroke.getWidth() <= 0.0f && parentStroke.getWidth() > 0.0f) {
            if (strokeColor.isSet() && !strokeColor.isNone()) {
                newStroke.setWidth(parentStroke.getWidth());
                isStrokeChanged = true;
            }
        }
       
        if (!element->isStrokeOpacitySet() && this->isStrokeOpacitySet()) {
            element->setStrokeOpacity(this->getStrokeOpacity());
            element->setIsStrokeOpacitySet(true);
            isStrokeOpChanged = true;
        }
     
        if (strokeColor.isSet()) {
            float effectiveStrokeOp = element->getStrokeOpacity();
            float baseAlpha = (float)strokeColor.getA();
            float finalAlpha = 255.0f;

            if (element->isGroup()) {
                finalAlpha = baseAlpha * groupAlpha * element->getOpacity();
            }
            else {
                finalAlpha = baseAlpha * effectiveStrokeOp * groupAlpha * element->getOpacity();
            }

            strokeColor.setA((BYTE)finalAlpha);
            newStroke.setColor(strokeColor);
            isStrokeChanged = true;
        }

       
        if (isStrokeChanged) {
            element->setStroke(newStroke);
        }

       
        element->render(graphics);

        if (isFillChanged) element->setFill(originalFill);
        if (isFillOpChanged) {
            element->setFillOpacity(originalFillOp);
            element->setIsFillOpacitySet(originalFillOpSet);
        }
        if (isStrokeChanged) element->setStroke(originalStroke);
        if (isStrokeOpChanged) {
            element->setStrokeOpacity(originalStrokeOp);
            element->setIsStrokeOpacitySet(originalStrokeOpSet);
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
