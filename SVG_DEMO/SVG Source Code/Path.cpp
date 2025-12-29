#include "stdafx.h"
#include "Path.h"
#include <cmath>
#include <algorithm>
#include <sstream> 

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Hàm tính góc giữa 2 vector
double angleBetween(PointF u, PointF v) {
    double dot = u.X * v.X + u.Y * v.Y;
    double len = sqrt(u.X * u.X + u.Y * u.Y) * sqrt(v.X * v.X + v.Y * v.Y);
    double ang = acos(max(-1.0, min(1.0, dot / len))); // Clamp để tránh lỗi NaN
    if (u.X * v.Y - u.Y * v.X < 0) ang = -ang;
    return ang;
}

// Hàm xử lý toán học để thêm Arc vào Path
void AddArcToPath(GraphicsPath& path, PointF start, float rx, float ry, float rotation, bool largeArc, bool sweep, float endX, float endY)
{
    // 1. Xử lý dữ liệu đầu vào
    float x0 = start.X;
    float y0 = start.Y;

    rx = abs(rx);
    ry = abs(ry);

    if (rx == 0 || ry == 0) {
        path.AddLine(start, PointF(endX, endY));
        return;
    }

    double phi = rotation * M_PI / 180.0;
    double dx2 = (x0 - endX) / 2.0;
    double dy2 = (y0 - endY) / 2.0;
    double x1p = cos(phi) * dx2 + sin(phi) * dy2;
    double y1p = -sin(phi) * dx2 + cos(phi) * dy2;

    double rx_sq = rx * rx;
    double ry_sq = ry * ry;
    double x1p_sq = x1p * x1p;
    double y1p_sq = y1p * y1p;

    double check = x1p_sq / rx_sq + y1p_sq / ry_sq;
    if (check > 1) {
        double scale = sqrt(check);
        rx *= scale;
        ry *= scale;
        rx_sq = rx * rx;
        ry_sq = ry * ry;
    }

    double sign = (largeArc == sweep) ? -1 : 1;
    double num = max(0.0, rx_sq * ry_sq - rx_sq * y1p_sq - ry_sq * x1p_sq);
    double den = rx_sq * y1p_sq + ry_sq * x1p_sq;
    double coef = sign * sqrt(num / den);

    double cxp = coef * ((rx * y1p) / ry);
    double cyp = coef * (-(ry * x1p) / rx);

    double cx = cos(phi) * cxp - sin(phi) * cyp + (x0 + endX) / 2.0;
    double cy = sin(phi) * cxp + cos(phi) * cyp + (y0 + endY) / 2.0;

    PointF v1((x1p - cxp) / rx, (y1p - cyp) / ry);
    PointF v2((-x1p - cxp) / rx, (-y1p - cyp) / ry);

    double theta1 = angleBetween(PointF(1, 0), v1);
    double dtheta = angleBetween(v1, v2);

    if (!sweep && dtheta > 0) dtheta -= 2 * M_PI;
    else if (sweep && dtheta < 0) dtheta += 2 * M_PI;

    int segments = (int)ceil(abs(dtheta) / (M_PI / 2.0));
    double delta = dtheta / segments;
    double t = 8.0 / 3.0 * sin(delta / 4.0) * sin(delta / 4.0) / sin(delta / 2.0);

    double currentX = x0;
    double currentY = y0;

    for (int i = 0; i < segments; i++)
    {
        double cos1 = cos(theta1);
        double sin1 = sin(theta1);
        double theta2 = theta1 + delta;
        double cos2 = cos(theta2);
        double sin2 = sin(theta2);

        double e1x = cos1 - t * sin1;
        double e1y = sin1 + t * cos1;
        double e2x = cos2 + t * sin2;
        double e2y = sin2 - t * cos2;

        auto transform = [&](double u, double v) {
            double tx = rx * u;
            double ty = ry * v;
            return PointF(
                cx + cos(phi) * tx - sin(phi) * ty,
                cy + sin(phi) * tx + cos(phi) * ty
            );
            };

        PointF cp1 = transform(e1x, e1y);
        PointF cp2 = transform(e2x, e2y);
        PointF dest = transform(cos2, sin2);

        path.AddBezier(PointF(currentX, currentY), cp1, cp2, dest);

        theta1 = theta2;
        currentX = dest.X;
        currentY = dest.Y;
    }
}

SVGPath::SVGPath() { d = ""; }
SVGPath::SVGPath(const string& D) { d = D; }
SVGPath::SVGPath(const SVGPath& other) { this->d = other.getD(); }

string SVGPath::getD() const { return d; }
void SVGPath::setD(const string& newD) { d = newD; }

void SVGPath::parseAttributes(xml_node<>* Node)
{
    GeometricElement::parseAttributes(Node);
    if (xml_attribute<>* attribute = Node->first_attribute("d"))
    {
        this->d = attribute->value();
    }
    if (xml_attribute<>* attribute = Node->first_attribute("fill-rule"))
    {
        this->fillRule = attribute->value();
    }
}

void SVGPath::parsePathData(GraphicsPath& path)
{
    // 1. Xử lý chuỗi d (Thay vì dùng handleData sửa trực tiếp member, ta sửa trên bản sao local)
    string processedD = d;
    for (size_t i = 0; i < processedD.size(); i++) {
        if (processedD[i] == ',') processedD[i] = ' ';
    }
    stringstream ss(processedD);

    // 2. Set Fill Mode
    if (fillRule == "evenodd")
        path.SetFillMode(FillModeAlternate);
    else
        path.SetFillMode(FillModeWinding);

    // 3. Khởi tạo biến cục bộ
    char current = 0;
    float x = 0, y = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    PointF lastPoint(0, 0);// Diem ve cuoi cung

    PointF subPathStart(0, 0);

    // Diem dieu khien cuoi cung (dung cho duong cong muot S/s)
    PointF lastControlPoint(0, 0);

    // 4. Vòng lặp parse
    while (!ss.eof())
    {
        while (isspace(ss.peek())) ss.get();
        if (ss.eof()) break;

        char next = ss.peek();
        if (isalpha(next)) ss >> current;

        if (current == 'M') // MoveTo Absolute
        {
            if (ss >> x >> y) {
                path.StartFigure();
                lastPoint = PointF(x, y);
                lastControlPoint = lastPoint; 
                subPathStart = lastPoint;
                current = 'L'; 

            }
        }
        else if (current == 'm') // MoveTo Relative
        {
            if (ss >> x >> y) {
                path.StartFigure();
                lastPoint = PointF(lastPoint.X + x, lastPoint.Y + y);
                lastControlPoint = lastPoint;
                subPathStart = lastPoint;
                current = 'l';
            }
        }
        else if (current == 'L') // LineTo Absolute
        {
            if (ss >> x >> y) {
                path.AddLine(lastPoint, PointF(x, y));
                lastPoint = PointF(x, y);
                lastControlPoint = lastPoint;
            }
        }
        else if (current == 'l') // LineTo Relative
        {
            if (ss >> x >> y) {
                PointF nextP(lastPoint.X + x, lastPoint.Y + y);
                path.AddLine(lastPoint, nextP);
                lastPoint = nextP;
                lastControlPoint = lastPoint;
            }
        }
        else if (current == 'H') // Horizontal Absolute
        {
            if (ss >> x) {
                path.AddLine(lastPoint, PointF(x, lastPoint.Y));
                lastPoint = PointF(x, lastPoint.Y);
                lastControlPoint = lastPoint;
            }
        }
        else if (current == 'h') // Horizontal Relative
        {
            if (ss >> x) {
                PointF nextP(lastPoint.X + x, lastPoint.Y);
                path.AddLine(lastPoint, nextP);
                lastPoint = nextP;
                lastControlPoint = lastPoint;
            }
        }
        else if (current == 'V') // Vertical Absolute
        {
            if (ss >> y) {
                path.AddLine(lastPoint, PointF(lastPoint.X, y));
                lastPoint = PointF(lastPoint.X, y);
                lastControlPoint = lastPoint;
            }
        }
        else if (current == 'v') // Vertical Relative
        {
            if (ss >> y) {
                PointF nextP(lastPoint.X, lastPoint.Y + y);
                path.AddLine(lastPoint, nextP);
                lastPoint = nextP;
                lastControlPoint = lastPoint;
            }
        }
        else if (current == 'C') // Cubic Bezier Absolute
        {
            if (ss >> x1 >> y1 >> x2 >> y2 >> x >> y) {
                path.AddBezier(lastPoint, PointF(x1, y1), PointF(x2, y2), PointF(x, y));
                lastPoint = PointF(x, y);
                lastControlPoint = PointF(x2, y2);
            }
        }
        else if (current == 'c') // Cubic Bezier Relative
        {
            if (ss >> x1 >> y1 >> x2 >> y2 >> x >> y) {
                PointF p1(lastPoint.X + x1, lastPoint.Y + y1);
                PointF p2(lastPoint.X + x2, lastPoint.Y + y2);
                PointF pEnd(lastPoint.X + x, lastPoint.Y + y);
                path.AddBezier(lastPoint, p1, p2, pEnd);
                lastPoint = pEnd;
                lastControlPoint = p2;
            }
        }
        else if (current == 'S') // Smooth Cubic Absolute
        {
            if (ss >> x2 >> y2 >> x >> y) {
                float ctrl1X = 2 * lastPoint.X - lastControlPoint.X;
                float ctrl1Y = 2 * lastPoint.Y - lastControlPoint.Y;
                PointF p1(ctrl1X, ctrl1Y);
                PointF p2(x2, y2);
                PointF pEnd(x, y);
                path.AddBezier(lastPoint, p1, p2, pEnd);
                lastPoint = pEnd;
                lastControlPoint = p2;
            }
        }
        else if (current == 's') // Smooth Cubic Relative
        {
            if (ss >> x2 >> y2 >> x >> y) {
                float ctrl1X = 2 * lastPoint.X - lastControlPoint.X;
                float ctrl1Y = 2 * lastPoint.Y - lastControlPoint.Y;
                PointF p1(ctrl1X, ctrl1Y);
                PointF p2(lastPoint.X + x2, lastPoint.Y + y2);
                PointF pEnd(lastPoint.X + x, lastPoint.Y + y);
                path.AddBezier(lastPoint, p1, p2, pEnd);
                lastPoint = pEnd;
                lastControlPoint = p2;
            }
        }
        else if (current == 'Q') // Quadratic Absolute
        {
            if (ss >> x1 >> y1 >> x >> y) {
                PointF p0 = lastPoint;
                PointF p1(x1, y1);
                PointF p2(x, y);
                PointF c1(p0.X + (2.0f / 3.0f) * (p1.X - p0.X), p0.Y + (2.0f / 3.0f) * (p1.Y - p0.Y));
                PointF c2(p2.X + (2.0f / 3.0f) * (p1.X - p2.X), p2.Y + (2.0f / 3.0f) * (p1.Y - p2.Y));
                path.AddBezier(p0, c1, c2, p2);
                lastPoint = p2;
                lastControlPoint = p1;
            }
        }
        else if (current == 'q') // Quadratic Relative
        {
            if (ss >> x1 >> y1 >> x >> y) {
                PointF p0 = lastPoint;
                PointF p1(p0.X + x1, p0.Y + y1);
                PointF p2(p0.X + x, p0.Y + y);
                PointF c1(p0.X + (2.0f / 3.0f) * (p1.X - p0.X), p0.Y + (2.0f / 3.0f) * (p1.Y - p0.Y));
                PointF c2(p2.X + (2.0f / 3.0f) * (p1.X - p2.X), p2.Y + (2.0f / 3.0f) * (p1.Y - p2.Y));
                path.AddBezier(p0, c1, c2, p2);
                lastPoint = p2;
                lastControlPoint = p1;
            }
        }
        else if (current == 'T') // Smooth Quadratic Absolute
        {
            if (ss >> x >> y) {
                float ctrlX = 2 * lastPoint.X - lastControlPoint.X;
                float ctrlY = 2 * lastPoint.Y - lastControlPoint.Y;
                PointF p0 = lastPoint;
                PointF p1(ctrlX, ctrlY);
                PointF p2(x, y);
                PointF c1(p0.X + (2.0f / 3.0f) * (p1.X - p0.X), p0.Y + (2.0f / 3.0f) * (p1.Y - p0.Y));
                PointF c2(p2.X + (2.0f / 3.0f) * (p1.X - p2.X), p2.Y + (2.0f / 3.0f) * (p1.Y - p2.Y));
                path.AddBezier(p0, c1, c2, p2);
                lastPoint = p2;
                lastControlPoint = p1;
            }
        }
        else if (current == 't') // Smooth Quadratic Relative
        {
            if (ss >> x >> y) {
                float ctrlX = 2 * lastPoint.X - lastControlPoint.X;
                float ctrlY = 2 * lastPoint.Y - lastControlPoint.Y;
                PointF p0 = lastPoint;
                PointF p1(ctrlX, ctrlY);
                PointF p2(p0.X + x, p0.Y + y);
                PointF c1(p0.X + (2.0f / 3.0f) * (p1.X - p0.X), p0.Y + (2.0f / 3.0f) * (p1.Y - p0.Y));
                PointF c2(p2.X + (2.0f / 3.0f) * (p1.X - p2.X), p2.Y + (2.0f / 3.0f) * (p1.Y - p2.Y));
                path.AddBezier(p0, c1, c2, p2);
                lastPoint = p2;
                lastControlPoint = p1;
            }
        }
        else if (current == 'A') // Arc Absolute
        {
            float rx, ry, rotation;
            ss >> rx >> ry >> rotation;
            while (isspace(ss.peek())) ss.get();
            char fA = ss.get();
            while (isspace(ss.peek())) ss.get();
            char fS = ss.get();
            bool bFlagA = (fA == '1');
            bool bFlagS = (fS == '1');
            if (ss >> x >> y) {
                AddArcToPath(path, lastPoint, rx, ry, rotation, bFlagA, bFlagS, x, y);
                lastPoint = PointF(x, y);
                lastControlPoint = lastPoint;
            }
        }
        else if (current == 'a') // Arc Relative
        {
            float rx, ry, rotation;
            ss >> rx >> ry >> rotation;
            while (isspace(ss.peek())) ss.get();
            char fA = ss.get();
            while (isspace(ss.peek())) ss.get();
            char fS = ss.get();
            bool bFlagA = (fA == '1');
            bool bFlagS = (fS == '1');
            if (ss >> x >> y) {
                float endX = lastPoint.X + x;
                float endY = lastPoint.Y + y;
                AddArcToPath(path, lastPoint, rx, ry, rotation, bFlagA, bFlagS, endX, endY);
                lastPoint = PointF(endX, endY);
                lastControlPoint = lastPoint;
            }
        }
        else if (current == 'Z' || current == 'z') // Close
        {
            path.CloseFigure();
            lastControlPoint = lastPoint;
            lastPoint = subPathStart;
        }
        else
        {
            ss.get();
        }
    }
}

Gdiplus::RectF SVGPath::getBoundingBox()
{
    GraphicsPath path;
    parsePathData(path); // Tái sử dụng logic parse
    RectF bounds;
    path.GetBounds(&bounds);
    return this->TransformRect(bounds);
}

// Dùng createBrush cho Gradient
void SVGPath::draw(Graphics* graphics)
{
    GraphicsPath path;
    parsePathData(path); // Tạo hình

    // 1. Tính toán Bounding Box để lấy Gradient
    RectF bounds;
    path.GetBounds(&bounds);

    // 2. Tạo Brush thông minh (Solid hoặc Gradient từ class cha)
    Brush* brush = this->createBrush(bounds);

    // 3. Tạo Pen
    SVGColor sColor = getStroke().getColor();
    Color strokeColor = { sColor.getA(), sColor.getR(), sColor.getG(), sColor.getB() };
    Pen* pen = new Pen(strokeColor, getStroke().getWidth());
    pen->SetLineCap(getStroke().getLineCap(), getStroke().getLineCap(), DashCapRound);
    pen->SetLineJoin(getStroke().getLineJoin());

    // 4. Vẽ
    if (brush) graphics->FillPath(brush, &path); // Chỉ fill nếu có brush hợp lệ
    if (getStroke().getWidth() > 0 && sColor.getA() > 0) graphics->DrawPath(pen, &path);

    if (brush) delete brush;
    delete pen;
}

SVGPath::~SVGPath()
{
}
