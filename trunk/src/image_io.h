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

#ifndef _DE_IMAGE_IO_H
#define _DE_IMAGE_IO_H

#include <string>
#include "size.h"
#include "channel.h"

bool checkTIFF(const std::string& fileName);
bool checkJPEG(const std::string& fileName);
deSize getTIFFSize(const std::string& fileName);
deSize getJPEGSize( const std::string& fileName);
void loadTIFF(const std::string& fileName, deBaseChannel& channelR, deBaseChannel& channelG, deBaseChannel& channelB);
void loadJPEG(const std::string& fileName, deBaseChannel& channelR, deBaseChannel& channelG, deBaseChannel& channelB);
void saveJPEG(const std::string& fileName, const deBaseChannel& channelR, const deBaseChannel& channelG, const deBaseChannel& channelB, deSize& size);
void saveTIFF(const std::string& fileName, const deBaseChannel& channelR, const deBaseChannel& channelG, const deBaseChannel& channelB, deSize& size);

#endif