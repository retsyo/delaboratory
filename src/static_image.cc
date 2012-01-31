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

#include "static_image.h"
#include "channel.h"

deStaticImage::deStaticImage(const deColorSpace& _colorSpace)
:colorSpace(_colorSpace)
{
    int n = getColorSpaceSize(colorSpace);
    int i;
    for (i = 0; i < n; i++)
    {
        deChannel* c = new deChannel();
        channels.push_back(c);
    }
}

deStaticImage::~deStaticImage()
{
    while (!channels.empty())
    {
        std::vector<deChannel*>::iterator i = channels.end();
        i--;
        delete *i;
        channels.erase(i);
    }
}

deColorSpace deStaticImage::getColorSpace() const
{
    return colorSpace;
}
