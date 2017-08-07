#include "HTMLChart.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace SparCraft;

HTMLChart::HTMLChart(const std::string & title, const std::string & xTitle, const std::string & yTitle)
    : _title(title)
    , _xTitle(xTitle)
    , _yTitle(yTitle)
    , _isDateChart(false)
{

}

void HTMLChart::setData(const std::string & key, const std::vector<double> & data)
{
    _data[key] = data;
}

void HTMLChart::setDates(const std::string & key, const std::vector<std::string> & dates)
{
    _isDateChart = true;
    _dates[key] = dates;
}

void HTMLChart::appendHTMLChartToFile(const std::string & filename, const std::string & chartDivID, size_t minX, size_t maxX, bool initiallyHidden)
{
    const std::string & blank("");
    FILE * fp = fopen(filename.c_str(), "a");
   
    std::string dateTypeString = _isDateChart ? "type : 'datetime'," : "";

    std::stringstream chartss;
    chartss << "<script type='text/javascript'>\n//<![CDATA[\n";
    chartss << "$(function(){$('#" << chartDivID << "').highcharts({";
    chartss << "chart:{zoomType:'xy'},";
    chartss << "title:{text:'" << _title << "',style:{color:'black',fontSize:'18px'},x:-20},";
    chartss << "xAxis:{" << dateTypeString << "allowDecimals:false,title:{text:'" << _xTitle << "',style:{color:'black',fontSize:'14px'}},labels:{style:{color:'black',fontSize:'14px'}}},";
    chartss << "yAxis:{title:{text:'" << _yTitle << "',style:{color:'black',fontSize:'14px'}},labels:{style:{color:'black',fontSize:'14px'}},";
    chartss << "plotLines:[{value:0,width:0,color:'#ff0000'}]},";
    chartss << "tooltip:{crosshairs:true,valuePrefix:'',valueSuffix:''},";
    chartss << "legend:{layout:'vertical',align:'right',verticalAlign:'middle',borderWidth:0},";
    chartss << "plotOptions:{line:{lineWidth:2,marker:{enabled:false}}},series:[";

    if (maxX == 0)
    {
        maxX = std::numeric_limits<size_t>::max();
    }

    size_t numData = 0;
    for (auto & kv : _data)
    {
        const std::string & name = kv.first;
        const std::vector<double> & data = kv.second;
        
        chartss << "\n{ name: '" << name << "', visible:true, data: ["; 

        for (size_t d(minX); d < std::min(data.size(), maxX); ++d)
        {
            if (_isDateChart)
            {
                chartss << getUTCString(_dates[name][d], data[d]);
            }
            else
            {
                chartss << data[d];
            }

            if (d < data.size() - 1)
            {
                chartss << ", ";
            }
        }

        numData++;

        chartss << "] }";

        if (numData < _data.size())
        {
            chartss << ", ";
        }
    }

    chartss << "]});});//]]>\n";
    chartss << "</script>\n";

    std::string chartdiv = "<script src=\"javascript/highcharts.js\"></script><div id=\"" + chartDivID + "\" style=\"width: 1280px; height: 720px;" + (initiallyHidden ? " display: none;" : "") + "\"></div>\n";
    
    fprintf(fp, chartss.str().c_str());
    fprintf(fp, chartdiv.c_str());
    fprintf(fp, "\n");

    fclose(fp);
}

std::string HTMLChart::getUTCString(const std::string & date, const double & data) const
{
    int year=0, month=0, day=0;

    sscanf(date.c_str(), "%04d-%02d-%02d", &year, &month, &day);

    std::stringstream ss;
    ss << "[Date.UTC(" << year << ", " << (month-1) << ", " << day << "), " << data << "]";
    return ss.str();
}