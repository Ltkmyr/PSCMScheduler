#pragma once

#include <wx/legend.h>

struct MyLegend : public Legend {
    MyLegend(int vertPosition, int horizPosition); 
    virtual void Draw(wxDC &dc, wxRect rc, DatasetArray &datasets) override;
    virtual wxSize GetExtent(wxDC &dc, DatasetArray &datasets) override;
};