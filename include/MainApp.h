#pragma once

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
 
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <string>
#include <unordered_map>

#include <wx/chartpanel.h>
#include <wx/aui/aui.h>

#include "Config.h"
#include "Schedule.h"

struct MainApp : public wxApp {
    MainApp();
    virtual bool OnInit() override;
};

class MainFrame : public wxFrame {
public:
    struct Input {
        wxString name;
        wxString cmd;
        wxString solutionFile;
        wxString chartFile;
    };
    MainFrame();
    ~MainFrame();
    void setInput(Input input);
    wxString getSolverName() const;
    wxString getSolverCmd() const;
    wxString getSolverSolutionFile() const;
    wxString getSolverChartFile() const;
    Config* getConfig();
    void solve();
    void OnSolve(wxCommandEvent& event);
    bool createCharts(const char* filename);
    bool hasChart() const;
private:
    void OnClosePane(wxAuiManagerEvent& event);
    void OnOpenChart(wxCommandEvent& event);
    void OnShowChart(wxCommandEvent& event);
    void OnShowLog(wxCommandEvent& event);
    void OnPreferences(wxCommandEvent& event);
    void OnSolver(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSaveAsPNG(wxCommandEvent& event);
    void OnSaveLog(wxCommandEvent& event);
    Config _config;
    Input _input;
    wxMenu *_menuFile{nullptr};
    wxSizer *_chartSizer{nullptr};
    wxChartPanel *_chartPanel{nullptr};
    wxTextCtrl *_textSolution{nullptr};
    wxTextCtrl *_textLog{nullptr};
    wxAuiManager _aui;
};

class SolverDialog : public wxDialog {
public:
    SolverDialog(MainFrame *frame, wxTextCtrl* txt);
private:
    std::string getCommand() const;
    std::string getSolutionFile() const;
    std::string getChartFile() const;
    wxChoice* createChoice();
    wxSizer* createButtonSizer();
    wxFlexGridSizer* createOptionSizer(const wxString& name);
    void OnSolve(wxCommandEvent& event);
    void OnChooseSolver(wxCommandEvent& event);
    MainFrame *_parent;
    wxChoice *_choice{nullptr};
    wxSizer *_buttonSizer{nullptr};
    wxSizer *_optionSizer{nullptr};
    wxPanel *_panel{nullptr};
    std::unordered_map<std::string,wxTextCtrl*> _textCtrlMap;
};

class PreferencesDialog : public wxDialog {
public:
    PreferencesDialog(MainFrame *frame);
private:
    void OnSave(wxCommandEvent& event);
    MainFrame *_parent;
    wxCheckBox *_checkBox{nullptr};
};
 
enum {
    MENU_FILE_OPEN_CHART = 101,
    MENU_FILE_SHOW_LOG,
    MENU_FILE_SHOW_CHART,
    MENU_FILE_CHOOSE_SOLVER,
    MENU_FILE_SOLVE,
    MENU_FILE_SAVE_AS_PNG,
    MENU_FILE_SAVE_LOG,
    MENU_FILE_PREFERENCES,
    DIALOG_BUTTON_SOLVE,
    DIALOG_CHOICE_SOLVER,
    PREFERENCES_SAVE
};