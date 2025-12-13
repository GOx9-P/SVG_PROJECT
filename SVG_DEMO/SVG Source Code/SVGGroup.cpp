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
//void SVGGroup::draw(Graphics* graphics)
//{
//    if (!graphics) return;
//
//    // Lưu trạng thái Graphics của GDI+ (để đảm bảo các thiết lập như SmoothingMode không ảnh hưởng ra ngoài)
//    GraphicsState state = graphics->Save();
//    graphics->SetSmoothingMode(SmoothingModeAntiAlias);
//
//    // --- CHUẨN BỊ THUỘC TÍNH CỦA GROUP (CHA) ---
//    SVGColor parentFill = this->getFill();
//    SVGStroke parentStroke = this->getStroke();
//    float parentOpacity = this->getOpacity(); // Nếu parser hỗ trợ opacity=""
//
//    // Nếu parser chưa hỗ trợ opacity riêng, ta tạm dùng alpha của fill làm opacity (theo code cũ của bạn)
//    float groupAlpha = 1.0f;
//    if (parentFill.getA() > 0) {
//        // Lưu ý: Đây là logic tạm thời nếu chưa có thuộc tính opacity riêng biệt
//       groupAlpha = (float)parentFill.getA() / 255.0f;
//    }
//    
//    // Duyệt qua từng phần tử con để vẽ
//    for (SVGElement* element : group)
//    {
//        if (!element) continue;
//
//        // 1. LƯU TRẠNG THÁI GỐC CỦA CON (SNAPSHOT)
//        // Quan trọng: Lưu lại để revert sau khi vẽ, tránh làm hỏng dữ liệu của con
//        SVGColor originalFill = element->getFill();
//        SVGStroke originalStroke = element->getStroke();
//
//        bool isFillChanged = false;
//        bool isStrokeChanged = false;
//
//        // 2. TÍNH TOÁN THỪA KẾ (INHERITANCE LOGIC)
//
//        // --- XỬ LÝ FILL ---
//        // Logic: Nếu con chưa có màu (Alpha = 0) và cha có màu -> Con nhận màu cha.
//        if (originalFill.getA() == 0 && !originalFill.isNone() && parentFill.getA() > 0)
//        {
//            element->setFill(parentFill);
//            isFillChanged = true;
//        }
//        else if (originalFill.getA() > 0)
//        {
//            // Nếu con đã có màu, nhân thêm độ trong suốt của Group (Opacity Stacking giả lập)
//            if (groupAlpha < 1.0f) {
//                SVGColor blendedFill = originalFill;
//                float currentAlpha = (float)originalFill.getA() / 255.0f;
//                blendedFill.setA((BYTE)(currentAlpha * groupAlpha * 255.0f));
//                element->setFill(blendedFill);
//                isFillChanged = true;
//            }
//        }
//
//        // --- XỬ LÝ STROKE ---
//        SVGStroke newStroke = originalStroke;
//        SVGColor childStrokeColor = newStroke.getColor();
//
//        // Case A: Con không có độ dày nét, nhưng Cha có -> Thừa kế độ dày
//        if (newStroke.getWidth() <= 0.0f && parentStroke.getWidth() > 0.0f && !childStrokeColor.isNone())
//        {
//            newStroke.setWidth(parentStroke.getWidth());
//            isStrokeChanged = true;
//        }
//
//        // Case B: Con chưa có màu nét, nhưng Cha có -> Thừa kế màu nét
//        if (childStrokeColor.getA() == 0 && !childStrokeColor.isNone() && parentStroke.getColor().getA() > 0)
//        {
//            newStroke.setColor(parentStroke.getColor());
//            childStrokeColor = newStroke.getColor();
//            isStrokeChanged = true;
//        }
//
//        // Case C: Xử lý Opacity cho Stroke (nhân với opacity của group)
//        if (childStrokeColor.getA() > 0 && groupAlpha < 1.0f)
//        {
//            float currentStrokeAlpha = (float)childStrokeColor.getA() / 255.0f;
//            childStrokeColor.setA((BYTE)(currentStrokeAlpha * groupAlpha * 255.0f));
//            newStroke.setColor(childStrokeColor);
//            isStrokeChanged = true;
//        }
//
//        // Áp dụng Stroke mới nếu có thay đổi
//        if (isStrokeChanged) {
//            element->setStroke(newStroke);
//        }
//
//        if (element->getFill().getA() == 0 && !element->getFill().isNone())
//        {
//            SVGColor defaultBlack(0, 0, 0, 255);
//            element->setFill(defaultBlack);
//            isFillChanged = true; // Đánh dấu để revert sau khi vẽ xong
//        }
//
//        // 3. VẼ (RENDER)
//        // Gọi đệ quy vẽ con với các thuộc tính đã được tính toán (resolved attributes)
//        element->render(graphics);// thay draw thanh render de trung gian qua element::render xu li transform.
//
//        // 4. HOÀN TÁC TRẠNG THÁI (REVERT MUTATION)
//        // Trả lại nguyên hiện trạng cho object con để không ảnh hưởng các lần vẽ sau
//        if (isFillChanged) {
//            element->setFill(originalFill);
//        }
//        if (isStrokeChanged) {
//            element->setStroke(originalStroke);
//        }
//    }
//
//    graphics->Restore(state);
//}


SVGGroup::~SVGGroup()
{
	for (int i = 0; i < group.size(); i++)
	{
		delete group[i];
		group[i] = nullptr;
	}
}
