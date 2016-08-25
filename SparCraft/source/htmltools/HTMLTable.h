#pragma once

#include "../Common.h"
#include <map>

namespace SparCraft
{

struct Cell
{
    size_t row;
    size_t col;

    Cell() : row(0), col(0) {}
    Cell(size_t r, size_t c) : row(r), col(c) {}
    bool operator < (const Cell & rhs) const { return (row == rhs.row ? col < rhs.col : row < rhs.row); }
    bool operator == (const Cell & rhs) const { return row == rhs.row && col == rhs.col; }
};

class HTMLTable
{
    std::string _title;
    std::vector<std::string> _header;
    std::vector<size_t> _colWidth;

    std::map<Cell, std::string> _table;

    size_t _rows;
    size_t _cols;
    size_t _minRow;

    void expandToFit(const size_t & row, const size_t & column);

public:

    HTMLTable(const std::string & title);

    void setData(const size_t & row, const size_t & column, const std::string & data);
    void setData(const size_t & row, const size_t & column, const int & data);
    void setData(const size_t & row, const size_t & column, const size_t & data);
    void setData(const size_t & row, const size_t & column, const double & data);
    void setHeader(const std::vector<std::string> & header);
    void setColWidth(const std::vector<size_t> & width);

    const std::string & getData(const size_t & row, const size_t column);

    
    const size_t & rows() const;
    const size_t & cols() const;

    void appendHTMLTableToFile(const std::string & filename, const std::string & tableID, bool initiallyHidden = false);
};

}