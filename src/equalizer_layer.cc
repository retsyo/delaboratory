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

#include "equalizer_layer.h"
#include "project.h"
#include <iostream>
#include "frame_factory.h"
#include "equalizer.h"

deEqualizerLayer::deEqualizerLayer(deColorSpace _colorSpace, int _index, int _sourceLayer, deLayerStack& _layerStack, deChannelManager& _channelManager, deViewManager& _viewManager, const std::string& _name)
:deActionLayer(_name, _colorSpace, _index, _sourceLayer, _layerStack, _channelManager, _viewManager) 
{
    int n = getColorSpaceSize(colorSpace);
    int i;

    bands = 8;

    for (i = 0; i < n; i++)
    {
        equalizers.push_back( new deEqualizer(bands));
    }

}

deEqualizerLayer::~deEqualizerLayer()
{
    int n = getColorSpaceSize(colorSpace);
    int i;
    for (i = 0; i < n; i++)
    {
        delete equalizers[i];
    }
}

bool deEqualizerLayer::processAction4(int i, const deChannel* s1, const deChannel* s2, const deChannel* s3, const deChannel* s4, deChannel& channel, int channelSize)
{
    int e = getEqualizerChannel(colorSpace);

    const deChannel* eChannel = NULL;
    const deChannel* sChannel = NULL;

    switch (e)
    {
        case 0:
            eChannel = s1;
            break;
        case 1:
            eChannel = s2;
            break;
        case 2:
            eChannel = s3;
            break;
        case 3:
            eChannel = s4;
            break;
    }

    switch (i)
    {
        case 0:
            sChannel = s1;
            break;
        case 1:
            sChannel = s2;
            break;
        case 2:
            sChannel = s3;
            break;
        case 3:
            sChannel = s4;
            break;
    }

    bool wrap = isChannelWrapped(colorSpace, e);

    if ((eChannel) && (sChannel))
    {
        equalizers[i]->process(*sChannel, *eChannel, channel, channelSize, wrap);
    }

    return true;
}

bool deEqualizerLayer::isChannelNeutral(int index)
{
    return false;
}    


void deEqualizerLayer::save(xmlNodePtr root)
{
    saveCommon(root);
    saveBlend(root);

    int n = getColorSpaceSize(colorSpace);
    int i;
    for (i = 0; i < n; i++)
    {
        xmlNodePtr child = xmlNewChild(root, NULL, BAD_CAST("mixer"), NULL);
    }
}

void deEqualizerLayer::load(xmlNodePtr root)
{
    loadBlend(root);

    xmlNodePtr child = root->xmlChildrenNode;

    int i = 0;
    while (child)
    {

        child = child->next;
    }
}

bool deEqualizerLayer::randomize()
{
    return true;
}

deEqualizer* deEqualizerLayer::getEqualizer(int index)
{
    int n = getColorSpaceSize(colorSpace);
    if ((index >= 0) && ( index < n))
    {
        return equalizers[index];
    }
    return NULL;
}
