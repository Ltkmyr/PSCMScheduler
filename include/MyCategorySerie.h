#pragma once

#include <wx/category/categorysimpledataset.h>

#include "Schedule.h"

class MyCategorySerie : public CategorySerie {
public: 
    MyCategorySerie(const wxString &label,
                    const wxString &name, 
                    double *values, 
                    size_t count);
    wxString getLabel() const;
private:
    wxString _label;
};

struct MyCategorySimpleDataset : public CategorySimpleDataset {
    MyCategorySimpleDataset(wxString *names, size_t count);
    wxString GetSerieLabel(size_t index) const;
};