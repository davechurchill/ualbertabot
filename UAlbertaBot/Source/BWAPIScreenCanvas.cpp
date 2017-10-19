#include "BWAPIScreenCanvas.h"
#include <BWAPI\Game.h>

using namespace AKBot;

BWAPIScreenCanvas::BWAPIScreenCanvas()
{
}


BWAPIScreenCanvas::~BWAPIScreenCanvas()
{
}

void BWAPIScreenCanvas::setTextSize(BWAPI::Text::Size::Enum size)
{
	BWAPI::Broodwar->setTextSize(size);
}

void AKBot::BWAPIScreenCanvas::vDrawText(BWAPI::CoordinateType::Enum ctype, int x, int y, const char * format, va_list arg)
{
	BWAPI::Broodwar->vDrawText(ctype, x, y, format, arg);
}

void AKBot::BWAPIScreenCanvas::drawBox(BWAPI::CoordinateType::Enum ctype, int left, int top, int right, int bottom, BWAPI::Color color, bool isSolid)
{
	BWAPI::Broodwar->drawBox(ctype, left, top, right, bottom, color, isSolid);
}

void AKBot::BWAPIScreenCanvas::drawTriangle(BWAPI::CoordinateType::Enum ctype, int ax, int ay, int bx, int by, int cx, int cy, BWAPI::Color color, bool isSolid)
{
	BWAPI::Broodwar->drawTriangle(ctype, ax, ay, bx, by, cx, cy, color, isSolid);
}

void AKBot::BWAPIScreenCanvas::drawCircle(BWAPI::CoordinateType::Enum ctype, int x, int y, int radius, BWAPI::Color color, bool isSolid)
{
	BWAPI::Broodwar->drawCircle(ctype, x, y, radius, color, isSolid);
}

void AKBot::BWAPIScreenCanvas::drawEllipse(BWAPI::CoordinateType::Enum ctype, int x, int y, int xrad, int yrad, BWAPI::Color color, bool isSolid)
{
	BWAPI::Broodwar->drawEllipse(ctype, x, y, xrad, yrad, color, isSolid);
}

void AKBot::BWAPIScreenCanvas::drawDot(BWAPI::CoordinateType::Enum ctype, int x, int y, BWAPI::Color color)
{
	BWAPI::Broodwar->drawDot(ctype, x, y, color);
}

void AKBot::BWAPIScreenCanvas::drawLine(BWAPI::CoordinateType::Enum ctype, int x1, int y1, int x2, int y2, BWAPI::Color color)
{
	BWAPI::Broodwar->drawLine(ctype, x1, y1, x2, y2, color);
}
