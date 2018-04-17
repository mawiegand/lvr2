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
 * SimpleFinalizer.tcc
 *
 *  @date 13.06.2017
 *  @author Johan M. von Behren <johan@vonbehren.eu>
 */

#include <vector>
#include <utility>
#include <cmath>
#include <lvr2/io/MeshBuffer.hpp>
#include <lvr2/algorithm/Materializer.hpp>

#include <lvr/io/Progress.hpp>

namespace lvr2
{

template<typename BaseVecT>
boost::shared_ptr<MeshBuffer<BaseVecT>> SimpleFinalizer<BaseVecT>::apply(const BaseMesh <BaseVecT>& mesh)
{
    // Create vertex and normal buffer
    DenseVertexMap<size_t> idxMap;
    idxMap.reserve(mesh.numVertices());

    vector<float> vertices;
    vertices.reserve(mesh.numVertices() * 3);

    vector<float> normals;
    if (m_normalData)
    {
        normals.reserve(mesh.numVertices() * 3);
    }

    vector<unsigned char> colors;
    if (m_colorData)
    {
        colors.reserve(mesh.numVertices() * 3);
    }

    // for all vertices
    size_t vertexCount = 0;
    for (auto vH : mesh.vertices())
    {
        auto point = mesh.getVertexPosition(vH);

        // add vertex positions to buffer
        vertices.push_back(point.x);
        vertices.push_back(point.y);
        vertices.push_back(point.z);

        if (m_normalData)
        {
            // add normal data to buffer if given
            auto normal = (*m_normalData)[vH];
            normals.push_back(normal.getX());
            normals.push_back(normal.getY());
            normals.push_back(normal.getZ());
        }

        if (m_colorData)
        {
            // add color data to buffer if given
            colors.push_back(static_cast<unsigned char>((*m_colorData)[vH][0]));
            colors.push_back(static_cast<unsigned char>((*m_colorData)[vH][1]));
            colors.push_back(static_cast<unsigned char>((*m_colorData)[vH][2]));
        }

        // Save index of vertex for face mapping
        idxMap.insert(vH, vertexCount);
        vertexCount++;
    }

    // Create face buffer
    vector<unsigned int> faces;
    faces.reserve(mesh.numFaces() * 3);
    for (auto fH : mesh.faces())
    {
        auto handles = mesh.getVerticesOfFace(fH);
        for (auto handle : handles)
        {
            // add faces to buffer
            faces.push_back(idxMap[handle]);
        }
    }

    // create buffer object and pass values
    auto buffer = boost::make_shared<lvr2::MeshBuffer<BaseVecT>>();
    buffer->setVertices(vertices);
    buffer->setFaceIndices(faces);

    if (m_normalData)
    {
        buffer->setVertexNormals(normals);
    }

    if (m_colorData)
    {
        buffer->setVertexColors(colors);
    }

    return buffer;
}

template<typename BaseVecT>
void SimpleFinalizer<BaseVecT>::setColorData(const VertexMap<Rgb8Color>& colorData)
{
    m_colorData = colorData;
}

template<typename BaseVecT>
void SimpleFinalizer<BaseVecT>::setNormalData(const VertexMap<Normal<BaseVecT>>& normalData)
{
    m_normalData = normalData;
}

template<typename BaseVecT>
TextureFinalizer<BaseVecT>::TextureFinalizer(
    const ClusterBiMap<FaceHandle>& cluster
)
    : m_cluster(cluster)
{}

template<typename BaseVecT>
void TextureFinalizer<BaseVecT>::setVertexNormals(const VertexMap<Normal<BaseVecT>>& normals)
{
    m_vertexNormals = normals;
}

template<typename BaseVecT>
void TextureFinalizer<BaseVecT>::setClusterColors(const ClusterMap<Rgb8Color>& colors)
{
    m_clusterColors = colors;
}

template<typename BaseVecT>
void TextureFinalizer<BaseVecT>::setVertexColors(const VertexMap<Rgb8Color>& vertexColors)
{
    m_vertexColors = vertexColors;
}

template<typename BaseVecT>
void TextureFinalizer<BaseVecT>::setMaterializerResult(const MaterializerResult<BaseVecT>& matResult)
{
    m_materializerResult = matResult;
}


template<typename BaseVecT>
boost::shared_ptr<MeshBuffer<BaseVecT>> TextureFinalizer<BaseVecT>::apply(const BaseMesh<BaseVecT>& mesh)
{
    // Create vertex buffer and all buffers holding vertex attributes
    vector<float> vertices;
    vertices.reserve(mesh.numVertices() * 3 * 2);

    vector<float> normals;
    if (m_vertexNormals)
    {
        normals.reserve(mesh.numVertices() * 3 * 2);
    }

    vector<unsigned char> colors;
    if (m_clusterColors)
    {
        colors.reserve(mesh.numVertices() * 3 * 2);
    }

    // Create buffer and variables for texturizing
    bool useTextures = false;
    if (m_materializerResult && m_materializerResult.get().m_textures)
    {
        useTextures = true;
    }
    vector<float> texCoords;
    vector<Material> materials;
    vector<unsigned int> faceMaterials;
    vector<unsigned int> clusterMaterials;
    vector<Texture<BaseVecT>> textures;
    vector<vector<unsigned int>> clusterFaceIndices;
    size_t clusterCount = 0;
    size_t faceCount = 0;

    // Global material index will be used for indexing materials in the faceMaterialIndexBuffer
    // The basic material will have the index 0
    unsigned int globalMaterialIndex = 1;
    // Create default material
    unsigned char defaultR = 0, defaultG = 0, defaultB = 0;
    Material m;
    std::array<unsigned char, 3> arr = {defaultR, defaultG, defaultB};
    m.m_color = std::move(arr);
    materials.push_back(m);
    // This map remembers which texture and material are associated with each other
    std::map<int, unsigned int> textureMaterialMap; // Stores the ID of the material for each textureIndex
    textureMaterialMap[-1] = 0; // texIndex -1 => no texture => default material with index 0

    std::map<Rgb8Color, int> colorMaterialMap;

    // Create face buffer
    vector<unsigned int> faces;
    faces.reserve(mesh.numFaces() * 3);

    // This counter is used to determine the index of a newly inserted vertex
    size_t vertexCount = 0;

    string comment = lvr::timestamp.getElapsedTime() + "Finalizing mesh ";
    lvr::ProgressBar progress(m_cluster.numCluster(), comment);

    // Loop over all clusters
    for (auto clusterH: m_cluster)
    {
        // This map remembers which vertex we already inserted and at what
        // position. This is important to create the face map.
        SparseVertexMap<size_t> idxMap;

        // Vector for storing indices of created faces
        vector<unsigned int> faceIndices;

        ++progress;

        auto& cluster = m_cluster.getCluster(clusterH);

        // Loop over all faces of the cluster
        for (auto faceH: cluster.handles)
        {
            for (auto vertexH: mesh.getVerticesOfFace(faceH))
            {
                // Check if we already inserted this vertex. If not...
                if (!idxMap.containsKey(vertexH))
                {
                    // ... insert it into the buffers (with all its attributes)
                    auto point = mesh.getVertexPosition(vertexH);

                    vertices.push_back(point.x);
                    vertices.push_back(point.y);
                    vertices.push_back(point.z);

                    if (m_vertexNormals)
                    {
                        auto normal = (*m_vertexNormals)[vertexH];
                        normals.push_back(normal.getX());
                        normals.push_back(normal.getY());
                        normals.push_back(normal.getZ());
                    }

                    // If individual vertex colors are present: use these
                    if (m_vertexColors)
                    {
                        colors.push_back(static_cast<unsigned char>((*m_vertexColors)[vertexH][0]));
                        colors.push_back(static_cast<unsigned char>((*m_vertexColors)[vertexH][1]));
                        colors.push_back(static_cast<unsigned char>((*m_vertexColors)[vertexH][2]));
                    }
                    else if (m_clusterColors)
                    {
                        // else: use cluster colors if present
                        colors.push_back(static_cast<unsigned char>((*m_clusterColors)[clusterH][0]));
                        colors.push_back(static_cast<unsigned char>((*m_clusterColors)[clusterH][1]));
                        colors.push_back(static_cast<unsigned char>((*m_clusterColors)[clusterH][2]));
                    } // else: no colors

                    // Save index of vertex for face mapping
                    idxMap.insert(vertexH, vertexCount);
                    vertexCount++;
                }

                // At this point we know that the vertex is certainly in the
                // map (and the buffers).
                faces.push_back(idxMap[vertexH]);
            }
            faceIndices.push_back(faceCount++);
        }

        clusterFaceIndices.push_back(faceIndices);

        // When using textures,
        // For each cluster:
        if (m_materializerResult)
        {
            // This map remembers which vertices were already visited for materials and textures
            // Each vertex must be visited exactly once
            SparseVertexMap<size_t> vertexVisitedMap;
            size_t vertexVisitCount = 0;

            Material m = m_materializerResult.get().m_clusterMaterials.get(clusterH).get();
            bool clusterHasTextures = static_cast<bool>(m.m_texture); // optional
            bool clusterHasColor = static_cast<bool>(m.m_color); // optional

            unsigned int materialIndex;

            // Does this cluster use textures?
            if (useTextures && clusterHasTextures)
            {
                // Yes: read texture info
                TextureHandle texHandle = m.m_texture.get();
                auto texOptional = m_materializerResult.get()
                    .m_textures.get()
                    .get(texHandle);
                const Texture<BaseVecT>& texture = texOptional.get();
                int textureIndex = texture.m_index;

                // Material for this texture already created?
                if (textureMaterialMap.find(textureIndex) != textureMaterialMap.end())
                {
                    // Yes: get index from map
                    materialIndex = textureMaterialMap[textureIndex];
                }
                else
                {
                    // No: create material with texture
                    materials.push_back(m);
                    textures.push_back(texture);
                    textureMaterialMap[textureIndex] = globalMaterialIndex;
                    materialIndex = globalMaterialIndex;
                    globalMaterialIndex++;
                }
            }
            else if (clusterHasColor)
            {
                // Else: does this face have a color?
                Rgb8Color c = m.m_color.get();
                if (colorMaterialMap.count(c))
                {
                    materialIndex = colorMaterialMap[c];
                }
                else
                {
                    colorMaterialMap[c] = globalMaterialIndex;
                    materials.push_back(m);
                    materialIndex = globalMaterialIndex;
                    globalMaterialIndex++;
                }
            }
            else
            {
                materialIndex = 0;
            }

            clusterMaterials.push_back(materialIndex);



            // For each face in cluster:
            // Materials
            for (auto faceH : cluster.handles)
            {

                faceMaterials.push_back(materialIndex);

                // For each vertex in face:
                for (auto vertexH : mesh.getVerticesOfFace(faceH))
                {
                    if (!vertexVisitedMap.containsKey(vertexH))
                    {
                        auto& vertexTexCoords = m_materializerResult.get().m_vertexTexCoords;
                        bool vertexHasTexCoords = vertexTexCoords.is_initialized()
                                                  ? static_cast<bool>(vertexTexCoords.get().get(vertexH))
                                                  : false;

                        if (useTextures && vertexHasTexCoords)
                        {
                            // Use tex coord vertex map to find texture coords
                            const TexCoords coords = m_materializerResult.get()
                                .m_vertexTexCoords.get()
                                .get(vertexH).get()
                                .getTexCoords(clusterH);

                            texCoords.push_back(coords.u);
                            texCoords.push_back(coords.v);
                            texCoords.push_back(0.0);
                        } else {
                            // Cluster does not have a texture, use default coords
                            // Every vertex needs an entry in this buffer,
                            // This is why 0's are inserted
                            texCoords.push_back(0.0);
                            texCoords.push_back(0.0);
                            texCoords.push_back(0.0);
                        }
                        vertexVisitedMap.insert(vertexH, vertexVisitCount);
                        vertexVisitCount++;
                    }
                }
            }
        }
    }

    cout << endl;

    auto buffer = boost::make_shared<MeshBuffer<BaseVecT>>();
    buffer->setVertices(vertices);
    buffer->setFaceIndices(faces);

    if (m_vertexNormals)
    {
        buffer->setVertexNormals(normals);
    }

    if (m_clusterColors || m_vertexColors)
    {
        buffer->setVertexColors(colors);
    }

    if (m_materializerResult)
    {
        buffer->setMaterials(materials);
        buffer->setTextures(textures);
        buffer->setFaceMaterialIndices(faceMaterials);
        buffer->setClusterMaterialIndices(clusterMaterials);
        buffer->setVertexTextureCoordinates(texCoords);
        buffer->setClusterFaceIndices(clusterFaceIndices);
    }

    return buffer;
}

} // namespace lvr2
