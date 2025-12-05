#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

#include<windows.h>
#include<gdiplus.h>

class ViewPort {
private:
	//Trang thai
	float zoom;
	float panX;
	float panY;
	float angle;

	//Keo tha chuot
	bool isDragging;
	int lastMouseX;
	int lastMouseY;
public:
	ViewPort();
	void Reset();
	//Ham ap dung bien doi len Graphics
	void ApplyTransform(Graphics* graphic);
	bool HandleMouseWheel(short zDelta);
	void HandleMouseDown(int x, int y);
	bool HandleMouseMove(int x, int y);
	void HandleMouseUp();
	bool HandleKeyDown(WPARAM key);
};

#endif // !_VIEWPORT_H_

