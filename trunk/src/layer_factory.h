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

#ifndef _DE_LAYER_FACTORY_H
#define _DE_LAYER_FACTORY_H

class deLayer;
#include <string>
#include <vector>
#include "color_space.h"
class deLayerStack;
class deChannelManager;
class deViewManager;

deLayer* createLayer(const std::string& type, int source, deColorSpace colorSpace, deLayerStack& _layerStack, deChannelManager& _channelManager, deViewManager& _viewManager, const std::string& name, deChannelManager& _sourceChannelManager);
void getSupportedActions(std::vector<std::string>& actions);

#endif
