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

#include "blend_channel.h"

void blendChannel(const deValue* sourcePixels, const deValue* overlayPixels, deValue* resultPixels, deValue* maskPixels, deBlendMode blendMode, deValue opacity, int channelSize)
{
    int j;
    if (maskPixels)
    {
        for (j = 0; j < channelSize; j++)
        {
            deValue src = sourcePixels[j];
            deValue ov = overlayPixels[j];
            deValue dst = calcBlendResult(src, ov, blendMode);
            deValue m = maskPixels[j] * opacity;
            deValue result = (1 - m) * src + m * dst;
            resultPixels[j] = result;
        }        
    }
    else
    {
        for (j = 0; j < channelSize; j++)
        {
            deValue src = sourcePixels[j];
            deValue ov = overlayPixels[j];
            deValue dst = calcBlendResult(src, ov, blendMode);
            deValue result = (1 - opacity) * src + opacity * dst;
            resultPixels[j] = result;
        }        
    }
}
