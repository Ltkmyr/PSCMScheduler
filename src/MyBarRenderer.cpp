#include "MyBarRenderer.h"
#include "MyCategorySerie.h"
#include "Utils.h"

#include <wx/category/categorydataset.h>

MyStackedBarType::MyStackedBarType(int barWidth, double base) :
    StackedBarType{barWidth, base}
{
}

void MyStackedBarType::Draw(BarRenderer *barRenderer, 
                            wxDC &dc, 
                            wxRect rc, 
                            Axis *horizAxis, 
                            Axis *vertAxis, 
                            bool vertical, 
                            size_t item, 
                            CategoryDataset *dataset)
{
    FOREACH_SERIE(serie, dataset) {
        // bar geometry params
        int width;
        wxCoord shift;
        double base, value;

        // get bar geometry
        GetBarGeometry(dataset, item, serie, width, shift, base, value);

        double xBase, yBase;
        double xVal, yVal;

        if (vertical) {
            xBase = xVal = item;
            yBase = base;
            yVal = value;
        }
        else {
            xBase = base;
            yBase = yVal = item;
            xVal = value;
        }

        // transform base and value to graphics coordinates
        wxCoord xBaseG = horizAxis->ToGraphics(dc, rc.x, rc.width, xBase);
        wxCoord yBaseG = vertAxis->ToGraphics(dc, rc.y, rc.height, yBase);
        wxCoord xG = horizAxis->ToGraphics(dc, rc.x, rc.width, xVal);
        wxCoord yG = vertAxis->ToGraphics(dc, rc.y, rc.height, yVal);

        wxRect rcBar;
        if (vertical) {
            xBaseG += shift;
            xG += shift;

            rcBar.x = wxMin(xBaseG, xG);
            rcBar.y = wxMin(yBaseG, yG);
            rcBar.width = width;
            rcBar.height = ABS(yBaseG - yG);
        }
        else {
            yBaseG += shift;
            yG += shift;

            rcBar.x = wxMin(xBaseG, xG);
            rcBar.y = wxMin(yBaseG, yG);
            rcBar.width = ABS(xBaseG - xG);
            rcBar.height = width;
        }

        /////// MODIFICATION
        if (!isEpsPositive(dataset->GetValue(item,serie))) 
            continue;

        // this is bad design
        auto myDataset = dynamic_cast<MyCategorySimpleDataset*>(dataset);
        if (!myDataset) {
            wxLogError("Bad dynamic cast");
            continue;
        }
        
        // draw bar
        AreaDraw *barDraw = barRenderer->GetBarDraw(serie);
        
        if (barDraw) barDraw->Draw(dc, rcBar);
        if (barDraw && 
            myDataset->GetRenderer()->GetSerieColour(serie) != wxNullColour && 
            myDataset->GetRenderer()->GetSerieColour(serie) != *wxBLACK) 
                dc.DrawLabel(myDataset->GetSerieLabel(serie), rcBar, wxALIGN_CENTER | wxALIGN_CENTER);
        /////// 
    }                       
}

MyBarRenderer::MyBarRenderer(BarType *barType) :
    BarRenderer{barType}
{
}

void MyBarRenderer::DrawLegendSymbol(wxDC &dc, wxRect rcSymbol, size_t serie)
{
    AreaDraw *barDraw = GetBarDraw(serie);
    /////// MODIFICATION
    if (!barDraw) 
        return;
    barDraw->Draw(dc, rcSymbol);
    ///////
}

AreaDraw *MyBarRenderer::GetBarDraw(size_t serie)
{
    AreaDraw *barDraw = m_barDraws.GetAreaDraw(serie);
    if (barDraw == NULL) 
    {
        /////// MODIFICATION
        wxColour colour = GetSerieColour(serie);
        if (colour == wxNullColour) 
            return NULL;

        barDraw = new FillAreaDraw(*wxTRANSPARENT_PEN, colour);
        ///////

        m_barDraws.SetAreaDraw(serie, barDraw);
    }
    return barDraw;
}