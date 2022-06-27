#include "MainApp.h"
#include "MyBarRenderer.h"
#include "MyLegend.h"
#include "MyCategorySerie.h"

#include <wx/bars/barplot.h>
#include <wx/axis/numberaxis.h>
#include <wx/axis/categoryaxis.h>
#include <wx/xy/xyhistorenderer.h>
#include <wx/category/categorysimpledataset.h>

#include <wx/xml/xml.h>
#include <wx/aboutdlg.h>

#define USE_EXECUTE // define this to use wxExecute, otherwise just use system
#ifdef USE_EXECUTE
    #define EXEC(cmd) wxExecute((cmd), wxEXEC_SYNC)
#else
    #define EXEC(cmd) system(cmd)
#endif

static const char* CONFIG_FILE = "CONFIG";
static const char* SOLUTION = "Solution";
static const char* LOG = "Log";
static const char* CHART = "Chart";
static const wxString IMAGES_PATH = "images/"; // relative path
static const wxString VERSION = "1.0";

MainApp::MainApp() :
    wxApp() 
{
}

bool MainApp::OnInit()
{
    wxInitAllImageHandlers(); // necessary to save chart as png
    MainFrame *frame = new MainFrame();
    frame->SetSize(800,600);
    frame->Centre();
    frame->Show(true);
    return true;
}
 
wxIMPLEMENT_APP(MainApp);

MainFrame::MainFrame()
    : wxFrame{nullptr, wxID_ANY, "PSCMScheduler"},
    _config{CONFIG_FILE}
{
    // set the frame icon
    SetIcon(wxICON(UDE));

    // set aui widgets
    _aui.SetManagedWindow(this);
    _chartPanel = new wxChartPanel(this);
    wxFont tmpFont = wxFont(10, wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
    _textSolution = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL);
    _textSolution->SetFont(tmpFont);
    _aui.AddPane(_textSolution, wxAuiPaneInfo().Name(SOLUTION).Center().BestSize(400,300).Caption(SOLUTION).MaximizeButton(true).CloseButton(false));
    _textLog = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL);
    _textLog->SetFont(tmpFont);
    _aui.AddPane(_textLog, wxAuiPaneInfo().Name(LOG).Bottom().BestSize(400,300).Caption(LOG).MinimizeButton(true).MaximizeButton(true).CloseButton(true));
    _aui.AddPane(_chartPanel, wxAuiPaneInfo().Name(CHART).Bottom().BestSize(400,300).Caption(CHART).MinimizeButton(true).MaximizeButton(true).CloseButton(true));
    _aui.Update();
    
    // set textCtrl as active log target
    wxLog::SetActiveTarget(new wxLogTextCtrl(_textLog));

    // set menu file
    _menuFile = new wxMenu;
    wxMenuItem *menuOpenChart = new wxMenuItem(_menuFile, MENU_FILE_OPEN_CHART, "&Open Chart...\tCtrl-O", "Open chart from file");
    menuOpenChart->SetBitmap(wxBitmapBundle::FromSVGFile(IMAGES_PATH + "chart.svg", {16,16}));
    _menuFile->Append(menuOpenChart);
    _menuFile->AppendSeparator();
    wxMenuItem *menuItemSaveLog = new wxMenuItem(_menuFile, MENU_FILE_SAVE_LOG, "&Save Log...\tCtrl-S", "Save log to file");
    menuItemSaveLog->SetBitmap(wxBitmapBundle::FromSVGFile(IMAGES_PATH + "log.svg", {16,16}));
    _menuFile->Append(menuItemSaveLog);
    wxMenuItem *menuItemSave = new wxMenuItem(_menuFile, MENU_FILE_SAVE_AS_PNG, "&Save as PNG...\tCtrl-P", "Save current chart as PNG");
    menuItemSave->SetBitmap(wxBitmapBundle::FromSVGFile(IMAGES_PATH + "save.svg", {16,16}));
    _menuFile->Append(menuItemSave);
    _menuFile->AppendSeparator();
    wxMenuItem *menuItemPreferences = new wxMenuItem(_menuFile, MENU_FILE_PREFERENCES, "&Preferences...\tCtrl-+", "Open preferences");
    menuItemPreferences->SetBitmap(wxBitmapBundle::FromSVGFile(IMAGES_PATH + "preferences.svg", {16,16}));
    _menuFile->Append(menuItemPreferences);
    _menuFile->AppendCheckItem(MENU_FILE_SHOW_CHART, "Show Chart", "Hide or show chart window");
    _menuFile->Check(MENU_FILE_SHOW_CHART, true);
    _menuFile->AppendCheckItem(MENU_FILE_SHOW_LOG, "Show Log", "Hide or show log window");
    _menuFile->Check(MENU_FILE_SHOW_LOG, true);
    _menuFile->AppendSeparator();
    wxMenuItem *menuItemSolver = new wxMenuItem(_menuFile, MENU_FILE_CHOOSE_SOLVER, "&Choose Solver...\tCtrl-C", "Choose a solver and set command line options");
    menuItemSolver->SetBitmap(wxBitmapBundle::FromSVGFile(IMAGES_PATH + "solver.svg", {16,16}));
    _menuFile->Append(menuItemSolver);
    wxMenuItem *menuItemSolve = new wxMenuItem(_menuFile, MENU_FILE_SOLVE, "&Solve\tCtrl-L","Invoke solver");
    menuItemSolve->SetBitmap(wxBitmapBundle::FromSVGFile(IMAGES_PATH + "solve.svg", {16,16}));
    _menuFile->Append(menuItemSolve);
    _menuFile->AppendSeparator();
    wxMenuItem *menuItemExit = new wxMenuItem(_menuFile, wxID_EXIT, "&Quit\tCtrl-Q", "Quit this program");
    menuItemExit->SetBitmap(wxBitmapBundle::FromSVGFile(IMAGES_PATH + "quit.svg", {16,16}));
    _menuFile->Append(menuItemExit); 
   
    // set menu help
    wxMenu *menuHelp = new wxMenu;
    wxMenuItem *menuItemAbout = new wxMenuItem(menuHelp, wxID_ABOUT, "&About", "Show about dialog");
    menuItemAbout->SetBitmap(wxBitmapBundle::FromSVGFile(IMAGES_PATH + "about.svg", {16,16}));
    menuHelp->Append(menuItemAbout);
 
    // set menu bar
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(_menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar( menuBar );
    
    // create status bar
    CreateStatusBar();
    SetStatusText("PSCM Scheduling tool");
 
    // event handling
    _aui.Connect(wxEVT_AUI_PANE_CLOSE, (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(
        wxAuiManagerEventFunction, &MainFrame::OnClosePane), nullptr, this);

    Bind(wxEVT_MENU, &MainFrame::OnOpenChart, this, MENU_FILE_OPEN_CHART);
    Bind(wxEVT_MENU, &MainFrame::OnShowLog, this, MENU_FILE_SHOW_LOG);
    Bind(wxEVT_MENU, &MainFrame::OnShowChart, this, MENU_FILE_SHOW_CHART);
    Bind(wxEVT_MENU, &MainFrame::OnSolver, this, MENU_FILE_CHOOSE_SOLVER);
    Bind(wxEVT_MENU, &MainFrame::OnSolve, this, MENU_FILE_SOLVE);
    Bind(wxEVT_MENU, &MainFrame::OnPreferences, this, MENU_FILE_PREFERENCES);
    Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MainFrame::OnSaveAsPNG, this, MENU_FILE_SAVE_AS_PNG);
    Bind(wxEVT_MENU, &MainFrame::OnSaveLog, this, MENU_FILE_SAVE_LOG);
}

MainFrame::~MainFrame() 
{
    _aui.UnInit();
}

void MainFrame::setInput(Input input)
{
    _input = std::move(input);
}

wxString MainFrame::getSolverName() const
{
    return _input.name;
}

wxString MainFrame::getSolverCmd() const
{
    return _input.cmd;
}

wxString MainFrame::getSolverSolutionFile() const
{
    return _input.solutionFile;
}

wxString MainFrame::getSolverChartFile() const
{
    return _input.chartFile;
}

Config* MainFrame::getConfig()
{
    return &_config;
}

void MainFrame::OnClosePane(wxAuiManagerEvent& event)
{
    if (event.GetPane()->name == "Chart")
        _menuFile->Check(MENU_FILE_SHOW_CHART, false);
    if (event.GetPane()->name == "Log")
        _menuFile->Check(MENU_FILE_SHOW_LOG, false);
}

void MainFrame::OnOpenChart(wxCommandEvent& event)
{
    wxFileDialog dlg(this, "Open file...", wxEmptyString, wxEmptyString, "chart files (*.chart)|*.chart", wxFD_OPEN);
    if (dlg.ShowModal() != wxID_OK) 
        return;
    wxString filename = dlg.GetPath();
    setInput({"","","",filename});
    if (createCharts(filename))
        wxLogInfo("Opened chart \'%s\'", filename);
    else
        wxLogError("Could not open chart \'%s\'", filename);
}

void MainFrame::OnSaveAsPNG(wxCommandEvent& event)
{
    Chart *chart = _chartPanel->GetChart();
    if (chart) {
        wxFileDialog dlg(this, "Choose file...", wxEmptyString, wxEmptyString, "PNG files (*.png)|*.png", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
        if (dlg.ShowModal() != wxID_OK)
            return;
        wxString filename = dlg.GetPath();
        wxBitmap bitmap = _chartPanel->CopyBackbuffer();
        if (bitmap.ConvertToImage().SaveFile(filename, wxBITMAP_TYPE_PNG))
            wxLogInfo("Saved chart as \'%s\'", filename);
        else
            wxLogError("Could not save chart as \'%s\'", filename);
    }
    else {
        wxLogError("No chart is chosen");
    }
}

void MainFrame::OnSaveLog(wxCommandEvent& event)
{
    wxFileDialog dlg(this, "Choose file...", wxEmptyString, wxEmptyString, "txt files (*.txt)|*.txt", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() != wxID_OK)
        return;
    wxString filename = dlg.GetPath();
    if (_textLog->SaveFile(dlg.GetPath()))
        wxLogInfo("Saved log \'%s\'", filename);
    else 
        wxLogError("Could not save log \'%s\'", filename);
}

bool MainFrame::createCharts(const char* filename) 
{
    Schedule schedule(filename);

    if (schedule.isEmpty()) 
        return false;

    // Create dataset
    // CategorySimpleDataset *dataset = new CategorySimpleDataset(schedule.getPlainCategories(), schedule.getNbCategories());
    MyCategorySimpleDataset *dataset = new MyCategorySimpleDataset(schedule.getPlainCategories(), schedule.getNbCategories());

    // add series to it
    for (auto& serie : schedule.getSeries())
        //dataset->AddSerie(serie.info, serie.data.data(), serie.data.size());
        dataset->AddSerie(new MyCategorySerie(serie.label, serie.name, serie.data.data(), serie.data.size()));

    // Create stacked bar type
    BarType *barType = new MyStackedBarType(40, 0);

    // Set bar renderer for it, with stacked bar type
    auto black = std::remove_const_t<wxColour>(*wxBLACK);
    auto transparent = wxNullColour;
    BarRenderer *renderer = new MyBarRenderer(barType);
    int i = 0;
    for (auto& serie : schedule.getSeries()) {
        auto colour = wxColour(schedule.getColour(serie.name));
        if (serie.type == Serie::Type::Idle) 
            renderer->SetSerieColour(i,&transparent);
        else if (serie.type == Serie::Type::Setup) 
            renderer->SetSerieColour(i,&black);
        else 
            renderer->SetSerieColour(i,&colour);
        i++;
    }

    // assign renderer to dataset - necessary step
    dataset->SetRenderer(renderer);

    // Create bar plot
    BarPlot *plot = new BarPlot();

    // Add left category axis
    CategoryAxis *leftAxis = new CategoryAxis(AXIS_LEFT);
    leftAxis->SetMargins(20, 20);
    leftAxis->SetTitle(schedule.getYTitle());
    plot->AddAxis(leftAxis);

    // Add bottom number axis
    NumberAxis *bottomAxis = new NumberAxis(AXIS_BOTTOM);
    bottomAxis->SetMargins(0, 15);
    bottomAxis->SetTitle(schedule.getXTitle());
    plot->AddAxis(bottomAxis);

    // Add dataset to plot
    plot->AddDataset(dataset);

    // Link first dataset with first horizontal axis
    plot->LinkDataHorizontalAxis(0, 0);

    // Link first dataset with first vertical axis
    plot->LinkDataVerticalAxis(0, 0);

    if (_config.preferences.showLegend) 
        plot->SetLegend(new MyLegend(wxBOTTOM, wxCENTER));

    // and finally construct chart and set it into chart panel
    Footer* header = new Header();
    header->AddElement(TextElement(schedule.getName(), wxALIGN_CENTRE_HORIZONTAL, wxFontInfo(14)));
    header->AddElement(TextElement(schedule.getDescription(), wxALIGN_RIGHT));

    Chart *chart = new Chart(plot, header);

    // look at this code, we setup window, so
    // only part of data will be shown, not entire dataset as
    // in XYDemo1.
    // leftAxis->SetWindowPosition(0);
    // leftAxis->SetWindowWidth(1);
    // leftAxis->SetUseWindow(true);

    // bottomAxis->SetWindowPosition(0);
    // bottomAxis->SetWindowWidth(1000);
    // bottomAxis->SetUseWindow(true);
    
    // Scrolled
    // chart->SetScrolledAxis(leftAxis);
    // chart->SetScrolledAxis(bottomAxis);

    // auto labelCount = bottomAxis->GetLabelCount();
 
    _chartPanel->SetChart(chart);

    return true;
}

bool MainFrame::hasChart() const
{
    return (_chartPanel->GetChart());
}
 
void MainFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
 
void MainFrame::OnAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo about;
    about.SetName("PSCMScheduler");
    about.SetVersion(VERSION);
    about.SetDescription("PSCMScheduler is a tool for solving various scheduling problems in production and supply chain management.");
    about.SetCopyright("Copyright (C) 2022 Julian Luetkemeyer.");     
    about.AddDeveloper("Julian Luetkemeyer\nMail: julian.luetkemeyer@uni-due.de\nUniversity of Duisburg-Essen\nMercator School of Management\nChair for Production & Supply Chain Management\nLotharstr. 65 in 47057 Duisburg\nGermany.");
    wxAboutBox(about);
}

void MainFrame::OnShowChart(wxCommandEvent& event)
{
    wxAuiPaneInfo& info = _aui.GetPane(CHART);
    if (info.IsShown()) {
        if (!event.IsChecked()) {
            info.Show(false);
            _aui.Update();
        }
    }
    else {
        if ( event.IsChecked() ) {
            info.Show(true);
            _aui.Update();
        }
    }
}

void MainFrame::OnShowLog(wxCommandEvent& event)
{
    wxAuiPaneInfo& info = _aui.GetPane(LOG);
    if (info.IsShown()) {
        if (!event.IsChecked()) {
            info.Show(false);
            _aui.Update();
        }
    }
    else {
        if (event.IsChecked()) {
            info.Show(true);
            _aui.Update();
        }
    }
}

void MainFrame::OnPreferences(wxCommandEvent& event)
{
    PreferencesDialog dlg(this);
    dlg.ShowModal();
}

void MainFrame::OnSolver(wxCommandEvent& event)
{
    SolverDialog dlg(this, _textSolution);
    dlg.ShowModal();
}

void MainFrame::solve() 
{
    if (_input.name.empty()) {
        wxLogError("No solver is chosen",_input.name);
        return;
    }
    const Solver *solver = _config.findSolver(_input.name);
    if (!solver) {
        wxLogError("Solver \'%s\' could not be found",_input.name);
        return;
    }
    auto returned = EXEC(_input.cmd);
    auto pair = solver->getSuccessCode();
    if (pair.second && pair.first != returned)
        wxLogError("Command \'%s\' returned with %d", _input.cmd, returned);
    else {
        wxLogInfo("Command \'%s\' returned with %d", _input.cmd, returned);
        if (!pair.second)
            wxLogWarning("No succes exit code defined");
        if (_input.solutionFile.empty()) 
            wxLogWarning("No solution file defined");
        else 
            _textSolution->LoadFile(_input.solutionFile);
        if (_input.chartFile.empty())
            wxLogWarning("No chart file defined");
        else 
            createCharts(_input.chartFile.c_str());
    }
}

void MainFrame::OnSolve(wxCommandEvent& event) 
{
    solve();
}

SolverDialog::SolverDialog(MainFrame *frame, wxTextCtrl* txt) :
    wxDialog{frame, wxID_ANY, "Choose Solver", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER},
    _parent{frame}
{
    _choice = createChoice();
    _buttonSizer = createButtonSizer();
    wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *choiceSizer = new wxBoxSizer(wxHORIZONTAL);
    choiceSizer->Add(new wxStaticText(this, wxID_ANY, "Solver:"), 0, wxALIGN_CENTER | wxALL, 4);
    choiceSizer->Add(_choice, 0, wxALIGN_CENTER | wxALL, 4);
    topsizer->Add(choiceSizer, 0, wxALIGN_CENTER | wxALL, 4);
    topsizer->Add(_buttonSizer, 0, wxALIGN_CENTER | wxALL, 4);
    SetSizerAndFit(topsizer);

    Bind(wxEVT_BUTTON, &SolverDialog::OnSolve, this, DIALOG_BUTTON_SOLVE);
    Bind(wxEVT_CHOICE , &SolverDialog::OnChooseSolver, this, DIALOG_CHOICE_SOLVER);
}

std::string SolverDialog::getCommand() const
{
    wxString name = _choice->GetStringSelection();
    const Solver *solver = _parent->getConfig()->findSolver(name);
    if (!solver) {
        wxLogError("Solver \'%s\' could not be found", name);
        return std::string();
    }
    std::string cmd = solver->cmd;
    const auto& options = solver->options;
    for (const auto& option : options) {
        if (!option.cmd.empty())
            cmd += ' ' + option.cmd;
        auto it = _textCtrlMap.find(option.name);
        if (it != _textCtrlMap.cend()) {
            const wxString value = it->second->GetValue();
            if (!value.empty())
                cmd += ' ' + value;
        } 
        else
            if (!option.value.empty())
                cmd += ' ' + option.value;   
    }
    return cmd;
}

std::string SolverDialog::getSolutionFile() const
{
    auto value = _choice->GetStringSelection();
    const Solver *solver = _parent->getConfig()->findSolver(value);
    if (!solver) {
        wxLogError("Solver \'%s\' could not be found", value);
        return std::string();
    }
    if (solver->solution.empty()) 
        return std::string();
    const Option *option = solver->findOptionSolution();
    if (option) {
        auto it = _textCtrlMap.find(option->name);
        if (it != _textCtrlMap.cend()) 
            return it->second->GetValue().ToStdString();
        else 
            return option->value;
    }
    return solver->solution;
}

std::string SolverDialog::getChartFile() const
{
    auto value = _choice->GetStringSelection();
    const Solver *solver = _parent->getConfig()->findSolver(value);
    if (!solver) {
        wxLogError("Solver \'%s\' could not be found", value);
        return std::string();
    }
    if(solver->chart.empty()) 
        return std::string();
    const Option *option = solver->findOptionChart();
    if (option) {
        auto it = _textCtrlMap.find(option->name);
        if (it != _textCtrlMap.cend())
            return it->second->GetValue().ToStdString();
        else
            return option->value;
    }

    return solver->chart;
}

wxChoice* SolverDialog::createChoice()
{
    wxChoice *comboBox = new wxChoice(this, DIALOG_CHOICE_SOLVER);
    const auto& solvers = _parent->getConfig()->getSolvers();
    for (const auto& solver : solvers)
        comboBox->Append(solver.name);
    return comboBox;
}

wxSizer* SolverDialog::createButtonSizer()
{
    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(new wxButton(this, DIALOG_BUTTON_SOLVE, "Solve"), 0, wxALIGN_CENTER | wxALL, 4);
    sizer->Add(new wxButton(this, wxID_CANCEL, "Cancel"), 0, wxALIGN_CENTER | wxALL, 4);
    return CreateSeparatedSizer(sizer);
}

wxFlexGridSizer* SolverDialog::createOptionSizer(const wxString& name)
{
    const Solver *solver = _parent->getConfig()->findSolver(name);
    if (!solver) {
        wxLogError("Solver \'%s\' could not be found", name);
        return nullptr;
    }
    _textCtrlMap.clear();
    wxFlexGridSizer *gridSizer = new wxFlexGridSizer(2, 0, 3);
    const auto& options = solver->options;
    for (const auto& option : options) {
        if (!option.show) 
            continue;
        wxString value = option.value.empty() ? "" : option.value.c_str();
        wxTextCtrl *textCtrl = new wxTextCtrl(this, wxID_ANY, value);
        _textCtrlMap.emplace(option.name, textCtrl);
        textCtrl->SetMinSize(textCtrl->GetSizeFromTextSize(textCtrl->GetTextExtent(value.empty() ? std::string(40, ' ') : value)));
        wxStaticText *text = new wxStaticText(this, wxID_ANY, option.name);
        text->SetMinSize(textCtrl->GetSizeFromTextSize(textCtrl->GetTextExtent(option.name)));
        gridSizer->Add(text, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 4);
        gridSizer->Add(textCtrl, 0, wxEXPAND | wxALL, 4);
    }
    return gridSizer;
}

void SolverDialog::OnSolve(wxCommandEvent& event) 
{
    _parent->setInput({_choice->GetStringSelection(),
                       getCommand(),
                       getSolutionFile(),
                       getChartFile()});
    _parent->solve();
    Close(true);
}

void SolverDialog::OnChooseSolver(wxCommandEvent& event)
{
    const wxString name = _choice->GetStringSelection();
    GetSizer()->Detach(_buttonSizer);
    if (_optionSizer) {
        _optionSizer->DeleteWindows();
        GetSizer()->Remove(_optionSizer);
    }
    _optionSizer = createOptionSizer(name);
    if (_optionSizer)
        GetSizer()->Add(_optionSizer, 0, wxALIGN_CENTER | wxALL, 4);
    GetSizer()->Add(_buttonSizer, 0, wxALIGN_CENTER | wxALL, 4);
    Layout();
    GetSizer()->SetSizeHints(this);
}

PreferencesDialog::PreferencesDialog(MainFrame *frame) :
    wxDialog{frame, wxID_ANY, "Preferences", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER},
    _parent{frame} 
{
    _checkBox = new wxCheckBox(this, wxID_ANY, "Show Legend in Chart");
    _checkBox->SetValue(_parent->getConfig()->preferences.showLegend);
    wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(_checkBox, 0, wxALIGN_CENTER | wxALL, 10);
    topSizer->Add(new wxButton(this, PREFERENCES_SAVE, "Save"), 0, wxALIGN_CENTER | wxALL, 10);
    SetSizerAndFit(topSizer);

    Bind(wxEVT_BUTTON, &PreferencesDialog::OnSave, this, PREFERENCES_SAVE);
}

void PreferencesDialog::OnSave(wxCommandEvent& event) 
{
    bool showLegend = _checkBox->GetValue();
    if (showLegend != _parent->getConfig()->preferences.showLegend) {
        if(_parent->getConfig()->savePreferences({showLegend}))
            wxLogInfo("Saved preferences in \'%s\'", _parent->getConfig()->getFilename());
        else
            wxLogError("Could not save preferences in \'%s\'", _parent->getConfig()->getFilename());
        if (_parent->hasChart())
            _parent->createCharts(_parent->getSolverChartFile());
    }
    Close(true);
}