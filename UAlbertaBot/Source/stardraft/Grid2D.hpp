#pragma once

#include <vector>
#include <iostream>

template <class T>
class Grid2D
{
    size_t m_width = 0;
    size_t m_height = 0;

    std::vector<std::vector<T>> m_grid;

public:

    Grid2D() {}

    Grid2D(size_t width, size_t height, T val)
        : m_width(width)
        , m_height(height)
        , m_grid(width, std::vector<T>(height, val))
    {

    }

    T& get(size_t x, size_t y)
    {
        return m_grid[x][y];
    }

    T& get(int x, int y)
    {
        return m_grid[x][y];
    }

    const T& get(size_t x, size_t y) const
    {
        return m_grid[x][y];
    }

    const T& get(int x, int y) const
    {
        return m_grid[x][y];
    }

    void set(size_t x, size_t y, T val)
    {
        m_grid[x][y] = val;
    }

    void set(int x, int y, T val)
    {
        m_grid[x][y] = val;
    }

    size_t width() const
    {
        return m_width;
    }

    size_t height() const
    {
        return m_height;
    }

};