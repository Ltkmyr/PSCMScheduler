#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <wx/xml/xml.h>
#include <wx/log.h>

#include "Utils.h"

#define CHECK_ATTRIBUTE_BREAK(node,name,value) do {         \
    if (!node->GetAttribute(name,&value)) {                 \
        wxLogError("Attribute %s could not be found",name); \
        break;                                              \
    }                                                       \
} while (0)

struct Option {
    std::string name;
    std::string cmd;
    std::string value;
    bool show;
};

struct Solver {
    const Option* findOptionSolution() const;
    const Option* findOptionChart() const;
    std::pair<int,bool> getSuccessCode() const;
    std::string name;
    std::string cmd;
    std::string solution;
    std::string chart;
    std::string success;
    std::vector<Option> options;
};

struct Preferences  {   
    wxXmlAttribute* toAttribute() const;
    bool showLegend{true};
};

class Config {
public:
    explicit Config(const char* filename);
    const Solver* findSolver(const char* name) const;
    const std::vector<Solver>& getSolvers() const;
    std::string getFilename() const;
    bool savePreferences(Preferences p);
    Preferences preferences;
private:
    void create();
    std::string _filename;
    std::vector<Solver> _solvers;
};

static Preferences createPreferences(wxXmlNode *parent)
{
    Preferences p;
    wxXmlNode *children = parent->GetChildren();
    while (children) {
        if (children->GetName() == "preferences") {
            wxString attribute;
            CHECK_ATTRIBUTE_BREAK(children,"legend",attribute); 
            p.showLegend = to_bool(attribute);
        }
        children = children->GetNext();
    }
    return p;
}

static std::vector<Option> createOptions(wxXmlNode *parent)
{
    std::vector<Option> options;
    wxXmlNode *children = parent->GetChildren();
    while (children){
        if (children->GetName() == "options") {
            wxXmlNode *child = children->GetChildren();
            while (child) {
                if (child->GetName() == "option") {
                    Option o;
                    wxString attribute;
                    CHECK_ATTRIBUTE_BREAK(child,"name",attribute);  
                    o.name = attribute;
                    CHECK_ATTRIBUTE_BREAK(child,"cmd",attribute);
                    o.cmd = attribute;
                    CHECK_ATTRIBUTE_BREAK(child,"value",attribute);
                    o.value = attribute;
                    CHECK_ATTRIBUTE_BREAK(child,"show",attribute);
                    o.show = to_bool(attribute);
                    // TODO emplace_back
                    options.push_back(std::move(o));
                }
                child = child->GetNext();
            }
        }
        children = children->GetNext();
    }
    return options;
}

static std::vector<Solver> createSolvers(wxXmlNode *parent)
{
    std::vector<Solver> solvers;
    wxXmlNode *children = parent->GetChildren();
    while (children) {
        if (children->GetName() == "solvers") {
            wxXmlNode *child = children->GetChildren();
            while (child) {
                if (child->GetName() == "solver") {
                    Solver s;
                    wxString attribute;
                    CHECK_ATTRIBUTE_BREAK(child,"name",attribute);
                    s.name = attribute;
                    CHECK_ATTRIBUTE_BREAK(child,"cmd",attribute);
                    s.cmd = attribute;
                    CHECK_ATTRIBUTE_BREAK(child,"success",attribute);
                    s.success = attribute;
                    CHECK_ATTRIBUTE_BREAK(child,"solution",attribute);
                    s.solution = attribute;
                    CHECK_ATTRIBUTE_BREAK(child,"chart",attribute);
                    s.chart = attribute;
                    s.options = createOptions(child);
                    // TODO emplace_back
                    solvers.push_back(std::move(s));
                }
                child = child->GetNext();
            }
        }
        children = children->GetNext();
    }
    return solvers;
}
