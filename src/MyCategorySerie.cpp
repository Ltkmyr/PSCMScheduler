#include "MyCategorySerie.h"

MyCategorySerie::MyCategorySerie(const wxString &label, 
                                 const wxString &name, 
                                 double *values, 
                                 size_t count) :
    CategorySerie{name, values, count},
    _label{label}
{
}

wxString MyCategorySerie::getLabel() const
{
    return _label;
}

MyCategorySimpleDataset::MyCategorySimpleDataset(wxString *names, size_t count) :
    CategorySimpleDataset{names, count}
{
}

wxString MyCategorySimpleDataset::GetSerieLabel(size_t index) const
{
    // this is bad design
    auto serie = dynamic_cast<MyCategorySerie*>(m_series[index]); 
    if (!serie) {
        wxLogError("Bad dynamic cast");
        return wxString();
    }
    return serie->getLabel();
}