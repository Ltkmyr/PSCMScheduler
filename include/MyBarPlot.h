#pragma once

#include <wx/bars/barplot.h>

#include "MyLegend.h"

class MyBarPlot : public BarPlot {
public:
    MyBarPlot();
    void MyAddAxis(Axis *axis);
    void MyLinkDataHorizontalAxis(size_t nData, size_t nAxis);
    void MyLinkDataVerticalAxis(size_t nData, size_t nAxis);
private:
    virtual void DrawBackground(ChartDC& cdc, wxRect rcData) override;
    void MyCalcDataArea(wxDC &dc, wxRect rc, wxRect &rcData, wxRect &rcLegend);
    void MyDrawGridLines(wxDC &dc, wxRect rcData);
    void MyDrawAxes(wxDC &dc, wxRect &rc, wxRect rcData);
    void MyDrawMarkers(wxDC &dc, wxRect rcData);
    void MyDrawLegend(wxDC &dc, wxRect rcLegend);
    void MyDrawAxesArray(wxDC &dc, wxRect rc, AxisArray *axes, bool vertical);
    AxisArray _leftAxes;
    AxisArray _rightAxes;
    AxisArray _topAxes;
    AxisArray _bottomAxes;
    AxisArray _horizontalAxes;
    AxisArray _verticalAxes;
    DataAxisLinkArray _links;
    wxCoord _legendPlotGap;
    Legend *_legend;
    Crosshair *_crosshair;
};