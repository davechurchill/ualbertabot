#pragma once

namespace AKBot
{
	struct Rect
	{
		int left;
		int top;
		int right;
		int bottom;
		Rect() {}

		Rect(int left, int top, int right, int bottom)
			: left(left), top(top), right(right), bottom(bottom)
		{}

		int center() const
		{

		};
	};
}