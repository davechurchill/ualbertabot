#pragma once

#include <vector>

template <class T>
class Grid
{
    size_t m_width = 0;
    size_t m_height = 0;

    std::vector<std::vector<T>> m_grid;

public:

    Grid() {}

    Grid(size_t width, size_t height, T val)
        : m_width(width)
        , m_height(height)
        , m_grid(width, std::vector<T>(height, val))
    {

    }

    inline T & get(size_t x, size_t y)
    {
        return m_grid[x][y];
    }

    inline const T & get(size_t x, size_t y) const
    {
        return m_grid[x][y];
    }

    inline void set(size_t x, size_t y, T val)
    {
        m_grid[x][y] = val;
    }

    inline size_t width() const
    {
        return m_width;
    }

    inline size_t height() const
    {
        return m_height;
    }
};