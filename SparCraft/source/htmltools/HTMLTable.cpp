#include "HTMLTable.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace SparCraft;

HTMLTable::HTMLTable(const std::string & title)
    : _rows(0)
    , _cols(0)
    , _minRow(std::numeric_limits<size_t>::max())
    , _title(title)
{

}

void HTMLTable::setData(const size_t & row, const size_t & column, const size_t & data)
{
    std::stringstream ss;
    ss << data;
    setData(row, column, ss.str());
}

void HTMLTable::setData(const size_t & row, const size_t & column, const double & data)
{
    std::stringstream ss;
    ss << data;
    setData(row, column, ss.str());
}

void HTMLTable::setData(const size_t & row, const size_t & column, const int & data)
{
    std::stringstream ss;
    ss << data;
    setData(row, column, ss.str());
}

void HTMLTable::setData(const size_t & row, const size_t & column, const std::string & data)
{
    _table[Cell(row, column)] = data;

    if (row < _minRow)
    {
        _minRow = row;
    }

    if (row > _rows)
    {
        _rows = row;
    }

    if (column > _cols)
    {
        _cols = column;
    }
}

const std::string & HTMLTable::getData(const size_t & row, const size_t column)
{
    return _table[Cell(row, column)];
}

void HTMLTable::appendHTMLTableToFile(const std::string & filename, const std::string & tableID, bool initiallyHidden)
{
    const std::string & blank("");
    FILE * fp = fopen(filename.c_str(), "a");

    fprintf(fp, "<br><br>\n");
    fprintf(fp, _title.c_str());

    std::stringstream divID;
    divID << tableID << "Div";
   
    std::string showJS = " <a href=\"#\" onclick='document.getElementById(\"" + divID.str() + "\").style.display=\"block\";'>Show</a> ";
    std::string hideJS = " <a href=\"#\" onclick='document.getElementById(\"" + divID.str() + "\").style.display=\"none\";'>Hide</a> ";
    std::string tableJS = "<script type=\"text/javascript\"> $(function() { $(\"#" + tableID + "\").tablesorter({widgets: ['zebra']}); });</script>\n";
    std::string tableHeader = "<div id=\"" + divID.str() + "\"" + (initiallyHidden ? "style=\"display: none\"" : "") + "><table cellpadding=2 border=1 rules=all style=\"font: 12px/1.5em Verdana\" id=\"" + tableID + "\" class=\"tablesorter\">\n";
    
    fprintf(fp, showJS.c_str());
    fprintf(fp, hideJS.c_str());
    fprintf(fp, tableJS.c_str());
    fprintf(fp, tableHeader.c_str());

    fprintf(fp, "<thead>");
    for (size_t h(0); h < _header.size(); ++h)
    {
        if (h < _colWidth.size() && _colWidth[h] > 0)
        {
            std::stringstream ss;
            ss << "<th width=\"";
            ss << _colWidth[h];
            ss << "\">";
            fprintf(fp, ss.str().c_str());
        }
        else
        {
            fprintf(fp, "<th width=\"80\">");
        }
        fprintf(fp, _header[h].c_str());
        fprintf(fp, "</th>");
    }

    fprintf(fp, "</thead>\n");

    for (size_t r(_minRow); r <= _rows; ++r)
    {
        fprintf(fp, "<tr align=center>");
        for (size_t c(0); c <= _cols; ++c)
        {
            auto itr = _table.find(Cell(r,c));

            const std::string & str = (itr == _table.end() ? blank : (*itr).second);

            fprintf(fp, "<td>");
            fprintf(fp, "%s", str.c_str());
            fprintf(fp, "</td>");
        }
        fprintf(fp, "</tr>\n");
    }

    fprintf(fp, "</table></div>\n");
    fclose(fp);
}

void HTMLTable::setHeader(const std::vector<std::string> & header)
{
    _header = header;
}

void HTMLTable::setColWidth(const std::vector<size_t> & colWidth)
{
    _colWidth = colWidth;
}