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

#include "blend_layer.h"
#include "blend.h"
#include "layer_stack.h"
#include "exception.h"
#include <iostream>
#include "preview_stack.h"
#include "preview.h"
#include "project.h"
#include <sstream>

deBlendLayer::deBlendLayer(deLayerStack& _stack, int _index, const std::string& _name)
:deLayer(_stack, _index, _name), alpha(*this, "alpha", 0.0, 1.0), blendMode(*this), channels(*this), overlayLayer(*this, "overlay")/*, blendMask(*this, "mask")*/
{
    alpha.setValue(0.5);
    blendMode.setBlendMode(deBlendNormal);
    overlayLayer.setIndex(-1);
}

deBlendLayer::~deBlendLayer()
{
}


void deBlendLayer::changeAlpha(deValue _alpha)
{
    alpha.setValue(_alpha);
}

void deBlendLayer::updatePreview(dePreviewStack& previewStack)
{
    const dePreview* sourcePreview = previewStack.getPreview(sourceLayer.getIndex());
    const dePreview* overlayPreview = previewStack.getPreview(overlayLayer.getIndex());
    dePreview* preview = previewStack.getPreview(index);

    if ((sourcePreview) && (preview) && (overlayPreview))
    {
        deBaseChannel* mask = NULL;

/*
        if (blendMask.isEnabled())
        {
            dePreview* maskPreview = previewStack.getPreview(blendMask.getLayerIndex());
            if (maskPreview)
            {
                mask = maskPreview->getChannel(blendMask.getChannel());
            }
        }        
        */

        blend(*sourcePreview, *overlayPreview, alpha.getValue(), *preview, channels.getChannels(), blendMode.getBlendMode(), mask);
    }
}

/*
void deBlendLayer::setOverlayChannel(int _channel)
{
    overlayChannel = _channel;
}

int deBlendLayer::getOverlayChannel() const
{
    return overlayChannel;
}

void deBlendLayer::setSingleOverlayChannel(bool _singleChannel)
{
    singleOverlayChannel = _singleChannel;
}

bool deBlendLayer::isSingleOverlayChannel() const
{
    return singleOverlayChannel;
}
*/

deBlendMode deBlendLayer::getBlendMode() const
{
    return blendMode.getBlendMode();
}

void deBlendLayer::setBlendMode(deBlendMode mode)
{
    blendMode.setBlendMode(mode);
}

void deBlendLayer::saveSpecific(xmlNodePtr node)
{
    alpha.save(node, "alpha");
    blendMode.save(node, "blend_mode");
    //blendMask.save(node, "blend_mask");
    channels.save(node, "channels");
    overlayLayer.save(node, "overlay_layer");
}

void deBlendLayer::loadSpecific(xmlNodePtr node)
{
    xmlNodePtr child = node->xmlChildrenNode;

    std::string overlayLayerString = "";

    while (child)
    {
        if ((!xmlStrcmp(child->name, xmlCharStrdup("alpha")))) 
        {
            alpha.load(child);
        }

        if ((!xmlStrcmp(child->name, xmlCharStrdup("blend_mode")))) 
        {
            blendMode.load(child);
        }

/*
        if ((!xmlStrcmp(child->name, xmlCharStrdup("blend_mask")))) 
        {
            blendMask.load(child);
        }
        */

        if ((!xmlStrcmp(child->name, xmlCharStrdup("channels")))) 
        {
            channels.load(child);
        }

        if ((!xmlStrcmp(child->name, xmlCharStrdup("overlay_layer")))) 
        {
            xmlChar* s = xmlNodeGetContent(child);            
            overlayLayerString = (char*)(s);
            xmlFree(s);
        }

        child = child->next;
    }

    {
        std::istringstream iss(overlayLayerString);
        int s;
        iss >> s;

        overlayLayer.setIndex(s);
    }        
}

void deBlendLayer::onChangeSourceLayer()
{
    if (overlayLayer.getIndex() < 0)
    {
        overlayLayer.setIndex(sourceLayer.getIndex());
    }
}

void deBlendLayer::onChangeColorSpace()
{
    channels.fill();
}

bool deBlendLayer::checkUsage(int id)
{
    if (sourceLayer.getIndex() == id)
    {
        return true;
    }
    if (overlayLayer.getIndex() == id)
    {
        return true;
    }
    return false;
}

