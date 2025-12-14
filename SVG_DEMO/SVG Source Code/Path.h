#ifndef _PATH_H_
#define _PATH_H_

#include "GeometricElement.h"

class SVGPath : public GeometricElement {
private:
	string d;
	string fillRule;

	// Hàm helper nội bộ: Phân tích chuỗi d và đưa vào GraphicsPath
	void parsePathData(GraphicsPath& path);

public:
	SVGPath();
	SVGPath(const string&);
	SVGPath(const SVGPath&);

	string getD() const;
	void  setD(const string&);

	void parseAttributes(xml_node<>*);

	// Implement hàm bắt buộc để tính Gradient
	Gdiplus::RectF getBoundingBox() override;

	// Cập nhật hàm vẽ
	void draw(Graphics* graphics) override;

	~SVGPath();
};

#endif // !_PATH_H_