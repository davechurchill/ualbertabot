#pragma once

#include "Common.h"
#include <math.h>
#include <sstream>

namespace SparCraft
{
class Position
{
	// x,y location will be used for Units in a 'grid'
	PositionType		_x, 
						_y;

public:
	
	Position()
		: _x(0)
		, _y(0)
	{
	}

	Position(const PositionType & x, const PositionType & y)
		: _x(x)
		, _y(y)
	{
	}

    Position(const BWAPI::Position & p)
        : _x(p.x())
        , _y(p.y())
    {

    }

	const bool operator < (const Position & rhs) const
	{
        return (x() < rhs.x()) || ((x() == rhs.x()) && y() < rhs.y());
	}

    const bool operator == (const Position & rhs) const
    {
        return x() == rhs.x() && y() == rhs.y();
    }

	const Position operator + (const Position & rhs) const
	{
		return Position(x() + rhs.x(), y() + rhs.y());
	}

	const Position operator - (const Position & rhs) const
	{
		return Position(x() - rhs.x(), y() - rhs.y());
	}

	const Position scale(const float & f) const
	{
		return Position((PositionType)(f * x()), (PositionType)(f * y()));
	}

    void scalePosition(const float & f)
    {
        _x = (PositionType)(f * _x);
        _y = (PositionType)(f * _y);
    }

    void addPosition(const Position & rhs)
    {
        _x += rhs.x();
        _y += rhs.y();
    }

    void subtractPosition(const Position & rhs)
    {
        _x -= rhs.x();
        _y -= rhs.y();
    }
	
	void moveTo(const Position & pos)
	{
		_x = pos.x();
		_y = pos.y();
	}

	void addPosition(const PositionType & x, const PositionType & y)
	{
		_x += x;
		_y += y;
	}

	void moveTo(const PositionType & x, const PositionType & y)
	{
		_x = x;
		_y = y;
	}

	const PositionType x() const
	{
		return _x;
	}

	const PositionType y() const
	{
		return _y;
	}

	const Position flipX() const
	{
		return Position(-_x,_y);
	}

	const Position flipY() const
	{
		return Position(_y,_x);
	}

    const float Q_rsqrt( float number ) const
    {
        long i;
        float x2, y;
        const float threehalfs = 1.5F;
 
        x2 = number * 0.5F;
        y  = number;
        i  = * ( long * ) &y;                       // evil floating point bit level hacking
        i  = 0x5f3759df - ( i >> 1 );               
        y  = * ( float * ) &i;
        y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//      y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
 
        return y;
    }

	const Position flip() const
	{
		return Position(-_x, -_y);
	}

    inline const PositionType getDistance(const Position & p) const	
	{
        PositionType dX = x() - p.x();
        PositionType dY = y() - p.y();

        if (dX == 0)
        {
            return abs(dY);
        }
        else if (dY == 0)
        {
            return abs(dX);
        }
        else
        {
            return (PositionType)sqrt((float)(dX*dX - dY*dY));
        }
	}

	inline const PositionType getDistanceSq(const Position & p) const	
	{
        return (x()-p.x())*(x()-p.x()) + (y()-p.y())*(y()-p.y());
	}

	void print() const
	{
		printf("Position = (%d, %d)\n", _x, _y);
	}

    const std::string getString() const
    {
        std::stringstream ss;
        ss << "(" << x() << ", " << y() << ")";
        return ss.str();
    }

};
}