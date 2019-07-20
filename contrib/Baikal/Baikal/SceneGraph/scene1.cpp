#include "scene1.h"
#include "light.h"
#include "camera.h"
#include "iterator.h"

#include <vector>
#include <list>
#include <cassert>
#include <set>
#include <mutex>

namespace Baikal
{
    // Data structures for shapes and lights
    using ShapeList = std::vector<Shape::Ptr>;
    using LightList = std::vector<Light::Ptr>;

    // Internal data
    struct Scene1::SceneImpl
    {
        ShapeList m_shapes;
        LightList m_lights;
        Camera::Ptr m_camera;
        Baikal::Texture::Ptr m_background_texture;
        EnvironmentOverride m_environment_override;

        DirtyFlags m_dirty_flags;
        std::mutex m_scene_mutex;
    };

    Scene1::Scene1()
    : m_impl(new SceneImpl)
    {
        m_impl->m_camera = nullptr;
        ClearDirtyFlags();
    }

    Scene1::~Scene1() = default;

    Scene1::DirtyFlags Scene1::GetDirtyFlags() const
    {
        return m_impl->m_dirty_flags;
    }

    void Scene1::ClearDirtyFlags() const
    {
        m_impl->m_dirty_flags = 0;
    }

    void Scene1::SetDirtyFlag(DirtyFlags flag) const
    {
        m_impl->m_dirty_flags = m_impl->m_dirty_flags | flag;
    }

    void Scene1::SetCamera(Camera::Ptr camera)
    {
        m_impl->m_camera = camera;
        SetDirtyFlag(kCamera);
    }

    Camera::Ptr Scene1::GetCamera() const
    {
        return m_impl->m_camera;
    }

    void Scene1::AttachLight(Light::Ptr light)
    {
        assert(light);

        // Check if the light is already in the scene
        LightList::const_iterator citer =  std::find(m_impl->m_lights.cbegin(),
                                                     m_impl->m_lights.cend(),
                                                     light);
        
        // And insert only if not
        if (citer == m_impl->m_lights.cend())
        {
            m_impl->m_lights.push_back(light);

            SetDirtyFlag(kLights);
        }
    }

    void Scene1::DetachLight(Light::Ptr light)
    {
        // Check if the light is in the scene
        LightList::const_iterator citer =  std::find(m_impl->m_lights.cbegin(),
                                                     m_impl->m_lights.cend(),
                                                     light);
        
        // And remove it if yes
        if (citer != m_impl->m_lights.cend())
        {
            m_impl->m_lights.erase(citer);
            
            SetDirtyFlag(kLights);
        }
    }

    std::size_t Scene1::GetNumLights() const
    {
        return m_impl->m_lights.size();
    }

    std::unique_ptr<Iterator> Scene1::CreateShapeIterator() const
    {
        return std::make_unique<IteratorImpl<ShapeList::const_iterator>>
            (m_impl->m_shapes.begin(), m_impl->m_shapes.end());
    }
    
    void Scene1::AttachShape(Shape::Ptr shape)
    {
        assert(shape);
        
        // Check if the shape is already in the scene
        ShapeList::const_iterator citer =  std::find(m_impl->m_shapes.cbegin(),
                                                     m_impl->m_shapes.cend(),
                                                     shape);
        
        // And attach it if not
        if (citer == m_impl->m_shapes.cend())
        {
            m_impl->m_shapes.push_back(shape);
            
            SetDirtyFlag(kShapes);
        }
    }
    
    void Scene1::DetachShape(Shape::Ptr shape)
    {
        assert(shape);
        
        // Check if the shape is in the scene
        ShapeList::const_iterator citer =  std::find(m_impl->m_shapes.cbegin(),
                                                     m_impl->m_shapes.cend(),
                                                     shape);
        
        // And detach if yes
        if (citer != m_impl->m_shapes.cend())
        {
            m_impl->m_shapes.erase(citer);
            
            SetDirtyFlag(kShapes);
        }
    }
    
    std::size_t Scene1::GetNumShapes() const
    {
        return m_impl->m_shapes.size();
    }
    
    std::unique_ptr<Iterator> Scene1::CreateLightIterator() const
    {
        return std::make_unique<IteratorImpl<LightList::const_iterator>>
            (m_impl->m_lights.begin(), m_impl->m_lights.end());
    }
    
    bool Scene1::IsValid() const
    {
        return GetCamera() &&
        GetNumLights() > 0 &&
        GetNumShapes() > 0;
    }

    RadeonRays::bbox Scene1::GetWorldAABB() const
    {
        auto iter = CreateShapeIterator();

        RadeonRays::bbox result;
        for (; iter->IsValid(); iter->Next())
        {
            auto aabb = iter->ItemAs<Shape>()->GetWorldAABB();
            result.grow(aabb);
        }

        return result;
    }

    float Scene1::GetRadius() const
    {
        auto aabb = GetWorldAABB();

        auto c = aabb.center();

        return std::sqrt((aabb.pmax - c).sqnorm());
    }
    
    void Scene1::SetBackgroundImage(Baikal::Texture::Ptr texture)
    {
        m_impl->m_background_texture = texture;
        SetDirtyFlag(kBackground);
    }

    Baikal::Texture::Ptr Scene1::GetBackgroundImage() const
    {
        return m_impl->m_background_texture;
    }

    void Scene1::SetEnvironmentOverride(const EnvironmentOverride& env_override)
    {
        auto check_and_set_light = [&](ImageBasedLight::Ptr current_light, ImageBasedLight::Ptr new_light)
        {
            if (current_light != new_light)
            {
                if (current_light)
                    DetachLight(current_light);
                if (new_light)
                    AttachLight(new_light);
            }
        };

        check_and_set_light(m_impl->m_environment_override.m_background, env_override.m_background);
        check_and_set_light(m_impl->m_environment_override.m_reflection, env_override.m_reflection);
        check_and_set_light(m_impl->m_environment_override.m_refraction, env_override.m_refraction);
        check_and_set_light(m_impl->m_environment_override.m_transparency, env_override.m_transparency);
        m_impl->m_environment_override = env_override;
    }
    const Scene1::EnvironmentOverride& Scene1::GetEnvironmentOverride() const
    {
        return m_impl->m_environment_override;
    }

    void Scene1::Acquire(std::uint32_t controller_id)
    {
        m_impl->m_scene_mutex.lock();
        SceneObject::SetSceneControllerId(controller_id);
    }

    void Scene1::Release()
    {
        SceneObject::ResetSceneControllerId();
        m_impl->m_scene_mutex.unlock();
    }

    namespace {
        struct Scene1Concrete : public Scene1 {
        };
    }
    
    Scene1::Ptr Scene1::Create() {
        return std::make_shared<Scene1Concrete>();
    }
}
