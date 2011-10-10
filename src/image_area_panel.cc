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

#include "image_area_panel.h"
#include "size.h"
#include "project.h"
#include "image_panel.h"

void deImageAreaPanel::paint(wxPaintEvent& event)
{

}

void deImageAreaPanel::resize(wxSizeEvent& event)
{
    updateSize();
}

void deImageAreaPanel::updateSize()
{
    wxSize s = GetSize();

    const deSize ps(s.GetWidth(), s.GetHeight());
//    std::cout << "ps " << ps.str() << std::endl;
    
    const deSize& ss = project->getSourceChannelManager().getChannelSize();
//    std::cout << "ss " << ss.str() << std::endl;

    deSize fit = fitInside(ps, ss);
//    std::cout << "fit " << fit.str() << std::endl;

    project->setPreviewSize(fit);

    imagePanel->SetSize(wxSize(fit.getW(), fit.getH()));

    Refresh();
}


deImageAreaPanel::deImageAreaPanel(wxWindow* parent, deProject* _project)
:wxPanel(parent), project(_project)
{
/*
    project->getGUI().setCenterPanel(this);

    Connect(wxEVT_PAINT, wxPaintEventHandler(deImageAreaPanel::paint));

*/
    imagePanel = new deImagePanel(this, project);
//    imagePanel->SetBackgroundColour(*wxBLACK);

    Connect(wxEVT_SIZE, wxSizeEventHandler(deImageAreaPanel::resize));
}

deImageAreaPanel::~deImageAreaPanel()
{
}

