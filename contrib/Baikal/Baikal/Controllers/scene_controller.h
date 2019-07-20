/**********************************************************************
 Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ********************************************************************/

/**
 \file scene_controller.h
 \author Dmitry Kozlov
 \version 1.0
 \brief Contains SceneController interface.
 */
#pragma once

#include "SceneGraph/Collector/collector.h"
#include "SceneGraph/material.h"
#include "SceneGraph/scene1.h"

#include <memory>
#include <map>

namespace Baikal
{
    class Scene1;
    class Bundle;
    class Material;
    class Light;
    class Texture;
    class VolumeMaterial;

    /**
     \brief Tracks changes of a scene and serialized data if needed.

     SceneTracker class is intended to keep track of CPU side scene changes and update all
     necessary renderer buffers.
     */
    template <typename CompiledScene> class SceneController
    {
    public:
        // Constructor
        SceneController();
        // Destructor
        virtual ~SceneController() = default;

        // Given a scene this method produces (or loads from cache) corresponding GPU representation.
        CompiledScene& CompileScene(Scene1::Ptr scene) const;

        CompiledScene& GetCachedScene(Scene1::Ptr scene) const;

        static void ResetId();

    protected:
        // Recompile the scene from scratch, i.e. not loading from cache.
        // All the buffers are recreated and reloaded.
        void RecompileFull(Scene1 const& scene, Collector& mat_collector, Collector& tex_collector,
                           Collector& vol_collector, Collector& input_maps_collector,
                           Collector& input_map_leafs_collector, CompiledScene& out) const;

        // set dirty flag to false for camera object
        void DropCameraDirty(Scene1 const& scene) const;
        // set dirty flag to false for iterator
        void DropDirty(Iterator& light_iterator) const;
    public:
        // Update camera data only.
        virtual void UpdateCamera(Scene1 const& scene, Collector& mat_collector, Collector& tex_collector, Collector& vol_collector, CompiledScene& out) const = 0;
        // Update shape data only.
        virtual void UpdateShapes(Scene1 const& scene, Collector& mat_collector, Collector& tex_collector, Collector& volume_collector, CompiledScene& out) const = 0;
        // Update shape transforms
        virtual void UpdateShapeProperties(Scene1 const& scene, Collector& mat_collector, Collector& tex_collector, Collector& volume_collector, CompiledScene& out) const = 0;
        // Update lights data only.
        virtual void UpdateLights(Scene1 const& scene, Collector& mat_collector, Collector& tex_collector, CompiledScene& out) const = 0;
        // Update material data.
        virtual void UpdateMaterials(Scene1 const& scene, Collector& mat_collector, Collector& tex_collector, CompiledScene& out) const = 0;
        // Update texture data only.
        virtual void UpdateTextures(Scene1 const& scene, Collector& mat_collector, Collector& tex_collector, CompiledScene& out) const = 0;
        // Update input maps only
        virtual void UpdateInputMaps(Scene1 const& scene, Collector& input_map_collector, Collector& input_map_leafs_collector, CompiledScene& out) const = 0;
        // Update input map leafs only
        virtual void UpdateLeafsData(Scene1 const& scene, Collector& input_map_leafs_collector, Collector& tex_collector, CompiledScene& out) const = 0;
        // Default material
        virtual Material::Ptr GetDefaultMaterial() const = 0;
        // If m_current_scene changes
        virtual void UpdateCurrentScene(Scene1 const& scene, CompiledScene& out) const = 0;
        // Update volume materials only
        virtual void UpdateVolumes(Scene1 const& scene, Collector& volume_collector, Collector& tex_collector, CompiledScene& out) const = 0;
        // If scene attributes changed
        virtual void UpdateSceneAttributes(Scene1 const& scene, Collector& tex_collector, CompiledScene& out) const = 0;


    private:
        mutable Scene1::Ptr m_current_scene;
        // Scene cache map (CPU scene -> GPU scene mapping)
        mutable std::map<Scene1::Ptr, CompiledScene> m_scene_cache;

        mutable Collector m_material_collector;
        mutable Collector m_volume_collector;
        mutable Collector m_texture_collector;
        mutable Collector m_input_maps_collector;
        mutable Collector m_input_map_leafs_collector;

        // Scene controller id
        std::uint32_t m_id;
    };
}

#include "scene_controller.inl"
