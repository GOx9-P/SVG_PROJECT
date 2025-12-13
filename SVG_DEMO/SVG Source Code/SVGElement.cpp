#include "stdafx.h"
#include "SVGElement.h"

// Hàm hỗ trợ xóa khoảng trắng thừa đầu/cuối chuỗi
string trim(const string& str) {
	size_t first = str.find_first_not_of(" \t\n\r");
	if (string::npos == first) return str;
	size_t last = str.find_last_not_of(" \t\n\r");
	return str.substr(first, (last - first + 1));
}

// Hàm phân tích cú pháp CSS Inline
void parseStyleString(const string& styleStr, SVGColor& outFill,float&outFillOp, bool& outFillOpSet, SVGStroke& outStroke, float& outStrokeOp,bool& outStrokeOpSet) {
	stringstream ss(styleStr);
	string segment;

	// 1. Tách các cặp thuộc tính bằng dấu chấm phẩy ';'
	while (getline(ss, segment, ';')) {
		size_t colonPos = segment.find(':');
		if (colonPos != string::npos) {
			string key = trim(segment.substr(0, colonPos));
			string value = trim(segment.substr(colonPos + 1));

			// 2. Map sang các thuộc tính tương ứng
			if (key == "fill") {
				outFill = SVGColor::fromString(value);
			}
			else if (key == "stroke") {
				if (value == "none")
				{
					SVGColor temp(0, 0, 0, 0);
					temp.setIsNone(true);
					outStroke.setColor(temp);
				}
				else
				{
					SVGColor c = SVGColor::fromString(value);
					outStroke.setColor(c);
				}
			}
			else if (key == "stroke-width") {
				// Xóa chữ "px" nếu có
				size_t pxPos = value.find("px");
				if (pxPos != string::npos) value = value.substr(0, pxPos);
				outStroke.setWidth(atof(value.c_str()));
			}
			else if (key == "fill-opacity") {
				outFillOp = atof(value.c_str());
				outFillOpSet = true;
			}
			else if (key == "stroke-opacity") {
				outStrokeOp = atof(value.c_str());
				outStrokeOpSet = true;
			}
			else if (key == "stroke-linecap") {
				outStroke.setLineCap(value);
			}
			else if (key == "stroke-linejoin") {
				outStroke.setLineJoin(value);
			}
		}
	}
}

SVGElement::SVGElement()
{
    id = className = style = transform =  "";
    opacity = 1.0f;
	fillOpacity = 1.0f;
	strokeOpacity = 1.0f;
	isFillOpSet = false;
	isStrokeOpSet = false;
	this->transformMatrix = new Gdiplus::Matrix();
}

SVGElement::~SVGElement()
{

}

string SVGElement::getId() const {
    return id;
}

void SVGElement::setId(const string& newId) {
    id = newId;
}

string SVGElement::getClassName() const {
    return className;
}

void SVGElement::setClassName(const string& newClassName) {
    className = newClassName;
}

string SVGElement::getStyle() const {
    return style;
}

void SVGElement::setStyle(const string& newStyle) {
    style = newStyle;
}

string SVGElement::getTransform() const {
    return transform;
}

void SVGElement::setTransform(const string& newTransform) {
    transform = newTransform;
}

SVGColor SVGElement::getFill() const {
    return fill;
}

void SVGElement::setFill(const SVGColor& newFill) {
    fill = newFill;
}

SVGStroke SVGElement::getStroke() const {
    return stroke;
}

void SVGElement::setStroke(const SVGStroke& newStroke) {
    stroke = newStroke;
}

float SVGElement::getOpacity() const {
    return opacity;
}

void SVGElement::setOpacity(const float& newOpacity) {
    opacity = newOpacity;
}

float SVGElement::getFillOpacity() const
{ 
	return fillOpacity; 
}
void SVGElement::setFillOpacity(const float& op) 
{ 
	fillOpacity = op; 
}

float SVGElement::getStrokeOpacity() const
{
	return strokeOpacity;
}
void SVGElement::setStrokeOpacity(const float& op)
{ 
	strokeOpacity = op; 
}

bool SVGElement::isFillOpacitySet() const 
{ 
	return isFillOpSet; 
}
void SVGElement::setIsFillOpacitySet(bool val) { 
	isFillOpSet = val; 
}

bool SVGElement::isStrokeOpacitySet() const
{ 
	return isStrokeOpSet; 
}
void SVGElement::setIsStrokeOpacitySet(bool val) 
{ 
	isStrokeOpSet = val;
}

bool SVGElement::isGroup() const { return false; }

void preProcessingTranSform(string& s)
{
	for (int i = 0; i < s.size(); i++)
	{
		if (s[i] == ',')s[i] = ' ';
	}
}

string deleteSpace(const string& s) {
	int l = 0, r = s.size() - 1;
	while (l <= r && isspace(s[l])) l++;
	while (r >= l && isspace(s[r])) r--;
	return s.substr(l, r - l + 1);
}

float DegToRad(float Deg)
{
	return Deg * 3.1415926 / 180.0f;
}

void SVGElement::parseTransform()
{
	preProcessingTranSform(transform);
	stringstream ss(transform);
	string info;
	while (getline(ss, info, ')'))
	{
		info = deleteSpace(info);
		if (info.empty()) continue;
		stringstream ss1(info);
		string command;
		getline(ss1, command, '(');
		command = deleteSpace(command);
		
		// vi trong transform nhan cac lenh tu phai sang trai nen ta phai nhan vao phia truoc ma tran.
		if (command == "translate")
		{
			float x = 0, y = 0;
			ss1 >> x;
			if (!(ss1 >> y)) y = 0;
			transformMatrix->Translate(x, y, Gdiplus::MatrixOrderPrepend);
		}
		else if (command == "rotate")
		{
			float angle = 0, cx = 0, cy = 0;
			ss1 >> angle;
			bool hasCenter = (bool)(ss1 >> cx);
			if (hasCenter) ss1 >> cy;
			if (hasCenter) {
				transformMatrix->Translate(cx, cy, Gdiplus::MatrixOrderPrepend);
				transformMatrix->Rotate(angle, Gdiplus::MatrixOrderPrepend);
				transformMatrix->Translate(-cx, -cy, Gdiplus::MatrixOrderPrepend);
			}
			else {
				transformMatrix->Rotate(angle, Gdiplus::MatrixOrderPrepend);
			}
		}
		else if (command == "scale")
		{
			float sx = 1, sy = 1;
			ss1 >> sx;
			if (!(ss1 >> sy)) sy = sx;
			transformMatrix->Scale(sx, sy, Gdiplus::MatrixOrderPrepend);
		}
		else if (command == "skewX")
		{
			float a = 0;
			ss1 >> a;
			float shearFactor = tan(DegToRad(a));
			transformMatrix->Shear(shearFactor, 0, Gdiplus::MatrixOrderPrepend);
		}
		else if (command == "skewY")
		{
			float a = 0;
			ss1 >> a;
			float shearFactor = tan(DegToRad(a));
			transformMatrix->Shear(0, shearFactor, Gdiplus::MatrixOrderPrepend);
		}
		else if (command == "matrix")
		{
			float a = 1, b = 0, c = 0, d = 1, e = 0, f = 0;
			ss1 >> a >> b >> c >> d >> e >> f;
			Gdiplus::Matrix tempMatrix(a, b, c, d, e, f);
			transformMatrix->Multiply(&tempMatrix, Gdiplus::MatrixOrderPrepend);
		}
	}
}

//void SVGElement::parseAttributes(xml_node<>* Node)
//{
//	if (xml_attribute<>* attribute = Node->first_attribute("id"))
//	{
//		id = attribute->value();
//		/*if (!this->id.empty()) {*/
//		/*	SVGElement::idMap[this->id] = this;
//		}*/
//	}
//	if (xml_attribute<>* attribute = Node->first_attribute("class"))
//		className = attribute->value();
//	if (xml_attribute<>* attribute = Node->first_attribute("style"))
//		style = attribute->value();
//	if (xml_attribute<>* attribute = Node->first_attribute("transform"))
//	{
//		transform = attribute->value();
//		parseTransform();
//	}
//
//	if (xml_attribute<>* attr = Node->first_attribute("opacity"))
//	{
//		this->opacity = atof(attr->value());
//	}
//	 //--- FIX 1: LOGIC FILL ---
//	bool isFillNone = false;
//	if (xml_attribute<>* attribute = Node->first_attribute("fill"))
//	{
//		string Tempcolor = attribute->value();
//		if (Tempcolor == "none") isFillNone = true;
//		fill = SVGColor::fromString(Tempcolor);
//	}
//
//	// 1. Đọc fill-opacity (Mặc định là 1.0f nếu không có)
//	float fillOp = 1.0f;
//	if (xml_attribute<>* attribute = Node->first_attribute("fill-opacity")) {
//		fillOp = atof(attribute->value());
//	}
//
//	// 2. Tính toán Alpha cuối cùng
//	if (!isFillNone) {
//		// QUAN TRỌNG: Chỉ nhân Opacity nếu hình ĐÃ CÓ MÀU (Alpha > 0).
//		// Nếu Alpha == 0 (tức là chưa có màu, ví dụ <circle>), ta GIỮ NGUYÊN là 0
//		// để SVGGroup có thể truyền màu của cha xuống (Inheritance).
//
//		if (fill.getA() > 0) {
//			float currentAlpha = (float)fill.getA();
//			// Công thức chuẩn: Alpha Gốc * Fill-Opacity * Global-Opacity
//			fill.setA((BYTE)(currentAlpha * fillOp * this->opacity));
//		}
//	}
//
//	 // --- FIX 2: LOGIC STROKE ---
//	bool isStrokeNone = false;
//	if (xml_attribute<>* attribute = Node->first_attribute("stroke"))
//	{
//		string TempColorStroke = attribute->value();
//		if (TempColorStroke == "none") isStrokeNone = true;
//		SVGColor temp = SVGColor::fromString(TempColorStroke);
//		stroke.setColor(temp);
//	}
//
//	if (xml_attribute<>* attribute = Node->first_attribute("stroke-width"))
//	{
//		stroke.setWidth(atof(attribute->value()));
//	}
//
//	// 1. Đọc stroke-opacity
//	float strokeOp = 1.0f;
//	if (xml_attribute<>* attribute = Node->first_attribute("stroke-opacity")) {
//		strokeOp = atof(attribute->value());
//	}
//
//	// 2. Tính toán Alpha cuối cùng
//	if (!isStrokeNone) {
//		SVGColor c = stroke.getColor();
//
//		// Tương tự Fill: Chỉ nhân nếu nét vẽ ĐÃ CÓ MÀU
//		if (c.getA() > 0) {
//			float currentAlpha = (float)c.getA();
//			c.setA((BYTE)(currentAlpha * strokeOp * this->opacity));
//			stroke.setColor(c);
//		}
//	}
//
//
//	if (xml_attribute<>* attr = Node->first_attribute("stroke-linecap"))
//	{
//		this->stroke.setLineCap(attr->value());
//	}
//	if (xml_attribute<>* attr = Node->first_attribute("stroke-linejoin"))
//	{
//		this->stroke.setLineJoin(attr->value());
//	}
//
//	if (stroke.getColor().getA() > 0 && stroke.getWidth() <= 0.0f)
//	{
//		stroke.setWidth(1.0f);
//	}
//	if (xml_attribute<>* attribute = Node->first_attribute("style"))
//	{
//		string styleStr = attribute->value();
//		parseStyleString(styleStr, this->fill, this->stroke);
//	}
//}

void SVGElement::parseAttributes(xml_node<>* Node)
{
	// 1. Lấy ID, Class, Transform (Giữ nguyên)
	if (xml_attribute<>* attribute = Node->first_attribute("id")) {
		id = attribute->value();
	}
	if (xml_attribute<>* attribute = Node->first_attribute("class"))
		className = attribute->value();
	if (xml_attribute<>* attribute = Node->first_attribute("style"))
		style = attribute->value();
	if (xml_attribute<>* attribute = Node->first_attribute("transform")) {
		transform = attribute->value();
		parseTransform();
	}

	// 2. Lấy Global Opacity
	if (xml_attribute<>* attr = Node->first_attribute("opacity")) {
		this->opacity = atof(attr->value());
	}

	// ================= XỬ LÝ FILL =================

	// Bước A: Đọc màu fill
	if (xml_attribute<>* attribute = Node->first_attribute("fill")) {
		string Tempcolor = attribute->value();
		if (Tempcolor == "none")
		{
			fill.setIsNone(true);
		}
		else
		{
			fill = SVGColor::fromString(Tempcolor); // Hàm này sẽ set isSet() = true
		}
	}

	// Bước B: Đọc fill-opacity và xử lý logic đặc biệt
	if (xml_attribute<>* attribute = Node->first_attribute("fill-opacity")) {
		this->fillOpacity = atof(attribute->value());
		this->isFillOpSet = true;
	}

	// ================= XỬ LÝ STROKE =================
	// Bước A: Đọc màu stroke
	if (xml_attribute<>* attribute = Node->first_attribute("stroke")) {
		string TempColorStroke = attribute->value();
		if (TempColorStroke == "none")
		{
			SVGColor c(0, 0, 0, 0);
			c.setIsNone(true);
			stroke.setColor(c); 
		}
		else
		{
			SVGColor tmp = SVGColor::fromString(TempColorStroke);
			stroke.setColor(tmp);
		}
	}

	if (xml_attribute<>* attribute = Node->first_attribute("stroke-width")) {
		stroke.setWidth(atof(attribute->value()));
	}

	// Bước B: Đọc stroke-opacity
	if (xml_attribute<>* attribute = Node->first_attribute("stroke-opacity")) {
		this->strokeOpacity = atof(attribute->value());
		this->isStrokeOpSet = true;
	}

	// ================= CÁC THUỘC TÍNH KHÁC =================
	if (xml_attribute<>* attr = Node->first_attribute("stroke-linecap")) {
		this->stroke.setLineCap(attr->value());
	}
	if (xml_attribute<>* attr = Node->first_attribute("stroke-linejoin")) {
		this->stroke.setLineJoin(attr->value());
	}

	// Fix độ dày nét vẽ nếu có màu mà chưa có độ dày
	if (stroke.getColor().isSet() && stroke.getColor().getA() > 0 && stroke.getWidth() <= 0.0f) {
		stroke.setWidth(1.0f);
	}

	// Parse Style (Inline CSS) - Lưu ý: Phần này nên đặt cẩn thận vì nó có thể override các thuộc tính trên
	if (xml_attribute<>* attribute = Node->first_attribute("style")) {
		string styleStr = attribute->value();
		parseStyleString(styleStr, this->fill, this->fillOpacity, this->isFillOpSet, this->stroke, this->strokeOpacity, this->isStrokeOpSet);

		// Lưu ý bổ sung: Nếu trong style có opacity, bạn cần áp dụng logic nhân this->opacity tương tự ở trên
		// Tuy nhiên với cấu trúc hiện tại hàm parseStyleString của bạn đang parse trực tiếp vào fill/stroke
		// Nên nếu muốn chính xác tuyệt đối, bạn nên chuyển việc nhân Global Opacity xuống cuối cùng hàm này.
	}
}

// them ham render de transform, no se transform sau do goi draw de ve cac hinh binh thuong. 
// (o svgroot va group khi ve hinh thay vi goi element->draw thi ta se qua trung gian element->render de xu li).
void SVGElement::render(Gdiplus::Graphics* graphics)
{
	Gdiplus::GraphicsState state = graphics->Save();
	graphics->MultiplyTransform(this->transformMatrix, Gdiplus::MatrixOrderPrepend);
	this->draw(graphics);
	graphics->Restore(state);
}

