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

//void SVGGroup::draw(Graphics* graphics)
//{
//	if (!graphics) return;
//
//	GraphicsState state = graphics->Save();
//	graphics->SetSmoothingMode(SmoothingModeAntiAlias);
//
//	//string transformStr = this->getTransform();
//	//if (!transformStr.empty())
//	//{
//	//	size_t pos = transformStr.find("translate");
//	//	if (pos != string::npos)
//	//	{
//	//		string valStr = transformStr.substr(pos + 10);
//	//		size_t endPos = valStr.find(")");
//	//		if (endPos != string::npos) {
//	//			valStr = valStr.substr(0, endPos);
//	//			for (char& c : valStr) if (c == ',') c = ' ';
//	//			stringstream ss(valStr);
//	//			float tx = 0, ty = 0;
//	//			ss >> tx >> ty;
//	//			graphics->TranslateTransform(tx, ty);
//	//		}
//	//	}
//	//	pos = transformStr.find("rotate");
//	//	if (pos != string::npos) {
//	//		string valStr = transformStr.substr(pos + 7);
//	//		size_t endPos = valStr.find(")");
//	//		if (endPos != string::npos) {
//	//			valStr = valStr.substr(0, endPos);
//	//			stringstream ss(valStr);
//	//			float angle = 0;
//	//			ss >> angle;
//	//			graphics->RotateTransform(angle);
//	//		}
//	//	}
//	//	pos = transformStr.find("scale");
//	//	if (pos != string::npos) {
//	//		string valStr = transformStr.substr(pos + 6);
//	//		size_t endPos = valStr.find(")");
//	//		if (endPos != string::npos) {
//	//			valStr = valStr.substr(0, endPos);
//	//			stringstream ss(valStr);
//	//			float sx = 1.0f, sy = 1.0f;
//	//			ss >> sx;
//	//			if (!(ss >> sy)) sy = sx;
//	//			graphics->ScaleTransform(sx, sy);
//	//		}
//	//	}
//	//}
//
//	float parentFillAlpha = 1.0f;
//	if (this->getFill().getA() > 0) {
//		parentFillAlpha = (float)this->getFill().getA() / 255.0f;
//	}
//
//	float parentStrokeAlpha = 1.0f;
//	if (this->getStroke().getColor().getA() < 255 && this->getStroke().getColor().getA() > 0) {
//		parentStrokeAlpha = (float)this->getStroke().getColor().getA() / 255.0f;
//	}
//
//	SVGStroke parentStrokeStyle = this->getStroke();
//
//	for (SVGElement* element : group)
//	{
//		if (element)
//		{
//			SVGColor originalFill = element->getFill();
//			SVGStroke originalStroke = element->getStroke();
//			if (originalFill.getA() > 0)
//			{
//				float childFillAlpha = (float)originalFill.getA() / 255.0f;
//				float finalFillAlpha = childFillAlpha * parentFillAlpha;
//				SVGColor newFill = originalFill;
//				newFill.setA((BYTE)(finalFillAlpha * 255.0f));
//				element->setFill(newFill);
//			}
//
//			// 1. XU LY FILL
//			if (originalFill.getA() == 0 && this->getFill().getA() > 0) {
//				element->setFill(this->getFill());
//				originalFill = this->getFill();
//			}
//
//			SVGStroke newStroke = originalStroke;
//			bool strokeUpdated = false;
//			if (newStroke.getWidth() == 0 && parentStrokeStyle.getWidth() > 0)
//			{
//				newStroke = parentStrokeStyle;
//				strokeUpdated = true;
//			}
//			else if (newStroke.getWidth() > 0 && newStroke.getColor().getA() == 0)
//			{
//				if (parentStrokeStyle.getColor().getA() > 0) {
//					SVGColor inheritedColor = parentStrokeStyle.getColor();
//					newStroke.setColor(inheritedColor);
//					strokeUpdated = true;
//				}
//			}
//
//			if (newStroke.getWidth() > 0 && newStroke.getColor().getA() > 0)
//			{
//				float childStrokeAlpha = (float)newStroke.getColor().getA() / 255.0f;
//				float finalStrokeAlpha = childStrokeAlpha * parentStrokeAlpha;
//
//				if (finalStrokeAlpha != childStrokeAlpha) {
//					SVGColor color = newStroke.getColor();
//					color.setA((BYTE)(finalStrokeAlpha * 255.0f));
//					newStroke.setColor(color);
//					strokeUpdated = true;
//				}
//			}
//
//			if (strokeUpdated) {
//				element->setStroke(newStroke);
//			}
//
//			element->draw(graphics);
//
//			element->setFill(originalFill);
//			element->setStroke(originalStroke);
//		}
//	}
//
//	graphics->Restore(state);
//}



void SVGGroup::draw(Graphics* graphics)
{
    if (!graphics) return;

    // Lưu trạng thái Graphics của GDI+ (để đảm bảo các thiết lập như SmoothingMode không ảnh hưởng ra ngoài)
    GraphicsState state = graphics->Save();
    graphics->SetSmoothingMode(SmoothingModeAntiAlias);

    // --- CHUẨN BỊ THUỘC TÍNH CỦA GROUP (CHA) ---
    SVGColor parentFill = this->getFill();
    SVGStroke parentStroke = this->getStroke();
    float parentOpacity = this->getOpacity(); // Nếu parser hỗ trợ opacity=""

    // Nếu parser chưa hỗ trợ opacity riêng, ta tạm dùng alpha của fill làm opacity (theo code cũ của bạn)
    float groupAlpha = 1.0f;
    if (parentFill.getA() > 0) {
        // Lưu ý: Đây là logic tạm thời nếu chưa có thuộc tính opacity riêng biệt
        groupAlpha = (float)parentFill.getA() / 255.0f;
    }

    // Duyệt qua từng phần tử con để vẽ
    for (SVGElement* element : group)
    {
        if (!element) continue;

        // 1. LƯU TRẠNG THÁI GỐC CỦA CON (SNAPSHOT)
        // Quan trọng: Lưu lại để revert sau khi vẽ, tránh làm hỏng dữ liệu của con
        SVGColor originalFill = element->getFill();
        SVGStroke originalStroke = element->getStroke();

        bool isFillChanged = false;
        bool isStrokeChanged = false;

        // 2. TÍNH TOÁN THỪA KẾ (INHERITANCE LOGIC)

        // --- XỬ LÝ FILL ---
        // Logic: Nếu con chưa có màu (Alpha = 0) và cha có màu -> Con nhận màu cha.
        // (Lưu ý: Vẫn còn hạn chế với trường hợp fill="none", cần flag isNone trong SVGElement để fix triệt để)
        if (originalFill.getA() == 0 && parentFill.getA() > 0)
        {
            element->setFill(parentFill);
            isFillChanged = true;
        }
        else if (originalFill.getA() > 0)
        {
            // Nếu con đã có màu, nhân thêm độ trong suốt của Group (Opacity Stacking giả lập)
            if (groupAlpha < 1.0f) {
                SVGColor blendedFill = originalFill;
                float currentAlpha = (float)originalFill.getA() / 255.0f;
                blendedFill.setA((BYTE)(currentAlpha * groupAlpha * 255.0f));
                element->setFill(blendedFill);
                isFillChanged = true;
            }
        }

        // --- XỬ LÝ STROKE ---
        SVGStroke newStroke = originalStroke;
        SVGColor childStrokeColor = newStroke.getColor();

        // Case A: Con không có độ dày nét, nhưng Cha có -> Thừa kế độ dày
        if (newStroke.getWidth() <= 0.0f && parentStroke.getWidth() > 0.0f)
        {
            newStroke.setWidth(parentStroke.getWidth());
            isStrokeChanged = true;
        }

        // Case B: Con chưa có màu nét, nhưng Cha có -> Thừa kế màu nét
        if (childStrokeColor.getA() == 0 && parentStroke.getColor().getA() > 0)
        {
            newStroke.setColor(parentStroke.getColor()); // Lấy màu cha
            childStrokeColor = newStroke.getColor();     // Cập nhật biến tạm
            isStrokeChanged = true;
        }

        // Case C: Xử lý Opacity cho Stroke (nhân với opacity của group)
        if (childStrokeColor.getA() > 0 && groupAlpha < 1.0f)
        {
            float currentStrokeAlpha = (float)childStrokeColor.getA() / 255.0f;
            childStrokeColor.setA((BYTE)(currentStrokeAlpha * groupAlpha * 255.0f));
            newStroke.setColor(childStrokeColor);
            isStrokeChanged = true;
        }

        // Áp dụng Stroke mới nếu có thay đổi
        if (isStrokeChanged) {
            element->setStroke(newStroke);
        }

        // 3. VẼ (RENDER)
        // Gọi đệ quy vẽ con với các thuộc tính đã được tính toán (resolved attributes)
        element->render(graphics);// thay draw thanh render de trung gian qua element::render xu li transform.

        // 4. HOÀN TÁC TRẠNG THÁI (REVERT MUTATION)
        // Trả lại nguyên hiện trạng cho object con để không ảnh hưởng các lần vẽ sau
        if (isFillChanged) {
            element->setFill(originalFill);
        }
        if (isStrokeChanged) {
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
