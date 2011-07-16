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

#include "blur_layer.h"
#include "blur.h"
#include "preview_stack.h"
#include "preview.h"
#include "project.h"

deBlurLayer::deBlurLayer(deLayerStack& _stack, int _index, const std::string& _name)
:deLayer(_stack, _index, _name), radiusX(*this, "radius x", 0.0, 0.05), radiusY(*this, "radius y", 0.0, 0.05), channels(*this), iterations(*this, "iterations", 1, 10)
{
    radiusX.setValue(0.01);
    radiusY.setValue(0.01);
    iterations.setValue(1);
}

deBlurLayer::~deBlurLayer()
{
}

dePreview* deBlurLayer::createPreview(dePreviewStack& previewStack)
{
    const dePreview* sourcePreview = previewStack.getPreview(sourceLayer.getIndex());

    if (!sourcePreview)
    {
        return NULL;
    }

    const deSize& sourceSize = sourcePreview->getSize();

    dePreview* tmp = new dePreview(colorSpace.getColorSpace(), sourceSize);
    dePreview* preview = new dePreview(colorSpace.getColorSpace(), sourceSize);

    int i;
    int n = iterations.getValue();
    for (i = 0; i < n; i++)
    {
        const dePreview* src;
        if (i == 0)
        {
            src = sourcePreview;
        }
        else
        {
            src = preview;
        }

        blur(*src, *tmp, deBlurHorizontal, radiusX.getValue(), channels.getChannels());
        blur(*tmp, *preview, deBlurVertical, radiusY.getValue(), channels.getChannels());
    }

    delete tmp;

    return preview;
}

/*
deActionFrame* deBlurLayer::createActionFrame(wxWindow* parent, int layerNumber, deProject* project)
{
    //return new deBlurFrame(parent, *this, project->getPreviewStack(), layerNumber);
    return NULL;
}
*/

void deBlurLayer::onChangeColorSpace()
{
    channels.fill();
}

void deBlurLayer::enableChannel(int c)
{
    channels.insert(c);
}

void deBlurLayer::clearEnabledChannels()
{
    channels.clear();
}

const std::set<int>& deBlurLayer::getEnabledChannels() const
{
    return channels.getChannels();
}

void deBlurLayer::saveSpecific(xmlNodePtr node)
{
    radiusX.save(node, "radius_x");
    radiusY.save(node, "radius_y");
    channels.save(node, "channels");
    iterations.save(node, "iterations");
}

void deBlurLayer::loadSpecific(xmlNodePtr node)
{
    xmlNodePtr child = node->xmlChildrenNode;

    while (child)
    {
        if ((!xmlStrcmp(child->name, xmlCharStrdup("radius_x")))) 
        {
            radiusX.load(child);
        }

        if ((!xmlStrcmp(child->name, xmlCharStrdup("radius_y")))) 
        {
            radiusY.load(child);
        }

        if ((!xmlStrcmp(child->name, xmlCharStrdup("channels")))) 
        {
            channels.load(child);
        }

        if ((!xmlStrcmp(child->name, xmlCharStrdup("iterations")))) 
        {
            iterations.load(child);
        }

        child = child->next;
    }
}