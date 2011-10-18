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

#include "image_panel.h"
#include <wx/dcbuffer.h>
#include "project.h"
#include "layer.h"

BEGIN_EVENT_TABLE(deImagePanel, wxPanel)
EVT_PAINT(deImagePanel::paintEvent)
END_EVENT_TABLE()

void deImagePanel::click(wxMouseEvent &event)
{
    int ex = event.GetX();
    int ey = event.GetY();
    int xx;
    int yy;
    GetSize(&xx, &yy);
    float x = (float) ex / xx;
    float y = (float) ey / yy;

    clicked = true;

    setPosition(x, y);
}

void deImagePanel::release(wxMouseEvent &event)
{
    int ex = event.GetX();
    int ey = event.GetY();
    int xx;
    int yy;
    GetSize(&xx, &yy);
    float x = (float) ex / xx;
    float y = (float) ey / yy;
    clicked = false;

}

void deImagePanel::move(wxMouseEvent &event)
{
    int ex = event.GetX();
    int ey = event.GetY();
    int xx;
    int yy;
    GetSize(&xx, &yy);
    float x = (float) ex / xx;
    float y = (float) ey / yy;

    setPosition(x, y);

}

void deImagePanel::setPosition(deValue x, deValue y)
{
    if (clicked)
    {
        deViewManager& viewManager = project->getViewManager();
        int view = viewManager.getView();
        deLayerStack& layerStack = project->getLayerStack();
        deLayer* layer = layerStack.getLayer(view);
        bool used = layer->onImageClick(x, y);

        if (!used)
        {
            project->showSamplers();
        }

        deSamplerManager& samplerManager = project->getSamplerManager();
        samplerManager.onImageClick(x, y);

    }
}

deImagePanel::deImagePanel(wxWindow* parent, deProject* _project)
:wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize), project(_project), renderer(project)
{
    project->setImagePanel(this);
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(deImagePanel::click));
    Connect(wxEVT_LEFT_UP, wxMouseEventHandler(deImagePanel::release));
    Connect(wxEVT_MOTION, wxMouseEventHandler(deImagePanel::move));
    clicked = false;
}

deImagePanel::~deImagePanel()
{
}

void deImagePanel::paintEvent(wxPaintEvent & evt)
{
    wxBufferedPaintDC dc(this);
    dc.Clear();
    render(dc);
}

void deImagePanel::paint()
{
    wxClientDC dc(this);
    wxBufferedDC bufferedDC(&dc);
    bufferedDC.Clear();
    render(bufferedDC);
}

void deImagePanel::render(wxDC& dc)
{
    renderer.render(dc);
    if (project->samplersVisible())
    {
        drawSamplers(dc);
    }        
}


void deImagePanel::drawSamplers(wxDC& dc)
{
    int w;
    int h;
    GetSize(&w, &h);

    deSamplerManager& samplerManager = project->getSamplerManager();
    int n = samplerManager.getNumberOfSamplers();
    int selected = samplerManager.getSelected();

    int i;

    wxPen penBLACK(*wxBLACK);
    wxPen penGREEN(*wxGREEN);

    for (i = 0; i < n; i++)
    {
        deSampler* sampler = samplerManager.getSampler(i);
        if (i == selected)
        {
            dc.SetPen(penGREEN);
        }
        else
        {
            dc.SetPen(penBLACK);
        }
        float x = sampler->getX();
        float y = sampler->getY();

        if ((x >= 0) && (y >= 0) && (x <= 1) && (y<= 1))
        {
            dc.DrawCircle(w * x, h * y, 5);
        }            
    }

}
