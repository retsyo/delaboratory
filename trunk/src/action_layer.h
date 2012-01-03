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

#ifndef _DE_ACTION_LAYER_H
#define _DE_ACTION_LAYER_H

#include "layer.h"
#include "blend_mode.h"
#include "size.h"
#include "image.h"
#include "channel.h"
class deLayerStack;
class deLayerProcessor;
class deChannelManager;
class deViewManager;
#include <set>

enum deApplyMode
{
    deApplyLuminance,
    deApplyColor,
    deApplyLuminanceAndColor
};

class deActionLayer:public deLayer
{
    private:

        // settings
        bool enabled;

        std::set<int> channels;

        deBlendMode blendMode;
        deValue opacity;

        deApplyMode applyMode;

        bool blendMask;
        bool blendMaskShow;

        int blendMaskLayer;
        int blendMaskChannel;

        deValue blendBlurRadius;
        deValue blendMaskMin;
        deValue blendMaskMax;

        virtual bool hasAction() const {return true;};
        virtual bool hasBlending() const {return true;};
        virtual bool canDisable() const {return true;};

        virtual bool simpleActionProcessing() const {return false;};

        virtual const deImage& getSourceImage() const;

        virtual bool isEnabled() const;
        virtual void setEnabled(bool e);

        virtual const deImage& getImage() const;

        virtual bool isChannelNeutral(int index) = 0;

        void updateAction(int i);

        void renderBlendMask();

        bool isBlendingEnabled() const;

        void updateActionAllChannels();
        void updateBlendAllChannels();

    protected:
        deLayerStack& layerStack;
        deLayerProcessor& layerProcessor;

    private:

        deChannelManager& channelManager;

    protected:
        deViewManager& viewManager;

        deImage imageActionPass;

        deImage imageBlendMask;

    private:        
        deImage imageApplyPass;

        deImage imageBlendPass;

        void updateBlend(int i);
        virtual void updateApply();

        virtual void processAction(int i) {};
        virtual void processAction(int i, const deChannel& sourceChannel, deChannel& channel, deSize size) {};
        virtual void processAction4(int i, const deChannel* s1, const deChannel* s2, const deChannel* s3, const deChannel* s4, deChannel& channel, int channelSize) {};

        virtual bool singleChannelProcessing() const = 0;

        virtual void updateImage();

    public:
        deActionLayer(const std::string& _name, deColorSpace _colorSpace, int _index, int _sourceLayer, deLayerStack& _layerStack, deLayerProcessor& _processor, deChannelManager& _channelManager, deViewManager& _viewManager);
        virtual ~deActionLayer();

        deChannel* getSourceChannel(int index);
        deSize getChannelSize() const;
        void setOpacity(deValue _alpha);
        void setBlendMode(deBlendMode mode);
        deBlendMode getBlendMode() const {return blendMode;};
        deApplyMode getApplyMode() const {return applyMode;};
        void setApplyMode(deApplyMode mode);
        deValue getOpacity();

        deColorSpace getBlendMaskLayerColorSpace() const;

        virtual void updateChannelUsage(std::map<int, int>& channelUsage) const;

        deViewManager& getViewManager() {return viewManager;};

        void showBlendMask();
        void hideBlendMask();
        void setBlendMask(int l, int c);
        void disableBlendMask();
        int getAllocatedBlendMaskChannel() const {return imageBlendMask.getChannelIndex(0);};

        void enableBlendMaskChannel();
        void disableBlendMaskChannel();

        deValue getBlendBlurRadius() const {return blendBlurRadius;};
        void setBlendBlurRadius(deValue r);
//        void onBlendSet();
        void tryRenderBlendMask();
        bool isBlendMaskEnabled() const {return blendMask;};
        bool isBlendMaskVisible() const {return blendMaskShow;};

        int getBlendMaskLayer() const {return blendMaskLayer;};
        int getBlendMaskChannel() const {return blendMaskChannel;};

        void setBlendMaskMin(deValue v);
        void setBlendMaskMax(deValue v);

        deValue getBlendMaskMin() const {return blendMaskMin;};
        deValue getBlendMaskMax() const {return blendMaskMax;};

        bool isChannelEnabled(int index) const;
        void enableChannel(int index);
        void disableChannel(int index);

        virtual void loadBlend(xmlNodePtr root);
        virtual void saveBlend(xmlNodePtr root);

        void updateImageInActionLayer(bool action, bool blend, int channel);
        void updateActionOnThread(int i);
        void updateBlendOnThread(int i);


};

#endif