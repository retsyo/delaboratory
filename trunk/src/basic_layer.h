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

#ifndef _DE_BASIC_LAYER_H
#define _DE_BASIC_LAYER_H

#include "action_layer.h"
#include "property_value.h"
#include "curve.h"

class deBasicLayer:public deActionLayer
{
    private:
        std::vector<deBasicSetting> settings;
        std::vector<dePropertyValue*> basicProperties;
        deCurve* curves;
        int shiftIndex;
        deValue shiftValue;

        void updateCurve(int i);

    protected:
        virtual bool singleChannelProcessing() const {return true;};
        virtual std::string getType() const {return "basic";};
        virtual std::string getLabel() const {return "basic settings";};

    public:
        deBasicLayer(deColorSpace _colorSpace, int _index, int _sourceLayer, deLayerStack& _layerStack, deChannelManager& _channelManager, deViewManager& _viewManager, const std::string& _name);
        virtual ~deBasicLayer();

        virtual bool isChannelNeutral(int index);

        virtual bool processAction(int i, const deChannel& sourceChannel, deChannel& channel, deSize size);

        virtual void load(xmlNodePtr root);
        virtual void save(xmlNodePtr root);

        virtual std::string getActionName() {return "setup";};

        int getNumberOfSettings();
        dePropertyValue* getBasicProperty(int n);

        virtual bool randomize();


        virtual void onUpdateProperties();

        void reset();

};

#endif