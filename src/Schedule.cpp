#include "Schedule.h"
#include "Utils.h"
#include <wx/xml/xml.h>

#define CHECK_ATTRIBUTE_RETURN(node,name,value) do {        \
    if (!node->GetAttribute(name,&value)) {                 \
        wxLogError("Attribute %s could not be found",name); \
        return false;                                       \
    }                                                       \
} while (0)

Serie::Serie(Type t,
             std::string n,
             std::string l,
             unsigned long c,
             std::vector<double> d) noexcept :
    type{t},
    name{n},
    label{l},
    colour{c},
    data{std::move(d)}
{
}

Schedule::Schedule(const char* filename) :
    _filename{filename ? filename : ""}
{
    create();
}

std::string Schedule::getName() const 
{
    return _name;
}

std::string Schedule::getDescription() const
{
    return _description;
}

std::string Schedule::getXTitle() const
{
    return _xTitle;
}

std::string Schedule::getYTitle() const
{
    return _yTitle;
}

wxString* Schedule::getPlainCategories()
{
    return _categories.data();
}

std::vector<Serie>& Schedule::getSeries()
{
    return _series;
}

unsigned long Schedule::getColour(const std::string& serie)
{
    auto it = _colorMap.find(serie);
    if (it != _colorMap.cend()) 
        return it->second;
    // TODO unique random numbers
    const unsigned long colour = rand();
    _colorMap.emplace(serie, colour);
    return colour;
}

int Schedule::getNbCategories() const
{
    return _categories.size();
}

bool Schedule::isEmpty() const
{
    return _categories.empty() || _series.empty();
}

void Schedule::create()
{
    wxXmlDocument doc;
    if (!doc.Load(_filename)) 
        return;
    wxXmlNode *root = doc.GetRoot();
    if (!createCategories(root)) // create categories before dataset!
        return;
    createDataset(root);
}

bool Schedule::addSerie(wxXmlNode *parent, int resource, double& makespan)
{
    const int nResources = getNbCategories();
    wxXmlNode *children = parent->GetChildren();
    if (children->GetName() == "series") {
        children = children->GetChildren();
        while (children) {
            if (children->GetName() == "serie") {
                wxString name, label, setupStr, startStr, durationStr;
                CHECK_ATTRIBUTE_RETURN(children,"name",name);
                CHECK_ATTRIBUTE_RETURN(children,"label",label);
                CHECK_ATTRIBUTE_RETURN(children,"setup",setupStr);
                CHECK_ATTRIBUTE_RETURN(children,"start",startStr);
                CHECK_ATTRIBUTE_RETURN(children,"duration",durationStr);
                const unsigned long colour = getColour(name.ToStdString());
                const double setup = wxAtof(setupStr);
                const double start = wxAtof(startStr);
                const double duration = wxAtof(durationStr);
                if (isEpsPositive(setup)) {
                    // idle
                    const double idle = start - setup - makespan;
                    if (isEpsPositive(idle)) { 
                        // TODO idle and setup colour should not have (unsigned long) 0
                        _series.emplace_back(Serie::Type::Idle, "", "", 0, createData(nResources, resource, idle));
                        makespan += idle;
                    }
                    // setup
                    _series.emplace_back(Serie::Type::Setup, "setup", "", 0, createData(nResources, resource, setup)); // the name is shown in chart legend
                    makespan += setup;
                }
                else {
                    // idle
                    const double idle = start - makespan;
                    if (isEpsPositive(start - makespan)) { 
                        _series.emplace_back(Serie::Type::Idle, "", "", 0, createData(nResources, resource, idle));
                        makespan += idle;
                    }
                }
                // duration
                _series.emplace_back(Serie::Type::Regular,
                                     name.ToStdString(),
                                     label.ToStdString(),
                                     colour,
                                     createData(nResources, resource, duration));
                makespan += duration;
            }
            children = children->GetNext();
        }
    }
    return true;
}

bool Schedule::createDataset(wxXmlNode *root)
{
    wxXmlNode *children = root->GetChildren();
    while (children) {
        if (children->GetName() == "gantt") {
            children = children->GetChildren();
            if (children->GetName() == "categories") {
                children = children->GetChildren();
                int resource = 0;
                while (children) {
                    if (children->GetName() == "category") {
                        double makespan = 0.0;
                        if (!addSerie(children, resource, makespan)) 
                            return false;
                        resource++;
                    }
                    children = children->GetNext();
                }
            }
        }
    }
    return true;
}

bool Schedule::createCategories(wxXmlNode *root)
{
    wxXmlNode *children = root->GetChildren();
    while (children) {
        if (children->GetName() == "gantt") {
            wxString attribute;
            CHECK_ATTRIBUTE_RETURN(children,"name",attribute);
            _name = attribute;
            CHECK_ATTRIBUTE_RETURN(children,"description",attribute);
            _description = attribute;
            CHECK_ATTRIBUTE_RETURN(children,"xTitle",attribute);
            _xTitle = attribute;
            CHECK_ATTRIBUTE_RETURN(children,"yTitle",attribute);
            _yTitle = attribute;
            children = children->GetChildren();
            if (children->GetName() == "categories") {
                children = children->GetChildren();
                while (children) {
                    if (children->GetName() == "category") {
                        CHECK_ATTRIBUTE_RETURN(children,"name",attribute);
                        // TODO std::move()?
                        _categories.push_back(attribute);
                    }
                    children = children->GetNext();
                }
            }
        }
    }
    return true;
}