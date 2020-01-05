#pragma once

#include "Common.h"


namespace SparCraft
{

    // type definitions for storing data
    typedef		size_t		        IDType;
    typedef		size_t		        UnitCountType;
    typedef		size_t		        ChildCountType;
    typedef 	int					PositionType;
    typedef 	int					TimeType;
    typedef		short				HealthType;
    typedef		int					ScoreType;
    typedef		unsigned int		HashType;
    typedef     int                 UCTValue;

    class StateEvalScore
    {
        ScoreType	_val;
        int			_numMoves;

    public:

        StateEvalScore()
            : _val(0)
            ,_numMoves(0)
        {
        }

        StateEvalScore(const ScoreType & val,const int & numMoves)
            : _val(val)
            ,_numMoves(numMoves)
        {
        }

        const bool operator < (const StateEvalScore & rhs) const
        {
            if (_val < rhs._val)
            {
                return true;
            }
            else if (_val == rhs._val)
            {
                return _numMoves > rhs._numMoves;
            }
            else
            {
                return false;
            }
        }

        const bool operator > (const StateEvalScore & rhs) const
        {
            if (_val > rhs._val)
            {
                return true;
            }
            else if (_val == rhs._val)
            {
                return _numMoves < rhs._numMoves;
            }
            else
            {
                return false;
            }
        }

        const bool operator <= (const StateEvalScore & rhs) const
        {
            if (_val > rhs._val)
            {
                return true;
            }
            else if (_val == rhs._val)
            {
                return _numMoves >= rhs._numMoves;
            }
            else
            {
                return false;
            }
        }

        const bool operator >= (const StateEvalScore & rhs) const
        {
            if (_val > rhs._val)
            {
                return true;
            }
            else if (_val == rhs._val)
            {
                return _numMoves <= rhs._numMoves;
            }
            else
            {
                return false;
            }
        }

        const bool operator == (const StateEvalScore & rhs) const
        {
            return (_val == rhs._val) && (_numMoves == rhs._numMoves);
        }

        const ScoreType & val() const { return _val; }
        const TimeType & numMoves() const { return _numMoves; }
    };



}

