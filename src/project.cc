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

#include "project.h"
#include "image_io.h"
#include <cassert>
#include <wx/wx.h>
#include <iostream>
#include "source_image_layer.h"
#include "curves_layer.h"
#include "conversion_layer.h"
#include "image_panel.h"
#include "str.h"
#include "histogram_panel.h"
#include "histogram_mode_panel.h"
#include "control_panel.h"
#include "view_mode_panel.h"
#include <sstream>
#include "layer_factory.h"
#include "convert_image.h"
#include "fractal.h"
#include "xml.h"
#include "image_area_panel.h"
#include "memory_info_frame.h"
#include <iostream>

deProject::deProject()
:viewModePanel(NULL),
 controlPanel(NULL),
 memoryInfoFrame(NULL),
 viewManager(*this),
 samplerManager(*this)
{
    sourceR = -1;
    sourceG = -1;
    sourceB = -1;
    imageFileName = "";
    sourceImageFileName = "";
    imagePanel = NULL;
    resetLayerStack();
    histogramPanel = NULL;

    receiveKeys = true;
}

void deProject::disableKeys()
{
    receiveKeys = false;
}

void deProject::enableKeys()
{
    receiveKeys = true;
}

deProject::~deProject()
{
    layerStack.clear();
}

void deProject::setHistogramChannel(int channel)
{
    histogramPanel->setChannel(channel);
    histogramPanel->generate();
    histogramPanel->paint();
    histogramModePanel->updateMode();
}

void deProject::onKey(int key)
{
    if (key == '`')
    {
        viewManager.setNormal();
    }
    if (key == '1')
    {
        viewManager.setSingleChannel(0);
    }
    if (key == '2')
    {
        viewManager.setSingleChannel(1);
    }
    if (key == '3')
    {
        viewManager.setSingleChannel(2);
    }
    if (key == '4')
    {
        viewManager.setSingleChannel(3);
    }
    if (key == WXK_F1)
    {
        setHistogramChannel(0);
    }
    if (key == WXK_F2)
    {
        setHistogramChannel(1);
    }
    if (key == WXK_F3)
    {
        setHistogramChannel(2);
    }
    if (key == WXK_F4)
    {
        setHistogramChannel(3);
    }

    layerStack.onKey(key);
    if (controlPanel)
    {
        controlPanel->onKey(key);
    }        
}

void deProject::init(const std::string& fileName)
{
    if (!openImage(fileName))
    {
        open(fileName, true);
    }

}

void deProject::freeImage()
{
    if (sourceR > 0)
    {
        sourceChannelManager.freeChannel(sourceR);
    }
    if (sourceG > 0)
    {
        sourceChannelManager.freeChannel(sourceG);
    }
    if (sourceB > 0)
    {
        sourceChannelManager.freeChannel(sourceB);
    }
}

void deProject::setTestImage(int s)
{
    freeImage();

    deSize size(s, s);

    sourceChannelManager.setChannelSize(size);
    sourceR = sourceChannelManager.allocateNewChannel();
    sourceG = sourceChannelManager.allocateNewChannel();
    sourceB = sourceChannelManager.allocateNewChannel();

    deChannel* channelR = sourceChannelManager.getChannel(sourceR);
    deChannel* channelG = sourceChannelManager.getChannel(sourceG);
    deChannel* channelB = sourceChannelManager.getChannel(sourceB);

    generateFractal(channelR->getPixels(), channelG->getPixels(), channelB->getPixels(), size);

    imageFileName = "delaboratory_test_image";

    setSource();
    imageAreaPanel->updateSize(true);
    layerStack.updateImages();
    repaintImage(true);
}

void deProject::setSource()
{
    deSourceImageLayer* l = dynamic_cast<deSourceImageLayer*>(layerStack.getLayer(0));

    l->setSource(sourceR, sourceG, sourceB, &sourceChannelManager);

    previewChannelManager.destroyAllChannels();
}

void deProject::resetLayerStack()
{
    layerStack.clear();

    deLayer* layer = createLayer("source_image", -1, deColorSpaceRGB, layerStack, previewChannelManager, viewManager, "source image");

    if (layer)
    {
        addLayer(layer);
    }        

    setSource();
    layerStack.updateImages();
}

void deProject::addLayer(deLayer* layer)
{
    layerStack.addLayer(layer);
    updateMemoryInfo();
}

deChannelManager& deProject::getPreviewChannelManager() 
{
    return previewChannelManager;
}

deChannelManager& deProject::getSourceChannelManager() 
{
    return sourceChannelManager;
}

deLayerStack& deProject::getLayerStack()
{
    return layerStack;
}

void deProject::setPreviewSize(const deSize& size, bool calcHistogram)
{
    deSize oldSize = previewChannelManager.getChannelSize();
    if (oldSize == size)
    {
        return;
    }

    previewChannelManager.setChannelSize(size);

    layerStack.updateImages();

    if (calcHistogram)
    {
        if (histogramPanel)
        {
            histogramPanel->generate();
            histogramPanel->paint();
        }
    }
}

void deProject::onChangeView(int a, int b)
{
    layerStack.updateImages(a + 1, b);
    if (controlPanel)
    {
        controlPanel->onChangeView();
    }
    if (viewModePanel)
    {
        viewModePanel->updateNames();
    }
    if (histogramModePanel)
    {
        histogramModePanel->updateNames();
    }
    updateSamplers();
    updateMemoryInfo();
}

const deViewManager& deProject::getViewManager() const
{
    return viewManager;
}

deViewManager& deProject::getViewManager() 
{
    return viewManager;
}

deSamplerManager& deProject::getSamplerManager() 
{
    return samplerManager;
}

void deProject::setImagePanel(deImagePanel* _imagePanel)
{
    imagePanel = _imagePanel;
}

void deProject::repaintImage(bool calcHistogram)
{
    if (imagePanel)
    {
        imagePanel->paint();
    }        
    if (calcHistogram)
    {
        if (histogramPanel)
        {
            histogramPanel->generate();
            histogramPanel->paint();
        }
    }
    updateSamplers();
    updateMemoryInfo();
}

void deProject::exportFinalImage(const std::string& app, const std::string& type, const std::string& name, wxProgressDialog* progressDialog)
{
    if ((name == "") && (imageFileName == ""))
    {
        return;
    }

    std::string path = "";

    if (app.size() > 0)
    {
        path = "/tmp/";
    }

    std::string fileName = name;
    if (fileName == "")
    {
        fileName = path + imageFileName + "." + type;
    }        

    deSize originalSize = previewChannelManager.getChannelSize();

    int view = viewManager.getView();

    viewManager.setScale(1.0);
    previewChannelManager.setChannelSize(sourceChannelManager.getChannelSize());
    layerStack.updateImagesSmart(previewChannelManager, view, progressDialog, memoryInfoFrame);

    deLayer* layer = layerStack.getLayer(view);
    const deImage& image = layer->getImage();

    if (image.getColorSpace() == deColorSpaceRGB)
    {
        deChannel* r = previewChannelManager.getChannel(image.getChannelIndex(0));
        deChannel* g = previewChannelManager.getChannel(image.getChannelIndex(1));
        deChannel* b = previewChannelManager.getChannel(image.getChannelIndex(2));

        if (type == "tiff")
        {
            saveTIFF(fileName, *r, *g, *b, previewChannelManager.getChannelSize());
        }            
        if (type == "jpeg")
        {
            saveJPEG(fileName, *r, *g, *b, previewChannelManager.getChannelSize());
        }            
    }
    else
    {
        deImage finalImage(deColorSpaceRGB, previewChannelManager);
        finalImage.enableAllChannels();
        convertImage(image, finalImage, previewChannelManager);
        deChannel* r = previewChannelManager.getChannel(finalImage.getChannelIndex(0));
        deChannel* g = previewChannelManager.getChannel(finalImage.getChannelIndex(1));
        deChannel* b = previewChannelManager.getChannel(finalImage.getChannelIndex(2));
        if (type == "tiff")
        {
            saveTIFF(fileName, *r, *g, *b, previewChannelManager.getChannelSize());
        }            
        if (type == "jpeg")
        {
            saveJPEG(fileName, *r, *g, *b, previewChannelManager.getChannelSize());
        }            
    }

    previewChannelManager.setChannelSize(originalSize);

    if (app.size() > 0)
    {
        const char* c = fileName.c_str();
        wxString s(c, wxConvUTF8);
        const wxString command = wxString::FromAscii(app.c_str()) + _T(" ") + s;
        wxExecute(command);
    }

    layerStack.updateImages();
    repaintImage(true);

}

void deProject::deleteLayer()
{
    if (layerStack.getSize() < 2)
    {
        return;
    }
    layerStack.removeTopLayer();
    int view = viewManager.getView();
    if (view >= layerStack.getSize())
    {
        viewManager.setView( layerStack.getSize() - 1 );
    }
    updateMemoryInfo();
}

void deProject::setLastView()
{
    int a = viewManager.getView();
    int n = layerStack.getSize();
    n--;
    viewManager.setView(n);
    onChangeView(a, n);
}

void deProject::setHistogramPanel(deHistogramPanel* _histogramPanel)
{
    histogramPanel = _histogramPanel;
}

void deProject::setViewModePanel(deViewModePanel* _viewModePanel)
{
    viewModePanel = _viewModePanel;
}

void deProject::setHistogramModePanel(deHistogramModePanel* _histogramModePanel)
{
    histogramModePanel = _histogramModePanel;
}


void deProject::setControlPanel(deControlPanel* _controlPanel)
{
    controlPanel = _controlPanel;
}

void deProject::onChangeViewMode()
{
    if (viewModePanel)
    {
        viewModePanel->updateMode();
    }
}

void deProject::updateSamplers()
{
    if (controlPanel)
    {
        controlPanel->updateSamplerManagerFrame();
    }
}

bool deProject::samplersVisible() const
{
    if (controlPanel)
    {
        return controlPanel->samplersVisible();
    }
    return false;
}

void deProject::save(const std::string& fileName, bool image)
{
    std::string f = fileName;

    size_t pos = f.rfind(".delab");
    if (pos != f.size() - 6) 
    {
        f += ".delab";
    }

    xmlDocPtr doc = xmlNewDoc(BAD_CAST("1.0"));

    xmlNodePtr root = xmlNewNode(NULL, BAD_CAST("project"));
    xmlDocSetRootElement(doc, root);

    {
        xmlNodePtr child = xmlNewChild(root, NULL, BAD_CAST("layer_stack"), NULL);
        layerStack.save(child);

        if (image)
        {
            saveChild(root, "source_image", sourceImageFileName);
        }
    }

    xmlSaveFormatFile (f.c_str(), doc, 1); 
}

void deProject::loadLayer(xmlNodePtr root)
{
    xmlNodePtr child = root->xmlChildrenNode;

    std::string type = "";
    std::string name = "";
    int source = -1;
    deColorSpace colorSpace = deColorSpaceInvalid;

    while (child)
    {
        if ((!xmlStrcmp(child->name, BAD_CAST("source")))) 
        {
            source = getInt(getContent(child));
        }

        if ((!xmlStrcmp(child->name, BAD_CAST("color_space")))) 
        {
            colorSpace = colorSpaceFromString(getContent(child));
        }

        if (type == "")
        // this is fix for multiple "type" node in single layer
        {
            if ((!xmlStrcmp(child->name, BAD_CAST("type")))) 
            {
                type = getContent(child);
            }
        }
        
        if ((!xmlStrcmp(child->name, BAD_CAST("name")))) 
        {
            name = getContent(child);
        }

        child = child->next;
    }
       
    deLayer* layer = createLayer(type, source, colorSpace, layerStack, previewChannelManager, viewManager, name);

    if (layer)
    {
        addLayer(layer);
        layer->load(root);
    }        


}

void deProject::loadLayers(xmlNodePtr root)
{
    layerStack.clear();

    xmlNodePtr child = root->xmlChildrenNode;

    while (child)
    {
        if ((!xmlStrcmp(child->name, BAD_CAST("layer")))) 
        {
            loadLayer(child);
        }

        child = child->next;
    }

    setSource();
    layerStack.updateImages();
}

void deProject::open(const std::string& fileName, bool image)
{
    xmlDocPtr doc = xmlParseFile(fileName.c_str());

    if (!doc)
    {
        return;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);

    if (!root)
    {
        return;
    }

    xmlNodePtr child = root->xmlChildrenNode;

    std::string imageFile = "";

    while (child)
    {
        if ((!xmlStrcmp(child->name, BAD_CAST("layer_stack")))) 
        {
            loadLayers(child);
        }

        if ((!xmlStrcmp(child->name, BAD_CAST("source_image")))) 
        {
            imageFile = getContent(child);
        }

        child = child->next;
    }

    if (image)
    {
        openImage(imageFile);
    }        

    setLastView();
    controlPanel->updateLayerGrid();
}

void deProject::newProject()
{
    resetLayerStack();
    setLastView();
    controlPanel->updateLayerGrid();
}

void deProject::showSamplers()
{
    if (controlPanel)
    {
        controlPanel->showSamplers();
    }
}

void deProject::setImageAreaPanel(deImageAreaPanel* _imageAreaPanel)
{
    imageAreaPanel = _imageAreaPanel;
}

deHistogramPanel* deProject::getHistogramPanel()
{
    return histogramPanel;
}

bool deProject::openImage(const std::string& fileName)
{
    freeImage();

    bool tiff = checkTIFF(fileName);
    bool jpeg = checkJPEG(fileName);

    if ((!tiff) && (!jpeg))
    {
        return false;
    }

    if ((tiff) && (jpeg))
    {
        assert(false);
    }

    deSize size(0,0);

    if (tiff)
    {
        size = getTIFFSize(fileName);
    }

    if (jpeg)
    {
        size = getJPEGSize(fileName);
    }

    sourceChannelManager.setChannelSize(size);
    sourceR = sourceChannelManager.allocateNewChannel();
    sourceG = sourceChannelManager.allocateNewChannel();
    sourceB = sourceChannelManager.allocateNewChannel();

    deChannel* channelR = sourceChannelManager.getChannel(sourceR);
    deChannel* channelG = sourceChannelManager.getChannel(sourceG);
    deChannel* channelB = sourceChannelManager.getChannel(sourceB);

    bool status = false;

    if (tiff)
    {
        bool icc;
        loadTIFF(fileName, *channelR, *channelG, *channelB, icc);
        if (icc)
        {
            wxMessageBox( _T("Warning! this TIFF file contains ICC profile which is ignored by delaboratory\n\ndelaboratory expects sRGB - colors may be not accurate\n\nThis problem happens (for instance) when tiff is created by dcraw\nyou can fix it by calling tifficc command, by default it converts tiff to sRGB"), _T("ICC profile ignored"), wxOK | wxICON_INFORMATION, NULL );
        }
        status = true;
    }

    if (jpeg)
    {
        loadJPEG(fileName, *channelR, *channelG, *channelB);
        status = true;
    }

    imageFileName = removePathAndExtension(fileName);
    sourceImageFileName = fileName;

    setSource();
    imageAreaPanel->updateSize(true);
    layerStack.updateImages();
    repaintImage(true);

    return status;
}

void deProject::openMemoryInfoFrame(wxWindow* parent)
{
    if (!memoryInfoFrame)
    {
        memoryInfoFrame = new deMemoryInfoFrame(parent, *this);
        memoryInfoFrame->Show();
    }
}

void deProject::closeMemoryInfoFrame()
{
    memoryInfoFrame = NULL;
}

void deProject::updateMemoryInfo()
{
    if (memoryInfoFrame)
    {
        memoryInfoFrame->update();
    }
}

void deProject::zoom(int a)
{
    deValue mul = 1.0;
    if (a > 0)
    {
        mul = 1.0 + 0.005 * a;
    }
    else
    {
        mul = 1.0 + 0.002 * a;
    }
    //deValue scale = viewManager.getScale() + 0.005 * a;
    deValue scale = viewManager.getScale() * mul;
    if (scale < 1.0)
    {
        scale = 1.0;
    }
    if (scale > 100.0)
    {
        scale = 100.0;
    }
    viewManager.setScale(scale);
    onScaleSet();
}    

void deProject::fullZoomOut()
{
    viewManager.setScale(1.0);
    onScaleSet();
}

void deProject::maxZoom()
{
    viewManager.setScale(10);
    onScaleSet();
}

void deProject::onScaleSet()
{
    previewChannelManager.destroyAllChannels();

    imageAreaPanel->updateSize(false);

    int view = viewManager.getView();
    layerStack.updateImages(0, view);
    repaintImage(false);
}

void deProject::setViewOffset(deValue x, deValue y)
{
    viewManager.setOffset(x, y);
    previewChannelManager.destroyAllChannels();

    int view = viewManager.getView();
    layerStack.updateImages(0, view);
    repaintImage(false);
}

