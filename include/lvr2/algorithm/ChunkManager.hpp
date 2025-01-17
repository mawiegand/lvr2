/**
 * Copyright (c) 2019, University Osnabrück
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University Osnabrück nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL University Osnabrück BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * ChunkManager.hpp
 *
 * @date 21.07.2019
 * @author Malte kl. Piening
 * @author Marcel Wiegand
 * @author Raphael Marx
 */

#ifndef CHUNK_MANAGER_HPP
#define CHUNK_MANAGER_HPP

#include "lvr2/algorithm/ChunkBuilder.hpp"
#include "lvr2/algorithm/ChunkHashGrid.hpp"
#include "lvr2/geometry/BaseVector.hpp"
#include "lvr2/geometry/BoundingBox.hpp"
#include "lvr2/io/ChunkIO.hpp"
#include "lvr2/io/Model.hpp"
#include "lvr2/types/Channel.hpp"

namespace lvr2
{

class ChunkManager
{
  public:
    /**
     * @brief ChunkManager creates chunks from an original mesh
     *
     * Chunks the original model into chunks of given size.
     * Every created chunk has the same length in height, width and depth.
     *
     * @param mesh mesh to be chunked
     * @param chunksize size of a chunk - unit depends on the given mesh
     * @param maxChunkOverlap maximum allowed overlap between chunks relative to the chunk size.
     * Larger triangles will be cut
     * @param savePath JUST FOR TESTING - REMOVE LATER ON
     * @param cacheSize maximum number of chunks loaded in the ChunkHashGrid
     */
    ChunkManager(MeshBufferPtr mesh, float chunksize, float maxChunkOverlap, std::string savePath, size_t cacheSize = 200);
    /**
     * @brief ChunkManager loads a ChunkManager from a given HDF5-file
     *
     * Creates a ChunkManager from an already chunked HDF5 file and allows loading individual chunks
     * and combining them to partial meshes.
     * Every loaded chunk has the same length in height, width and depth.
     *
     * @param hdf5Path path to the HDF5 file, where chunks and additional information are stored
     * @param cacheSize maximum number of chunks loaded in the ChunkHashGrid
     */
    ChunkManager(std::string hdf5Path, size_t cacheSize = 200);
    /**
     * @brief extractArea creates and returns MeshBufferPtr of merged chunks for given area.
     *
     * Finds corresponding chunks for given area inside the grid and merges those chunks to a new
     * mesh without duplicated vertices. The new mesh is returned as MeshBufferPtr.
     *
     * @param area
     * @return mesh of the given area
     */
    MeshBufferPtr extractArea(const BoundingBox<BaseVector<float>>& area);

    /**
     * @brief Calculates the hash value for the given index triple
     *
     * @param i index of x-axis
     * @param j index of y-axis
     * @param k index of z-axis
     * @return hash value
     */
    inline std::size_t hashValue(int i, int j, int k) const
    {
        return i * m_amount.y * m_amount.z + j * m_amount.z + k;
    }

    /**
     * @brief Loads all chunks into the ChunkHashGrid.
     * DEBUG -- Only used for testing, but might be useful for smaller meshes.
     */
    void loadAllChunks();

  private:
    /**
     * @brief initBoundingBox calculates a bounding box of the original mesh
     *
     * This calculates the bounding box of the given model and saves it to m_boundingBox.
     *
     * @param mesh mesh whose bounding box shall be calculated
     */
    void initBoundingBox(MeshBufferPtr mesh);

    /**
     * @brief cutLargeFaces cuts a face if it is too large
     *
     * Checks whether or not a triangle is overlapping the chunk borders too much and
     * cuts those faces. The resulting smaller faces will be added as additional vertices
     * and faces to the chunk hat holds their center point.
     *
     * @param halfEdgeMesh mesh  that is being cut
     * @param overlapRatio ration of maximum allowed overlap and the chunks side length
     * @param splitVertices map from new vertex indices to old vertex indices for all faces that
     * will be cut
     * @param splitFaces map from new face indices to old face indices for all faces that will be
     * cut
     */
    void
    cutLargeFaces(std::shared_ptr<HalfEdgeMesh<BaseVector<float>>> halfEdgeMesh,
                  float overlapRatio,
                  std::shared_ptr<std::unordered_map<unsigned int, unsigned int>> splitVertices,
                  std::shared_ptr<std::unordered_map<unsigned int, unsigned int>> splitFaces);

    /**
     * @brief buildChunks builds chunks from an original mesh
     *
     * Creates chunks from an original mesh and initializes the initial chunk structure
     *
     * @param mesh mesh which is being chunked
     * @param maxChunkOverlap maximum allowed overlap between chunks relative to the chunk size.
     * Larger triangles will be cut
     * @param savePath UST FOR TESTING - REMOVE LATER ON
     */
    void buildChunks(MeshBufferPtr mesh, float maxChunkOverlap, std::string savePath);

    /**
     * @brief getFaceCenter gets the center point for a given face
     *
     * @param verticesChannel channel of mesh that holds the vertices
     * @param facesChannel channel of mesh that holds the faces
     * @param faceIndex index of the requested face
     * @return center point of the given face
     */
    BaseVector<float> getFaceCenter(std::shared_ptr<HalfEdgeMesh<BaseVector<float>>> mesh,
                                    const FaceHandle& handle) const;

    //    /**
    //     * @brief find corresponding grid cell of given point
    //     *
    //     * @param vec point of mesh to find cell id for
    //     * @return cell id
    //     */
    //    std::string getCellName(const BaseVector<float>& vec) const;

    /**
     * @brief returns the grid coordinates of a given point
     *
     * @param vec point of which we want the grid coordinates
     * @return the grid coordinates as a BaseVector
     */
    BaseVector<int> getCellCoordinates(const BaseVector<float>& vec) const;

    /**
     * @brief returns the HashValue of a grid cell which would include the given point
     *
     * @param vec point of which we want the hashValue of its grid cell
     * @return the HashValue of a grid cell which would include the given point
     */
    std::size_t getCellIndex(const BaseVector<float>& vec) const;

    /**
     * @brief reads and combines a channel of multiple chunks
     *
     * @param chunks list of chunks to combine
     * @param channelName name of channel to extract
     * @param staticVertexIndexOffset amount of duplicate vertices in the combined mesh
     * @param numVertices amount of vertices in the combined mesh
     * @param numFaces amount of faces in the combined mesh
     * @param areaVertexIndices mapping from old vertex index to new vertex index per chunk
     */
    template <typename T>
    ChannelPtr<T> extractChannelOfArea(std::unordered_map<std::size_t, MeshBufferPtr>& chunks,
                                       std::string channelName,
                                       std::size_t staticVertexIndexOffset,
                                       std::size_t numVertices,
                                       std::size_t numFaces,
                                       std::vector<std::unordered_map<std::size_t, std::size_t>>& areaVertexIndices);

    // bounding box of the entire chunked model
    BoundingBox<BaseVector<float>> m_boundingBox;

    // size of chunks
    float m_chunkSize;

    // amount of chunks
    BaseVector<std::size_t> m_amount;

    // used for loading chunks from the HDF5 file and saving them in a HashGrid
    std::shared_ptr<ChunkHashGrid> m_chunkHashGrid;

    // path to the HDF5 file (either to save or to load the file)
    std::string m_hdf5Path;
};

} /* namespace lvr2 */

#include "ChunkManager.tcc"

#endif // CHUNK_MANAGER_HPP
