#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <wx/log.h>
#include <wx/xml/xml.h>

struct Serie {
    enum class Type : unsigned char {
        Setup,
        Idle,
        Regular,
        Unknown
    };
    Serie(Type t,
          std::string n,
          std::string l,
          unsigned long c,
          std::vector<double> d) noexcept;
    Type type;
    std::string name;
    std::string label;
    unsigned long colour;
    std::vector<double> data;
};

class Schedule {
public:
    explicit Schedule(const char* filename);
    std::string getName() const;
    std::string getDescription() const;
    std::string getXTitle() const;
    std::string getYTitle() const;
    wxString* getPlainCategories();
    std::vector<Serie>& getSeries();
    unsigned long getColour(const std::string& serie);
    int getNbCategories() const;
    bool isEmpty() const;
private:
    void create();
    bool createCategories(wxXmlNode *root);
    bool addSerie(wxXmlNode *parent, int resource, double& makespan);
    bool createDataset(wxXmlNode *root);
    std::string _name;
    std::string _description;
    std::string _xTitle;
    std::string _yTitle;
    std::string _filename;
    std::vector<wxString> _categories;
    std::vector<Serie> _series;
    std::unordered_map<std::string,unsigned long> _colorMap;
};

static std::vector<double> createData(int size, int index, double value)
{
    if (index > size - 1) {
        wxLogError("Index out of bounds");
        return std::vector<double>();
    }
    std::vector<double> data(size);
    data[index] = value;
    return data;
}