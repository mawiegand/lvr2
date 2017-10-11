/* Copyright (C) 2011 Uni Osnabrück
 * This file is part of the LAS VEGAS Reconstruction Toolkit,
 *
 * LAS VEGAS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * LAS VEGAS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 */

/*
 * ColorAlgorithms.tcc
 *
 * @date 21.07.2017
 * @author Johan M. von Behren <johan@vonbehren.eu>
 */

#include <algorithm>
#include <array>
#include <cmath>

using std::array;

#include <lvr2/geometry/Point.hpp>

namespace lvr2
{

template <typename BaseVecT>
optional<DenseVertexMap<Rgb8Color>> calcColorFromPointCloud(
    const BaseMesh<BaseVecT>& mesh,
    const PointsetSurfacePtr<BaseVecT> surface
)
{
    if (!surface->pointBuffer()->hasRgbColor())
    {
        // cout << "none" << endl;
        return boost::none;
    }

    DenseVertexMap<Rgb8Color> vertexMap;
    vertexMap.reserve(mesh.numVertices());

    // k-nearest-neighbors
    const int k = 1;

    vector<size_t> cv;
    for (auto vertexH: mesh.vertices())
    {
        cv.clear();
        auto p = mesh.getVertexPosition(vertexH);
        surface->searchTree().kSearch(p, k, cv);

        float r = 0.0f, g = 0.0f, b = 0.0f;

        for (size_t pointIdx : cv)
        {
            auto colors = *(surface->pointBuffer()->getRgbColor(pointIdx));
            r += colors[0];
            g += colors[1];
            b += colors[2];
        }

        r /= k;
        g /= k;
        b /= k;

        vertexMap.insert(vertexH, {
            static_cast<uint8_t>(r),
            static_cast<uint8_t>(g),
            static_cast<uint8_t>(b)
        });
    }

    return vertexMap;
}

static Rgb8Color floatToRainbowColor(float value)
{
    value = std::min(value, 1.0f);
    value = std::max(value, 0.0f);

    //TODO: understand and fix style
    float h = value * 5.0f + 1.0f;
    int i = floor(h);
    float f = h - i;

    // if i is even
    if (i % 2 == 0)
    {
        f = 1 - f;
    }

    float n = 1 - f;

    if (i <= 1)
    {
        return { static_cast<uint8_t>(floor(n * 255)), 0, 255 };
    }
    else if (i == 2)
    {
        return { 0, static_cast<uint8_t>(floor(n * 255)), 255 };
    }
    else if (i == 3)
    {
        return { 0, 255, static_cast<uint8_t>(floor(n * 255)) };
    }
    else if (i == 4)
    {
        return { static_cast<uint8_t>(floor(n * 255)), 255, 0 };
    }
    else if (i >= 5)
    {
        return { 255, static_cast<uint8_t>(floor(n * 255)), 0 };
    }
}

static Rgb8Color floatToGrayScaleColor(float value)
{
    if(value > 1)
    {
        value = 1;
    }
    if(value < 0)
    {
        value = 0;
    }
    int grayscaleResult = 255 * (value);

    return {
        static_cast<uint8_t>(grayscaleResult),
        static_cast<uint8_t>(grayscaleResult),
        static_cast<uint8_t>(grayscaleResult)
    };
}


template<typename BaseVecT>
Rgb8Color calcColorForFaceCentroid(
    const BaseMesh<BaseVecT>& mesh,
    const PointsetSurface<BaseVecT>& surface,
    FaceHandle faceH
)
{
    if (surface.pointBuffer()->hasRgbColor())
    {
        vector<size_t> cv;
        auto centroid = mesh.calcFaceCentroid(faceH);

        // Find color of face centroid
        int k = 1; // k-nearest-neighbors
        surface.searchTree().kSearch(centroid, k, cv);
        uint8_t r = 0, g = 0, b = 0;
        for (size_t pointIdx : cv)
        {
            optional<array<uint8_t,3>&> colorsOptional = surface.pointBuffer()->getRgbColor(pointIdx);
            if (colorsOptional)
            {
                r += (*colorsOptional)[0];
                g += (*colorsOptional)[1];
                b += (*colorsOptional)[2];
            }
        }
        r /= k;
        g /= k;
        b /= k;

        // "Smooth" colors: convert 0:255 to 0:1, round to 2 decimal places, convert back
        // For better re-using of a single color later on
        Rgb8Color color = {
            static_cast<uint8_t>((floor((((float)r)/255.0)*100.0+0.5)/100.0) * 255.0),
            static_cast<uint8_t>((floor((((float)g)/255.0)*100.0+0.5)/100.0) * 255.0),
            static_cast<uint8_t>((floor((((float)b)/255.0)*100.0+0.5)/100.0) * 255.0)
        };

        return color;
    }
    else
    {
        Rgb8Color color = {0,0,0};
        return color;
    }
}



} // namespace lvr2