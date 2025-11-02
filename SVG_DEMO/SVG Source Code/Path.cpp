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

void SVGPath::draw()
{
}

SVGPath::~SVGPath()
{
}
