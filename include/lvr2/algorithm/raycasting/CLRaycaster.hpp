/**
 * Copyright (c) 2018, University Osnabrück
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

/*
 * CLRaycaster.hpp
 *
 *  @date 25.01.2019
 *  @author Johan M. von Behren <johan@vonbehren.eu>
 *  @author Alexander Mock <amock@uos.de>
 */

#pragma once

#include <lvr2/io/MeshBuffer.hpp>
#include <lvr2/geometry/BaseVector.hpp>
#include <lvr2/geometry/Vector.hpp>
#include <lvr2/geometry/Point.hpp>
#include <lvr2/geometry/BVH.hpp>

#include <lvr2/algorithm/raycasting/BVHRaycaster.hpp>

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_MINIMUM_OPENCL_VERSION 120 // Need to set to 120 on CUDA 8
#define CL_HPP_TARGET_OPENCL_VERSION 120 // Need to set to 120 on CUDA 8
#include <CL/cl2.hpp>
#include <lvr2/util/CLUtil.hpp>

const char *CAST_RAYS_BVH_PROGRAM =
    #include "opencl/cast_rays_bvh.cl"
;

namespace lvr2
{

/**
 *  @brief CLRaycaster: GPU OpenCL version of BVH Raycasting: WIP
 */
template <typename BaseVecT>
class CLRaycaster : public BVHRaycaster<BaseVecT> {
public:

    /**
     * @brief Constructor: Generate BVH tree on mesh, loads CL kernels
     */
    CLRaycaster(const MeshBufferPtr mesh);

    /// Overload functions ///

    bool castRay(
        const Point<BaseVecT>& origin,
        const Vector<BaseVecT>& direction,
        Point<BaseVecT>& intersection
    );

    void castRays(
        const Point<BaseVecT>& origin,
        const std::vector<Vector<BaseVecT> >& directions,
        std::vector<Point<BaseVecT> >& intersections,
        std::vector<uint8_t>& hits
    );

    void castRays(
        const std::vector<Point<BaseVecT> >& origins,
        const std::vector<Vector<BaseVecT> >& directions,
        std::vector<Point<BaseVecT> >& intersections,
        std::vector<uint8_t>& hits
    );

private:
    /**
     * @brief Initializes OpenCL related stuff
     */
    void initOpenCL();

    /**
     * @brief TODO docu
     */
    void initOpenCLTreeBuffer();

    /**
     * @brief TODO
     */
    void initOpenCLRayBuffer(int num_origins, int num_rays);

    /**
     * @brief TODO
     */
    void copyBVHToGPU();

    /**
     * @brief TODO
     */
    void createKernel();

    /**
     * @brief TODO
     */
    void copyRayDataToGPU(
        const vector<float>& origins,
        const vector<float>& rays
    );

    /**
     * @brief TODO
     */
    void copyRayDataToGPU(
        const float* origin_buffer, size_t origin_buffer_size,
        const float* ray_buffer, size_t ray_buffer_size
    );

    // Member vars
    // OpenCL variables
    cl::Platform m_platform;
    cl::Device m_device;
    cl::Context m_context;
    cl::Program m_program;
    cl::CommandQueue m_queue;
    cl::Kernel m_kernel_one_one;
    cl::Kernel m_kernel_one_multi;
    cl::Kernel m_kernel_multi_multi;
    cl::Kernel m_kernel_test;


    /// BUFFER ///
    // buffer bvh tree
    cl::Buffer m_bvhIndicesOrTriListsBuffer;
    cl::Buffer m_bvhLimitsnBuffer;
    cl::Buffer m_bvhTriangleIntersectionDataBuffer;
    cl::Buffer m_bvhTriIdxListBuffer;

    // buffer rays
    cl::Buffer m_rayBuffer;
    cl::Buffer m_rayOriginBuffer;

    // buffer results
    cl::Buffer m_resultBuffer;
    cl::Buffer m_resultHitsBuffer;

};

} // namespace lvr2

#include <lvr2/algorithm/raycasting/CLRaycaster.tcc>