#include "stdafx.h"
#include "Path.h"

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
