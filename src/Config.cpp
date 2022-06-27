#include "Config.h"
#include <wx/xml/xml.h>

const Option* Solver::findOptionSolution() const
{
    // TODO check hash table
    auto it = std::find_if(options.cbegin(), options.cend(), [&](const Option& option){return option.name == solution;});
    if (it == options.cend())
        return nullptr;
    return &(*it);
}

const Option* Solver::findOptionChart() const
{
    // TODO check hash table
    auto it = std::find_if(options.cbegin(), options.cend(), [&](const Option& option){return option.name == chart;});
    if (it == options.cend())
        return nullptr;
    return &(*it);
}

std::pair<int,bool> Solver::getSuccessCode() const
{
    if (success.empty())
        return {0, false};
    int code = std::atoi(success.c_str());
    return {code, true};
}

wxXmlAttribute* Preferences::toAttribute() const
{
    std::stringstream s;
    s << std::boolalpha << showLegend;
    return new wxXmlAttribute("legend", s.str());
}

Config::Config(const char* filename) :
    _filename{filename ? filename : ""}
{
    create();
}

const Solver* Config::findSolver(const char* name) const
{
    // TODO check hash table 
    auto it = std::find_if(_solvers.cbegin(), _solvers.cend(), [&](const Solver& s){return s.name == name;});
    if (it == _solvers.cend()) 
        return nullptr;
    return &(*it);
}

const std::vector<Solver>& Config::getSolvers() const
{
    return _solvers;
}

std::string Config::getFilename() const
{
    return _filename;
}

bool Config::savePreferences(Preferences p)
{
    wxXmlDocument doc;
    if (!doc.Load(_filename)) 
        return false;
    wxXmlNode *root = doc.GetRoot();
    wxXmlNode *children = root->GetChildren();
    while (children) {
        if (children->GetName() == "preferences") {
            children->SetAttributes(p.toAttribute());
            preferences.showLegend = p.showLegend;
        }
        children = children->GetNext();
    }
    return doc.Save(_filename);
}

void Config::create()
{
    wxXmlDocument doc;
    if (!doc.Load(_filename))
        return;
    wxXmlNode *child = doc.GetRoot();
    preferences = createPreferences(child);
    _solvers = createSolvers(child);
}