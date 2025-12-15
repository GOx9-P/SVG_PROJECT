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

        // Nếu bounds con rỗng (ví dụ gradient def), bỏ qua
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

    // --- CHUẨN BỊ THUỘC TÍNH CỦA GROUP (CHA) ---
    SVGColor parentFill = this->getFill();
    SVGStroke parentStroke = this->getStroke();

    // Tính toán độ trong suốt của Group (Global Opacity)
    // Tạm dùng logic cũ của bạn: lấy alpha của fill làm chuẩn opacity cho group
    float groupAlpha = this->getOpacity();

    // Duyệt qua từng phần tử con
    for (SVGElement* element : group)
    {
        if (!element) continue;

        // 1. SNAPSHOT (Lưu trạng thái gốc)
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

        // ================= XỬ LÝ FILL =================

        // A. THỪA KẾ (Inheritance)
        // Thay vì check getA() == 0, ta check !isSet()
        // Nếu con CHƯA SET màu và cha ĐÃ SET màu -> Lấy của cha
        if (!originalFill.isSet() && !originalFill.isNone() && parentFill.isSet())
        {
            element->setFill(parentFill);
            isFillChanged = true;
        }

        // B. BLACK FALLBACK (Sửa lỗi Mắt người tuyết)
        // Nếu sau khi thử thừa kế mà vẫn CHƯA SET (và không phải none) -> Gán màu Đen
        // LƯU Ý QUAN TRỌNG: Với hình Thái Cực Đồ (fill-opacity="0"), isSet() đã là TRUE 
        // nên nó sẽ KHÔNG nhảy vào đây -> Giữ nguyên màu trong suốt -> FIX ĐƯỢC LỖI
        if (!element->getFill().isSet() && !element->getFill().isNone())
        {
            SVGColor defaultBlack(0, 0, 0, 255);
            element->setFill(defaultBlack);
            isFillChanged = true;
        }

        // C. Thừa kế Opacity (Propagate Opacity Value)
        // Nếu con chưa set opacity, copy từ cha xuống để con dùng
        if (!element->isFillOpacitySet() && this->isFillOpacitySet())
        {
            element->setFillOpacity(this->getFillOpacity());
            element->setIsFillOpacitySet(true);
            isFillOpChanged = true; 
        }

        // D. Tính Alpha (Baking Alpha)
        if (element->getFill().isSet())
        {
            float effectiveOp = element->getFillOpacity(); // Đã bao gồm kế thừa ở bước C
            float baseAlpha = (float)element->getFill().getA();
            float finalAlpha = 255.0f;

            if (element->isGroup()) {
                // NẾU LÀ GROUP: KHÔNG nhân fill-opacity vào màu.
                // Vì group con sẽ tự nhân khi nó vẽ các shape của nó.
                // Chỉ nhân Global Opacity vì nó stack theo layer.
                finalAlpha = baseAlpha * groupAlpha * element->getOpacity();
            }
            else {
                // NẾU LÀ SHAPE: Nhân tất cả.
                finalAlpha = baseAlpha * effectiveOp * groupAlpha * element->getOpacity();
            }

            SVGColor c = element->getFill();
            c.setA((BYTE)finalAlpha);
            element->setFill(c);
            isFillChanged = true;
        }

        // ================= XỬ LÝ STROKE =================

        SVGStroke newStroke = originalStroke;
        SVGColor strokeColor = newStroke.getColor();

        // A. Inherit Stroke Color
        if (!strokeColor.isSet() && !strokeColor.isNone() && parentStroke.getColor().isSet()) {
            newStroke.setColor(parentStroke.getColor());
            strokeColor = newStroke.getColor();
            isStrokeChanged = true;
        }
        // B. Inherit Width
        if (newStroke.getWidth() <= 0.0f && parentStroke.getWidth() > 0.0f) {
            if (strokeColor.isSet() && !strokeColor.isNone()) {
                newStroke.setWidth(parentStroke.getWidth());
                isStrokeChanged = true;
            }
        }
        // C. Inherit Stroke Opacity
        if (!element->isStrokeOpacitySet() && this->isStrokeOpacitySet()) {
            element->setStrokeOpacity(this->getStrokeOpacity());
            element->setIsStrokeOpacitySet(true);
            isStrokeOpChanged = true;
        }
        // D. Calculate Stroke Alpha
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

        // Áp dụng Stroke mới nếu có thay đổi
        if (isStrokeChanged) {
            element->setStroke(newStroke);
        }

        // 3. VẼ (RENDER)
        element->render(graphics);

        // 4. HOÀN TÁC TRẠNG THÁI (REVERT)
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
