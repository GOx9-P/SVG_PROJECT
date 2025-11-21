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
	handleData();
	stringstream ss(d);
	char current=0;
	float x, y, x1, y1, x2, y2;
	PointF lastPoint(0, 0);
	while (!ss.eof())
	{
		while (isspace(ss.peek())) ss.get();
		char next = ss.peek();

		if (isalpha(next))
		{
			ss >> current;
		}

		if (current == 'M')
		{
			if (ss >> x>> y)
			{
				path.StartFigure();
				lastPoint = PointF(x, y);
				current = 'L';
			}
		}
		else if (current == 'L')
		{
			if (ss >> x >> y)
			{
				path.AddLine(lastPoint, PointF(x, y));
				lastPoint = PointF(x, y);
			}
		}
		else if (current == 'C')
		{
			if (ss >> x >> y >> x1 >> y1 >> x2 >> y2)
			{
				path.AddBezier(lastPoint, PointF(x, y), PointF(x1, y1), PointF(x2, y2));
				lastPoint = PointF(x2, y2);
			}
		}
		else if (current == 'Z' || current == 'z')
		{
			path.CloseFigure();
		}
		else
		{
			ss.get();
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
