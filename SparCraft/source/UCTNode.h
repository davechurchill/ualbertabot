#pragma once

#include "Common.h"
#include "Action.h"

namespace SparCraft
{


class UCTNode
{
    // uct stat counting variables
    size_t                      _numVisits;         // total visits to this node
    double                      _numWins;           // wins from this node
    double                      _uctVal;            // previous computed UCT value
            
    // game specific variables
    size_t                      _player;            // the player who made a move to generate this node
    size_t                      _nodeType;
    Move     _move;              // the ove that generated this node

    // holds children
    std::vector<UCTNode>        _children;

    // nodes for traversing the tree
    UCTNode *                   _parent;
    
public:

    UCTNode ()
        : _numVisits            (0)
        , _numWins              (0)
        , _uctVal               (0)
        , _player               (Players::Player_None)
        , _nodeType             (SearchNodeType::Default)
        , _parent               (NULL)
    {

    }

    UCTNode (UCTNode * parent, const size_t player, const size_t nodeType, const Move & move, const size_t & maxChildren, std::vector<UCTNode> * fromPool = NULL)
        : _numVisits            (0)
        , _numWins              (0)
        , _uctVal               (0)
        , _player               (player)
        , _nodeType             (nodeType)
        , _move                 (move)
        , _parent               (parent)
    {
        _children.reserve(maxChildren);
    }

    const size_t    numVisits()                 const           { return _numVisits; }
    const double    numWins()                   const           { return _numWins; }
    const size_t    numChildren()               const           { return _children.size(); }
    const double    getUCTVal()                 const           { return _uctVal; }
    const bool      hasChildren()               const           { return numChildren() > 0; }
    const size_t    getNodeType()               const           { return _nodeType; }
    const size_t    getPlayer()                 const           { return _player; }

    UCTNode *       getParent()                 const           { return _parent; }
    const UCTNode & getChild(const size_t & c)  const           { return _children[c]; }
    UCTNode &       getChild(const size_t & c)                  { return _children[c]; }

    void            setUCTVal(double val)                       { _uctVal = val; }
    void            incVisits()                                 { _numVisits++; }
    void            addWins(double val)                         { _numWins += val; }

    std::vector<UCTNode> & getChildren()                        { return _children; }

    const Move & getMove() const
    {
        return _move;
    }

    void setMove(const Move & move)
    {
        _move = move;
    }

    void addChild(UCTNode * parent, const size_t player, const size_t nodeType, const Move & move, const size_t & maxChildren, std::vector<UCTNode> * fromPool = NULL)
    {
        _children.push_back(UCTNode(parent, player, nodeType, move, maxChildren));
    }

    UCTNode & mostVisitedChild() 
    {
        UCTNode * mostVisitedChild = nullptr;
        size_t mostVisits = 0;

        for (size_t c(0); c < numChildren(); ++c)
        {
            UCTNode & child = getChild(c);

            if (!mostVisitedChild || (child.numVisits() > mostVisits))
            {
                mostVisitedChild = &child;
                mostVisits = child.numVisits();
            }
        }

        SPARCRAFT_ASSERT(mostVisitedChild != nullptr, "Most visit child is a null pointer");
        return *mostVisitedChild;
    }

    UCTNode & bestUCTValueChild(const bool maxPlayer, const UCTSearchParameters & params) 
    {
        UCTNode * bestChild = nullptr;
        double bestVal = maxPlayer ? std::numeric_limits<double>::min() : std::numeric_limits<double>::max();

        for (size_t c(0); c < numChildren(); ++c)
        {
            UCTNode & child = getChild(c);
       
            double winRate      = (double)child.numWins() / (double)child.numVisits();
            double uctVal       = params.cValue() * sqrt( log( (double)numVisits() ) / ( child.numVisits() ) );
			double currentVal   = maxPlayer ? (winRate + uctVal) : (winRate - uctVal);

            if (maxPlayer)
            {
                if (currentVal > bestVal)
                {
                    bestVal             = currentVal;
			        bestChild           = &child;
                }
            }
            else if (currentVal < bestVal)
            {
                bestVal             = currentVal;
		        bestChild           = &child;
            }
        }

        SPARCRAFT_ASSERT(bestChild != nullptr, "Best child is a null pointer");
        return *bestChild;
    }
};
}