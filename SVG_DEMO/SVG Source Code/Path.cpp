#include "stdafx.h"
#include "Path.h"
#include <cmath>
#include <algorithm>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


//////// Phan code ho tro doc lenh A///////// phan xu ly so hoc kham thao AI
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

    // Bán kính không được âm
    rx = abs(rx);
    ry = abs(ry);

    // Nếu bán kính = 0, coi như vẽ đường thẳng (theo chuẩn SVG)
    if (rx == 0 || ry == 0) {
        path.AddLine(start, PointF(endX, endY));
        return;
    }

    // Chuyển độ sang radian
    double phi = rotation * M_PI / 180.0;

    // 2. Chuyển đổi từ Endpoint sang Center Parameterization (Công thức F.6.5 của W3C)
    double dx2 = (x0 - endX) / 2.0;
    double dy2 = (y0 - endY) / 2.0;

    double x1p = cos(phi) * dx2 + sin(phi) * dy2;
    double y1p = -sin(phi) * dx2 + cos(phi) * dy2;

    // Check tỉ lệ bán kính (Công thức F.6.6)
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

    // Tính tâm (cx', cy') (Công thức F.6.5.2)
    double sign = (largeArc == sweep) ? -1 : 1;
    double num = max(0.0, rx_sq * ry_sq - rx_sq * y1p_sq - ry_sq * x1p_sq);
    double den = rx_sq * y1p_sq + ry_sq * x1p_sq;
    double coef = sign * sqrt(num / den);

    double cxp = coef * ((rx * y1p) / ry);
    double cyp = coef * (-(ry * x1p) / rx);

    // Chuyển về toạ độ gốc (cx, cy) (Công thức F.6.5.3)
    double cx = cos(phi) * cxp - sin(phi) * cyp + (x0 + endX) / 2.0;
    double cy = sin(phi) * cxp + cos(phi) * cyp + (y0 + endY) / 2.0;

    // 3. Tính góc bắt đầu và góc quét (Công thức F.6.5.4 -> F.6.5.6)
    PointF v1((x1p - cxp) / rx, (y1p - cyp) / ry);
    PointF v2((-x1p - cxp) / rx, (-y1p - cyp) / ry);

    double theta1 = angleBetween(PointF(1, 0), v1);
    double dtheta = angleBetween(v1, v2);

    if (!sweep && dtheta > 0) dtheta -= 2 * M_PI;
    else if (sweep && dtheta < 0) dtheta += 2 * M_PI;

    // 4. Vẽ xấp xỉ bằng Bezier (Arc to Bezier)
    // Chia nhỏ cung tròn thành các đoạn nhỏ (mỗi đoạn không quá 90 độ để đảm bảo chính xác)
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

        // Tính điểm điều khiển (trên đường tròn đơn vị chưa xoay)
        double e1x = cos1 - t * sin1;
        double e1y = sin1 + t * cos1;
        double e2x = cos2 + t * sin2;
        double e2y = sin2 - t * cos2;

        // Xoay và tịnh tiến về vị trí thực
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

        // Add vào Path
        path.AddBezier(PointF(currentX, currentY), cp1, cp2, dest);

        // Cập nhật cho vòng lặp sau
        theta1 = theta2;
        currentX = dest.X;
        currentY = dest.Y;
    }
}

/////////////////// het code phuc vu lenh A ////////////////


SVGPath::SVGPath()
{
	d = "";
}

SVGPath::SVGPath(const string& D)
{
	d = D;
}

SVGPath::SVGPath(const SVGPath& other)
{
	this->d = other.getD();
}

string SVGPath::getD() const
{
	return d;
}

void SVGPath::setD(const string& newD)
{
	d = newD;
}

void SVGPath::parseAttributes(xml_node<>* Node)
{
	GeometricElement::parseAttributes(Node);
	if (xml_attribute<>* attribute = Node->first_attribute("d"))
	{
		this->d = attribute->value();
	}
}

void SVGPath::handleData()
{
	for (int i = 0; i < d.size(); i++)
	{
		if (d[i] == ',') d[i] = ' ';
	}
}

void SVGPath::draw(Graphics* graphics)
{
    GraphicsPath path;
    handleData(); // xu li ',' thanh ' '
    stringstream ss(d);

    char current = 0;
    float x = 0, y = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    PointF lastPoint(0, 0);// Diem ve cuoi cung

    // Diem dieu khien cuoi cung (dung cho duong cong muot S/s)
    PointF lastControlPoint(0, 0);

    while (!ss.eof())
    {
        while (isspace(ss.peek())) ss.get();

        if (ss.eof()) break;

        char next = ss.peek();
        if (isalpha(next))
        {
            ss >> current;
        }

        if (current == 'M') // Di chuyen tuyet doi
        {
            if (ss >> x >> y)
            {
                path.StartFigure();
                lastPoint = PointF(x, y);
                lastControlPoint = lastPoint; 
                current = 'L'; 
            }
        }
        else if (current == 'm')// Di chuyen tuong doi
        {
            if (ss >> x >> y)
            {
                path.StartFigure();
                lastPoint = PointF(lastPoint.X + x, lastPoint.Y + y);
                lastControlPoint = lastPoint;
                current = 'l';
            }
        }
        // Ve duong thang tuyet doi
        else if (current == 'L') 
        {
            if (ss >> x >> y)
            {
                path.AddLine(lastPoint, PointF(x, y));
                lastPoint = PointF(x, y);
                lastControlPoint = lastPoint;
            }
        }
        else if (current == 'l') // Ve duong thang tuong doi
        {
            if (ss >> x >> y)
            {
                PointF nextP(lastPoint.X + x, lastPoint.Y + y);
                path.AddLine(lastPoint, nextP);
                lastPoint = nextP;
                lastControlPoint = lastPoint;
            }
        }
        else if (current == 'H') // Ve duong ngang tuyet doi
        {
            if (ss >> x)
            {
                path.AddLine(lastPoint, PointF(x, lastPoint.Y));
                lastPoint = PointF(x, lastPoint.Y);
                lastControlPoint = lastPoint;
            }
        }
        else if (current == 'h')  // Ve duong ngang tuong doi
        {
            if (ss >> x)
            {
                PointF nextP(lastPoint.X + x, lastPoint.Y);
                path.AddLine(lastPoint, nextP);
                lastPoint = nextP;
                lastControlPoint = lastPoint;
            }
        }
        else if (current == 'V') // Ve duong doc tuyet doi
        {
            if (ss >> y)
            {
                path.AddLine(lastPoint, PointF(lastPoint.X, y));
                lastPoint = PointF(lastPoint.X, y);
                lastControlPoint = lastPoint;
            }
        }
        else if (current == 'v') // Ve duong doc tuong doi
        {
            if (ss >> y)
            {
                PointF nextP(lastPoint.X, lastPoint.Y + y);
                path.AddLine(lastPoint, nextP);
                lastPoint = nextP;
                lastControlPoint = lastPoint;
            }
        }

        else if (current == 'C')// Ve duong cong Bezier tuyet doi   
        {
            if (ss >> x1 >> y1 >> x2 >> y2 >> x >> y)
            {
                path.AddBezier(lastPoint, PointF(x1, y1), PointF(x2, y2), PointF(x, y));
                lastPoint = PointF(x, y);
                lastControlPoint = PointF(x2, y2);
            }
        }
        else if (current == 'c') // Ve duong cong Bezier tuong doi
        {
            if (ss >> x1 >> y1 >> x2 >> y2 >> x >> y)
            {
                PointF p1(lastPoint.X + x1, lastPoint.Y + y1);
                PointF p2(lastPoint.X + x2, lastPoint.Y + y2);
                PointF pEnd(lastPoint.X + x, lastPoint.Y + y);

                path.AddBezier(lastPoint, p1, p2, pEnd);
                lastPoint = pEnd;
                lastControlPoint = p2;
            }
        }
        else if (current == 'S') // Ve duong cong muot tuyet doi (Absolute Smooth Cubic Bezier)
        {
            if (ss >> x2 >> y2 >> x >> y)
            {
                // Tinh diem dieu khien thu nhat: doi xung voi diem dieu khien cu qua lastPoint
                // Cong thuc: P1 = 2 * Current - LastControl
                float ctrl1X = 2 * lastPoint.X - lastControlPoint.X;
                float ctrl1Y = 2 * lastPoint.Y - lastControlPoint.Y;

                PointF p1(ctrl1X, ctrl1Y);
                PointF p2(x2, y2);     // Toa do tuyet doi, khong can cong them lastPoint
                PointF pEnd(x, y);     // Toa do tuyet doi, khong can cong them lastPoint

                path.AddBezier(lastPoint, p1, p2, pEnd);

                // Cap nhat vi tri
                lastPoint = pEnd;
                lastControlPoint = p2;
            }
        }
        else if (current == 's') // Ve duong cong muot tuong doi
        {
            if (ss >> x2 >> y2 >> x >> y)
            {
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
        else if (current == 'Q') // ve quadratuc bezier tuyet doi
        {
            if (ss >> x1 >> y1 >> x >> y)
            {
            
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
           
        else if (current == 'q') // ve quadratuc bezier tuong doi
        {
            if (ss >> x1 >> y1 >> x >> y)
            {
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

        else if (current == 'T') // Smooth Quadratic Tuyet doi (noi tiep Q)
        {
            if (ss >> x >> y)
            {
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
        else if (current == 't')  //Smooth Quadratic tuong doi
        {
            if (ss >> x >> y)
            {
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
        else if (current == 'A')
        {
            float rx, ry, rotation, flagA, flagS;
            if (ss >> rx >> ry >> rotation >> flagA >> flagS >> x >> y)
            {
                // Gọi hàm toán học đã viết ở trên
                AddArcToPath(path, lastPoint, rx, ry, rotation, (bool)flagA, (bool)flagS, x, y);

                lastPoint = PointF(x, y);
                lastControlPoint = lastPoint; // Reset control point
            }
        }
            // --- BỔ SUNG LỆNH a: Arc Tương đối ---
        else if (current == 'a')
        {
            float rx, ry, rotation, flagA, flagS;
            if (ss >> rx >> ry >> rotation >> flagA >> flagS >> x >> y)
            {
                // Tính toạ độ đích thực tế
                float endX = lastPoint.X + x;
                float endY = lastPoint.Y + y;

                AddArcToPath(path, lastPoint, rx, ry, rotation, (bool)flagA, (bool)flagS, endX, endY);

                lastPoint = PointF(endX, endY);
                lastControlPoint = lastPoint;
            }
        }
        else if (current == 'Z' || current == 'z')  // Dong hinh
        {
            path.CloseFigure();
            lastControlPoint = lastPoint;
        }
        else
        {
            ss.get(); // Bo qua ky tu rac
        }
    }



	Color fillColor = { getFill().getA(),getFill().getR(),getFill().getG(),getFill().getB() };
	Brush* brush = new SolidBrush(fillColor);
	Color fillColorWidth = { getStroke().getColor().getA(),getStroke().getColor().getR() ,getStroke().getColor().getG() ,getStroke().getColor().getB() };
	Pen* pen = new Pen(fillColorWidth, getStroke().getWidth());



	graphics->FillPath(brush, &path);
	graphics->DrawPath(pen, &path);
	delete brush;
	delete pen;
}


SVGPath::~SVGPath()
{
}
