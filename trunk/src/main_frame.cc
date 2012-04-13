/*
    delaboratory - color correction utility
    Copyright (C) 2011 Jacek Poplawski

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main_frame.h"
#include "layer_grid_panel.h"
#include "control_panel.h"
#include "image_area_panel.h"
#include "histogram_panel.h"
#include "view_mode_panel.h"
#include "histogram_mode_panel.h"
#include "help_color_spaces_frame.h"
#include "help_color_spaces_frame2.h"
#include "help_color_spaces_frame3.h"
#include "help_color_spaces_frame4.h"
#include "help_color_spaces_frame5.h"
#include "help_color_spaces_frame6.h"
#include "benchmark_frame.h"
#include "color_matrix_frame.h"
#include "project.h"
#include "str.h"
#include "file_dialogs.h"
#include "delaboratory.h"
#include "samplers_panel.h"
#include "layer_processor.h"
#include "zoom_panel.h"
#include "threads_panel.h"

#include "image_panel.h"

#include "wx/thread.h"
#include "wx/notebook.h"


BEGIN_EVENT_TABLE(deMainFrame, wxFrame)
EVT_MENU(ID_Quit, deMainFrame::onQuit)
EVT_MENU(ID_NewProject, deMainFrame::onNewProject)
EVT_MENU(ID_TestImageSmall, deMainFrame::onTestImageSmall)
EVT_MENU(ID_TestImageBig, deMainFrame::onTestImageBig)
EVT_MENU(ID_OpenImage, deMainFrame::onOpenImage)
EVT_MENU(ID_OpenRawImageProPhoto, deMainFrame::onOpenRawImageProPhoto)
EVT_MENU(ID_OpenProject, deMainFrame::onOpenProject)
EVT_MENU(ID_SaveProject, deMainFrame::onSaveProject)
EVT_MENU(ID_OpenLayerStack, deMainFrame::onOpenLayerStack)
EVT_MENU(ID_SaveLayerStack, deMainFrame::onSaveLayerStack)
EVT_MENU(ID_HelpColorSpaces, deMainFrame::onHelpColorSpaces)
EVT_MENU(ID_HelpColorSpaces2, deMainFrame::onHelpColorSpaces2)
EVT_MENU(ID_HelpColorSpaces3, deMainFrame::onHelpColorSpaces3)
EVT_MENU(ID_HelpColorSpaces4, deMainFrame::onHelpColorSpaces4)
EVT_MENU(ID_HelpColorSpaces5, deMainFrame::onHelpColorSpaces5)
EVT_MENU(ID_LABColors1, deMainFrame::onLABColors1)
EVT_MENU(ID_LABColors2, deMainFrame::onLABColors2)
EVT_MENU(ID_LABColors5, deMainFrame::onLABColors5)
EVT_MENU(ID_MemoryInfo, deMainFrame::onMemoryInfo)
EVT_MENU(ID_ColorMatrix1, deMainFrame::onColorMatrix1)
EVT_MENU(ID_ColorMatrix2, deMainFrame::onColorMatrix2)
EVT_MENU(ID_ColorMatrix3, deMainFrame::onColorMatrix3)
EVT_MENU(ID_ColorMatrix4, deMainFrame::onColorMatrix4)
EVT_MENU(ID_ColorMatrix5, deMainFrame::onColorMatrix5)
EVT_MENU(ID_BenchmarkBlur, deMainFrame::onBenchmarkBlur)
EVT_MENU(ID_BenchmarkColor, deMainFrame::onBenchmarkColor)
EVT_MENU(DE_REPAINT_EVENT, deMainFrame::onRepaintEvent)
EVT_MENU(DE_IMAGE_LOAD_EVENT, deMainFrame::onImageLoadEvent)
EVT_MENU(DE_HISTOGRAM_EVENT, deMainFrame::onHistogramEvent)
EVT_MENU(DE_INFO_EVENT, deMainFrame::onInfoEvent)
END_EVENT_TABLE()

deMainFrame::deMainFrame(const wxSize& size, deProject& _project, deLayerProcessor& _layerProcessor, deSamplerManager& _samplerManager, deZoomManager& _zoomManager, const std::string& dcraw_version)
: wxFrame() , project(_project), layerProcessor(_layerProcessor), imageSize(0,0)
{
    project.setMainFrame(this);

    project.log("creating main frame");

    imageName = "";

#ifdef __WXOSX_MAC__
    Create((wxFrame *)NULL, wxID_ANY, _T("main frame"), wxDefaultPosition, wxSize(1440, 800), wxDEFAULT_FRAME_STYLE);
#else
    Create((wxFrame *)NULL, wxID_ANY, _T("main frame"), wxDefaultPosition, size, wxDEFAULT_FRAME_STYLE | wxMAXIMIZE);
#endif    

    updateTitle();

    mainSizer = new wxBoxSizer(wxHORIZONTAL);

    wxSizer* leftSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(leftSizer, 1, wxEXPAND);

    topPanel = new wxPanel(this);
    leftSizer->Add(topPanel, 0, wxEXPAND);

    wxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    topPanel->SetSizer(topSizer);

    wxPanel* viewModePanel = new deViewModePanel(topPanel, project);
    topSizer->Add(viewModePanel);

    threadsPanel = new deThreadsPanel(topPanel);
    topSizer->Add(threadsPanel, 0, wxEXPAND);

    wxSizer* sizerR = new wxStaticBoxSizer(wxHORIZONTAL, topPanel,  _T("options"));
    topSizer->Add(sizerR, 0, wxEXPAND);

    realtime = new wxCheckBox(topPanel, wxID_ANY, _T("real time"));
    sizerR->Add(realtime);

    autoUI = new wxCheckBox(topPanel, wxID_ANY, _T("auto UI"));
    sizerR->Add(autoUI);

    bool r = layerProcessor.isRealtime();
    if (r)
    {
        realtime->SetValue(1);
    }
    else
    {
        realtime->SetValue(0);
    }

    deZoomPanel* zoomPanel = new deZoomPanel(topPanel, _zoomManager);
    topSizer->Add(zoomPanel);

    wxSizer* sizerI = new wxStaticBoxSizer(wxVERTICAL, this,  _T("image"));
    leftSizer->Add(sizerI, 1, wxEXPAND);

    imageAreaPanel = new deImageAreaPanel(this, project, _samplerManager, _zoomManager, zoomPanel);
    sizerI->Add(imageAreaPanel, 1, wxEXPAND);

    wxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
    hPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(280, 250));
    wxSizer* sizerH = new wxStaticBoxSizer(wxVERTICAL, hPanel,  _T("histogram"));
    hPanel->SetSizer(sizerH);

    histogramPanel = new deHistogramPanel(hPanel, &project);
    sizerH->Add(histogramPanel, 0, wxCENTER);

    deHistogramModePanel* histogramModePanel = new deHistogramModePanel(hPanel, project, histogramPanel);
    sizerH->Add(histogramModePanel, 0, wxLEFT);

    rightSizer->Add(hPanel, 0, wxEXPAND);

    wxNotebook* notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, _T("notebook"));
    rightSizer->Add(notebook, 1, wxEXPAND);

    layerGridPanel = new deLayerGridPanel(notebook, project, _layerProcessor);

    notebook->AddPage(layerGridPanel, _T("layers"));
    samplersPanel = new deSamplersPanel(notebook, project, _samplerManager);
    notebook->AddPage(samplersPanel, _T("samplers"));

    controlPanel = new deControlPanel(this, project, _layerProcessor, layerGridPanel);
    rightSizer->Add(controlPanel, 0, wxEXPAND);

    bool a = controlPanel->getAutoUI();
    if (a)
    {
        autoUI->SetValue(1);
    }
    else
    {
        autoUI->SetValue(0);
    }

    mainSizer->Add(rightSizer, 0, wxEXPAND);

    SetSizer(mainSizer);

    full = false;

    wxMenu *menuFile = new wxMenu;
    menuFile->Append( ID_NewProject, _("New project") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_OpenRawImageProPhoto, _("Open RAW image as ProPhoto RGB") );
    menuFile->Append( ID_OpenImage, _("Open TIFF/JPG image") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_TestImageSmall, _("Generate test image (small)") );
    menuFile->Append( ID_TestImageBig, _("Generate test image (big, slow)") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_OpenLayerStack, _("Open layer stack") );
    menuFile->Append( ID_SaveLayerStack, _("Save layer stack") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_OpenProject, _("Open project ( stack + image )") );
    menuFile->Append( ID_SaveProject, _("Save project ( stack + image )") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_Quit, _("E&xit") );

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append( ID_HelpColorSpaces, _("channels") );
    menuHelp->Append( ID_HelpColorSpaces2, _("mix of channels") );
    menuHelp->Append( ID_HelpColorSpaces3, _("lightness/value in LAB/LCH/HSL/HSV") );
    menuHelp->Append( ID_HelpColorSpaces4, _("hue in LCH/HSL/HSV") );
    menuHelp->Append( ID_HelpColorSpaces5, _("CMYK skin colors") );
    menuHelp->Append( ID_LABColors1, _("LAB colors 1") );
    menuHelp->Append( ID_LABColors2, _("LAB colors 2") );
    menuHelp->Append( ID_LABColors5, _("LAB colors 5") );

    wxMenu *menuInfo = new wxMenu;
    menuInfo->Append( ID_MemoryInfo, _("memory info") );
    menuInfo->AppendSeparator();
    menuInfo->Append( ID_BenchmarkColor, _("benchmark color conversion") );
    menuInfo->Append( ID_BenchmarkBlur, _("benchmark blur (slow!)") );

    wxMenu *menuPalette = new wxMenu;
    menuPalette->Append( ID_ColorMatrix1, _("LAB color matrix 40x40") );
    menuPalette->Append( ID_ColorMatrix2, _("LAB color matrix 20x20") );
    menuPalette->Append( ID_ColorMatrix3, _("color tiles 20") );
    menuPalette->Append( ID_ColorMatrix4, _("color tiles 40") );
    menuPalette->Append( ID_ColorMatrix5, _("color tiles 80") );

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, _("&File") );
    menuBar->Append( menuInfo, _("&Info") );
    menuBar->Append( menuPalette, _("&Palette") );
    menuBar->Append( menuHelp, _("&Channels") );

    SetMenuBar( menuBar );

    Layout();

    imageAreaPanel->SetFocus();
    Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(deMainFrame::check));

    Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(deMainFrame::onCloseEvent));

    Connect(wxEVT_TIMER, wxTimerEventHandler(deMainFrame::onTimerEvent));

    project.log("main frame created");

    threadsPanel->setInfoColor(DE_DCRAW_END);
}

#define TILE1 20
#define TILE2 40
#define TILE3 60
#define TILER1 30
#define TILER2 50
#define TILER3 80
#define PALRW 60
#define PALRH 40
#define PAL1 16
#define PAL2 12
#define PAL3 10
#define MAR1 0.03
#define MAR2 0.06


void deMainFrame::hidePanels()
{
    topPanel->Hide();
    hPanel->Hide();
    layerGridPanel->Hide();
    controlPanel->Hide();
    mainSizer->Layout();
    full = true;
    imageAreaPanel->SetFocus();
}

deMainFrame::~deMainFrame()
{
    project.log("closing main frame");
    layerProcessor.stopWorkerThread();
    project.log("closing main frame lock layerProcessor...");
    layerProcessor.lock();
    project.log("closing main frame unlock layerProcessor...");
    layerProcessor.unlock();
    project.log("closing main frame done");
}

void deMainFrame::showPanels()
{
    topPanel->Show();
    hPanel->Show();
    layerGridPanel->Show();
    controlPanel->Show();
    mainSizer->Layout();
    full = false;
    imageAreaPanel->SetFocus();
}

void deMainFrame::rebuild()
{
    topPanel->Layout();
    topPanel->Update();
    topPanel->Refresh();
}

void deMainFrame::onKey(int key)
{
    if (key == 'F')
    {
        if (full)
        {
           showPanels();
        }        
        else
        {
           hidePanels();
        }        
    }
}

void deMainFrame::onQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(TRUE);
}

void deMainFrame::onSaveProject(wxCommandEvent& WXUNUSED(event))
{
    std::string fileName = getSaveFile(this, "save project", "delab");
    project.save(fileName, true);
}

void deMainFrame::onSaveLayerStack(wxCommandEvent& WXUNUSED(event))
{
    std::string fileName = getSaveFile(this, "save layer stack", "delab");
    project.save(fileName, false);
}

void deMainFrame::onOpenProject(wxCommandEvent& WXUNUSED(event))
{
    std::string fileName = getOpenFile(this, "open project", "delab");
    project.open(fileName, true);
}

void deMainFrame::onOpenLayerStack(wxCommandEvent& WXUNUSED(event))
{
    std::string fileName = getOpenFile(this, "open layer stack", "delab");
    project.open(fileName, false);
}

void deMainFrame::onNewProject(wxCommandEvent& WXUNUSED(event))
{
    project.newProject();
}

void deMainFrame::onTestImageSmall(wxCommandEvent& WXUNUSED(event))
{
    project.setTestImage(900);
}

void deMainFrame::onTestImageBig(wxCommandEvent& WXUNUSED(event))
{
    project.setTestImage(1800);
}

void deMainFrame::onOpenImage(wxCommandEvent& WXUNUSED(event))
{
    std::string fileName = getOpenFile(this, "load source image", "image");
    project.openImage(fileName, false, deColorSpaceRGB);
}

void deMainFrame::onOpenRawImageProPhoto(wxCommandEvent& WXUNUSED(event))
{
    std::string fileName = getOpenFile(this, "load source image", "raw");
    project.openImage(fileName, true, deColorSpaceProPhoto);
}

void deMainFrame::onHelpColorSpaces(wxCommandEvent& event)
{
    wxFrame* help = new deHelpColorSpacesFrame(this);
    help->Show();
}

void deMainFrame::onHelpColorSpaces2(wxCommandEvent& event)
{
    wxFrame* help = new deHelpColorSpacesFrame2(this);
    help->Show();
}

void deMainFrame::onHelpColorSpaces3(wxCommandEvent& event)
{
    wxFrame* help = new deHelpColorSpacesFrame3(this);
    help->Show();
}

void deMainFrame::onHelpColorSpaces4(wxCommandEvent& event)
{
    wxFrame* help = new deHelpColorSpacesFrame4(this);
    help->Show();
}

void deMainFrame::onHelpColorSpaces5(wxCommandEvent& event)
{
    wxFrame* help = new deHelpColorSpacesFrame5(this);
    help->Show();
}

void deMainFrame::onLABColors1(wxCommandEvent& event)
{
    wxFrame* help = new deHelpColorSpacesFrame6(this, 1);
    help->Show();
}

void deMainFrame::onLABColors2(wxCommandEvent& event)
{
    wxFrame* help = new deHelpColorSpacesFrame6(this, 2);
    help->Show();
}

void deMainFrame::onLABColors5(wxCommandEvent& event)
{
    wxFrame* help = new deHelpColorSpacesFrame6(this, 5);
    help->Show();
}

void deMainFrame::onMemoryInfo(wxCommandEvent& event)
{
    project.openMemoryInfoFrame(this);
}

void deMainFrame::onColorMatrix1(wxCommandEvent& event)
{
    dePalette3* palette = NULL;
    wxFrame* help = new deColorMatrixFrame2(this, project, 1, 2, 0, 32, 32, palette);
    help->Show();
}

void deMainFrame::onColorMatrix2(wxCommandEvent& event)
{
    dePalette3* palette = NULL;
    wxFrame* help = new deColorMatrixFrame2(this, project, 1, 2, 0, 16, 16, palette);
    help->Show();
}

void deMainFrame::onColorMatrix3(wxCommandEvent& event)
{
    wxFrame* help = new deColorMatrixFrame(this, project, 20, 20, 20, 20, 40, 40, 12, 0.1);
    help->Show();
}

void deMainFrame::onColorMatrix4(wxCommandEvent& event)
{
    wxFrame* help = new deColorMatrixFrame(this, project, 40, 40, 40, 40, 40, 40, 12, 0.1);
    help->Show();
}

void deMainFrame::onColorMatrix5(wxCommandEvent& event)
{
    wxFrame* help = new deColorMatrixFrame(this, project, 80, 80, 80, 80, 40, 40, 12, 0.1);
    help->Show();
}


void deMainFrame::onBenchmarkBlur(wxCommandEvent& event)
{
    deBenchmarkFrame* help = new deBenchmarkFrame(this, "blur");
    help->Show();
    help->perform();
}

void deMainFrame::onBenchmarkColor(wxCommandEvent& event)
{
    deBenchmarkFrame* help = new deBenchmarkFrame(this, "color");
    help->Show();
    help->perform();
}

void deMainFrame::onRepaintEvent(wxCommandEvent& event)
{
    repaintMainFrame(true);
}

void deMainFrame::onImageLoadEvent(wxCommandEvent& event)
{
    layerProcessor.onImageLoad();
}

void deMainFrame::onInfoEvent(wxCommandEvent& event)
{
    int i = event.GetInt();
    threadsPanel->setInfoColor(i);
}

void deMainFrame::repaintMainFrame(bool calcHistogram)
{
    project.log("repaint main frame start");
    if (!project.isSourceValid())
    {
        std::cout << "source not valid" << std::endl;
        return;
    }

    project.log("repaint main frame 1");
    imageAreaPanel->getImagePanel()->repaintImagePanel();
    project.log("repaint main frame 2");
    project.log("repaint main frame 3");
    samplersPanel->update();
    project.log("repaint main frame 4");
    project.updateMemoryInfo();
    project.log("repaint main frame end");

}

void deMainFrame::check(wxCommandEvent &event)
{
    bool r = realtime->IsChecked();
    layerProcessor.setRealtime(r);

    bool aui = autoUI->IsChecked();
    controlPanel->setAutoUI(aui);
}

void deMainFrame::onCloseEvent(wxCloseEvent& event)
{
    project.log("deMainFrame::onCloseEvent");
    this->Destroy();
}

void deMainFrame::generateHistogram()
{
    if (histogramPanel)
    {
        histogramPanel->generateHistogram();
    }
}

void deMainFrame::onHistogramEvent(wxCommandEvent& event)
{
    if (histogramPanel)
    {
        histogramPanel->paintHistogram();
    }
}

void deMainFrame::updateTitle()
{
    std::string s = imageName + " " + str(imageSize.getW()) + "x" + str(imageSize.getH()) + " - " + getApplicationName() + " " + getVersion() + " " + getCopyright();

    SetTitle(wxString::FromAscii(s.c_str()));
}

void deMainFrame::setImageName(const std::string& _imageName, const deSize& _size)
{
    imageName = _imageName;
    imageSize = _size;
    updateTitle();
}    
    
void deMainFrame::onTimerEvent(wxTimerEvent& event)
{
    project.onTimerUpdate();
}

