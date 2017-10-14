#pragma once

#include "../Common.h"
#include <map>

namespace SparCraft
{

class HTMLChart
{
    std::string _title;
    std::string _xTitle;
    std::string _yTitle;

    bool        _isDateChart;

    std::map<std::string, std::vector<double>> _data;
    std::map<std::string, std::vector<std::string>> _dates;

public:

    HTMLChart(const std::string & title, const std::string & xTitle, const std::string & yTitle);

    std::string getUTCString(const std::string & date, const double & data) const;
    void setData(const std::string & key, const std::vector<double> & data);
    void setDates(const std::string & key, const std::vector<std::string> & dates);
    
    void appendHTMLChartToFile(const std::string & filename, const std::string & chartDivID, size_t minX, size_t maxX, bool initiallyHidden = false);
};

}