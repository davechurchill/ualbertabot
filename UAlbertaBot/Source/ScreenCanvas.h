#pragma once

#include<BWAPI\Color.h>
#include<BWAPI\CoordinateType.h>
#include<BWAPI\Position.h>

namespace AKBot
{
	/*
	 Abstract class which allow drawing on the game screen.
	*/
	class ScreenCanvas
	{
	public:
		/// @name Debugging Members
		/// @{

		/// <summary>Sets the size of the text for all calls to drawText following this one.</summary>
		///
		/// <param name="size"> (optional)
		///   The size of the text. This value is one of Text::Size::Enum. If this value is omitted,
		///   then a default value of Text::Size::Default is used.
		/// </param>
		///
		/// Example usage
		/// @code
		///   void ExampleAIModule::onFrame()
		///   {
		///     // Centers the name of the player in the upper middle of the screen
		///     BWAPI::Broodwar->setTextSize(BWAPI::Text::Size::Large);
		///     BWAPI::Broodwar->drawTextScreen(BWAPI::Positions::Origin, "%c%c%s",
		///                                     BWAPI::Text::Align_Center,
		///                                     BWAPI::Text::Green,
		///                                     BWAPI::Broodwar->self()->getName().c_str() );
		///     BWAPI::Broodwar->setTextSize();   // Set text size back to default
		///   }
		/// @endcode
		/// @see Text::Size::Enum
		virtual void setTextSize(BWAPI::Text::Size::Enum size = BWAPI::Text::Size::Default) = 0;

		/// <summary>Draws text on the screen at the given coordinates.</summary> Text can be drawn in
		/// different colors or formatted using the Text::Enum members.
		///
		/// <param name="ctype">
		///   The coordinate type. Indicates the relative position to draw the shape.
		/// </param>
		/// <param name="x">
		///   The x coordinate, in pixels, relative to ctype.
		/// </param>
		/// <param name="y">
		///   The y coordinate, in pixels, relative to ctype.
		/// </param>
		/// <param name="format">
		///   The string formatting portion. This is the same as printf style formatting.
		/// </param>
		/// <param name="arg">
		///   Arglist containing the intermediate list of arguments to format before finally sending
		///   the string to Broodwar.
		/// </param>
		virtual void vDrawText(BWAPI::CoordinateType::Enum ctype, int x, int y, const char *format, va_list arg) = 0;
		/// @overload
		void drawText(BWAPI::CoordinateType::Enum ctype, int x, int y, const char *format, ...);
		/// @overload
		void drawTextMap(int x, int y, const char *format, ...);
		/// @overload
		void drawTextMap(BWAPI::Position p, const char *format, ...);
		/// @overload
		void drawTextMouse(int x, int y, const char *format, ...);
		/// @overload
		void drawTextMouse(BWAPI::Position p, const char *format, ...);
		/// @overload
		void drawTextScreen(int x, int y, const char *format, ...);
		/// @overload
		void drawTextScreen(BWAPI::Position p, const char *format, ...);

		/// <summary>Draws a rectangle on the screen with the given color.</summary>
		///
		/// <param name="ctype">
		///   The coordinate type. Indicates the relative position to draw the shape.
		/// </param>
		/// <param name="left">
		///   The x coordinate, in pixels, relative to ctype, of the left edge of the rectangle.
		/// </param>
		/// <param name="top">
		///   The y coordinate, in pixels, relative to ctype, of the top edge of the rectangle.
		/// </param>
		/// <param name="right">
		///   The x coordinate, in pixels, relative to ctype, of the right edge of the rectangle.
		/// </param>
		/// <param name="bottom">
		///   The y coordinate, in pixels, relative to ctype, of the bottom edge of the rectangle.
		/// </param>
		/// <param name="color">
		///   The color of the rectangle.
		/// </param>
		/// <param name="isSolid"> (optional)
		///   If true, then the shape will be filled and drawn as a solid, otherwise it will be drawn
		///   as an outline. If omitted, this value will default to false.
		/// </param>
		virtual void drawBox(BWAPI::CoordinateType::Enum ctype, int left, int top, int right, int bottom, BWAPI::Color color, bool isSolid = false) = 0;
		/// @overload
		void drawBoxMap(int left, int top, int right, int bottom, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawBoxMap(BWAPI::Position leftTop, BWAPI::Position rightBottom, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawBoxMouse(int left, int top, int right, int bottom, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawBoxMouse(BWAPI::Position leftTop, BWAPI::Position rightBottom, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawBoxScreen(int left, int top, int right, int bottom, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawBoxScreen(BWAPI::Position leftTop, BWAPI::Position rightBottom, BWAPI::Color color, bool isSolid = false);

		/// <summary>Draws a triangle on the screen with the given color.</summary>
		///
		/// <param name="ctype">
		///   The coordinate type. Indicates the relative position to draw the shape.
		/// </param>
		/// <param name="ax">
		///   The x coordinate, in pixels, relative to ctype, of the first point.
		/// </param>
		/// <param name="ay">
		///   The y coordinate, in pixels, relative to ctype, of the first point.
		/// </param>
		/// <param name="bx">
		///   The x coordinate, in pixels, relative to ctype, of the second point.
		/// </param>
		/// <param name="by">
		///   The y coordinate, in pixels, relative to ctype, of the second point.
		/// </param>
		/// <param name="cx">
		///   The x coordinate, in pixels, relative to ctype, of the third point.
		/// </param>
		/// <param name="cy">
		///   The y coordinate, in pixels, relative to ctype, of the third point.
		/// </param>
		/// <param name="color">
		///   The color of the triangle.
		/// </param>
		/// <param name="isSolid"> (optional)
		///   If true, then the shape will be filled and drawn as a solid, otherwise it will be drawn
		///   as an outline. If omitted, this value will default to false.
		/// </param>
		virtual void drawTriangle(BWAPI::CoordinateType::Enum ctype, int ax, int ay, int bx, int by, int cx, int cy, BWAPI::Color color, bool isSolid = false) = 0;
		/// @overload
		void drawTriangleMap(int ax, int ay, int bx, int by, int cx, int cy, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawTriangleMap(BWAPI::Position a, BWAPI::Position b, BWAPI::Position c, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawTriangleMouse(int ax, int ay, int bx, int by, int cx, int cy, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawTriangleMouse(BWAPI::Position a, BWAPI::Position b, BWAPI::Position c, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawTriangleScreen(int ax, int ay, int bx, int by, int cx, int cy, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawTriangleScreen(BWAPI::Position a, BWAPI::Position b, BWAPI::Position c, BWAPI::Color color, bool isSolid = false);

		/// <summary>Draws a circle on the screen with the given color.</summary>
		///
		/// <param name="ctype">
		///   The coordinate type. Indicates the relative position to draw the shape.
		/// </param>
		/// <param name="x">
		///   The x coordinate, in pixels, relative to ctype.
		/// </param>
		/// <param name="y">
		///   The y coordinate, in pixels, relative to ctype.
		/// </param>
		/// <param name="radius">
		///   The radius of the circle, in pixels.
		/// </param>
		/// <param name="color">
		///   The color of the circle.
		/// </param>
		/// <param name="isSolid"> (optional)
		///   If true, then the shape will be filled and drawn as a solid, otherwise it will be drawn
		///   as an outline. If omitted, this value will default to false.
		/// </param>
		virtual void drawCircle(BWAPI::CoordinateType::Enum ctype, int x, int y, int radius, BWAPI::Color color, bool isSolid = false) = 0;
		/// @overload
		void drawCircleMap(int x, int y, int radius, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawCircleMap(BWAPI::Position p, int radius, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawCircleMouse(int x, int y, int radius, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawCircleMouse(BWAPI::Position p, int radius, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawCircleScreen(int x, int y, int radius, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawCircleScreen(BWAPI::Position p, int radius, BWAPI::Color color, bool isSolid = false);

		/// <summary>Draws an ellipse on the screen with the given color.</summary>
		///
		/// <param name="ctype">
		///   The coordinate type. Indicates the relative position to draw the shape.
		/// </param>
		/// <param name="x">
		///   The x coordinate, in pixels, relative to ctype.
		/// </param>
		/// <param name="y">
		///   The y coordinate, in pixels, relative to ctype.
		/// </param>
		/// <param name="xrad">
		///   The x radius of the ellipse, in pixels.
		/// </param>
		/// <param name="yrad">
		///   The y radius of the ellipse, in pixels.
		/// </param>
		/// <param name="color">
		///   The color of the ellipse.
		/// </param>
		/// <param name="isSolid"> (optional)
		///   If true, then the shape will be filled and drawn as a solid, otherwise it will be drawn
		///   as an outline. If omitted, this value will default to false.
		/// </param>
		virtual void drawEllipse(BWAPI::CoordinateType::Enum ctype, int x, int y, int xrad, int yrad, BWAPI::Color color, bool isSolid = false) = 0;
		/// @overload
		void drawEllipseMap(int x, int y, int xrad, int yrad, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawEllipseMap(BWAPI::Position p, int xrad, int yrad, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawEllipseMouse(int x, int y, int xrad, int yrad, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawEllipseMouse(BWAPI::Position p, int xrad, int yrad, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawEllipseScreen(int x, int y, int xrad, int yrad, BWAPI::Color color, bool isSolid = false);
		/// @overload
		void drawEllipseScreen(BWAPI::Position p, int xrad, int yrad, BWAPI::Color color, bool isSolid = false);

		/// <summary>Draws a dot on the map or screen with a given color.</summary>
		///
		/// <param name="ctype">
		///   The coordinate type. Indicates the relative position to draw the shape.
		/// </param>
		/// <param name="x">
		///   The x coordinate, in pixels, relative to ctype.
		/// </param>
		/// <param name="y">
		///   The y coordinate, in pixels, relative to ctype.
		/// </param>
		/// <param name="color">
		///   The color of the dot.
		/// </param>
		virtual void drawDot(BWAPI::CoordinateType::Enum ctype, int x, int y, BWAPI::Color color) = 0;
		/// @overload
		void drawDotMap(int x, int y, BWAPI::Color color);
		/// @overload
		void drawDotMap(BWAPI::Position p, BWAPI::Color color);
		/// @overload
		void drawDotMouse(int x, int y, BWAPI::Color color);
		/// @overload
		void drawDotMouse(BWAPI::Position p, BWAPI::Color color);
		/// @overload
		void drawDotScreen(int x, int y, BWAPI::Color color);
		/// @overload
		void drawDotScreen(BWAPI::Position p, BWAPI::Color color);

		/// <summary>Draws a line on the map or screen with a given color.</summary>
		///
		/// <param name="ctype">
		///   The coordinate type. Indicates the relative position to draw the shape.
		/// </param>
		/// <param name="x1">
		///   The starting x coordinate, in pixels, relative to ctype.
		/// </param>
		/// <param name="y1">
		///   The starting y coordinate, in pixels, relative to ctype.
		/// </param>
		/// <param name="x2">
		///   The ending x coordinate, in pixels, relative to ctype.
		/// </param>
		/// <param name="y2">
		///   The ending y coordinate, in pixels, relative to ctype.
		/// </param>
		/// <param name="color">
		///   The color of the line.
		/// </param>
		virtual void drawLine(BWAPI::CoordinateType::Enum ctype, int x1, int y1, int x2, int y2, BWAPI::Color color) = 0;
		/// @overload
		void drawLineMap(int x1, int y1, int x2, int y2, BWAPI::Color color);
		/// @overload
		void drawLineMap(BWAPI::Position a, BWAPI::Position b, BWAPI::Color color);
		/// @overload
		void drawLineMouse(int x1, int y1, int x2, int y2, BWAPI::Color color);
		/// @overload
		void drawLineMouse(BWAPI::Position a, BWAPI::Position b, BWAPI::Color color);
		/// @overload
		void drawLineScreen(int x1, int y1, int x2, int y2, BWAPI::Color color);
		/// @overload
		void drawLineScreen(BWAPI::Position a, BWAPI::Position b, BWAPI::Color color);

		/// @}
	};
}
