#include "MicroUtil.h"

using namespace UAlbertaBot;

const double MicroUtil::PI = 3.14159265;
void MicroUtil::rotate(double &x, double &y, double angle)
{
	angle = angle*PI/180.0;
	x = (x * cos(angle)) - (y * sin(angle));
	y = (y * cos(angle)) + (x * sin(angle));
}

void MicroUtil::normalize(double &x, double &y)
{
	double length = sqrt((x * x) + (y * y));
	if (length != 0)
	{
		x = (x / length);
		y = (y / length);
	}
}