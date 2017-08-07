#pragma once
#include "ScreenCanvas.h"

namespace AKBot
{
	class BWAPIScreenCanvas :
		public ScreenCanvas
	{
	public:
		BWAPIScreenCanvas();
		~BWAPIScreenCanvas();
		void setTextSize(BWAPI::Text::Size::Enum size = BWAPI::Text::Size::Default) override;
		void vDrawText(BWAPI::CoordinateType::Enum ctype, int x, int y, const char *format, va_list arg) override;
		void drawBox(BWAPI::CoordinateType::Enum ctype, int left, int top, int right, int bottom, BWAPI::Color color, bool isSolid = false) override;
		void drawTriangle(BWAPI::CoordinateType::Enum ctype, int ax, int ay, int bx, int by, int cx, int cy, BWAPI::Color color, bool isSolid = false) override;
		void drawCircle(BWAPI::CoordinateType::Enum ctype, int x, int y, int radius, BWAPI::Color color, bool isSolid = false) override;
		void drawEllipse(BWAPI::CoordinateType::Enum ctype, int x, int y, int xrad, int yrad, BWAPI::Color color, bool isSolid = false) override;
		void drawDot(BWAPI::CoordinateType::Enum ctype, int x, int y, BWAPI::Color color) override;
		void drawLine(BWAPI::CoordinateType::Enum ctype, int x1, int y1, int x2, int y2, BWAPI::Color color) override;
	};
}