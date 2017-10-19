#include "ScreenCanvas.h"
#include <cstdarg>

using namespace AKBot;
using namespace BWAPI;

//------------------------------------------ DRAW TEXT ----------------------------------------------
void ScreenCanvas::drawText(BWAPI::CoordinateType::Enum ctype, int x, int y, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	this->vDrawText(ctype, x, y, format, ap);
	va_end(ap);
}
void ScreenCanvas::drawTextMap(int x, int y, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	this->vDrawText(BWAPI::CoordinateType::Map, x, y, format, ap);
	va_end(ap);
}
void ScreenCanvas::drawTextMouse(int x, int y, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	this->vDrawText(BWAPI::CoordinateType::Mouse, x, y, format, ap);
	va_end(ap);
}
void ScreenCanvas::drawTextScreen(int x, int y, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	this->vDrawText(BWAPI::CoordinateType::Screen, x, y, format, ap);
	va_end(ap);
}
void ScreenCanvas::drawTextMap(BWAPI::Position p, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	this->vDrawText(BWAPI::CoordinateType::Map, p.x, p.y, format, ap);
	va_end(ap);
}
void ScreenCanvas::drawTextMouse(BWAPI::Position p, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	this->vDrawText(BWAPI::CoordinateType::Mouse, p.x, p.y, format, ap);
	va_end(ap);
}
void ScreenCanvas::drawTextScreen(BWAPI::Position p, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	this->vDrawText(BWAPI::CoordinateType::Screen, p.x, p.y, format, ap);
	va_end(ap);
}
//------------------------------------------ DRAW BOX -----------------------------------------------
void ScreenCanvas::drawBoxMap(int left, int top, int right, int bottom, Color color, bool isSolid)
{
	this->drawBox(CoordinateType::Map, left, top, right, bottom, color, isSolid);
}
void ScreenCanvas::drawBoxMouse(int left, int top, int right, int bottom, Color color, bool isSolid)
{
	this->drawBox(CoordinateType::Mouse, left, top, right, bottom, color, isSolid);
}
void ScreenCanvas::drawBoxScreen(int left, int top, int right, int bottom, Color color, bool isSolid)
{
	this->drawBox(CoordinateType::Screen, left, top, right, bottom, color, isSolid);
}
void ScreenCanvas::drawBoxMap(Position leftTop, Position rightBottom, Color color, bool isSolid)
{
	this->drawBoxMap(leftTop.x, leftTop.y, rightBottom.x, rightBottom.y, color, isSolid);
}
void ScreenCanvas::drawBoxMouse(Position leftTop, Position rightBottom, Color color, bool isSolid)
{
	this->drawBoxMouse(leftTop.x, leftTop.y, rightBottom.x, rightBottom.y, color, isSolid);
}
void ScreenCanvas::drawBoxScreen(Position leftTop, Position rightBottom, Color color, bool isSolid)
{
	this->drawBoxScreen(leftTop.x, leftTop.y, rightBottom.x, rightBottom.y, color, isSolid);
}
//------------------------------------------ DRAW TRIANGLE -----------------------------------------------
void ScreenCanvas::drawTriangleMap(int ax, int ay, int bx, int by, int cx, int cy, Color color, bool isSolid)
{
	this->drawTriangle(CoordinateType::Map, ax, ay, bx, by, cx, cy, color, isSolid);
}
void ScreenCanvas::drawTriangleMouse(int ax, int ay, int bx, int by, int cx, int cy, Color color, bool isSolid)
{
	this->drawTriangle(CoordinateType::Mouse, ax, ay, bx, by, cx, cy, color, isSolid);
}
void ScreenCanvas::drawTriangleScreen(int ax, int ay, int bx, int by, int cx, int cy, Color color, bool isSolid)
{
	this->drawTriangle(CoordinateType::Screen, ax, ay, bx, by, cx, cy, color, isSolid);
}
void ScreenCanvas::drawTriangleMap(Position a, Position b, Position c, Color color, bool isSolid)
{
	this->drawTriangleMap(a.x, a.y, b.x, b.y, c.x, c.y, color, isSolid);
}
void ScreenCanvas::drawTriangleMouse(Position a, Position b, Position c, Color color, bool isSolid)
{
	this->drawTriangleMouse(a.x, a.y, b.x, b.y, c.x, c.y, color, isSolid);
}
void ScreenCanvas::drawTriangleScreen(Position a, Position b, Position c, Color color, bool isSolid)
{
	this->drawTriangleScreen(a.x, a.y, b.x, b.y, c.x, c.y, color, isSolid);
}
//------------------------------------------ DRAW CIRCLE -----------------------------------------------
void ScreenCanvas::drawCircleMap(int x, int y, int radius, Color color, bool isSolid)
{
	this->drawCircle(CoordinateType::Map, x, y, radius, color, isSolid);
}
void ScreenCanvas::drawCircleMouse(int x, int y, int radius, Color color, bool isSolid)
{
	this->drawCircle(CoordinateType::Mouse, x, y, radius, color, isSolid);
}
void ScreenCanvas::drawCircleScreen(int x, int y, int radius, Color color, bool isSolid)
{
	this->drawCircle(CoordinateType::Screen, x, y, radius, color, isSolid);
}
void ScreenCanvas::drawCircleMap(Position p, int radius, Color color, bool isSolid)
{
	this->drawCircleMap(p.x, p.y, radius, color, isSolid);
}
void ScreenCanvas::drawCircleMouse(Position p, int radius, Color color, bool isSolid)
{
	this->drawCircleMouse(p.x, p.y, radius, color, isSolid);
}
void ScreenCanvas::drawCircleScreen(Position p, int radius, Color color, bool isSolid)
{
	this->drawCircleScreen(p.x, p.y, radius, color, isSolid);
}
//------------------------------------------ DRAW ELLIPSE -----------------------------------------------
void ScreenCanvas::drawEllipseMap(int x, int y, int xrad, int yrad, Color color, bool isSolid)
{
	this->drawEllipse(CoordinateType::Map, x, y, xrad, yrad, color, isSolid);
}
void ScreenCanvas::drawEllipseMouse(int x, int y, int xrad, int yrad, Color color, bool isSolid)
{
	this->drawEllipse(CoordinateType::Mouse, x, y, xrad, yrad, color, isSolid);
}
void ScreenCanvas::drawEllipseScreen(int x, int y, int xrad, int yrad, Color color, bool isSolid)
{
	this->drawEllipse(CoordinateType::Screen, x, y, xrad, yrad, color, isSolid);
}
void ScreenCanvas::drawEllipseMap(Position p, int xrad, int yrad, Color color, bool isSolid)
{
	this->drawEllipseMap(p.x, p.y, xrad, yrad, color, isSolid);
}
void ScreenCanvas::drawEllipseMouse(Position p, int xrad, int yrad, Color color, bool isSolid)
{
	this->drawEllipseMouse(p.x, p.y, xrad, yrad, color, isSolid);
}
void ScreenCanvas::drawEllipseScreen(Position p, int xrad, int yrad, Color color, bool isSolid)
{
	this->drawEllipseScreen(p.x, p.y, xrad, yrad, color, isSolid);
}
//------------------------------------------ DRAW DOT -----------------------------------------------
void ScreenCanvas::drawDotMap(int x, int y, Color color)
{
	this->drawDot(CoordinateType::Map, x, y, color);
}
void ScreenCanvas::drawDotMouse(int x, int y, Color color)
{
	this->drawDot(CoordinateType::Mouse, x, y, color);
}
void ScreenCanvas::drawDotScreen(int x, int y, Color color)
{
	this->drawDot(CoordinateType::Screen, x, y, color);
}
void ScreenCanvas::drawDotMap(Position p, Color color)
{
	this->drawDotMap(p.x, p.y, color);
}
void ScreenCanvas::drawDotMouse(Position p, Color color)
{
	this->drawDotMouse(p.x, p.y, color);
}
void ScreenCanvas::drawDotScreen(Position p, Color color)
{
	this->drawDotScreen(p.x, p.y, color);
}
//------------------------------------------ DRAW LINE -----------------------------------------------
void ScreenCanvas::drawLineMap(int x1, int y1, int x2, int y2, Color color)
{
	this->drawLine(CoordinateType::Map, x1, y1, x2, y2, color);
}
void ScreenCanvas::drawLineMouse(int x1, int y1, int x2, int y2, Color color)
{
	this->drawLine(CoordinateType::Mouse, x1, y1, x2, y2, color);
}
void ScreenCanvas::drawLineScreen(int x1, int y1, int x2, int y2, Color color)
{
	this->drawLine(CoordinateType::Screen, x1, y1, x2, y2, color);
}
void ScreenCanvas::drawLineMap(Position a, Position b, Color color)
{
	this->drawLineMap(a.x, a.y, b.x, b.y, color);
}
void ScreenCanvas::drawLineMouse(Position a, Position b, Color color)
{
	this->drawLineMouse(a.x, a.y, b.x, b.y, color);
}
void ScreenCanvas::drawLineScreen(Position a, Position b, Color color)
{
	this->drawLineScreen(a.x, a.y, b.x, b.y, color);
}