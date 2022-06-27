#pragma once

#include <wx/bars/barrenderer.h>

struct MyStackedBarType : public StackedBarType {
    MyStackedBarType(int barWidth, double base);
    virtual void Draw(BarRenderer *barRenderer, 
              wxDC &dc, 
              wxRect rc, 
              Axis *horizAxis, 
              Axis *vertAxis, 
              bool vertical, 
              size_t item, 
              CategoryDataset *dataset) override;
};

struct MyBarRenderer : public BarRenderer {
    MyBarRenderer(BarType *barType);
    virtual void DrawLegendSymbol(wxDC &dc, wxRect rcSymbol, size_t serie) override;
    virtual AreaDraw *GetBarDraw(size_t serie) override;
};