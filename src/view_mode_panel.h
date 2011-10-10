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

#ifndef _DE_VIEW_MODE_PANEL_H
#define _DE_VIEW_MODE_PANEL_H

#include <wx/wx.h>
#include <vector>
#include <map>
#include "color_space.h"
class deProject;

class deViewModePanel:public wxPanel
{
    private:
        std::vector<wxRadioButton*> buttons;
        deProject& project;
    /*
        deGUI& gui;
        wxSizer* sizer;
        std::map<int, int> channels;

        void addButton(const std::string& label, int c);

        */
        void select(wxCommandEvent &event);
    public:
        deViewModePanel(wxWindow* parent, deProject& _project);
        virtual ~deViewModePanel();

        void updateNames();
        void updateMode();

/*
        void updateButtons(deColorSpace colorSpace);

        void setButton(int b);*/

};

#endif
