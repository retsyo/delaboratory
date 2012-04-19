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

#include "layer_frame.h"
#include "action_layer.h"

//deLayerFrame::deLayerFrame(wxWindow *parent, deLayer& _layer, const std::string& name, deLayerFrameManager& _frameManager, int _layerIndex)
deLayerFrame::deLayerFrame(wxWindow *parent, deBaseLayer& _layer, const std::string& name, deLayerFrameManager& _frameManager, int _layerIndex)
:deFrame(parent, name),
layer(_layer),
frameManager(_frameManager),
layerIndex(_layerIndex)
{
}

deLayerFrame::~deLayerFrame()
{
}

bool deLayerFrame::checkIndex(int index)
{
    if (layerIndex == index)
    {
        return true;
    }

    return false;
}
