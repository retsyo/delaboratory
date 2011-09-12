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

#ifndef _DE_SAMPLER_LIST_PANEL_H
#define _DE_SAMPLER_LIST_PANEL_H

#include <wx/wx.h>
#include <list>
class deProject;
class deSampler;
class deSamplerPanel;

class deSamplerListPanel:public wxPanel
{
    private:
        deProject* project;
        wxSizer* sizer;
        std::list<deSamplerPanel*> panels;

        void createList();
        void destroyList();
        void clear(wxCommandEvent &event);

    public:
        deSamplerListPanel(wxWindow* parent, deProject* _project);
        virtual ~deSamplerListPanel();

        void updateList();
        void updateSamplers();
};

#endif