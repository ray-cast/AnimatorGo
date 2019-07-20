#include "SceneGraph/scene1.h"
#include "SceneGraph/camera.h"
#include "SceneGraph/light.h"
#include "SceneGraph/shape.h"
#include "SceneGraph/material.h"
#include "SceneGraph/texture.h"
#include "SceneGraph/Collector/collector.h"
#include "SceneGraph/iterator.h"
#include "SceneGraph/uberv2material.h"

#include <chrono>
#include <memory>
#include <stack>
#include <vector>
#include <array>

namespace Baikal
{
    // Defined in scene_controller.cpp
    std::uint32_t GetNextControllerId();
    void ResetControllerId();

    template <typename CompiledScene>
    SceneController<CompiledScene>::SceneController()
        : m_id(GetNextControllerId())
    {
    }

    template <typename CompiledScene>
    void SceneController<CompiledScene>::ResetId()
    {
        ResetControllerId();
    }

    template <typename CompiledScene>
    inline
    CompiledScene& SceneController<CompiledScene>::GetCachedScene(Scene1::Ptr scene) const {
        // Try to find scene in cache first
        auto iter = m_scene_cache.find(scene);

        if (iter != m_scene_cache.cend()) {
            return iter->second;
        } else {
            throw std::runtime_error("Scene has not been compiled");
        }
    }

    template <typename CompiledScene>
    inline
    CompiledScene& SceneController<CompiledScene>::CompileScene(
        Scene1::Ptr scene
    ) const {

        scene->Acquire(m_id);

        // The overall approach is:
        // 1) Check if materials have changed, update collector if yes
        // 2) Check if textures have changed, update collector if yes
        // Note, that material are collected from shapes (potentially recursively).
        // Textures are collected from materials and lights.
        // As soon as we updated both collectors we have established
        // correct pointer to buffer index mapping for both materials and textures.
        // As soon as we have this mapping we are analyzing dirty flags and
        // updating necessary parts.

        // We need to make sure collectors are empty before proceeding
        m_material_collector.Clear();
        m_texture_collector.Clear();
        m_volume_collector.Clear();
        m_input_maps_collector.Clear();
        m_input_map_leafs_collector.Clear();

        // Create shape and light iterators
        auto shape_iter = scene->CreateShapeIterator();
        auto light_iter = scene->CreateLightIterator();

        auto default_material = GetDefaultMaterial();
        // Collect materials from shapes first
        m_material_collector.Collect(*shape_iter,
                              // This function adds all materials to resulting map
                              // recursively via Material dependency API
                              [default_material](SceneObject::Ptr item) ->
                              std::set<SceneObject::Ptr>
                              {
                                  // Resulting material set
                                  std::set<SceneObject::Ptr> mats;
                                  // Material stack
                                  std::stack<Material::Ptr> material_stack;

                                  // Get material from current shape
                                  auto shape = std::static_pointer_cast<Shape>(item);
                                  auto material = shape->GetMaterial();

                                  // If shape does not have a material, use default one
                                  if (!material)
                                  {
                                      material = default_material;
                                  }

                                  // Push to stack as an initializer
                                  material_stack.push(material);

                                  // Drain the stack
                                  while (!material_stack.empty())
                                  {
                                      // Get current material
                                      auto m = material_stack.top();
                                      material_stack.pop();

                                      // Emplace into the set
                                      mats.emplace(m);

                                      // Create dependency iterator
                                      auto mat_iter = m->CreateMaterialIterator();

                                      // Push all dependencies into the stack
                                      for (; mat_iter->IsValid(); mat_iter->Next())
                                      {
                                          material_stack.push(
                                            mat_iter->ItemAs<Material>()
                                          );
                                      }
                                  }

                                  // Return resulting set
                                  return mats;
                              });

        // Commit stuff (we can iterate over it after commit has happened)
        m_material_collector.Commit();

        // set iterator position at begin
        shape_iter->Reset();
        // Collect volume materials from shapes first
        m_volume_collector.Collect(*shape_iter,
                                    [](SceneObject::Ptr item) -> std::set<SceneObject::Ptr>
                                    {
                                        // Resulting material set
                                        std::set<SceneObject::Ptr> vol_mats;

                                        // Get volume material from current shape
                                        auto shape = std::static_pointer_cast<Shape>(item);
                                        auto volume_material = shape->GetVolumeMaterial();

                                        if (volume_material)
                                            vol_mats.emplace(volume_material);

                                        return vol_mats;
                                    });

        // Commit stuff
        m_volume_collector.Commit();

        // Now we need to collect textures from our materials
        // Create material iterator
        auto mat_iter = m_material_collector.CreateIterator();

        // Collect textures from materials
        m_texture_collector.Collect(*mat_iter,
                                    [](SceneObject::Ptr item) -> std::set<SceneObject::Ptr>
                              {
                                  // Texture set
                                  std::set<SceneObject::Ptr> textures;

                                  auto material = std::static_pointer_cast<Material>(item);

                                  // Create texture dependency iterator
                                  auto tex_iter = material->CreateTextureIterator();

                                  // Emplace all dependent textures
                                  for (; tex_iter->IsValid(); tex_iter->Next())
                                  {
                                      textures.emplace(tex_iter->ItemAs<Texture>());
                                  }

                                  // Return resulting set
                                  return textures;
                              });

        // Now we need to collect textures from volumes
        // Create volume iterator
        auto vol_iter = m_volume_collector.CreateIterator();

        // Collect textures from materials
        m_texture_collector.Collect(*vol_iter,
            [](SceneObject::Ptr item) -> std::set<SceneObject::Ptr>
        {
            // Texture set
            std::set<SceneObject::Ptr> textures;

            auto volume = std::static_pointer_cast<VolumeMaterial>(item);

            // Create texture dependency iterator
            auto tex_iter = volume->CreateTextureIterator();

            // Emplace all dependent textures
            for (; tex_iter->IsValid(); tex_iter->Next())
            {
                textures.emplace(tex_iter->ItemAs<Texture>());
            }

            // Return resulting set
            return textures;
        });

        // Collect textures from lights
        m_texture_collector.Collect(*light_iter,
                                    [](SceneObject::Ptr item) -> std::set<SceneObject::Ptr>
                              {
                                  // Resulting set
                                  std::set<SceneObject::Ptr> textures;

                                  auto light = std::static_pointer_cast<Light>(item);

                                  // Create texture dependency iterator
                                  auto tex_iter = light->CreateTextureIterator();

                                  // Emplace all dependent textures
                                  for (; tex_iter->IsValid(); tex_iter->Next())
                                  {
                                      textures.emplace(tex_iter->ItemAs<Texture>());
                                  }

                                  // Return resulting set
                                  return textures;
                              });

        mat_iter->Reset();
        m_input_maps_collector.Collect(*mat_iter,
                                [](SceneObject::Ptr item) -> std::set<SceneObject::Ptr>
                                {
                                    // Texture set
                                    std::set<SceneObject::Ptr> input_maps;

                                    auto material = std::static_pointer_cast<Material>(item);

                                    // Create texture dependency iterator
                                    auto input_map_iter = material->CreateInputMapsIterator();

                                    // Emplace all dependent textures
                                    for (; input_map_iter->IsValid(); input_map_iter->Next())
                                    {
                                        input_maps.emplace(input_map_iter->ItemAs<InputMap>());
                                    }

                                    // Return resulting set
                                    return input_maps;
                                });
        m_input_maps_collector.Commit();

        mat_iter->Reset();
        m_input_map_leafs_collector.Collect(*mat_iter,
                                [](SceneObject::Ptr item) -> std::set<SceneObject::Ptr>
                                {
                                    // Texture set
                                    std::set<SceneObject::Ptr> input_maps;

                                    auto material = std::static_pointer_cast<Material>(item);

                                    // Create texture dependency iterator
                                    auto input_map_iter = material->CreateInputMapLeafsIterator();

                                    // Emplace all dependent textures
                                    for (; input_map_iter->IsValid(); input_map_iter->Next())
                                    {
                                        input_maps.emplace(input_map_iter->ItemAs<InputMap>());
                                    }

                                    // Return resulting set
                                    return input_maps;
                                });
        m_input_map_leafs_collector.Commit();


        // Add background texture from scene into texture collector
        auto background_texture = scene->GetBackgroundImage();
        if (background_texture)
            m_texture_collector.Collect(background_texture);

        // Commit textures
        m_texture_collector.Commit();

        // Try to find scene in cache first
        auto iter = m_scene_cache.find(scene);

        if (iter == m_scene_cache.cend())
        {
            // If not found create scene entry in cache
            auto res = m_scene_cache.emplace(std::make_pair(scene, CompiledScene()));

            // Recompile all the stuff into cached scene
            RecompileFull(*scene, m_material_collector, m_texture_collector, m_volume_collector,
                          m_input_maps_collector, m_input_map_leafs_collector, res.first->second);

            // Set scene as current
            m_current_scene = scene;

            // Drop all dirty flags for the scene
            scene->ClearDirtyFlags();

            // Drop dirty flags for materials
            m_material_collector.Finalize([](SceneObject::Ptr item)
            {
                auto material = std::static_pointer_cast<Material>(item);
                material->SetDirty(false);
            });

            m_volume_collector.Finalize([](SceneObject::Ptr item)
            {
                auto volume = std::static_pointer_cast<VolumeMaterial>(item);
                volume->SetDirty(false);
            });

            // It will mark entire hierarchy as not dirty
            m_input_maps_collector.Finalize([](SceneObject::Ptr item)
            {
                auto input_map = std::static_pointer_cast<InputMap>(item);
                input_map->SetDirty(false);
            });

            // Return the scene
            scene->Release();
            return res.first->second;
        }
        else
        {
            // Exctract cached scene entry
            auto& out = iter->second;
            auto dirty = scene->GetDirtyFlags();

            bool should_update_materials = !out.material_bundle ||
                m_material_collector.NeedsUpdate(out.material_bundle.get(),
                                                 [](SceneObject::Ptr ptr)->bool
            {
                auto mat = std::static_pointer_cast<Material>(ptr);
                return mat->IsDirty();
            });

            bool should_update_volumes = !out.volume_bundle ||
                m_volume_collector.NeedsUpdate(out.volume_bundle.get(),
                                               [](SceneObject::Ptr ptr)->bool
            {
                auto volume = std::static_pointer_cast<VolumeMaterial>(ptr);
                return volume->IsDirty();
            });

            bool should_update_textures = m_texture_collector.GetNumItems() > 0 && (
                !out.texture_bundle ||
                m_texture_collector.NeedsUpdate(out.texture_bundle.get(), [](SceneObject::Ptr ptr) {
                auto tex = std::static_pointer_cast<Texture>(ptr);
                return tex->IsDirty(); }));

            bool should_update_leafs_data = (m_input_map_leafs_collector.GetNumItems() > 0) && (
                !out.input_map_leafs_bundle ||
                m_input_map_leafs_collector.NeedsUpdate(out.input_map_leafs_bundle.get(), [](SceneObject::Ptr ptr)
                {
                    return ptr->IsDirty();
                }));

            bool should_update_input_maps = (m_input_maps_collector.GetNumItems() > 0) && (
                !out.input_map_bundle ||
                m_input_maps_collector.NeedsUpdate(out.input_map_bundle.get(), [](SceneObject::Ptr ptr)
                {
                    return ptr->IsDirty();
                }));

            // Check if we have valid camera
            auto camera = scene->GetCamera();

            if (!camera)
            {
                throw std::runtime_error("No camera in the scene");
            }

            // Check if camera parameters have been changed
            auto camera_changed = camera->IsDirty();

            // Update camera if needed
            if (dirty & Scene1::kCamera || camera_changed)
            {
                UpdateCamera(*scene, m_material_collector, m_texture_collector, m_volume_collector, out);
                DropCameraDirty(*scene);
            }

            // If materials need an update, do it.
            // We are passing material dirty state detection function in there.
            // We update materials before lights and shapes since they depends on it.
            if (should_update_materials)
            {
                UpdateMaterials(*scene, m_material_collector, m_texture_collector, out);
            }

            {
                // Check if we have lights in the scene
                auto light_iter = scene->CreateLightIterator();

                if (!light_iter->IsValid())
                {
                    throw std::runtime_error("No lights in the scene");
                }


                // Check if light parameters have been changed
                bool lights_changed = false;

                for (; light_iter->IsValid(); light_iter->Next())
                {
                    auto light = light_iter->ItemAs<Light>();

                    if (light->IsDirty())
                    {
                        lights_changed = true;
                        break;
                    }
                }


                // Update lights if needed
                if (dirty & Scene1::kLights || lights_changed ||
                    should_update_textures || should_update_materials)
                {
                    UpdateLights(*scene, m_material_collector, m_texture_collector, out);
                    light_iter->Reset();
                    DropDirty(*light_iter);
                }
            }

            {
                // Check if we have shapes in the scene
                auto shape_iter = scene->CreateShapeIterator();

                if (!shape_iter->IsValid())
                {
                    throw std::runtime_error("No shapes in the scene");
                }

                // Check if shape parameters have been changed
                bool shapes_changed = false;

                for (; shape_iter->IsValid(); shape_iter->Next())
                {
                    auto shape = shape_iter->ItemAs<Shape>();

                    if (shape->IsDirty())
                    {
                        shapes_changed = true;
                        break;
                    }
                }

                // Update shapes if needed
                if (dirty & Scene1::kShapes)
                {
                    UpdateShapes(*scene, m_material_collector, m_texture_collector, m_volume_collector, out);
                    shape_iter->Reset();
                    DropDirty(*shape_iter);
                }
                else if (shapes_changed)
                {
                    UpdateShapeProperties(*scene, m_material_collector, m_texture_collector, m_volume_collector, out);
                }
            }

            // If textures need an update, do it.
            if (should_update_textures)
            {
                UpdateTextures(*scene, m_material_collector, m_texture_collector, out);
            }

            // If volumes need an update, do it.
            if (should_update_volumes)
            {
                UpdateVolumes(*scene, m_volume_collector, m_texture_collector, out);
            }

            if (should_update_leafs_data)
            {
                UpdateLeafsData(*scene, m_input_map_leafs_collector, m_texture_collector, out);
            }

            if (should_update_input_maps)
            {
                UpdateInputMaps(*scene, m_input_maps_collector, m_input_map_leafs_collector, out);
            }

            // Set current scene
            if (m_current_scene != scene)
            {
                m_current_scene = scene;

                UpdateCurrentScene(*scene, out);
            }

            // If background image need an update, do it.
            if ((scene->GetDirtyFlags() & Scene1::kBackground) == Scene1::kBackground)
            {
                UpdateSceneAttributes(*scene, m_texture_collector, out);
            }

            // Make sure to clear dirty flags
            scene->ClearDirtyFlags();

            // Clear material dirty flags
            m_material_collector.Finalize([](SceneObject::Ptr item)
            {
                auto material = std::static_pointer_cast<Material>(item);
                material->SetDirty(false);
            });

            m_texture_collector.Finalize([](SceneObject::Ptr item)
            {
                auto tex = std::static_pointer_cast<Texture>(item);
                tex->SetDirty(false);
            });

            m_volume_collector.Finalize([](SceneObject::Ptr item)
            {
                auto volume = std::static_pointer_cast<VolumeMaterial>(item);
                volume->SetDirty(false);
            });

            // It will mark entire hierarchy as not dirty
            m_input_maps_collector.Finalize([](SceneObject::Ptr item)
            {
                auto input_map = std::static_pointer_cast<InputMap>(item);
                input_map->SetDirty(false);
            });

            // Return the scene
            scene->Release();
            return out;
        }
    }

    template <typename CompiledScene>
    inline
    void SceneController<CompiledScene>::RecompileFull(
        Scene1 const& scene, Collector& m_material_collector, Collector& m_texture_collector, Collector& vol_collector,
        Collector& input_maps_collector, Collector& input_map_leafs_collector, CompiledScene& out) const
    {
        UpdateCamera(scene, m_material_collector, m_texture_collector, m_volume_collector, out);
        DropCameraDirty(scene);

        //Lights and Shapes depends on Materials
        UpdateMaterials(scene, m_material_collector, m_texture_collector, out);

        UpdateLights(scene, m_material_collector, m_texture_collector, out);
        auto light_iterator = scene.CreateLightIterator();
        DropDirty(*light_iterator);

        UpdateShapes(scene, m_material_collector, m_texture_collector, vol_collector, out);
        auto shape_iterator = scene.CreateShapeIterator();
        DropDirty(*shape_iterator);

        UpdateTextures(scene, m_material_collector, m_texture_collector, out);

        UpdateLeafsData(scene, m_input_map_leafs_collector, m_texture_collector, out);

        UpdateInputMaps(scene, m_input_maps_collector, m_input_map_leafs_collector, out);

        UpdateVolumes(scene, vol_collector, m_texture_collector, out);

        UpdateSceneAttributes(scene, m_texture_collector, out);
    }

    template <typename CompiledScene>
    inline
    void SceneController<CompiledScene>::DropCameraDirty(Scene1 const& scene) const
    {
        auto camera = scene.GetCamera();

        if (!camera)
            throw std::runtime_error("SceneController::RecompileFull(...): camera was not set");

        camera->SetDirty(false);
    }

    template <typename CompiledScene>
    inline
    void SceneController<CompiledScene>::DropDirty(Iterator& iterator) const
    {
        for (; iterator.IsValid(); iterator.Next())
            iterator.ItemAs<SceneObject>()->SetDirty(false);
    }
}
