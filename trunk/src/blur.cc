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

#include "blur.h"
#include "preview.h"
#include "channel.h"

void boxBlur(deValue* source, deValue* destination, int n, int s)
{
    int i;
    for (i = 0; i < n; i++)
    {
        deValue result = 0.0;

        int n1 = i - s;
        int n2 = i + s;
        if (n1 < 0)
        {
            n1 = 0;
        }

        if (n2 >= n)
        {
            n2 = n - 1;
        }

        int j;
        for (j = n1; j <= n2; j++)
        {
            result += source[j];
        }
        destination[i] = result / (n2 - n1 + 1.0);
    }
}

void gaussianBlur(deValue* source, deValue* destination, int n, int s, deValue* weights)
{
    int i;
    for (i = 0; i < n; i++)
    {
        deValue result = 0.0;
        deValue sum = 0.0;
        
        int n1 = i - s + 1;
        int n2 = i + s - 1;
        if (n1 < 0)
        {
            n1 = 0;
        }

        if (n2 >= n)
        {
            n2 = n - 1;
        }

        int j;
        int p;

        j = n1;
        p = i - n1;
        while (p >= 0)
        {
            deValue v = source[j];
            deValue w = weights[p];
            result += w * v;
            sum += w;

            p--;
            j++;
        }
        p = 1;
        while (j <= n2)
        {

            deValue v = source[j];
            deValue w = weights[p];
            result += w * v;
            sum += w;

            p++;
            j++;
        }


        destination[i] = result / sum;
    }
}

void surfaceBlur(deValue* source, deValue* destination, int n, int s, deValue* weights, deValue t)
{
    deValue tt = 1.0 - t;

    int i;
    for (i = 0; i < n; i++)
    {
        deValue result = 0.0;
        deValue sum = 0.0;
        
        deValue reference = source[i];

        int n1 = i - s + 1;
        int n2 = i + s - 1;
        if (n1 < 0)
        {
            n1 = 0;
        }

        if (n2 >= n)
        {
            n2 = n - 1;
        }

        int j;
        int p;

        j = n1;
        p = i - n1;
        while (p >= 0)
        {
            deValue v = source[j];
            if (fabs(v - reference) <= tt)
            {
                deValue w = weights[p];
                result += w * v;
                sum += w;
            }                

            p--;
            j++;
        }
        p = 1;
        while (j <= n2)
        {

            deValue v = source[j];
            if (fabs(v - reference) <= tt)
            {
                deValue w = weights[p];
                result += w * v;
                sum += w;
            }                

            p++;
            j++;
        }


        destination[i] = result / sum;
    }
}

void fillWeightsFlat(deValue* weights, int blurSize)
{
    int i;
    for (i = 0 ; i < blurSize; i++)
    {
        weights[i] = 1.0;
    }
}    

void fillWeightsGaussian(deValue* weights, int blurSize)
{
    int i;
    deValue radius = blurSize / 3.0;
    deValue rr2 = 2.0 * radius * radius;
    for (i = 0 ; i < blurSize; i++)
    {
        deValue ii = i * i;
        deValue ee = exp( - ii / rr2 );
        weights[i] = 1.0 / sqrt(rr2 * M_PI) * ee;
    }
}    

void blurChannel(const deBaseChannel* source, deBaseChannel* destination, deBlurDirection direction, deValue radius, deBlurType type, deValue t)
{
    deChannel* d = dynamic_cast<deChannel*>(destination);
    if (!d)
    {
        return;
    }
    
    const deChannel* s = dynamic_cast<const deChannel*>(source);
    if (!s)
    {
        return;
    }

    const deSize& size = s->getSize();

    int w = size.getW();
    int h = size.getH();

    int n;

    if (direction == deBlurHorizontal)
    {
        n = w;
    }
    else
    {
        n = h;
    }

    int blurSize = n * radius;

    deValue* sourceBuffer = new deValue[n];
    deValue* destinationBuffer = new deValue[n];
    deValue* weights = new deValue[blurSize];

    int i;
    int j;
    const deValue* pixels = s->getPixels();

    if (type != deBoxBlur)
    {
        fillWeightsGaussian(weights, blurSize);
    }

    if (direction == deBlurHorizontal)
    {
        for (i = 0; i < h; i++)
        {
            int p = i * w;
            for (j = 0; j < n; j++)
            {
                sourceBuffer[j] = pixels[p + j]; 
            }
            switch (type)
            {
                case deBoxBlur:
                    boxBlur(sourceBuffer, destinationBuffer, n, blurSize);
                    break;
                case deGaussianBlur:
                    gaussianBlur(sourceBuffer, destinationBuffer, n, blurSize, weights);
                    break;
                case deSurfaceBlur:
                    surfaceBlur(sourceBuffer, destinationBuffer, n, blurSize, weights, t);
                    break;
            }
            for (j = 0; j < n; j++)
            {
                d->setValue(p + j, destinationBuffer[j]);
            }
        }
    }
    else
    {
        for (i = 0; i < w; i++)
        {
            for (j = 0; j < n; j++)
            {
                sourceBuffer[j] = pixels[j * w + i]; 
            }
            switch (type)
            {
                case deBoxBlur:
                    boxBlur(sourceBuffer, destinationBuffer, n, blurSize);
                    break;
                case deGaussianBlur:
                    gaussianBlur(sourceBuffer, destinationBuffer, n, blurSize, weights);
                    break;
                case deSurfaceBlur:
                    surfaceBlur(sourceBuffer, destinationBuffer, n, blurSize, weights, t);
                    break;
            }
            for (j = 0; j < n; j++)
            {
                d->setValue(j * w + i, destinationBuffer[j]);
            }
        }
    }        

    delete [] weights;
    delete [] destinationBuffer;
    delete [] sourceBuffer;

}

void blur(const dePreview& sourcePreview, dePreview& destinationPreview, deBlurDirection direction, deValue radius, const deChannels& enabledChannels, deBlurType type, deValue t)
{
    deColorSpace sc = sourcePreview.getColorSpace();
    deColorSpace dc = destinationPreview.getColorSpace();

    int sn = getColorSpaceSize(sc);
    int dn = getColorSpaceSize(dc);

    int n = sn;
    if (dn < n)
    {
        n = dn;
    }

    int i;
    for (i = 0; i < n; i++)
    {
        if (enabledChannels.count(i) == 1)
        {
            blurChannel(sourcePreview.getChannel(i), destinationPreview.getChannel(i), direction, radius, type, t);
        }
        else
        {
            destinationPreview.getChannel(i)->copy(sourcePreview.getChannel(i));
        }
    }

}