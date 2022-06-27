#include "MyLegend.h"

#include <unordered_set>

MyLegend::MyLegend(int vertPosition, int horizPosition) :
    Legend{vertPosition, horizPosition}
{
}

void MyLegend::Draw(wxDC &dc, wxRect rc, DatasetArray &datasets)
{
    dc.SetFont(m_font);

    m_background->Draw(dc, rc);

    wxCoord x = rc.x + m_margin;
    wxCoord y = rc.y + m_margin;

    for (size_t n = 0; n < datasets.Count(); n++) {
        Dataset *dataset = datasets[n];

        /////// MODIFICATION
        std::unordered_set<wxUint32> colourLookup;
        ///////

        FOREACH_SERIE(serie, dataset) {
            wxString serieName = dataset->GetSerieName(serie);
            
            /////// MODIFICATION
            Renderer *renderer = dataset->GetBaseRenderer();

            wxColour colour = renderer->GetSerieColour(serie);
            if (colour == wxNullColour) 
                continue;
            auto pair = colourLookup.insert(colour.GetRGBA());
            if (!pair.second) 
                continue;

            wxSize textExtent = dc.GetTextExtent(serieName);
            ///////

            wxRect rcSymbol(x, y, textExtent.y, textExtent.y);
            renderer->DrawLegendSymbol(dc, rcSymbol, serie);

            wxCoord textX = x + rcSymbol.width + m_symbolTextGap;

            dc.DrawText(serieName, textX, y);

            y += textExtent.y + labelsSpacing;
        }
    }
}

wxSize MyLegend::GetExtent(wxDC &dc, DatasetArray &datasets)
{
    wxSize extent(0, 0);

    dc.SetFont(m_font);

    extent.y = 2 * m_margin;

    for (size_t n = 0; n < datasets.Count(); n++) {
        Dataset *dataset = datasets[n];

        /////// MODIFICATION
        std::unordered_set<wxUint32> colourLookup;
        ///////

        FOREACH_SERIE(serie, dataset) {
            wxSize textExtent = dc.GetTextExtent(dataset->GetSerieName(serie));

            /////// MODIFICATION
            Renderer *renderer = dataset->GetBaseRenderer();
            wxColour colour = renderer->GetSerieColour(serie);
            if (colour == wxNullColour) 
                continue;
            auto pair = colourLookup.insert(colour.GetRGBA());
            if (!pair.second) 
                continue;
            ///////

            wxCoord symbolSize = textExtent.y; // symbol rectangle width and height

            wxCoord width = textExtent.x + symbolSize + m_symbolTextGap + 2 * m_margin;

            extent.x = wxMax(extent.x, width);

            extent.y += textExtent.y;
            if (serie < dataset->GetSerieCount() - 1) {
                extent.y += labelsSpacing;
            }
        }
    }
    return extent;
}
