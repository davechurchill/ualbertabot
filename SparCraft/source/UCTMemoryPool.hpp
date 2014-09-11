#pragma once

#include "Common.h"
#include "UCTNode.h"

namespace SparCraft
{
    class UCTMemoryPool;
}

class SparCraft::UCTMemoryPool
{	
    std::vector< std::vector<UCTNode> > _pool;

    const size_t    _poolSize;
    const size_t    _maxChildren;
    
    size_t    _currentIndex;

public:

	UCTMemoryPool(const size_t & poolSize, const size_t & maxChildren)
        : _pool             (poolSize, std::vector<UCTNode>())
        , _poolSize         (poolSize)
        , _maxChildren      (maxChildren)
        , _currentIndex     (0)
    {
        for (size_t s(0); s<poolSize; ++s)
        {
            _pool[s].reserve(maxChildren);
        }
    }

    std::vector<UCTNode> * alloc()
    {
        std::vector<UCTNode> & ret(_pool[_currentIndex]);
        if (ret.size() > 0)
        {
            ret.clear();
        }

        _currentIndex = (_currentIndex + 1) % _poolSize;
        return &ret;
    }

    void clearPool()
    {
        for (size_t i(0); i<_poolSize; ++i)
        {
            _pool[i].clear();
        }

        _currentIndex = 0;
    }
};
