#include "stdafx.h"
#include "SVGElement.h"

SVGElement::SVGElement()
{
    id = className = style = transform =  "";
    opacity = 0;
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

void SVGElement::parseAttributes(xml_node<>* Node)
{
	if (xml_attribute<>* attribute = Node->first_attribute("id"))
	{
		id = attribute->value();
		/*if (!this->id.empty()) {*/
		/*	SVGElement::idMap[this->id] = this;
		}*/
	}
	if (xml_attribute<>* attribute = Node->first_attribute("class"))
		className = attribute->value();
	if (xml_attribute<>* attribute = Node->first_attribute("style"))
		style = attribute->value();
	if (xml_attribute<>* attribute = Node->first_attribute("transform"))
	{
		transform = attribute->value();
		parseTransform();
	}

	// --- FIX 1: LOGIC FILL ---
	bool isFillNone = false;
	if (xml_attribute<>* attribute = Node->first_attribute("fill"))
	{
		string Tempcolor = attribute->value();
		if (Tempcolor == "none") isFillNone = true;
		fill = SVGColor::fromString(Tempcolor);
	}

	if (xml_attribute<>* attribute = Node->first_attribute("fill-opacity"))
	{
		float opacityVal = atof(attribute->value());
		// FIX: Bo dieu kien fill.getA() > 0. 
		// Cho phep set alpha ke ca khi mau la mac dinh (den/trong suot), 
		// MIEN LA khong phai "none" tuong minh.
		if (!isFillNone) {
			// Neu dang la 0 (mac dinh), ta cu set no thanh 255 * opacity 
			// de Group co the luu gia tri nay truyen cho con.
			float currentAlpha = (fill.getA() == 0) ? 255.0f : (float)fill.getA();
			fill.setA((BYTE)(currentAlpha * opacityVal));
		}
	}

	// --- FIX 2: LOGIC STROKE ---
	bool isStrokeNone = false;
	if (xml_attribute<>* attribute = Node->first_attribute("stroke"))
	{
		string TempColorStroke = attribute->value();
		if (TempColorStroke == "none") isStrokeNone = true;
		SVGColor temp = SVGColor::fromString(TempColorStroke);
		stroke.setColor(temp);
	}

	if (xml_attribute<>* attribute = Node->first_attribute("stroke-width"))
	{
		stroke.setWidth(atof(attribute->value()));
	}

	if (xml_attribute<>* attribute = Node->first_attribute("stroke-opacity"))
	{
		float opacityVal = atof(attribute->value());
		if (!isStrokeNone) {
			SVGColor c = stroke.getColor();
			float currentAlpha = (c.getA() == 0) ? 255.0f : (float)c.getA();
			c.setA((BYTE)(currentAlpha * opacityVal));
			stroke.setColor(c);
		}
	}

	 /*Chot ha do day vien (nhu cu)*/
	if (stroke.getColor().getA() > 0 && stroke.getWidth() <= 0.0f)
	{
		stroke.setWidth(1.0f);
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


//void SVGElement::parseAttributes(xml_node<>* Node)
//{
//    if (xml_attribute<>* attribute = Node->first_attribute("id"))
//    {
//        id = attribute->value();
//        cout << "id: " << id << ' ';
//    }
//    if (xml_attribute<>* attribute = Node->first_attribute("class"))
//    {
//        className = attribute->value();
//        cout << "class: " << className << ' ';
//    }
//    if (xml_attribute<>* attribute = Node->first_attribute("style"))
//    {
//        style = attribute->value();
//        cout << "style: " << style << ' ';
//    }
//    if (xml_attribute<>* attribute = Node->first_attribute("transform"))
//    {
//        transform = attribute->value();
//        cout << "transform: " << transform << ' ';
//    }
//    if (xml_attribute<>* attribute = Node->first_attribute("fill"))
//    {
//        string Tempcolor = attribute->value();
//        fill=SVGColor::fromString(Tempcolor);
//        //cout << "fill: " << Tempcolor << ' ';
//        cout << "fill: ";
//        cout << "r: " << (int)fill.getR() << " g: " << (int)fill.getG() << " B: " << (int)fill.getB() << ' ';
//    }
//    if (xml_attribute<>* attribute = Node->first_attribute("fill-opacity"))
//    {
//        fill.setA((BYTE)(atof(attribute->value())*255));
//        cout << "fill-opacity: " << (int)fill.getA() << ' ';
//    }
//    if (xml_attribute<>* attribute = Node->first_attribute("stroke"))
//    {
//        string TempColorStroke = attribute->value();
//        SVGColor temp = SVGColor::fromString(TempColorStroke);
//        stroke.setColor(temp);
//        cout << "stroke: ";
//        cout << "r: " << (int)stroke.getColor().getR() << " g: " << (int)stroke.getColor().getG() << " B: " <<(int) stroke.getColor().getB() << ' ';
//    }
//    if (xml_attribute<>* attribute = Node->first_attribute("stroke-width"))
//    {
//        stroke.setWidth(atof(attribute->value()));
//        cout << "stroke-width: " << stroke.getWidth() << ' ';
//    }
//    if (xml_attribute<>* attribute = Node->first_attribute("stroke-opacity"))
//    {
//        stroke.getColor().setA(BYTE(atof(attribute->value())*255));
//        cout << "stroke-opacity: " <<(int) stroke.getColor().getA() << ' ';
//    }
//}