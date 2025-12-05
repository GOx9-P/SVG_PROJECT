#include "stdafx.h"
#include "ViewPort.h"

ViewPort::ViewPort()
{
	Reset();
}

void ViewPort::Reset() {
    zoom = 1.0f;
    panX = 0.0f;
    panY = 0.0f;
    angle = 0.0f;
    isDragging = false;
    lastMouseX = 0;
    lastMouseY = 0;
}

void ViewPort::ApplyTransform(Graphics* graphics)
{
    if (!graphics) return;

    //Thu tu: Dich chuyen --> Xoay --> Phong to
    graphics->TranslateTransform(panX, panY);
    graphics->RotateTransform(angle);
    graphics->ScaleTransform(zoom, zoom);
}

//Xu li khi lan chuot de phong to
bool ViewPort::HandleMouseWheel(short zDelta)
{
    if (zDelta > 0) {
        zoom *= 1.1f;
    }
    else {
        zoom /= 1.1f;
    }
    return true; //Can ve lai
}

//Xu li nhan chuot (Bat dau keo)
void ViewPort::HandleMouseDown(int x, int y)
{
    isDragging = true;
    lastMouseX = x;
    lastMouseY = y;
}

//Xu li di chuyen chuot (Dang keo)
bool ViewPort::HandleMouseMove(int x, int y)
{
    if (isDragging) {
        int dx = x - lastMouseX;
        int dy = y - lastMouseY;
        panX += (float)dx;
        panY += (float)dy;
        lastMouseX = x;
        lastMouseY = y;
        return true; //Vi tri thay doi, can ve lai
    }
    return false;
}

//Xu li nha chuot (Ket thuc keo)
void ViewPort::HandleMouseUp()
{
    isDragging = false;
}

bool ViewPort::HandleKeyDown(WPARAM key)
{
    bool needRepaint = false;
    switch (key) {
    case 'R':
        angle += 10.0f;
        needRepaint = true; //ve lai
        break;
    case 'L':
        angle -= 10.0f;
        needRepaint = true;
        break;
    case '0': //Reset
        Reset();
        needRepaint = true;
        break;
    }
    return needRepaint;
}


